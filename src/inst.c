#include <stdio.h>
#include <assert.h>
#include "inst.h"
#include "utils.h"
#include "flash.h"
#include "avrdefs.h"
#include "opt.h"

static int is_32bit_inst(uint16_t inst) {
    return (inst & 0xfc0f) == 0x9000 || // lds, sts
           (inst & 0xfe0c) == 0x940c;   // jmp, call
}

static inline void set_sreg_add(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xc0;
    status |= (((a & b) | (~c & (a | b))) & 0x80) >> 7;     // carry
    status |= (c == 0) << 1;                                // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & b & ~c) | (~a & ~b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((a & b) | (~c & (a | b))) & 0x08) << 2;     // half carry
    avr->reg[avr->model.reg_status] = status;
}

static inline void set_sreg_sub(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xc0;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status |= (c == 0x00) << 1;                             // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->reg[avr->model.reg_status] = status;
}

static inline void set_sreg_subc(struct avr *avr, uint8_t a, uint8_t b, uint8_t c) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xc2;
    status |= ((~a & b) | (b & c) | (~a & c)) >> 7;         // carry/borrow
    status &= 0xfd | ((c == 0x00) << 1);                    // zero
    status |= (c & 0x80) >> 5;                              // negative
    status |= (((a & ~b & ~c) | (~a & b & c)) & 0x80) >> 4; // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    status |= (((~a & b) | (c & (~a | b))) & 0x08) << 2;    // half carry
    avr->reg[avr->model.reg_status] = status;
}

static inline void set_sreg_logical(struct avr *avr, uint8_t val) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xe1;
    status |= (val == 0) << 1;                              // zero
    status |= (val & 0x80) >> 5;                            // negative
    status |= (val & 0x80) >> 3;                            // sign
    avr->reg[avr->model.reg_status] = status;
}

static inline void set_sreg_mul(struct avr *avr, uint16_t val) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xfc;
    status |= (val & 0x8000) >> 15;                         // carry
    status |= (val == 0x00) << 1;                           // zero
    avr->reg[avr->model.reg_status] = status;
}

static inline void set_sreg_fmul(struct avr *avr, uint16_t val) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xfc;
    status |= (val & 0x8000) >> 15;                         // carry
    status |= (val == 0) << 1;                              // zero
    avr->reg[avr->model.reg_status] = status;
}

static inline void set_sreg_rshift(struct avr *avr, uint8_t before, uint8_t after) {
    uint8_t status = avr->reg[avr->model.reg_status] & 0xe0;
    status |= (before & 0x01);                              // carry
    status |= (after == 0) << 1;                            // zero
    status |= (after & 0x80) >> 5;                          // negative
    status |= ((after >> 4) ^ (before << 3)) & 0x08;        // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->reg[avr->model.reg_status] = status;
}

void inst_add(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf),
            a = avr->reg[dst],
            b = avr->reg[src],
            c = avr->reg[dst] + avr->reg[src];
    avr->reg[dst] = c;
    set_sreg_add(avr, a, b, c);
    avr->pc += 2;
}

void inst_adc(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0xf),
            a = avr->reg[dst],
            b = avr->reg[src],
            c = a + b + (avr->reg[avr->model.reg_status] & 0x01);
    avr->reg[dst] = c;
    set_sreg_add(avr, a, b, c);
    avr->pc += 2;
}

void inst_adiw(struct avr *avr, uint16_t inst) {
    uint8_t dst_l = ((inst >> 3) & 0x06) + 24,
            dst_h = dst_l + 1,
            imm = ((inst >> 2) & 0x30) | (inst & 0x0f),
            val_l = avr->reg[dst_l],
            val_h = avr->reg[dst_h],
            status = avr->reg[avr->model.reg_status] & 0xe0;
    uint16_t sum = ((uint16_t) val_h << 8) + (uint16_t) val_l + imm;
    avr->reg[dst_l] = sum & 0xff;
    avr->reg[dst_h] = sum >> 8;
    status |= (~(sum >> 8) & val_h) >> 7;                   // carry
    status |= (sum == 0) << 1;                              // zero
    status |= (sum >> 13) & 0x40;                           // negative
    status |= (((sum >> 8) & ~val_h) & 0x80) >> 4;          // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->reg[avr->model.reg_status] = status;
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_sub(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->reg[dst],
            b = avr->reg[src],
            c = a - b;
    avr->reg[dst] = c;
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_subi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->reg[dst],
            diff = val - imm;
    avr->reg[dst] = diff;
    set_sreg_sub(avr, val, imm, diff);
    avr->pc += 2;
}

void inst_sbc(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->reg[dst],
            b = avr->reg[src],
            c = a - b - (avr->reg[avr->model.reg_status] & 0x01);
    avr->reg[dst] = c;
    set_sreg_subc(avr, a, b, c);
    avr->pc += 2;
}

void inst_sbci(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->reg[dst],
            diff = val - imm - (avr->reg[avr->model.reg_status] & 0x01);
    avr->reg[dst] = diff;
    set_sreg_subc(avr, val, imm, diff);
    avr->pc += 2;
}

void inst_sbiw(struct avr *avr, uint16_t inst) {
    uint8_t dst_l = ((inst >> 3) & 0x06) + 24,
            dst_h = dst_l + 1,
            imm = ((inst >> 2) & 0x30) | (inst & 0x0f),
            val_l = avr->reg[dst_l],
            val_h = avr->reg[dst_h],
            status = avr->reg[avr->model.reg_status] & 0xe0;
    uint16_t diff = ((uint16_t) val_h << 8) + (uint16_t) val_l - imm;
    avr->reg[dst_l] = diff & 0xff;
    avr->reg[dst_h] = diff >> 8;
    status |= ((diff >> 8) & ~val_h) >> 7;                  // carry
    status |= (diff == 0) << 1;                             // zero
    status |= (diff >> 13) & 0x40;                          // negative
    status |= ((~(diff >> 8) & val_h) & 0x80) >> 4;         // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->reg[avr->model.reg_status] = status;
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_and(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->reg[dst] & avr->reg[src];
    avr->reg[dst] = val;
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_andi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->reg[dst] & imm;
    avr->reg[dst] = val;
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_or(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->reg[dst] | avr->reg[src];
    avr->reg[dst] = val;
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_ori(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            imm = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            val = avr->reg[dst] | imm;
    avr->reg[dst] = val;
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_eor(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f),
            val = avr->reg[dst] ^ avr->reg[src];
    avr->reg[dst] = val;
    set_sreg_logical(avr, val);
    avr->pc += 2;
}

void inst_com(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->reg[dst];
    avr->reg[dst] = ~val;
    set_sreg_logical(avr, ~val);
    avr->reg[avr->model.reg_status] |= 1;
    avr->pc += 2;
}

void inst_neg(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->reg[dst];
    avr->reg[dst] = -val;
    set_sreg_sub(avr, 0, val, -val);
    avr->pc += 2;
}

void inst_inc(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->reg[dst],
            inc = val+1,
            status = avr->reg[avr->model.reg_status] & 0xe1;
    avr->reg[dst] = inc;
    status |= (inc == 0) << 1;                              // zero
    status |= (inc & 0x80) >> 5;                            // negative
    status |= ((inc ^ 0x7f) == 0xff) << 3;                  // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->reg[avr->model.reg_status] = status;
    avr->pc += 2;
}

void inst_in(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr = (((inst >> 5) & 0x30) | (inst & 0xf)) + avr->model.io_offset;
    avr->reg[dst] = avr_get_reg(avr, addr);
    avr->pc += 2;
}

void inst_dec(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            val = avr->reg[dst],
            dec = val-1,
            status = avr->reg[avr->model.reg_status] & 0xe1;
    avr->reg[dst] = dec;
    status |= (dec == 0) << 1;                              // zero
    status |= (dec & 0x80) >> 5;                            // negative
    status |= ((dec ^ 0x80) == 0xff) << 3;                  // overflow
    status |= (((status << 1) ^ status) & 0x08) << 1;       // sign
    avr->reg[avr->model.reg_status] = status;
    avr->pc += 2;
}

void inst_mul(struct avr *avr, uint16_t inst) {
    uint8_t r1 = (inst >> 4) & 0x1f,
            r2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    uint16_t prod = (uint16_t) avr->reg[r1] * avr->reg[r2];
    avr->reg[AVR_REG_R0] = prod & 0xff;
    avr->reg[AVR_REG_R1] = prod >> 8;
    set_sreg_mul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_muls(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x0f) + 16,
            r2 = (inst & 0x0f) + 16;
    uint16_t prod = (int8_t) avr->reg[r1] * (int8_t) avr->reg[r2];
    avr->reg[AVR_REG_R0] = prod & 0xff;
    avr->reg[AVR_REG_R1] = (prod >> 8) & 0xff;
    set_sreg_mul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_mulsu(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = (int8_t) avr->reg[r1] * (uint8_t) avr->reg[r2];
    avr->reg[AVR_REG_R0] = prod & 0xff;
    avr->reg[AVR_REG_R1] = (prod >> 8) & 0xff;
    set_sreg_mul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_fmul(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = avr->reg[r1] * avr->reg[r2];
    avr->reg[AVR_REG_R0] = (prod << 1) & 0xff;
    avr->reg[AVR_REG_R1] = (prod >> 7) & 0xff;
    set_sreg_fmul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_fmuls(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = (int8_t) avr->reg[r1] * (int8_t) avr->reg[r2];
    avr->reg[AVR_REG_R0] = (prod << 1) & 0xff;
    avr->reg[AVR_REG_R1] = (prod >> 7) & 0xff;
    set_sreg_fmul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_fmulsu(struct avr *avr, uint16_t inst) {
    uint8_t r1 = ((inst >> 4) & 0x07) + 16,
            r2 = (inst & 0x07) + 16;
    uint16_t prod = (int8_t) avr->reg[r1] * (uint8_t) avr->reg[r2];
    avr->reg[AVR_REG_R0] = (prod << 1) & 0xff;
    avr->reg[AVR_REG_R1] = (prod >> 7) & 0xff;
    set_sreg_fmul(avr, prod);
    avr->pc += 2;
    avr->progress = 1;
    avr->status = MCU_STATUS_COMPLETING;
}

void inst_rjmp(struct avr *avr, uint16_t inst) {
    int16_t dpc = ((int16_t) (inst << 4) >> 3) + 2;
    uint32_t addr = avr->pc + dpc;
    if (addr < avr->model.romsize) {
        avr->pc = addr;
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    } else {
        LOG("cannot jump to address 0x%06x\n", addr);
        avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
    }
}

void inst_ijmp(struct avr *avr, uint16_t inst) {
    (void) inst;
    uint32_t addr = (((uint32_t) avr->reg[AVR_REG_Z+1] << 8) | avr->reg[AVR_REG_Z]) << 1;
    if (addr < avr->model.romsize) {
        avr->pc = addr;
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    } else {
        LOG("cannot jump to address 0x%06x\n", addr);
        avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
    }
}

void inst_eijmp(struct avr *avr, uint16_t inst) {
    (void) inst;
    uint32_t addr = (((uint32_t) avr->reg[avr->model.reg_eind] << 16) |
                     ((uint32_t) avr->reg[AVR_REG_Z+1] << 8) |
                     (uint32_t) avr->reg[AVR_REG_Z]) << 1;
    if (addr < avr->model.romsize) {
        avr->pc = addr;
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    } else {
        LOG("cannot jump to address 0x%06x\n", addr);
        avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
    }
}

void inst_jmp(struct avr *avr, uint16_t inst) {
    uint16_t inst2 = ((uint16_t) avr->rom[avr->pc+3] << 8) | avr->rom[avr->pc+2];
    uint32_t addr = ((((inst & 0x01f0) >> 3) | (inst & 1)) << 17) | (inst2 << 1);
    if (addr < avr->model.romsize) {
        avr->pc = addr;
        avr->progress = 2;
        avr->status = MCU_STATUS_COMPLETING;
    } else {
        LOG("cannot jump to address 0x%06x\n", addr);
        avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
    }
}

static void sim_call(struct avr *avr, uint32_t addr, uint32_t ret, uint8_t duration) {
    sim_push(avr, (ret >> 1) & 0xff);
    sim_push(avr, (ret >> 9) & 0xff);
    if (avr->model.pcsize == 3) {
        sim_push(avr, (ret >> 17) & 0xff);
    }
    if (avr->status == MCU_STATUS_NORMAL) {
        if (addr < avr->model.romsize) {
            avr->pc = addr;
            avr->progress = duration + (avr->model.pcsize > 2 ? 1 : 0);
            avr->status = MCU_STATUS_COMPLETING;
        } else {
            LOG("call address 0x%06x exceeds program memory\n", addr);
            avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
        }
    }
}

void inst_rcall(struct avr *avr, uint16_t inst) {
    int16_t diff = (int16_t) (inst << 4) >> 3;
    sim_call(avr, avr->pc+diff+2, avr->pc+2, 2);
}

void inst_icall(struct avr *avr, uint16_t inst) {
    (void) inst;
    uint16_t addr = ((uint32_t) avr->reg[AVR_REG_Z+1] << 9) |
                    ((uint32_t) avr->reg[AVR_REG_Z] << 1);
    sim_call(avr, addr, avr->pc+2, 2);
}

void inst_eicall(struct avr *avr, uint16_t inst) {
    (void) inst;
    if (avr->model.pcsize == 3) {
        uint32_t addr = ((uint32_t) avr->reg[avr->model.reg_eind] << 17) |
                        ((uint32_t) avr->reg[AVR_REG_Z+1] << 9) |
                        ((uint32_t) avr->reg[AVR_REG_Z] << 1);

        sim_call(avr, addr, avr->pc+2, 2);
    } else {
        avr_panic(avr, AVR_INVALID_INSTRUCTION);
    }
}

void inst_call(struct avr *avr, uint16_t inst) {
    uint16_t inst2 = ((uint16_t) avr->rom[avr->pc+3] << 8) | avr->rom[avr->pc+2];
    uint32_t addr = ((((inst >> 3) & 0x3e) | (inst & 1)) << 16) | inst2;
    sim_call(avr, 2*addr, avr->pc+4, 3);
}

static void sim_return(struct avr *avr, uint8_t duration) {
    uint32_t ret = 0;
    if (avr->model.pcsize == 3) {
        ret |= (uint32_t) sim_pop(avr) << 17;
    }
    ret |= (uint32_t) sim_pop(avr) << 9;
    ret |= (uint32_t) sim_pop(avr) << 1;

    if (avr->status == MCU_STATUS_NORMAL) {
        if (ret < avr->model.romsize) {
            avr->pc = ret;
            avr->progress = duration + (avr->model.pcsize > 2 ? 1 : 0);
            avr->status = MCU_STATUS_COMPLETING;
        } else {
            LOG("return address 0x%06x exceeds program memory\n", ret);
            avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
        }
    }
}

void inst_ret(struct avr *avr, uint16_t inst) {
    (void) inst;
    sim_return(avr, 3);
}

void inst_reti(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr->reg[avr->model.reg_status] |= AVR_STATUS_I; // enable interrupts
    sim_return(avr, 3);
}

static inline void sim_skip(struct avr *avr) {
    uint16_t next = ((uint16_t) avr->rom[avr->pc+3] << 8) | avr->rom[avr->pc+2];

    if (is_32bit_inst(next)) {
        avr->pc += 6;
        avr->progress = 2;
        avr->status = MCU_STATUS_COMPLETING;
    } else {
        avr->pc += 4;
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    }
}

void inst_cpse(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f);
    if (avr->reg[reg1] == avr->reg[reg2]) {
        sim_skip(avr);
    } else {
        avr->pc += 2;
    }
}

void inst_cp(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->reg[reg1],
            b = avr->reg[reg2],
            c = a - b;
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_cpc(struct avr *avr, uint16_t inst) {
    uint8_t reg1 = (inst >> 4) & 0x1f,
            reg2 = ((inst >> 5) & 0x10) | (inst & 0x0f),
            a = avr->reg[reg1],
            b = avr->reg[reg2],
            c = a - b - (avr->reg[avr->model.reg_status] & 0x01);
    set_sreg_subc(avr, a, b, c);
    avr->pc += 2;
}

void inst_cpi(struct avr *avr, uint16_t inst) {
    uint8_t reg = ((inst >> 4) & 0x0f) + 16,
            a = avr->reg[reg],
            b = ((inst >> 4) & 0xf0) | (inst & 0x0f),
            c = a - b;
    set_sreg_sub(avr, a, b, c);
    avr->pc += 2;
}

void inst_sbrc(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            mask = 1 << (inst & 0x7);
    if (avr->reg[reg] & mask) {
        avr->pc += 2;
    } else {
        sim_skip(avr);
    }
}

void inst_sbrs(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            mask = 1 << (inst & 0x7);
    if (avr->reg[reg] & mask) {
        sim_skip(avr);
    } else {
        avr->pc += 2;
    }
}

void inst_sbic(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 3) & 0x1f,
            mask = 1 << (inst & 0x7);
    if (avr->reg[reg+avr->model.io_offset] & mask) {
        avr->pc += 2;
    } else {
        sim_skip(avr);
    }
}

void inst_sbis(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 3) & 0x1f,
            mask = 1 << (inst & 0x7);
    if (avr->reg[reg+avr->model.io_offset] & mask) {
        sim_skip(avr);
    } else {
        avr->pc += 2;
    }
}

void inst_branch(struct avr *avr, uint16_t inst) {
    int8_t diff = ((int8_t) (inst >> 2)) >> 1;
    uint8_t val = (inst >> 10) & 0x01,
            chk = (avr->reg[avr->model.reg_status] >> (inst & 0x07)) ^ val;
    if (chk & 1) {
        avr->pc += 2*(diff+1);
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    } else {
        avr->pc += 2;
    }
}

void inst_sbi(struct avr *avr, uint16_t inst) {
    uint8_t reg = ((inst >> 3) & 0x1f) + avr->model.io_offset,
            mask = 1 << (inst & 0x7);
    avr_set_reg_bits(avr, reg, 0xff, mask);
    avr->pc += 2;
}

void inst_cbi(struct avr *avr, uint16_t inst) {
    uint8_t reg = ((inst >> 3) & 0x1f) + avr->model.io_offset,
            mask = 1 << (inst & 0x7);
    avr_set_reg_bits(avr, reg, 0x00, mask);
    avr->pc += 2;
}

void inst_lsr(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            val = avr->reg[reg];
    avr->reg[reg] = val >> 1;
    set_sreg_rshift(avr, val, avr->reg[reg]);
    avr->pc += 2;
}

void inst_ror(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            val = avr->reg[reg];
    avr->reg[reg] = (avr->reg[avr->model.reg_status] << 7) | (val >> 1);
    set_sreg_rshift(avr, val, avr->reg[reg]);
    avr->pc += 2;
}

void inst_asr(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            val = avr->reg[reg];
    avr->reg[reg] = (uint8_t) ((int8_t) val >> 1);
    set_sreg_rshift(avr, val, avr->reg[reg]);
    avr->pc += 2;
}

void inst_swap(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            val = avr->reg[reg];
    avr->reg[reg] = (val >> 4) | (val << 4);
    avr->pc += 2;
}

void inst_bset(struct avr *avr, uint16_t inst) {
    uint8_t mask = 1 << ((inst >> 4) & 0x7);
    avr->reg[avr->model.reg_status] |= mask;
    avr->pc += 2;
}

void inst_bclr(struct avr *avr, uint16_t inst) {
    uint8_t mask = 1 << ((inst >> 4) & 0x7);
    avr->reg[avr->model.reg_status] &= ~mask;
    avr->pc += 2;
}

void inst_bst(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            mask = 1 << (inst & 0x7);
    if (avr->reg[reg] & mask) {
        avr->reg[avr->model.reg_status] |= 0x40;
    } else {
        avr->reg[avr->model.reg_status] &= 0xbf;
    }
    avr->pc += 2;
}

void inst_bld(struct avr *avr, uint16_t inst) {
    uint8_t reg = (inst >> 4) & 0x1f,
            mask = 1 << (inst & 0x7);
    if (avr->reg[avr->model.reg_status] & 0x40) {
        avr->reg[reg] |= mask;
    } else {
        avr->reg[reg] &= ~mask;
    }
    avr->pc += 2;
}

void inst_mov(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            src = ((inst >> 5) & 0x10) | (inst & 0x0f);
    avr->reg[dst] = avr->reg[src];
    avr->pc += 2;
}

void inst_movw(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 3) & 0x1e,
            src = (inst << 1) & 0x1e;
    avr->reg[dst] = avr->reg[src];
    avr->reg[dst+1] = avr->reg[src+1];
    avr->pc += 2;
}

void inst_ldi(struct avr *avr, uint16_t inst) {
    uint8_t dst = ((inst >> 4) & 0x0f) + 16,
            val = ((inst >> 4) & 0xf0) | (inst & 0x0f);
    avr->reg[dst] = val;
    avr->pc += 2;
}

static void sim_access(struct avr *avr, uint16_t ptr, uint8_t disp, uint8_t reg, uint8_t opts) {
    uint16_t addr;

    // fetch address
    if (avr->model.memend <= 0x100) {
        addr = avr->reg[ptr];
    } else {
        addr = ((uint16_t) avr->reg[ptr+1] << 8) | avr->reg[ptr];
    }

    // pre-decrement
    if (opts & 0x2) addr--;

    if (addr+disp >= avr->model.memend) {
        avr_panic(avr, AVR_INVALID_RAM_ADDRESS);
        return;
    } else {
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
        avr->pc += 2;
    }

    // set or load the value at the address
    if (opts & 0x4) {
        if (addr+disp < avr->model.regsize) {
            avr_set_reg(avr, addr+disp, avr->reg[reg]);
        } else {
            avr->pending_inst.type = AVR_PENDING_COPY;
            avr->pending_inst.dst = addr+disp;
            avr->pending_inst.src = reg;
        }
    } else {
        if (addr+disp < avr->model.regsize) {
            avr->reg[reg] = avr_get_reg(avr, addr+disp);
        } else {
            avr->pending_inst.type = AVR_PENDING_COPY;
            avr->pending_inst.dst = reg;
            avr->pending_inst.src = addr+disp;
        }
    }

    // post-increment
    if (opts & 0x1) addr++;

    // save the updated address
    if (avr->model.memend <= 0x100) {
        avr->reg[ptr] = addr & 0xff;
    } else {
        avr->reg[ptr+1] = addr >> 8;
        avr->reg[ptr] = addr & 0xff;
    }
}

void inst_ldx(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    sim_access(avr, AVR_REG_X, 0, dst, inst & 0x3);
}

void inst_ldy(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    sim_access(avr, AVR_REG_Y, 0, dst, inst & 0x3);
}

void inst_ldz(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    sim_access(avr, AVR_REG_Z, 0, dst, inst & 0x3);
}

void inst_ldd(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            dsp = ((inst & 0x2000) >> 8) | ((inst & 0x0c00) >> 7) | (inst & 0x07);
    sim_access(avr, (inst & 0x08) ? AVR_REG_Y : AVR_REG_Z, dsp, dst, 0);
}

void inst_lds(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f,
            addr_l = avr->rom[avr->pc+2],
            addr_h = avr->rom[avr->pc+3];
    uint16_t addr = (addr_h << 8) | addr_l;
    if (addr >= avr->model.memend) {
        avr_panic(avr, AVR_INVALID_RAM_ADDRESS);
    } else {
        avr->pending_inst.type = AVR_PENDING_COPY;
        avr->pending_inst.dst = dst;
        avr->pending_inst.src = addr;
        avr->pc += 4;
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    }
}

void inst_stx(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    sim_access(avr, AVR_REG_X, 0, src, 0x4 | (inst & 0x3));
}

void inst_sty(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    sim_access(avr, AVR_REG_Y, 0, src, 0x4 | (inst & 0x3));
}

void inst_stz(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    sim_access(avr, AVR_REG_Z, 0, src, 0x4 | (inst & 0x3));
}

void inst_std(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            dsp = ((inst & 0x2000) >> 8) | ((inst & 0x0c00) >> 7) | (inst & 0x07);
    sim_access(avr, (inst & 0x08) ? AVR_REG_Y : AVR_REG_Z, dsp, src, 0x4);
}

void inst_sts(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            addr_l = avr->rom[avr->pc+2],
            addr_h = avr->rom[avr->pc+3];
    uint16_t addr = (addr_h << 8) | addr_l;
    if (addr >= avr->model.memend) {
        avr_panic(avr, AVR_INVALID_RAM_ADDRESS);
    } else {
        avr->pending_inst.type = AVR_PENDING_COPY;
        avr->pending_inst.dst = addr;
        avr->pending_inst.src = src;
        avr->pc += 4;
        avr->progress = 1;
        avr->status = MCU_STATUS_COMPLETING;
    }
}

void inst_lpm(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    uint16_t addr = ((uint16_t) avr->reg[AVR_REG_Z+1] << 8) | avr->reg[AVR_REG_Z];
    if (addr >= avr->model.romsize) {
        avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
    } else {
        avr->reg[dst] = avr->rom[addr];
        // post-increment
        if (inst & 1) {
            addr++;
            avr->reg[AVR_REG_Z+1] = addr >> 8;
            avr->reg[AVR_REG_Z] = addr & 0xff;
        }
        avr->pc += 2;
        avr->progress = 2;
        avr->status = MCU_STATUS_COMPLETING;
    }
}

void inst_elpm(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    uint32_t addr = ((uint32_t) avr->reg[avr->model.reg_rampz] << 16) |
                    ((uint32_t) avr->reg[AVR_REG_Z+1] << 8) |
                    avr->reg[AVR_REG_Z];
    if (addr >= avr->model.romsize) {
        avr_panic(avr, AVR_INVALID_ROM_ADDRESS);
    } else {
        avr->reg[dst] = avr->rom[addr];
        // post-increment
        if (inst & 1) {
            addr++;
            avr->reg[avr->model.reg_rampz] = addr >> 16;
            avr->reg[AVR_REG_Z+1] = addr >> 8;
            avr->reg[AVR_REG_Z] = addr & 0xff;
        }
        avr->pc += 2;
        avr->progress = 2;
        avr->status = MCU_STATUS_COMPLETING;
    }
}

void inst_spm(struct avr *avr, uint16_t inst) {
    avr_exec_spm(avr, inst);

    if (avr->status == MCU_STATUS_NORMAL) {
        avr->pc += 2;
        avr->progress = 2;
        avr->status = MCU_STATUS_COMPLETING;
    }
}

void inst_out(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f,
            addr = (((inst >> 5) & 0x30) | (inst & 0xf)) + avr->model.io_offset;
    avr_set_reg(avr, addr, avr->reg[src]);
    avr->pc += 2;
}

void inst_push(struct avr *avr, uint16_t inst) {
    uint8_t src = (inst >> 4) & 0x1f;
    sim_push(avr, avr->reg[src]);
    if (avr->status == MCU_STATUS_NORMAL) {
        avr->pc += 2;
    }
}

void inst_pop(struct avr *avr, uint16_t inst) {
    uint8_t dst = (inst >> 4) & 0x1f;
    avr->reg[dst] = sim_pop(avr);
    if (avr->status == MCU_STATUS_NORMAL) {
        avr->pc += 2;
    }
}

void inst_nop(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr->pc += 2;
}

void inst_sleep(struct avr *avr, uint16_t inst) {
    (void) inst;
    // this is woefully incomplete but good enough for now
    avr->status = MCU_STATUS_IDLE;
    avr->pc += 2;
}

void inst_wdr(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}

void inst_break(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}

void inst_xch(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}

void inst_lat(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}

void inst_lac(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}

void inst_las(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}

void inst_des(struct avr *avr, uint16_t inst) {
    (void) inst;
    avr_panic(avr, AVR_UNSUPPORTED_INSTRUCTION);
}
