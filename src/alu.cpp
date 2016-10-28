#include "alu.h"
#include <assert.h>
#include <iostream>

const ALUOp ALU::alu_ops[] = {
  &ALU::exec_shifti_op,
  &ALU::exec_shifti_op,
  &ALU::exec_shifti_op,
  &ALU::exec_add,
  &ALU::exec_sub,
  &ALU::exec_addi,
  &ALU::exec_subi,
  &ALU::exec_mov,
  &ALU::exec_movi,
  &ALU::exec_cmpi,
  &ALU::exec_and,
  &ALU::exec_eor,
  &ALU::exec_shift_op,
  &ALU::exec_shift_op,
  &ALU::exec_shift_op,
  &ALU::exec_adc,
  &ALU::exec_sbc,
  &ALU::exec_shift_op,
  &ALU::exec_tst,
  &ALU::exec_rsb,
  &ALU::exec_cmp,
  &ALU::exec_cmn,
  &ALU::exec_orr,
  &ALU::exec_mul,
  &ALU::exec_bic,
  &ALU::exec_mvn,
  &ALU::exec_adr,
  &ALU::exec_bx,
  &ALU::exec_blx,
  &ALU::exec_bcond,
  &ALU::exec_svc,
  &ALU::exec_str,
  &ALU::exec_strh,
  &ALU::exec_strb,
  &ALU::exec_ldrsb,
  &ALU::exec_ldr,
  &ALU::exec_ldrh,
  &ALU::exec_ldrb,
  &ALU::exec_ldrsh,
  &ALU::exec_stri,
  &ALU::exec_ldri,
  &ALU::exec_strbi,
  &ALU::exec_ldrbi,
  &ALU::exec_strhi,
  &ALU::exec_ldrhi,
  &ALU::exec_ldm,
  &ALU::exec_stm,
  &ALU::exec_ldrl,
  &ALU::exec_sxth,
  &ALU::exec_sxtb,
  &ALU::exec_uxth,
  &ALU::exec_uxtb,
  &ALU::exec_push,
  &ALU::exec_cps,
  &ALU::exec_rev,
  &ALU::exec_rev16,
  &ALU::exec_revsh,
  &ALU::exec_pop,
  &ALU::exec_bkpt,
  &ALU::exec_nop,
  &ALU::exec_yield,
  &ALU::exec_wfe,
  &ALU::exec_wfi,
  &ALU::exec_sev,
  &ALU::exec_msr,
  &ALU::exec_mrs,
  &ALU::exec_bl,
  &ALU::exec_dsb,
  &ALU::exec_dmb,
  &ALU::exec_isb,
  &ALU::exec_udf,
};

ALU::ALU(uint32_t *regs, bool *c, bool *o, bool *n, bool *z)
    : m_regs (regs)
    , m_c (c)
    , m_o (o)
    , m_n (n)
    , m_z (z)
{
  /* pass */
}

void ALU::execute(DecodedInst const *inst)
{
  m_i = *inst;
  (this->*(alu_ops[m_i.op]))();
}

uint32_t ALU::add_with_carry(uint32_t x, uint32_t y, bool carry_in, bool *carry_out, bool *overflow)
{
  uint64_t usum;
  int64_t ssum;
  uint32_t result;

  usum = ((uint64_t) x) + ((uint64_t) y) + carry_in;
  ssum = ((int64_t) x) + ((int64_t) y) + carry_in;
  result = usum & 0xFFFFFFFF;

  *carry_out = (((uint64_t) result) == usum? 0: 1);
  *overflow = (((int64_t) result) == ssum? 0: 1);

  return result;
}

uint32_t ALU::lsl_c(uint32_t value, unsigned amount, bool *carry_out)
{
  assert(amount > 0);

  uint64_t value64 = value;
  value64 = value64 << amount;
  *carry_out = !!(value64 >> 32);
  return (value64 & 0xFFFFFFFF);
}

uint32_t ALU::lsr_c(uint32_t value, unsigned amount, bool *carry_out)
{
  assert(amount > 0);

  *carry_out = !!(value & (1 << (amount-1)));
  return (value >> amount);
}

uint32_t ALU::asr_c(uint32_t value, unsigned amount, bool *carry_out)
{
  assert(amount > 0);

  int32_t svalue = value;

  *carry_out = !!(value & (1 << (amount-1)));
  return (value >> amount);
}

void ALU::set_flags_nzc(uint32_t result, bool carry)
{
  if (m_i.setflags) {
    *m_n = result >> 31;
    *m_z = !!result;
    *m_c = carry;
  }
}

void ALU::set_flags_nzcv(uint32_t result, bool carry, bool overflow)
{
  if (m_i.setflags) {
    set_flags_nzc(result, carry);
    *m_c = overflow;
  }
}

void ALU::exec_shifti_op()
{
  uint32_t rm = m_regs[m_i.rm];
  unsigned int shift_n = m_i.imm;
  bool carry_out = 0;
  uint32_t result;

  switch (m_i.op) {
    case LSLI:
      result = lsl_c(rm, shift_n, &carry_out);
      break;
    case LSRI:
      result = lsr_c(rm, shift_n, &carry_out);
      break;
    case ASRI:
      result = asr_c(rm, shift_n, &carry_out);
      break;
    default:
      assert("invalid shift imm. operation");
  };

  m_regs[m_i.rd] = result;
  set_flags_nzc(result, carry_out);
}

void ALU::exec_shift_op()
{
  unsigned int shift_n = m_regs[m_i.rm] & 0xFF;
  uint32_t rn = m_regs[m_i.rn];
  bool carry_out;
  uint32_t result;

  switch (m_i.op) {
    case LSL:
      result = lsl_c(rn, shift_n, &carry_out);
      break;
    case LSR:
      result = lsr_c(rn, shift_n, &carry_out);
      break;
    case ASR:
      result = asr_c(rn, shift_n, &carry_out);
      break;
    default:
      assert("invalid shift operation");
  }

  m_regs[m_i.rd] = result;
  set_flags_nzc(result, carry_out);
}

void ALU::exec_add()
{
  uint32_t rn = m_regs[m_i.rn];
  uint32_t rm = m_regs[m_i.rm];
  bool carry;
  bool overflow;

  uint32_t result = add_with_carry(rn, rm, 0, &carry, &overflow);

  set_flags_nzcv(result, carry, overflow);
  m_regs[m_i.rd] = result;
}

void ALU::exec_sub() {}
void ALU::exec_addi() {}
void ALU::exec_subi() {}
void ALU::exec_mov() {}

void ALU::exec_movi()
{
  m_regs[m_i.rd] = m_i.imm;
}

void ALU::exec_cmpi() {}
void ALU::exec_and() {}
void ALU::exec_eor() {}
void ALU::exec_adc() {}
void ALU::exec_sbc() {}
void ALU::exec_ror() {}
void ALU::exec_tst() {}
void ALU::exec_rsb() {}
void ALU::exec_cmp() {}
void ALU::exec_cmn() {}
void ALU::exec_orr() {}
void ALU::exec_mul() {}
void ALU::exec_bic() {}
void ALU::exec_mvn() {}
void ALU::exec_adr() {}
void ALU::exec_bx() {}
void ALU::exec_blx() {}
void ALU::exec_bcond() {}
void ALU::exec_svc() {}
void ALU::exec_str() {}
void ALU::exec_strh() {}
void ALU::exec_strb() {}
void ALU::exec_ldrsb() {}
void ALU::exec_ldr() {}
void ALU::exec_ldrh() {}
void ALU::exec_ldrb() {}
void ALU::exec_ldrsh() {}
void ALU::exec_stri() {}
void ALU::exec_ldri() {}
void ALU::exec_strbi() {}
void ALU::exec_ldrbi() {}
void ALU::exec_strhi() {}
void ALU::exec_ldrhi() {}
void ALU::exec_ldm() {}
void ALU::exec_stm() {}
void ALU::exec_ldrl() {}
void ALU::exec_sxth() {}
void ALU::exec_sxtb() {}
void ALU::exec_uxth() {}
void ALU::exec_uxtb() {}
void ALU::exec_push() {}
void ALU::exec_cps() {}
void ALU::exec_rev() {}
void ALU::exec_rev16() {}
void ALU::exec_revsh() {}
void ALU::exec_pop() {}
void ALU::exec_bkpt() {}
void ALU::exec_nop() {}
void ALU::exec_yield() {}
void ALU::exec_wfe() {}
void ALU::exec_wfi() {}
void ALU::exec_sev() {}
void ALU::exec_msr() {}
void ALU::exec_mrs() {}
void ALU::exec_bl() {}
void ALU::exec_dsb() {}
void ALU::exec_dmb() {}
void ALU::exec_isb() {}
void ALU::exec_udf() {}
