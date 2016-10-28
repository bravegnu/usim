// -*- mode: c++ -*-

#ifndef ALU_H
#define ALU_H

#include "decoder.h"
#include <cstdint>

class ALU;

typedef void (ALU::*ALUOp)();

enum class ShiftType {
  LSL,
  LSR,
  ASR,
  RRX,
  ROR,
};

class ALU
{
 private:
  uint32_t *m_regs;
  bool *m_c;
  bool *m_o;
  bool *m_n;
  bool *m_z;
  DecodedInst m_i;
  static const ALUOp alu_ops[OP_MAX];

  uint32_t add_with_carry(uint32_t x, uint32_t y, bool carry_in, bool *carry_out, bool *overflow);
  void set_flags_nzcv(uint32_t result, bool carry, bool overflow);
  void set_flags_nzc(uint32_t result, bool carry);
  uint32_t lsl_c(uint32_t value, unsigned amount, bool *carry_out);
  uint32_t lsr_c(uint32_t value, unsigned amount, bool *carry_out);
  uint32_t asr_c(uint32_t value, unsigned amount, bool *carry_out);

  void exec_shifti_op();
  void exec_add();
  void exec_sub();
  void exec_addi();
  void exec_subi();
  void exec_movi();
  void exec_mov();
  void exec_cmpi();
  void exec_and();
  void exec_eor();
  void exec_shift_op();  
  void exec_adc();
  void exec_sbc();
  void exec_ror();
  void exec_tst();
  void exec_rsb();
  void exec_cmp();
  void exec_cmn();
  void exec_orr();
  void exec_mul();
  void exec_bic();
  void exec_mvn();
  void exec_adr();
  void exec_bx();
  void exec_blx();
  void exec_bcond();
  void exec_svc();
  void exec_str();
  void exec_strh();
  void exec_strb();
  void exec_ldrsb();
  void exec_ldr();
  void exec_ldrh();
  void exec_ldrb();
  void exec_ldrsh();
  void exec_stri();
  void exec_ldri();
  void exec_strbi();
  void exec_ldrbi();
  void exec_strhi();
  void exec_ldrhi();
  void exec_ldm();
  void exec_stm();
  void exec_ldrl();
  void exec_sxth();
  void exec_sxtb();
  void exec_uxth();
  void exec_uxtb();
  void exec_push();
  void exec_cps();
  void exec_rev();
  void exec_rev16();
  void exec_revsh();
  void exec_pop();
  void exec_bkpt();
  void exec_nop();
  void exec_yield();
  void exec_wfe();
  void exec_wfi();
  void exec_sev();
  void exec_msr();
  void exec_mrs();
  void exec_bl();
  void exec_dsb();
  void exec_dmb();
  void exec_isb();
  void exec_udf();
  
 public:
  ALU(uint32_t *regs, bool *c, bool *o, bool *n, bool *z);
  void execute(DecodedInst const *inst);
};

#endif
