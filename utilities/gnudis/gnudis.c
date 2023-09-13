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

/* This approach isn't very memory efficient or clear,
 * but it avoids external size/buffer tracking in this
 * example.
 */
static int dis_fprintf(void *stream, const char *fmt, ...)
{
    stream_state *ss = (stream_state *)stream;

    va_list arg;
    va_start(arg, fmt);
    if (!ss->reenter) {
        vasprintf(&ss->insn_buffer, fmt, arg);
        ss->reenter = true;
    } else {
        char *tmp;
        vasprintf(&tmp, fmt, arg);

        char *tmp2;
        asprintf(&tmp2, "%s%s", ss->insn_buffer, tmp);
        free(ss->insn_buffer);
        free(tmp);
        ss->insn_buffer = tmp2;
    }
    va_end(arg);

    return 0;
}

char *disassemble_raw(uint8_t *input_buffer, size_t input_buffer_size)
{
    char *disassembled = NULL;
    stream_state ss = {};

    disassemble_info disasm_info = {};
    init_disassemble_info(&disasm_info, &ss, dis_fprintf);
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
            char *tmp;
            asprintf(&tmp, "%s\n%s", disassembled, ss.insn_buffer);
            free(disassembled);
            disassembled = tmp;
        }

        /* Reset the stream state after each instruction decode.
         */
        free(ss.insn_buffer);
        ss.reenter = false;
    }

    return disassembled;
}

int main(int argc, char const *argv[])
{
    uint16_t input_buffer[] = {
        0x0000,
        0x1111,
        0x2222,
        0x3333,
        0x4444,
        0x5555,
        0x6666,
    };
    size_t input_buffer_size = sizeof(input_buffer);

    char *disassembled = disassemble_raw((uint8_t*)input_buffer, input_buffer_size);
    puts(disassembled);
    free(disassembled);

    return 0;
}
