/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Copyright (C) 2015 by pierrr kuo <vichy.kuo@gmail.com>
 */

#ifndef OPENOCD_TARGET_ARMV8_OPCODES_H
#define OPENOCD_TARGET_ARMV8_OPCODES_H

#include "arm_opcodes.h"

#define SYSTEM_CUREL_MASK		0xC0
#define SYSTEM_CUREL_SHIFT		6
#define SYSTEM_CUREL_EL0		0x0
#define SYSTEM_CUREL_EL1		0x1
#define SYSTEM_CUREL_EL2		0x2
#define SYSTEM_CUREL_EL3		0x3
#define SYSTEM_CUREL_NONCH		0xF
#define SYSTEM_AARCH64			0x1

#define SYSTEM_AAR64_MODE_EL0T	0x0
#define SYSTEM_AAR64_MODE_EL1T	0x4
#define SYSTEM_AAR64_MODE_EL1H	0x5
#define SYSTEM_AAR64_MODE_EL2T	0x8
#define SYSTEM_AAR64_MODE_EL2H	0x9
#define SYSTEM_AAR64_MODE_EL3T	0xC
#define SYSTEM_AAR64_MODE_EL3H	0xd

#define SYSTEM_DAIF			0xDA11
#define SYSTEM_DAIF_MASK		0x3C0
#define SYSTEM_DAIF_SHIFT		6

#define SYSTEM_ELR_EL1			0xC201
#define SYSTEM_ELR_EL2			0xE201
#define SYSTEM_ELR_EL3			0xF201

#define SYSTEM_SCTLR_EL1	0xC080
#define SYSTEM_SCTLR_EL2	0xE080
#define SYSTEM_SCTLR_EL3	0xF080

#define SYSTEM_FPCR		0xDA20
#define SYSTEM_FPSR		0xDA21
#define SYSTEM_DAIF		0xDA11
#define SYSTEM_NZCV		0xDA10
#define SYSTEM_SP_EL0		0xC208
#define SYSTEM_SP_EL1		0xE208
#define SYSTEM_SP_EL2		0xF208
#define SYSTEM_SP_SEL		0xC210
#define SYSTEM_SPSR_ABT		0xE219
#define SYSTEM_SPSR_FIQ		0xE21B
#define SYSTEM_SPSR_IRQ		0xE218
#define SYSTEM_SPSR_UND		0xE21A

#define SYSTEM_SPSR_EL1			0xC200
#define SYSTEM_SPSR_EL2			0xE200
#define SYSTEM_SPSR_EL3			0xF200

#define SYSTEM_ISR_EL1			0xC608

#define SYSTEM_DBG_DSPSR_EL0    0xDA28
#define SYSTEM_DBG_DLR_EL0		0xDA29
#define SYSTEM_DBG_DTRRX_EL0	0x9828
#define SYSTEM_DBG_DTRTX_EL0	0x9828
#define SYSTEM_DBG_DBGDTR_EL0	0x9820

#define SYSTEM_CCSIDR			0xC800
#define SYSTEM_CLIDR			0xC801
#define SYSTEM_CSSELR			0xD000
#define SYSTEM_CTYPE			0xD801
#define SYSTEM_CTR			0xD801

#define SYSTEM_DCCISW			0x43F2
#define SYSTEM_DCCSW			0x43D2
#define SYSTEM_ICIVAU			0x5BA9
/* Attention, SYSTEM_ICIALLU requires rt=0x1f */
#define SYSTEM_ICIALLU			0x03A8
#define SYSTEM_DCCVAU			0x5BD9
#define SYSTEM_DCCIVAC			0x5BF1

#define SYSTEM_MPIDR			0xC005

#define SYSTEM_TCR_EL1			0xC102
#define SYSTEM_TCR_EL2			0xE102
#define SYSTEM_TCR_EL3			0xF102

#define SYSTEM_TTBR0_EL1		0xC100
#define SYSTEM_TTBR0_EL2		0xE100
#define SYSTEM_TTBR0_EL3		0xF100
#define SYSTEM_TTBR1_EL1		0xC101

/* ARMv8 address translation */
#define SYSTEM_PAR_EL1			0xC3A0
#define SYSTEM_ATS12E0R			0x63C6
#define SYSTEM_ATS12E1R			0x63C4
#define SYSTEM_ATS1E2R			0x63C0
#define SYSTEM_ATS1E3R			0x73C0

/* fault status and fault address */
#define SYSTEM_FAR_EL1			0xC300
#define SYSTEM_FAR_EL2			0xE300
#define SYSTEM_FAR_EL3			0xF300
#define SYSTEM_ESR_EL1			0xC290
#define SYSTEM_ESR_EL2			0xE290
#define SYSTEM_ESR_EL3			0xF290

#define ARMV8_MRS_DSPSR(rt)	(0xd53b4500 | (rt))
#define ARMV8_MSR_DSPSR(rt)	(0xd51b4500 | (rt))
#define ARMV8_MRS_DLR(rt)	(0xd53b4520 | (rt))
#define ARMV8_MSR_DLR(rt)	(0xd51b4520 | (rt))

/* T32 instruction to access coprocessor registers */
#define ARMV8_MCR_T1(cp, crn, opc1, crm, opc2, rt) ARMV4_5_MCR(cp, opc1, rt, crn, crm, opc2)
#define ARMV8_MRC_T1(cp, crn, opc1, crm, opc2, rt) ARMV4_5_MRC(cp, opc1, rt, crn, crm, opc2)

/* T32 instructions to access DSPSR and DLR */
#define ARMV8_MRC_DSPSR(rt) ARMV8_MRC_T1(15, 4, 3, 5, 0, rt)
#define ARMV8_MCR_DSPSR(rt) ARMV8_MCR_T1(15, 4, 3, 5, 0, rt)
#define ARMV8_MRC_DLR(rt)	ARMV8_MRC_T1(15, 4, 3, 5, 1, rt)
#define ARMV8_MCR_DLR(rt)	ARMV8_MCR_T1(15, 4, 3, 5, 1, rt)

#define ARMV8_DCPS1(im)		(0xd4a00001 | (((im) & 0xFFFF) << 5))
#define ARMV8_DCPS2(im)		(0xd4a00002 | (((im) & 0xFFFF) << 5))
#define ARMV8_DCPS3(im)		(0xd4a00003 | (((im) & 0xFFFF) << 5))
#define ARMV8_DCPS(el, im)	(0xd4a00000 | (((im) & 0xFFFF) << 5) | el)
#define ARMV8_DCPS_T1(el)	(0xf78f8000 | el)
#define ARMV8_DRPS		0xd6bf03e0
#define ARMV8_ERET_T1		0xf3de8f00

#define ARMV8_DSB_SY				0xd5033F9F
#define ARMV8_DSB_SY_T1				0xf3bf8f4f
#define ARMV8_ISB				0xd5033fdf
#define ARMV8_ISB_SY_T1				0xf3bf8f6f

#define ARMV8_MRS(system, rt)	(0xd5300000 | ((system) << 5) | (rt))
/* ARM V8 Move to system register. */
#define ARMV8_MSR_GP(system, rt) \
	(0xd5100000 | ((system) << 5) | (rt))
/* ARM V8 Move immediate to process state field. */
#define ARMV8_MSR_IM(op1, crm, op2) \
	(0xd500401f | ((op1) << 16)  | ((crm) << 8) | ((op2) << 5))

#define ARMV8_MRS_T1(r, m1, rd, m) (0xF3E08020 | (r << 20) | (m1 << 16) | (rd << 8) | (m << 4))
#define ARMV8_MRS_XPSR_T1(r, rd) (0xF3EF8000 | (r << 20) | (rd << 8))
#define ARMV8_MSR_GP_T1(r, m1, rd, m) (0xF3808020 | (r << 20) | (m1 << 8) | (rd << 16) | (m << 4))
#define ARMV8_MSR_GP_XPSR_T1(r, rn, mask) (0xF3808000 | (r << 20) | (rn << 16) | (mask << 8))

#define ARMV8_BKPT(im) (0xD4200000 | ((im & 0xffff) << 5))
#define ARMV8_HLT(im) (0x0D4400000 | ((im & 0xffff) << 5))
#define ARMV8_HLT_A1(im) (0xE1000070 | ((im & 0xFFF0) << 4) | (im & 0xF))
#define ARMV8_HLT_T1(im) (0xba80 | (im & 0x3f))

#define ARMV8_MOVFSP_64(rt) ((1 << 31) | 0x11000000 | (0x1f << 5) | (rt))
#define ARMV8_MOVTSP_64(rt) ((1 << 31) | 0x11000000 | (rt << 5) | (0x1F))
#define ARMV8_MOVFSP_32(rt) (0x11000000 | (0x1f << 5) | (rt))
#define ARMV8_MOVTSP_32(rt) (0x11000000 | (rt << 5) | (0x1F))

#define ARMV8_LDRB_IP(rd, rn) (0x38401400 | (rn << 5) | rd)
#define ARMV8_LDRH_IP(rd, rn) (0x78402400 | (rn << 5) | rd)
#define ARMV8_LDRW_IP(rd, rn) (0xb8404400 | (rn << 5) | rd)
#define ARMV8_LDRD_IP(rd, rn) (0xf8408400 | (rn << 5) | rd)

#define ARMV8_LDRB_IP_T3(rd, rn) (0xf8100b01 | (rn << 16) | (rd << 12))
#define ARMV8_LDRH_IP_T3(rd, rn) (0xf8300b02 | (rn << 16) | (rd << 12))
#define ARMV8_LDRW_IP_T3(rd, rn) (0xf8500b04 | (rn << 16) | (rd << 12))

#define ARMV8_STRB_IP(rd, rn) (0x38001400 | (rn << 5) | rd)
#define ARMV8_STRH_IP(rd, rn) (0x78002400 | (rn << 5) | rd)
#define ARMV8_STRW_IP(rd, rn) (0xb8004400 | (rn << 5) | rd)
#define ARMV8_STRD_IP(rd, rn) (0xf8008400 | (rn << 5) | rd)

#define ARMV8_STRB_IP_T3(rd, rn) (0xf8000b01 | (rn << 16) | (rd << 12))
#define ARMV8_STRH_IP_T3(rd, rn) (0xf8200b02 | (rn << 16) | (rd << 12))
#define ARMV8_STRW_IP_T3(rd, rn) (0xf8400b04 | (rn << 16) | (rd << 12))

#define ARMV8_MOV_GPR_VFP(rd, rn, index) (0x4e083c00 | (index << 20) | (rn << 5) | rd)
#define ARMV8_MOV_VFP_GPR(rd, rn, index) (0x4e081c00 | (index << 20) | (rn << 5) | rd)

#define ARMV8_MRS_FPCR(rt)	(0xd53b4400 | (rt))
#define ARMV8_MRS_FPSR(rt)	(0xd53b4420 | (rt))
#define ARMV8_MSR_FPCR(rt)	(0xd51b4400 | (rt))
#define ARMV8_MSR_FPSR(rt)	(0xd51b4420 | (rt))

#define ARMV8_SYS(system, rt) (0xD5080000 | ((system) << 5) | rt)

enum armv8_opcode {
	READ_REG_CTR,
	READ_REG_CLIDR,
	READ_REG_CSSELR,
	READ_REG_CCSIDR,
	WRITE_REG_CSSELR,
	READ_REG_MPIDR,
	READ_REG_DTRRX,
	WRITE_REG_DTRTX,
	WRITE_REG_DSPSR,
	READ_REG_DSPSR,
	ARMV8_OPC_DSB_SY,
	ARMV8_OPC_DCPS,
	ARMV8_OPC_DRPS,
	ARMV8_OPC_ISB_SY,
	ARMV8_OPC_DCCISW,
	ARMV8_OPC_DCCIVAC,
	ARMV8_OPC_ICIVAU,
	ARMV8_OPC_HLT,
	ARMV8_OPC_STRB_IP,
	ARMV8_OPC_STRH_IP,
	ARMV8_OPC_STRW_IP,
	ARMV8_OPC_STRD_IP,
	ARMV8_OPC_LDRB_IP,
	ARMV8_OPC_LDRH_IP,
	ARMV8_OPC_LDRW_IP,
	ARMV8_OPC_LDRD_IP,
	ARMV8_OPC_ICIALLU,
	ARMV8_OPC_NUM,
};

extern uint32_t armv8_opcode(struct armv8_common *armv8, enum armv8_opcode);
extern void armv8_select_opcodes(struct armv8_common *armv8, bool state_is_aarch64);

#endif /* OPENOCD_TARGET_ARMV8_OPCODES_H */
