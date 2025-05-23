// SPDX-License-Identifier: GPL-2.0-or-later

/***************************************************************************
 *   Copyright (C) 2011 by Broadcom Corporation                            *
 *   Evan Hunter - ehunter@broadcom.com                                    *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <helper/time_support.h>
#include <jtag/jtag.h>
#include "target/target.h"
#include "rtos.h"
#include "helper/log.h"
#include "helper/types.h"
#include "rtos_embkernel_stackings.h"

#define EMBKERNEL_MAX_THREAD_NAME_STR_SIZE (64)

static bool embkernel_detect_rtos(struct target *target);
static int embkernel_create(struct target *target);
static int embkernel_update_threads(struct rtos *rtos);
static int embkernel_get_thread_reg_list(struct rtos *rtos, int64_t thread_id,
		struct rtos_reg **reg_list, int *num_regs);
static int embkernel_get_symbol_list_to_lookup(struct symbol_table_elem *symbol_list[]);

const struct rtos_type embkernel_rtos = {
		.name = "embKernel",
		.detect_rtos = embkernel_detect_rtos,
		.create = embkernel_create,
		.update_threads = embkernel_update_threads,
		.get_thread_reg_list =
		embkernel_get_thread_reg_list,
		.get_symbol_list_to_lookup = embkernel_get_symbol_list_to_lookup,
};

enum {
	SYMBOL_ID_S_CURRENT_TASK = 0,
	SYMBOL_ID_S_LIST_READY = 1,
	SYMBOL_ID_S_LIST_SLEEP = 2,
	SYMBOL_ID_S_LIST_SUSPENDED = 3,
	SYMBOL_ID_S_MAX_PRIORITIES = 4,
	SYMBOL_ID_S_CURRENT_TASK_COUNT = 5,
};

static const char * const embkernel_symbol_list[] = {
		"Rtos::sCurrentTask",
		"Rtos::sListReady",
		"Rtos::sListSleep",
		"Rtos::sListSuspended",
		"Rtos::sMaxPriorities",
		"Rtos::sCurrentTaskCount",
		NULL };

struct embkernel_params {
	const char *target_name;
	const unsigned char pointer_width;
	const unsigned char thread_count_width;
	const unsigned char rtos_list_size;
	const unsigned char thread_stack_offset;
	const unsigned char thread_name_offset;
	const unsigned char thread_priority_offset;
	const unsigned char thread_priority_width;
	const unsigned char iterable_next_offset;
	const unsigned char iterable_task_owner_offset;
	const struct rtos_register_stacking *stacking_info;
};

static const struct embkernel_params embkernel_params_list[] = {
		{
			"cortex_m", /* target_name */
			4, /* pointer_width */
			4, /* thread_count_width */
			8, /*rtos_list_size */
			0, /*thread_stack_offset */
			4, /*thread_name_offset */
			8, /*thread_priority_offset */
			4, /*thread_priority_width */
			4, /*iterable_next_offset */
			12, /*iterable_task_owner_offset */
			&rtos_embkernel_cortex_m_stacking, /* stacking_info*/
		},
		{ "hla_target", /* target_name */
			4, /* pointer_width */
			4, /* thread_count_width */
			8, /*rtos_list_size */
			0, /*thread_stack_offset */
			4, /*thread_name_offset */
			8, /*thread_priority_offset */
			4, /*thread_priority_width */
			4, /*iterable_next_offset */
			12, /*iterable_task_owner_offset */
			&rtos_embkernel_cortex_m_stacking, /* stacking_info */
		}
};

static bool embkernel_detect_rtos(struct target *target)
{
	if (target->rtos->symbols) {
		if (target->rtos->symbols[SYMBOL_ID_S_CURRENT_TASK].address != 0)
			return true;
	}
	return false;
}

static int embkernel_create(struct target *target)
{
	size_t i = 0;
	while ((i < ARRAY_SIZE(embkernel_params_list)) &&
			(strcmp(embkernel_params_list[i].target_name, target_type_name(target)) != 0))
		i++;

	if (i >= ARRAY_SIZE(embkernel_params_list)) {
		LOG_WARNING("Could not find target \"%s\" in embKernel compatibility "
				"list", target_type_name(target));
		return ERROR_FAIL;
	}

	target->rtos->rtos_specific_params = (void *) &embkernel_params_list[i];
	return ERROR_OK;
}

static int embkernel_get_tasks_details(struct rtos *rtos, int64_t iterable, const struct embkernel_params *param,
		struct thread_detail *details, const char *state_str)
{
	int64_t task = 0;
	int retval = target_read_buffer(rtos->target, iterable + param->iterable_task_owner_offset, param->pointer_width,
			(uint8_t *) &task);
	if (retval != ERROR_OK)
		return retval;
	details->threadid = (threadid_t) task;
	details->exists = true;

	int64_t name_ptr = 0;
	retval = target_read_buffer(rtos->target, task + param->thread_name_offset, param->pointer_width,
			(uint8_t *) &name_ptr);
	if (retval != ERROR_OK)
		return retval;

	details->thread_name_str = malloc(EMBKERNEL_MAX_THREAD_NAME_STR_SIZE);
	if (name_ptr) {
		retval = target_read_buffer(rtos->target, name_ptr, EMBKERNEL_MAX_THREAD_NAME_STR_SIZE,
				(uint8_t *) details->thread_name_str);
		if (retval != ERROR_OK)
			return retval;
		details->thread_name_str[EMBKERNEL_MAX_THREAD_NAME_STR_SIZE - 1] = 0;
	} else {
		snprintf(details->thread_name_str, EMBKERNEL_MAX_THREAD_NAME_STR_SIZE, "NoName:[0x%08X]", (unsigned int) task);
	}

	int64_t priority = 0;
	retval = target_read_buffer(rtos->target, task + param->thread_priority_offset, param->thread_priority_width,
			(uint8_t *) &priority);
	if (retval != ERROR_OK)
		return retval;
	details->extra_info_str = malloc(EMBKERNEL_MAX_THREAD_NAME_STR_SIZE);
	if (task == rtos->current_thread) {
		snprintf(details->extra_info_str, EMBKERNEL_MAX_THREAD_NAME_STR_SIZE, "State: Running, Priority: %u",
				(unsigned int) priority);
	} else {
		snprintf(details->extra_info_str, EMBKERNEL_MAX_THREAD_NAME_STR_SIZE, "State: %s, Priority: %u",
				state_str, (unsigned int) priority);
	}

	LOG_OUTPUT("Getting task details: iterable=0x%08X, task=0x%08X, name=%s\n", (unsigned int)iterable,
			(unsigned int)task, details->thread_name_str);
	return 0;
}

static int embkernel_update_threads(struct rtos *rtos)
{
	/* int i = 0; */
	int retval;
	const struct embkernel_params *param;

	if (!rtos)
		return -1;

	if (!rtos->rtos_specific_params)
		return -3;

	if (!rtos->symbols) {
		LOG_ERROR("No symbols for embKernel");
		return -4;
	}

	if (rtos->symbols[SYMBOL_ID_S_CURRENT_TASK].address == 0) {
		LOG_ERROR("Don't have the thread list head");
		return -2;
	}

	/* wipe out previous thread details if any */
	rtos_free_threadlist(rtos);

	param = (const struct embkernel_params *) rtos->rtos_specific_params;

	retval = target_read_buffer(rtos->target, rtos->symbols[SYMBOL_ID_S_CURRENT_TASK].address, param->pointer_width,
			(uint8_t *) &rtos->current_thread);
	if (retval != ERROR_OK) {
		LOG_ERROR("Error reading current thread in embKernel thread list");
		return retval;
	}

	int64_t max_used_priority = 0;
	retval = target_read_buffer(rtos->target, rtos->symbols[SYMBOL_ID_S_MAX_PRIORITIES].address, param->pointer_width,
			(uint8_t *) &max_used_priority);
	if (retval != ERROR_OK)
		return retval;

	int thread_list_size = 0;
	retval = target_read_buffer(rtos->target, rtos->symbols[SYMBOL_ID_S_CURRENT_TASK_COUNT].address,
			param->thread_count_width, (uint8_t *) &thread_list_size);

	if (retval != ERROR_OK) {
		LOG_ERROR("Could not read embKernel thread count from target");
		return retval;
	}

	/* create space for new thread details */
	rtos->thread_details = malloc(sizeof(struct thread_detail) * thread_list_size);
	if (!rtos->thread_details) {
		LOG_ERROR("Error allocating memory for %d threads", thread_list_size);
		return ERROR_FAIL;
	}

	int thread_idx = 0;
	/* Look for ready tasks */
	for (int pri = 0; pri < max_used_priority; pri++) {
		/* Get first item in queue */
		int64_t iterable = 0;
		retval = target_read_buffer(rtos->target,
				rtos->symbols[SYMBOL_ID_S_LIST_READY].address + (pri * param->rtos_list_size), param->pointer_width,
				(uint8_t *) &iterable);
		if (retval != ERROR_OK)
			return retval;
		for (; iterable && thread_idx < thread_list_size; thread_idx++) {
			/* Get info from this iterable item */
			retval = embkernel_get_tasks_details(rtos, iterable, param, &rtos->thread_details[thread_idx], "Ready");
			if (retval != ERROR_OK)
				return retval;
			/* Get next iterable item */
			retval = target_read_buffer(rtos->target, iterable + param->iterable_next_offset, param->pointer_width,
					(uint8_t *) &iterable);
			if (retval != ERROR_OK)
				return retval;
		}
	}
	/* Look for sleeping tasks */
	int64_t iterable = 0;
	retval = target_read_buffer(rtos->target, rtos->symbols[SYMBOL_ID_S_LIST_SLEEP].address, param->pointer_width,
			(uint8_t *) &iterable);
	if (retval != ERROR_OK)
		return retval;
	for (; iterable && thread_idx < thread_list_size; thread_idx++) {
		/*Get info from this iterable item */
		retval = embkernel_get_tasks_details(rtos, iterable, param, &rtos->thread_details[thread_idx], "Sleeping");
		if (retval != ERROR_OK)
			return retval;
		/*Get next iterable item */
		retval = target_read_buffer(rtos->target, iterable + param->iterable_next_offset, param->pointer_width,
				(uint8_t *) &iterable);
		if (retval != ERROR_OK)
			return retval;
	}

	/* Look for suspended tasks  */
	iterable = 0;
	retval = target_read_buffer(rtos->target, rtos->symbols[SYMBOL_ID_S_LIST_SUSPENDED].address, param->pointer_width,
			(uint8_t *) &iterable);
	if (retval != ERROR_OK)
		return retval;
	for (; iterable && thread_idx < thread_list_size; thread_idx++) {
		/* Get info from this iterable item */
		retval = embkernel_get_tasks_details(rtos, iterable, param, &rtos->thread_details[thread_idx], "Suspended");
		if (retval != ERROR_OK)
			return retval;
		/*Get next iterable item */
		retval = target_read_buffer(rtos->target, iterable + param->iterable_next_offset, param->pointer_width,
				(uint8_t *) &iterable);
		if (retval != ERROR_OK)
			return retval;
	}

	rtos->thread_count = 0;
	rtos->thread_count = thread_idx;
	LOG_OUTPUT("Found %u tasks\n", (unsigned int)thread_idx);
	return 0;
}

static int embkernel_get_thread_reg_list(struct rtos *rtos, int64_t thread_id,
		struct rtos_reg **reg_list, int *num_regs)
{
	int retval;
	const struct embkernel_params *param;
	int64_t stack_ptr = 0;

	if (!rtos)
		return -1;

	if (thread_id == 0)
		return -2;

	if (!rtos->rtos_specific_params)
		return -1;

	param = (const struct embkernel_params *) rtos->rtos_specific_params;

	/* Read the stack pointer */
	retval = target_read_buffer(rtos->target, thread_id + param->thread_stack_offset, param->pointer_width,
			(uint8_t *) &stack_ptr);
	if (retval != ERROR_OK) {
		LOG_ERROR("Error reading stack frame from embKernel thread");
		return retval;
	}

	return rtos_generic_stack_read(rtos->target, param->stacking_info, stack_ptr, reg_list, num_regs);
}

static int embkernel_get_symbol_list_to_lookup(struct symbol_table_elem *symbol_list[])
{
	unsigned int i;
	*symbol_list = calloc(ARRAY_SIZE(embkernel_symbol_list), sizeof(struct symbol_table_elem));

	for (i = 0; i < ARRAY_SIZE(embkernel_symbol_list); i++)
		(*symbol_list)[i].symbol_name = embkernel_symbol_list[i];

	return 0;
}
