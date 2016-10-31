#ifndef DECODER_H
#define DECODER_H

#include <cstdint>

enum Cond {
  COND_EQ,
  COND_NE,
  COND_CS,
  COND_CC,
  COND_MI,
  COND_PL,
  COND_VS,
  COND_VC,
  COND_HI,
  COND_LS,
  COND_GE,
  COND_LT,
  COND_GT,
  COND_LE,
  COND_AL,
  COND_UNDEF,
};

enum Op {
  /* Data Processing */
  LSLI, LSRI, ASRI, ADD, SUB, ADDI, SUBI, MOV, MOVI, CMPI,
  AND, EOR, LSL, LSR, ASR, ADC, SBC, ROR, TST, RSBI, CMP, CMN,
  ORR, MUL, BIC, MVN,

  /* Relative Addressing */
  ADR,

  /* Branching */
  BX, BLX, BCOND, SVC,

  /* Load Store */
  STR, STRH, STRB, LDRSB, LDR, LDRH, LDRB, LDRSH, STRI, LDRI, STRBI,
  LDRBI, STRHI, LDRHI, LDM, STM, LDRL,

  /* Misc. */
  SXTH, SXTB, UXTH, UXTB, PUSH, CPS, REV, REV16, REVSH, POP, BKPT,

  /* Hint */
  NOP, YIELD, WFE, WFI, SEV,

  /* Branch and Control */
  MSR, MRS, BL, DSB, DMB, ISB,

  /* Undefined */
  UDF,

  OP_MAX
};

#define DECODE_TAB_SIZE 77
#define DECODE32_TAB_SIZE 7

struct DecodedInst {
  uint8_t op;
  uint8_t rd;
  uint8_t rn;
  uint8_t rm;
  int32_t imm;
  uint16_t specific;
  bool setflags;
};

class Decoder;

struct DecoderTab {
  uint16_t mask;
  uint16_t match;
  void (Decoder::*decode)();
};

struct Decoder32Tab {
  uint32_t mask;
  uint32_t match;
  void (Decoder::*decode)();
};

class Decoder {
private:
  uint32_t m_inst;
  bool m_is32;
  DecodedInst m_di;
  static const DecoderTab m_decode_table[DECODE_TAB_SIZE];
  static const Decoder32Tab m_decode32_table[DECODE32_TAB_SIZE];

  void decode_lsli();
  void decode_lsri();
  void decode_asri();
  void decode_add_t1();
  void decode_sub();
  void decode_addi_t1();
  void decode_subi_t1();
  void decode_movi();
  void decode_cmpi();
  void decode_addi_t2();
  void decode_subi_t2();
  void decode_dproc();
  void decode_and();
  void decode_eor();
  void decode_lsl();
  void decode_lsr();
  void decode_asr();
  void decode_adc();
  void decode_sbc();
  void decode_ror();
  void decode_tst();
  void decode_rsbi();
  void decode_cmp();
  void decode_cmn();
  void decode_orr();
  void decode_mul();
  void decode_bic();
  void decode_mvn();
  void decode_add_t2();
  void decode_cmp_t1();
  void decode_cmp_t2();
  void decode_mov_t1();
  void decode_mov_t2();
  void decode_bx();
  void decode_blx();
  void decode_ldrl();
  void decode_load_store();
  void decode_str();
  void decode_ldr();
  void decode_strh();
  void decode_strb();
  void decode_ldrsb();
  void decode_ldrh();
  void decode_ldrb();
  void decode_ldrsh();
  void decode_load_store_imm();
  void decode_ldri_t2();
  void decode_stri_t2();
  void decode_ldrbi();
  void decode_strbi();
  void decode_ldrhi();
  void decode_strhi();
  void decode_ldri_t1();
  void decode_stri_t1();
  void decode_adr();
  void decode_add_sp_t1();
  void decode_add_sp_t2();
  void decode_addi_sp_t1();
  void decode_addi_sp_t2();
  void decode_subi_sp();
  void decode_sxth();
  void decode_sxtb();
  void decode_uxth();
  void decode_uxtb();
  void decode_push();
  void decode_cps();
  void decode_rev();
  void decode_rev16();
  void decode_revsh();
  void decode_pop();
  void decode_bkpt();
  void decode_nop();
  void decode_yield();
  void decode_wfe();
  void decode_wfi();
  void decode_sev();
  void decode_stm();
  void decode_ldm();
  void decode_udf();
  void decode_b_t1();
  void decode_b_t2();
  void decode_svc();
  void decode_inst();

  void decode_bl();
  void decode_msr();
  void decode_dsb();
  void decode_dmb();
  void decode_isb();
  void decode_mrs();
  void decode_udf32();
  void decode_inst32();

  uint8_t REG(unsigned offset)
  {
    return (m_inst >> offset) & 0x7;
  }

  uint8_t REG4(unsigned offset)
  {
    return (m_inst >> offset) & 0xF;
  }

  uint8_t REG4M()
  {
    return (m_inst >> 3) & 0xF;
  }

  uint8_t REG4N()
  {
    return ((m_inst >> 4) & 0x8) | ((m_inst >> 0) & 0x7);
  }

  uint32_t IMM3()
  {
    return (m_inst >> 6) & 0x7;
  }

  uint32_t IMM5()
  {
    return (m_inst >> 6) & 0x1F;
  }

  uint32_t IMM7()
  {
    return (m_inst >> 0) & 0x7F;
  }

  uint32_t IMM8()
  {
    return (m_inst >> 0) & 0xFF;
  }

  uint32_t SIMM8()
  {
    return (int32_t (IMM8() << 24)) >> 24;
  }

  uint32_t SIMM11()
  {
    return (int32_t ((m_inst & 0x7FF) << 21)) >> 21;
  }

  uint16_t HI16()
  {
    return (m_inst >> 16) & 0xFF;
  }

  uint16_t LO16()
  {
    return (m_inst & 0xFF);
  }

public:
  Decoder();
  DecodedInst decode(uint32_t inst, bool is32);
};

#endif
