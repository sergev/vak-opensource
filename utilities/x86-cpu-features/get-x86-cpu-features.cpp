#include <iostream>
#include <map>

/// getX86CpuIDAndInfoEx - Execute the specified cpuid with subleaf and return
/// the 4 values in the specified arguments.  If we can't run cpuid on the host,
/// return true.
static bool getX86CpuIDAndInfoEx(unsigned value, unsigned subleaf, unsigned *rEAX, unsigned *rEBX,
                                 unsigned *rECX, unsigned *rEDX)
{
#if defined(__GNUC__) || defined(__clang__)
#if defined(__x86_64__)
    // gcc doesn't know cpuid would clobber ebx/rbx. Preserve it manually.
    // FIXME: should we save this for Clang?
    __asm__(
        "movq\t%%rbx, %%rsi\n\t"
        "cpuid\n\t"
        "xchgq\t%%rbx, %%rsi\n\t"
        : "=a"(*rEAX), "=S"(*rEBX), "=c"(*rECX), "=d"(*rEDX)
        : "a"(value), "c"(subleaf));
    return false;
#elif defined(__i386__)
    __asm__(
        "movl\t%%ebx, %%esi\n\t"
        "cpuid\n\t"
        "xchgl\t%%ebx, %%esi\n\t"
        : "=a"(*rEAX), "=S"(*rEBX), "=c"(*rECX), "=d"(*rEDX)
        : "a"(value), "c"(subleaf));
    return false;
#else
    return true;
#endif
#elif defined(_MSC_VER)
    int registers[4];
    __cpuidex(registers, value, subleaf);
    *rEAX = registers[0];
    *rEBX = registers[1];
    *rECX = registers[2];
    *rEDX = registers[3];
    return false;
#else
    return true;
#endif
}

// Read control register 0 (XCR0). Used to detect features such as AVX.
static bool getX86XCR0(unsigned *rEAX, unsigned *rEDX) {
#if defined(__GNUC__) || defined(__clang__)
  // Check xgetbv; this uses a .byte sequence instead of the instruction
  // directly because older assemblers do not include support for xgetbv and
  // there is no easy way to conditionally compile based on the assembler used.
  __asm__(".byte 0x0f, 0x01, 0xd0" : "=a"(*rEAX), "=d"(*rEDX) : "c"(0));
  return false;
#elif defined(_MSC_FULL_VER) && defined(_XCR_XFEATURE_ENABLED_MASK)
  unsigned long long Result = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
  *rEAX = Result;
  *rEDX = Result >> 32;
  return false;
#else
  return true;
#endif
}

bool getHostCPUFeatures(std::map<std::string, bool> &Features)
{
    unsigned EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    unsigned MaxLevel;

    if (getX86CpuIDAndInfoEx(0, 0, &MaxLevel, &EBX, &ECX, &EDX) || MaxLevel < 1)
        return false;

    getX86CpuIDAndInfoEx(1, 0, &EAX, &EBX, &ECX, &EDX);

    Features["cx8"] = (EDX >> 8) & 1;
    Features["cmov"] = (EDX >> 15) & 1;
    Features["mmx"] = (EDX >> 23) & 1;
    Features["fxsr"] = (EDX >> 24) & 1;
    Features["sse"] = (EDX >> 25) & 1;
    Features["sse2"] = (EDX >> 26) & 1;

    Features["sse3"] = (ECX >> 0) & 1;
    Features["pclmul"] = (ECX >> 1) & 1;
    Features["ssse3"] = (ECX >> 9) & 1;
    Features["cx16"] = (ECX >> 13) & 1;
    Features["sse4.1"] = (ECX >> 19) & 1;
    Features["sse4.2"] = (ECX >> 20) & 1;
    Features["crc32"] = Features["sse4.2"];
    Features["movbe"] = (ECX >> 22) & 1;
    Features["popcnt"] = (ECX >> 23) & 1;
    Features["aes"] = (ECX >> 25) & 1;
    Features["rdrnd"] = (ECX >> 30) & 1;

    // If CPUID indicates support for XSAVE, XRESTORE and AVX, and XGETBV
    // indicates that the AVX registers will be saved and restored on context
    // switch, then we have full AVX support.
    bool HasXSave = ((ECX >> 27) & 1) && !getX86XCR0(&EAX, &EDX);
    bool HasAVXSave = HasXSave && ((ECX >> 28) & 1) && ((EAX & 0x6) == 0x6);
#if defined(__APPLE__)
    // Darwin lazily saves the AVX512 context on first use: trust that the OS will
    // save the AVX512 context if we use AVX512 instructions, even the bit is not
    // set right now.
    bool HasAVX512Save = true;
#else
    // AVX512 requires additional context to be saved by the OS.
    bool HasAVX512Save = HasAVXSave && ((EAX & 0xe0) == 0xe0);
#endif
    // AMX requires additional context to be saved by the OS.
    const unsigned AMXBits = (1 << 17) | (1 << 18);
    bool HasAMXSave = HasXSave && ((EAX & AMXBits) == AMXBits);

    Features["avx"] = HasAVXSave;
    Features["fma"] = ((ECX >> 12) & 1) && HasAVXSave;
    // Only enable XSAVE if OS has enabled support for saving YMM state.
    Features["xsave"] = ((ECX >> 26) & 1) && HasAVXSave;
    Features["f16c"] = ((ECX >> 29) & 1) && HasAVXSave;

    unsigned MaxExtLevel;
    getX86CpuIDAndInfoEx(0x80000000, 0, &MaxExtLevel, &EBX, &ECX, &EDX);

    bool HasExtLeaf1 =
        MaxExtLevel >= 0x80000001 && !getX86CpuIDAndInfoEx(0x80000001, 0, &EAX, &EBX, &ECX, &EDX);
    Features["sahf"] = HasExtLeaf1 && ((ECX >> 0) & 1);
    Features["lzcnt"] = HasExtLeaf1 && ((ECX >> 5) & 1);
    Features["sse4a"] = HasExtLeaf1 && ((ECX >> 6) & 1);
    Features["prfchw"] = HasExtLeaf1 && ((ECX >> 8) & 1);
    Features["xop"] = HasExtLeaf1 && ((ECX >> 11) & 1) && HasAVXSave;
    Features["lwp"] = HasExtLeaf1 && ((ECX >> 15) & 1);
    Features["fma4"] = HasExtLeaf1 && ((ECX >> 16) & 1) && HasAVXSave;
    Features["tbm"] = HasExtLeaf1 && ((ECX >> 21) & 1);
    Features["mwaitx"] = HasExtLeaf1 && ((ECX >> 29) & 1);

    Features["64bit"] = HasExtLeaf1 && ((EDX >> 29) & 1);

    // Miscellaneous memory related features, detected by
    // using the 0x80000008 leaf of the CPUID instruction
    bool HasExtLeaf8 =
        MaxExtLevel >= 0x80000008 && !getX86CpuIDAndInfoEx(0x80000008, 0, &EAX, &EBX, &ECX, &EDX);
    Features["clzero"] = HasExtLeaf8 && ((EBX >> 0) & 1);
    Features["rdpru"] = HasExtLeaf8 && ((EBX >> 4) & 1);
    Features["wbnoinvd"] = HasExtLeaf8 && ((EBX >> 9) & 1);

    bool HasLeaf7 = MaxLevel >= 7 && !getX86CpuIDAndInfoEx(0x7, 0x0, &EAX, &EBX, &ECX, &EDX);

    Features["fsgsbase"] = HasLeaf7 && ((EBX >> 0) & 1);
    Features["sgx"] = HasLeaf7 && ((EBX >> 2) & 1);
    Features["bmi"] = HasLeaf7 && ((EBX >> 3) & 1);
    // AVX2 is only supported if we have the OS save support from AVX.
    Features["avx2"] = HasLeaf7 && ((EBX >> 5) & 1) && HasAVXSave;
    Features["bmi2"] = HasLeaf7 && ((EBX >> 8) & 1);
    Features["invpcid"] = HasLeaf7 && ((EBX >> 10) & 1);
    Features["rtm"] = HasLeaf7 && ((EBX >> 11) & 1);
    // AVX512 is only supported if the OS supports the context save for it.
    Features["avx512f"] = HasLeaf7 && ((EBX >> 16) & 1) && HasAVX512Save;
    Features["avx512dq"] = HasLeaf7 && ((EBX >> 17) & 1) && HasAVX512Save;
    Features["rdseed"] = HasLeaf7 && ((EBX >> 18) & 1);
    Features["adx"] = HasLeaf7 && ((EBX >> 19) & 1);
    Features["avx512ifma"] = HasLeaf7 && ((EBX >> 21) & 1) && HasAVX512Save;
    Features["clflushopt"] = HasLeaf7 && ((EBX >> 23) & 1);
    Features["clwb"] = HasLeaf7 && ((EBX >> 24) & 1);
    Features["avx512pf"] = HasLeaf7 && ((EBX >> 26) & 1) && HasAVX512Save;
    Features["avx512er"] = HasLeaf7 && ((EBX >> 27) & 1) && HasAVX512Save;
    Features["avx512cd"] = HasLeaf7 && ((EBX >> 28) & 1) && HasAVX512Save;
    Features["sha"] = HasLeaf7 && ((EBX >> 29) & 1);
    Features["avx512bw"] = HasLeaf7 && ((EBX >> 30) & 1) && HasAVX512Save;
    Features["avx512vl"] = HasLeaf7 && ((EBX >> 31) & 1) && HasAVX512Save;

    Features["prefetchwt1"] = HasLeaf7 && ((ECX >> 0) & 1);
    Features["avx512vbmi"] = HasLeaf7 && ((ECX >> 1) & 1) && HasAVX512Save;
    Features["pku"] = HasLeaf7 && ((ECX >> 4) & 1);
    Features["waitpkg"] = HasLeaf7 && ((ECX >> 5) & 1);
    Features["avx512vbmi2"] = HasLeaf7 && ((ECX >> 6) & 1) && HasAVX512Save;
    Features["shstk"] = HasLeaf7 && ((ECX >> 7) & 1);
    Features["gfni"] = HasLeaf7 && ((ECX >> 8) & 1);
    Features["vaes"] = HasLeaf7 && ((ECX >> 9) & 1) && HasAVXSave;
    Features["vpclmulqdq"] = HasLeaf7 && ((ECX >> 10) & 1) && HasAVXSave;
    Features["avx512vnni"] = HasLeaf7 && ((ECX >> 11) & 1) && HasAVX512Save;
    Features["avx512bitalg"] = HasLeaf7 && ((ECX >> 12) & 1) && HasAVX512Save;
    Features["avx512vpopcntdq"] = HasLeaf7 && ((ECX >> 14) & 1) && HasAVX512Save;
    Features["rdpid"] = HasLeaf7 && ((ECX >> 22) & 1);
    Features["kl"] = HasLeaf7 && ((ECX >> 23) & 1); // key locker
    Features["cldemote"] = HasLeaf7 && ((ECX >> 25) & 1);
    Features["movdiri"] = HasLeaf7 && ((ECX >> 27) & 1);
    Features["movdir64b"] = HasLeaf7 && ((ECX >> 28) & 1);
    Features["enqcmd"] = HasLeaf7 && ((ECX >> 29) & 1);

    Features["uintr"] = HasLeaf7 && ((EDX >> 5) & 1);
    Features["avx512vp2intersect"] = HasLeaf7 && ((EDX >> 8) & 1) && HasAVX512Save;
    Features["serialize"] = HasLeaf7 && ((EDX >> 14) & 1);
    Features["tsxldtrk"] = HasLeaf7 && ((EDX >> 16) & 1);
    // There are two CPUID leafs which information associated with the pconfig
    // instruction:
    // EAX=0x7, ECX=0x0 indicates the availability of the instruction (via the 18th
    // bit of EDX), while the EAX=0x1b leaf returns information on the
    // availability of specific pconfig leafs.
    // The target feature here only refers to the the first of these two.
    // Users might need to check for the availability of specific pconfig
    // leaves using cpuid, since that information is ignored while
    // detecting features using the "-march=native" flag.
    // For more info, see X86 ISA docs.
    Features["pconfig"] = HasLeaf7 && ((EDX >> 18) & 1);
    Features["amx-bf16"] = HasLeaf7 && ((EDX >> 22) & 1) && HasAMXSave;
    Features["avx512fp16"] = HasLeaf7 && ((EDX >> 23) & 1) && HasAVX512Save;
    Features["amx-tile"] = HasLeaf7 && ((EDX >> 24) & 1) && HasAMXSave;
    Features["amx-int8"] = HasLeaf7 && ((EDX >> 25) & 1) && HasAMXSave;
    bool HasLeaf7Subleaf1 =
        MaxLevel >= 7 && !getX86CpuIDAndInfoEx(0x7, 0x1, &EAX, &EBX, &ECX, &EDX);
    Features["avxvnni"] = HasLeaf7Subleaf1 && ((EAX >> 4) & 1) && HasAVXSave;
    Features["avx512bf16"] = HasLeaf7Subleaf1 && ((EAX >> 5) & 1) && HasAVX512Save;
    Features["hreset"] = HasLeaf7Subleaf1 && ((EAX >> 22) & 1);

    bool HasLeafD = MaxLevel >= 0xd && !getX86CpuIDAndInfoEx(0xd, 0x1, &EAX, &EBX, &ECX, &EDX);

    // Only enable XSAVE if OS has enabled support for saving YMM state.
    Features["xsaveopt"] = HasLeafD && ((EAX >> 0) & 1) && HasAVXSave;
    Features["xsavec"] = HasLeafD && ((EAX >> 1) & 1) && HasAVXSave;
    Features["xsaves"] = HasLeafD && ((EAX >> 3) & 1) && HasAVXSave;

    bool HasLeaf14 = MaxLevel >= 0x14 && !getX86CpuIDAndInfoEx(0x14, 0x0, &EAX, &EBX, &ECX, &EDX);

    Features["ptwrite"] = HasLeaf14 && ((EBX >> 4) & 1);

    bool HasLeaf19 = MaxLevel >= 0x19 && !getX86CpuIDAndInfoEx(0x19, 0, &EAX, &EBX, &ECX, &EDX);
    Features["widekl"] = HasLeaf7 && HasLeaf19 && ((EBX >> 2) & 1);

    return true;
}

int main()
{
    unsigned a = 0, b = 0, c = 0, d = 0;

    bool status = getX86CpuIDAndInfoEx(0, 0, &a, &b, &c, &d);

    std::cout << "status = " << status << std::endl;
    std::cout << "a = 0x" << std::hex << a << std::endl;
    std::cout << "b = 0x" << std::hex << b << std::endl;
    std::cout << "c = 0x" << std::hex << c << std::endl;
    std::cout << "d = 0x" << std::hex << d << std::endl;

    std::map<std::string, bool> features;
    status = getHostCPUFeatures(features);

    std::cout << "status = " << status << std::endl;
    std::cout << "features = " << std::endl;
    for (auto& item : features) {
        std::cout << "    " << item.first
                  << " = " << item.second
                  << std::endl;
    }
}
