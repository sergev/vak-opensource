#define _GNU_SOURCE /* asprintf, vasprintf */

#include "config.h"
#include "dis-asm.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *insn_buffer;
    bool reenter;
} stream_state;

static void dis_vprintf(stream_state *stream, const char *fmt, va_list arg)
{
    if (!stream->reenter) {
        vasprintf(&stream->insn_buffer, fmt, arg);
        stream->reenter = true;
    } else {
        char *message;
        vasprintf(&message, fmt, arg);

        char *buf;
        asprintf(&buf, "%s%s", stream->insn_buffer, message);
        free(stream->insn_buffer);
        free(message);
        stream->insn_buffer = buf;
    }
}

static int dis_fprintf(void *stream, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    dis_vprintf(stream, fmt, arg);
    va_end(arg);
    return 0;
}

static int dis_fprintf_styled(void *stream, enum disassembler_style style, const char *fmt, ...)
{
    (void)style;

    va_list arg;
    va_start(arg, fmt);
    dis_vprintf(stream, fmt, arg);
//printf("%s: ", __func__);
//vprintf(fmt, arg);
//printf("\n");
    va_end(arg);
    return 0;
}

char *disassemble_raw(uint8_t *input_buffer, size_t input_buffer_size)
{
    char *disassembled = NULL;
    stream_state ss = {};

    disassemble_info disasm_info = {};
    init_disassemble_info(&disasm_info, &ss, dis_fprintf, dis_fprintf_styled);
    disasm_info.arch = bfd_arch_arm;
    disasm_info.mach = bfd_mach_arm_6M;
    disasm_info.read_memory_func = buffer_read_memory;
    disasm_info.buffer = input_buffer;
    disasm_info.buffer_vma = 0;
    disasm_info.buffer_length = input_buffer_size;
    disasm_info.disassembler_options = "force-thumb";
    disassemble_init_for_target(&disasm_info);

    disassembler_ftype disasm;
    disasm = disassembler(bfd_arch_arm, false, bfd_mach_arm_6M, NULL);

    size_t pc = 0;
    while (pc < input_buffer_size) {
        size_t insn_size = disasm(pc, &disasm_info);
        pc += insn_size;

        if (disassembled == NULL) {
            asprintf(&disassembled, "%s", ss.insn_buffer);
        } else {
            char *buf;
            asprintf(&buf, "%s\n%s", disassembled, ss.insn_buffer);
            free(disassembled);
            disassembled = buf;
        }

        /* Reset the stream state after each instruction decode.
         */
        free(ss.insn_buffer);
        ss.reenter = false;
    }

    return disassembled;
}

void print_short_insns()
{
#define SHORT_MAX 0xe800
    uint16_t short_insns[SHORT_MAX];
    unsigned count = 0;

    //
    // 16-bit opcodes.
    //
    for (int opcode=0; opcode < SHORT_MAX; opcode++) {
        // Ignore range 4780-47ff.
        if (opcode >= 0x4780 && opcode <= 0x47ff)
            continue;

        // Ignore range b600-b8ff.
        if (opcode >= 0xb600 && opcode <= 0xb8ff)
            continue;

        short_insns[count++] = opcode;
    }
    char *disassembled = disassemble_raw((uint8_t*)short_insns, count * sizeof(uint16_t));

    // Print result.
    const char *line = strtok(disassembled, "\n");
    for (int i=0; line != NULL; i++) {
        printf("%04x     %s\n", short_insns[i], line);
        line = strtok(NULL, "\n");
    }
    free(disassembled);
}

//
// 32-bit opcodes.
//
//                -----op1------            ---op2--
// 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
//  1  1  1  1  0  0 1 1 1 0 0 x x x x x  1  0  x  0  x  x x x x x x x x x x x  MSR
//  1  1  1  1  0  0 1 1 1 0 1 1 x x x x  1  0  x  0  x  x x x x x x x x x x x  Miscellaneous control instructions
//  1  1  1  1  0  0 1 1 1 1 1 x x x x x  1  0  x  0  x  x x x x x x x x x x x  MRS
//  1  1  1  1  0  1 1 1 1 1 1 1 x x x x  1  0  1  0  x  x x x x x x x x x x x  UDF
//  1  1  1  1  0  x x x x x x x x x x x  1  1  x  1  x  x x x x x x x x x x x  BL
//
void print_long_insns()
{
    static const uint16_t long_insns[] = {
        //
        // MSR instruction.
        //                -----op1------            ---op2--
        // 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 15 14 13 12 11 10  9  8 7 6 5 4 3 2 1 0
        //  1  1  1  1  0  0 1 1 1 0 0(0)--Rn---  1  0 (0) 0 (1)(0)(0)(0)------SYSm-----
        //
        0xf380, 0x8000,
        0xf390, 0x8000,

        //
        // Miscellaneous control instructions.
        //                -----op1------               ---op2--
        // 15 14 13 12 11 10 9 8 7 6 5 4 3  2  1  0 15 14 13 12 11 10  9  8 7 6 5 4 3 2 1 0
        //  1  1  1  1  0  0 1 1 1 0 1 1(1)(1)(1)(1) 1  0 (0) 0 (1)(1)(1)(1)0 1 0 0 option   DSB Data Synchronization Barrier
        //  1  1  1  1  0  0 1 1 1 0 1 1(1)(1)(1)(1) 1  0 (0) 0 (1)(1)(1)(1)0 1 0 1 option   DMB Data Memory Barrier
        //  1  1  1  1  0  0 1 1 1 0 1 1(1)(1)(1)(1) 1  0 (0) 0 (1)(1)(1)(1)0 1 1 0 option   ISB Instruction Synchronization Barrier
        //
        0xf3bf, 0x8f40,
        0xf3bf, 0x8f50,
        0xf3bf, 0x8f60,

        //
        // MRS instruction.
        //                -----op1------                ---op2--
        // 15 14 13 12 11 10 9 8 7 6 5 4  3  2  1  0 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        //  1  1  1  1  0  0 1 1 1 1 1(0)(1)(1)(1)(1) 1  0 (0) 0 ---Rd---- ------SYSm-----
        //
        0xf3ef, 0x8000,

        //
        // UDF instruction.
        //                -----op1------            ---op2--
        // 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        //  1  1  1  1  0  1 1 1 1 1 1 1 --imm4-  1  0  1  0 ---------imm12-----------
        //
        0xf7f0, 0xa000,

        //
        // BL instruction.
        //                -----op1------            ---op2--
        // 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        //  1  1  1  1  0  S -------imm10-------  1  1 J1  1 J2  --------imm11--------
        0xf000, 0xd000,
        0xf000, 0xd800,
        0xf000, 0xf000,
        0xf000, 0xf800,
        0xf400, 0xd000,
        0xf400, 0xd800,
        0xf400, 0xf000,
        0xf400, 0xf800,
    };
    char *disassembled = disassemble_raw((uint8_t*)long_insns, count * sizeof(uint16_t));

    // Print result.
    const char *line = strtok(disassembled, "\n");
    for (int i=0; line != NULL; i += 2) {
        printf("%04x %04x %s\n", long_insns[i], long_insns[i+1], line);
        line = strtok(NULL, "\n");
    }

    free(disassembled);
}

int main(int argc, char const *argv[])
{
    //print_short_insns();
    print_long_insns();
    return 0;
}
