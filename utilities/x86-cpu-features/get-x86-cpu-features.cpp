#include <iostream>
#include <map>

//
// Execute the specified cpuid with subleaf and return the 4 values
// in the specified arguments.
//
static void get_x86_cpuid(unsigned value, unsigned subleaf, unsigned *rEAX, unsigned *rEBX,
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
#elif defined(__i386__)
    __asm__(
        "movl\t%%ebx, %%esi\n\t"
        "cpuid\n\t"
        "xchgl\t%%ebx, %%esi\n\t"
        : "=a"(*rEAX), "=S"(*rEBX), "=c"(*rECX), "=d"(*rEDX)
        : "a"(value), "c"(subleaf));
#else
#error "Cpuid is available only for x86_64 and i386 architectures"
#endif
#elif defined(_MSC_VER)
    int registers[4];
    __cpuidex(registers, value, subleaf);
    *rEAX = registers[0];
    *rEBX = registers[1];
    *rECX = registers[2];
    *rEDX = registers[3];
#else
#error "Cpuid is available only for GCC, Clang and Microsoft compilers"
#endif
}

//
// Read control register 0 (XCR0).
// Used to detect features such as AVX.
//
static void get_x86_xcr0(unsigned *rEAX, unsigned *rEDX)
{
#if defined(__GNUC__) || defined(__clang__)
    // Check xgetbv; this uses a .byte sequence instead of the instruction
    // directly because older assemblers do not include support for xgetbv and
    // there is no easy way to conditionally compile based on the assembler used.
    __asm__(".byte 0x0f, 0x01, 0xd0" : "=a"(*rEAX), "=d"(*rEDX) : "c"(0));
#elif defined(_MSC_FULL_VER) && defined(_XCR_XFEATURE_ENABLED_MASK)
    unsigned long long Result = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
    *rEAX = Result;
    *rEDX = Result >> 32;
#else
#error "XGETBV is available only for GCC, Clang and Microsoft compilers"
#endif
}

//
// Detect features of X86 processor.
//
bool get_x86_features(std::map<std::string, bool> &features)
{
    unsigned a = 0, b = 0, c = 0, d = 0;
    unsigned max_level;

    get_x86_cpuid(0, 0, &max_level, &b, &c, &d);
    if (max_level < 1)
        return false;

    get_x86_cpuid(1, 0, &a, &b, &c, &d);

    features["cx8"] = (d >> 8) & 1;
    features["cmov"] = (d >> 15) & 1;
    features["mmx"] = (d >> 23) & 1;
    features["fxsr"] = (d >> 24) & 1;
    features["sse"] = (d >> 25) & 1;
    features["sse2"] = (d >> 26) & 1;

    features["sse3"] = (c >> 0) & 1;
    features["pclmul"] = (c >> 1) & 1;
    features["ssse3"] = (c >> 9) & 1;
    features["cx16"] = (c >> 13) & 1;
    features["sse4.1"] = (c >> 19) & 1;
    features["sse4.2"] = (c >> 20) & 1;
    features["crc32"] = features["sse4.2"];
    features["movbe"] = (c >> 22) & 1;
    features["popcnt"] = (c >> 23) & 1;
    features["aes"] = (c >> 25) & 1;
    features["rdrnd"] = (c >> 30) & 1;

    // If CPUID indicates support for XSAVE, XRESTORE and AVX, and XGETBV
    // indicates that the AVX registers will be saved and restored on context
    // switch, then we have full AVX support.
    bool has_xsave = (c >> 27) & 1;
    get_x86_xcr0(&a, &d);

    bool has_avx_save = has_xsave && ((c >> 28) & 1) && ((a & 0x6) == 0x6);
#if defined(__APPLE__)
    // Darwin lazily saves the AVX512 context on first use: trust that the OS will
    // save the AVX512 context if we use AVX512 instructions, even the bit is not
    // set right now.
    bool has_avx512_save = true;
#else
    // AVX512 requires additional context to be saved by the OS.
    bool has_avx512_save = has_avx_save && ((a & 0xe0) == 0xe0);
#endif

    // AMX requires additional context to be saved by the OS.
    const unsigned AMXBits = (1 << 17) | (1 << 18);
    bool has_amx_save = has_xsave && ((a & AMXBits) == AMXBits);

    features["avx"] = has_avx_save;
    features["fma"] = ((c >> 12) & 1) && has_avx_save;
    // Only enable XSAVE if OS has enabled support for saving YMM state.
    features["xsave"] = ((c >> 26) & 1) && has_avx_save;
    features["f16c"] = ((c >> 29) & 1) && has_avx_save;

    unsigned max_ext_level;
    get_x86_cpuid(0x80000000, 0, &max_ext_level, &b, &c, &d);
    bool has_ext_leaf1 = max_ext_level >= 0x80000001;

    get_x86_cpuid(0x80000001, 0, &a, &b, &c, &d);

    features["sahf"] = has_ext_leaf1 && ((c >> 0) & 1);
    features["lzcnt"] = has_ext_leaf1 && ((c >> 5) & 1);
    features["sse4a"] = has_ext_leaf1 && ((c >> 6) & 1);
    features["prfchw"] = has_ext_leaf1 && ((c >> 8) & 1);
    features["xop"] = has_ext_leaf1 && ((c >> 11) & 1) && has_avx_save;
    features["lwp"] = has_ext_leaf1 && ((c >> 15) & 1);
    features["fma4"] = has_ext_leaf1 && ((c >> 16) & 1) && has_avx_save;
    features["tbm"] = has_ext_leaf1 && ((c >> 21) & 1);
    features["mwaitx"] = has_ext_leaf1 && ((c >> 29) & 1);

    features["64bit"] = has_ext_leaf1 && ((d >> 29) & 1);

    // Miscellaneous memory related features, detected by
    // using the 0x80000008 leaf of the CPUID instruction
    bool has_ext_leaf8 = max_ext_level >= 0x80000008;

    get_x86_cpuid(0x80000008, 0, &a, &b, &c, &d);

    features["clzero"] = has_ext_leaf8 && ((b >> 0) & 1);
    features["rdpru"] = has_ext_leaf8 && ((b >> 4) & 1);
    features["wbnoinvd"] = has_ext_leaf8 && ((b >> 9) & 1);

    bool has_leaf7 = max_level >= 7;
    get_x86_cpuid(0x7, 0x0, &a, &b, &c, &d);

    features["fsgsbase"] = has_leaf7 && ((b >> 0) & 1);
    features["sgx"] = has_leaf7 && ((b >> 2) & 1);
    features["bmi"] = has_leaf7 && ((b >> 3) & 1);

    // AVX2 is only supported if we have the OS save support from AVX.
    features["avx2"] = has_leaf7 && ((b >> 5) & 1) && has_avx_save;
    features["bmi2"] = has_leaf7 && ((b >> 8) & 1);
    features["invpcid"] = has_leaf7 && ((b >> 10) & 1);
    features["rtm"] = has_leaf7 && ((b >> 11) & 1);

    // AVX512 is only supported if the OS supports the context save for it.
    features["avx512f"] = has_leaf7 && ((b >> 16) & 1) && has_avx512_save;
    features["avx512dq"] = has_leaf7 && ((b >> 17) & 1) && has_avx512_save;
    features["rdseed"] = has_leaf7 && ((b >> 18) & 1);
    features["adx"] = has_leaf7 && ((b >> 19) & 1);
    features["avx512ifma"] = has_leaf7 && ((b >> 21) & 1) && has_avx512_save;
    features["clflushopt"] = has_leaf7 && ((b >> 23) & 1);
    features["clwb"] = has_leaf7 && ((b >> 24) & 1);
    features["avx512pf"] = has_leaf7 && ((b >> 26) & 1) && has_avx512_save;
    features["avx512er"] = has_leaf7 && ((b >> 27) & 1) && has_avx512_save;
    features["avx512cd"] = has_leaf7 && ((b >> 28) & 1) && has_avx512_save;
    features["sha"] = has_leaf7 && ((b >> 29) & 1);
    features["avx512bw"] = has_leaf7 && ((b >> 30) & 1) && has_avx512_save;
    features["avx512vl"] = has_leaf7 && ((b >> 31) & 1) && has_avx512_save;

    features["prefetchwt1"] = has_leaf7 && ((c >> 0) & 1);
    features["avx512vbmi"] = has_leaf7 && ((c >> 1) & 1) && has_avx512_save;
    features["pku"] = has_leaf7 && ((c >> 4) & 1);
    features["waitpkg"] = has_leaf7 && ((c >> 5) & 1);
    features["avx512vbmi2"] = has_leaf7 && ((c >> 6) & 1) && has_avx512_save;
    features["shstk"] = has_leaf7 && ((c >> 7) & 1);
    features["gfni"] = has_leaf7 && ((c >> 8) & 1);
    features["vaes"] = has_leaf7 && ((c >> 9) & 1) && has_avx_save;
    features["vpclmulqdq"] = has_leaf7 && ((c >> 10) & 1) && has_avx_save;
    features["avx512vnni"] = has_leaf7 && ((c >> 11) & 1) && has_avx512_save;
    features["avx512bitalg"] = has_leaf7 && ((c >> 12) & 1) && has_avx512_save;
    features["avx512vpopcntdq"] = has_leaf7 && ((c >> 14) & 1) && has_avx512_save;
    features["rdpid"] = has_leaf7 && ((c >> 22) & 1);
    features["kl"] = has_leaf7 && ((c >> 23) & 1); // key locker
    features["cldemote"] = has_leaf7 && ((c >> 25) & 1);
    features["movdiri"] = has_leaf7 && ((c >> 27) & 1);
    features["movdir64b"] = has_leaf7 && ((c >> 28) & 1);
    features["enqcmd"] = has_leaf7 && ((c >> 29) & 1);

    features["uintr"] = has_leaf7 && ((d >> 5) & 1);
    features["avx512vp2intersect"] = has_leaf7 && ((d >> 8) & 1) && has_avx512_save;
    features["serialize"] = has_leaf7 && ((d >> 14) & 1);
    features["tsxldtrk"] = has_leaf7 && ((d >> 16) & 1);

    // There are two CPUID leafs which information associated with the pconfig
    // instruction:
    // a=0x7, c=0x0 indicates the availability of the instruction (via the 18th
    // bit of d), while the a=0x1b leaf returns information on the
    // availability of specific pconfig leafs.
    // The target feature here only refers to the the first of these two.
    // Users might need to check for the availability of specific pconfig
    // leaves using cpuid, since that information is ignored while
    // detecting features using the "-march=native" flag.
    // For more info, see X86 ISA docs.
    features["pconfig"] = has_leaf7 && ((d >> 18) & 1);
    features["amx-bf16"] = has_leaf7 && ((d >> 22) & 1) && has_amx_save;
    features["avx512fp16"] = has_leaf7 && ((d >> 23) & 1) && has_avx512_save;
    features["amx-tile"] = has_leaf7 && ((d >> 24) & 1) && has_amx_save;
    features["amx-int8"] = has_leaf7 && ((d >> 25) & 1) && has_amx_save;

    bool has_leaf7_subleaf1 = max_level >= 7;
    get_x86_cpuid(0x7, 0x1, &a, &b, &c, &d);

    features["avxvnni"] = has_leaf7_subleaf1 && ((a >> 4) & 1) && has_avx_save;
    features["avx512bf16"] = has_leaf7_subleaf1 && ((a >> 5) & 1) && has_avx512_save;
    features["hreset"] = has_leaf7_subleaf1 && ((a >> 22) & 1);

    bool has_leafD = max_level >= 0xd;
    get_x86_cpuid(0xd, 0x1, &a, &b, &c, &d);

    // Only enable XSAVE if OS has enabled support for saving YMM state.
    features["xsaveopt"] = has_leafD && ((a >> 0) & 1) && has_avx_save;
    features["xsavec"] = has_leafD && ((a >> 1) & 1) && has_avx_save;
    features["xsaves"] = has_leafD && ((a >> 3) & 1) && has_avx_save;

    bool has_leaf14 = max_level >= 0x14;
    get_x86_cpuid(0x14, 0x0, &a, &b, &c, &d);

    features["ptwrite"] = has_leaf14 && ((b >> 4) & 1);

    bool has_leaf19 = max_level >= 0x19;
    get_x86_cpuid(0x19, 0, &a, &b, &c, &d);

    features["widekl"] = has_leaf7 && has_leaf19 && ((b >> 2) & 1);

    return true;
}

void print_x86_vendor()
{
    unsigned a = 0, b = 0, c = 0, d = 0;

    get_x86_cpuid(0, 0, &a, &b, &c, &d);

    std::cout << "a = 0x" << std::hex << a << std::endl;
    std::cout << "b = 0x" << std::hex << b << std::endl;
    std::cout << "c = 0x" << std::hex << c << std::endl;
    std::cout << "d = 0x" << std::hex << d << std::endl;

    if (a < 1) {
        std::cout << "vendor unknown" << std::endl;
        return;
    }
#if 0
    // "Genu ineI ntel"
    if (b == 0x756e6547 && d == 0x49656e69 && c == 0x6c65746e) {
        std::cout << "vendor GenuineIntel" << std::endl;
        return;
    }

    // "Auth enti cAMD"
    if (b == 0x68747541 && d == 0x69746e65 && c == 0x444d4163) {
        std::cout << "vendor AuthenticAMD" << std::endl;
        return;
    }
#endif
    std::string signature = {
        (char) b,
        (char) (b >> 8),
        (char) (b >> 16),
        (char) (b >> 24),
        (char) d,
        (char) (d >> 8),
        (char) (d >> 16),
        (char) (d >> 24),
        (char) c,
        (char) (c >> 8),
        (char) (c >> 16),
        (char) (c >> 24),
    };
    std::cout << "vendor " << signature << std::endl;
}

void print_x86_features()
{
    std::map<std::string, bool> features;
    auto status = get_x86_features(features);

    std::cout << "status = " << status << std::endl;
    std::cout << "features = " << std::endl;
    for (auto &item : features) {
        std::cout << "    " << item.first << " = " << item.second << std::endl;
    }
}

int main()
{
    print_x86_vendor();
    print_x86_features();
}
