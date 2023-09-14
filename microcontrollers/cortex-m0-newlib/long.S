        .cpu cortex-m0plus
        .arch armv6s-m
        .fpu softvfp
        .text
        .syntax unified
        .code   16
        .thumb_func
        .type   main, %function
main:
        msr   APSR_nzcvq, r0            @ f380 8000 msr CPSR_, r0
        msr   IAPSR_nzcvq, r0           @ f380 8001 msr IAPSR, r0
        msr   EAPSR_nzcvq, r0           @ f380 8002 msr EAPSR, r0
        msr   PSR_nzcvq, r0             @ f380 8003 msr PSR, r0
        msr   IPSR, r0                  @ f380 8005 msr IPSR, r0
        msr   EPSR, r0                  @ f380 8006 msr EPSR, r0
        msr   IEPSR, r0                 @ f380 8007 msr IEPSR, r0
        msr   MSP, r0                   @ f380 8008 msr MSP, r0
        msr   PSP, r0                   @ f380 8009 msr PSP, r0
        msr   MSPLIM, r0                @ f380 800a msr MSPLIM, r0
        msr   PSPLIM, r0                @ f380 800b msr PSPLIM, r0
        msr   PRIMASK, r0               @ f380 8010 msr PRIMASK, r0
        msr   BASEPRI, r0               @ f380 8011 msr BASEPRI, r0
        msr   BASEPRI_MAX, r0           @ f380 8012 msr BASEPRI_MAX, r0
        msr   FAULTMASK, r0             @ f380 8013 msr FAULTMASK, r0
        msr   CONTROL, r0               @ f380 8014 msr CONTROL, r0
        msr   MSP_NS, r0                @ f380 8088 msr MSP_NS, r0
        msr   PSP_NS, r0                @ f380 8089 msr PSP_NS, r0
        msr   MSPLIM_NS, r0             @ f380 808a msr MSPLIM_NS, r0
        msr   PSPLIM_NS, r0             @ f380 808b msr PSPLIM_NS, r0
        msr   PRIMASK_NS, r0            @ f380 8090 msr PRIMASK_NS, r0
        msr   BASEPRI_NS, r0            @ f380 8091 msr BASEPRI_NS, r0
        msr   FAULTMASK_NS, r0          @ f380 8093 msr FAULTMASK_NS, r0
        msr   CONTROL_NS, r0            @ f380 8094 msr CONTROL_NS, r0
        msr   SP_NS, r0                 @ f380 8098 msr SP_NS, r0
        msr   APSR_nzcvq, r1            @ f381 8000 msr CPSR_, r1
        msr   APSR_nzcvq, r2            @ f382 8000 msr CPSR_, r2
        msr   APSR_nzcvq, r3            @ f383 8000 msr CPSR_, r3
        msr   APSR_nzcvq, r4            @ f384 8000 msr CPSR_, r4
        msr   APSR_nzcvq, r5            @ f385 8000 msr CPSR_, r5
        msr   APSR_nzcvq, r6            @ f386 8000 msr CPSR_, r6
        msr   APSR_nzcvq, r7            @ f387 8000 msr CPSR_, r7
        msr   APSR_nzcvq, r8            @ f388 8000 msr CPSR_, r8
        msr   APSR_nzcvq, r9            @ f389 8000 msr CPSR_, r9
        msr   APSR_nzcvq, sl            @ f38a 8000 msr CPSR_, sl
        msr   APSR_nzcvq, fp            @ f38b 8000 msr CPSR_, fp
        msr   APSR_nzcvq, ip            @ f38c 8000 msr CPSR_, ip
        msr   APSR_nzcvq, lr            @ f38e 8000 msr CPSR_, lr

        dsb                             @ f3bf 8f40 ssbb
        dsb   #1                        @ f3bf 8f41 dsb oshld
        dsb   oshst                     @ f3bf 8f42 dsb oshst
        dsb   osh                       @ f3bf 8f43 dsb osh
        dsb   #4                        @ f3bf 8f44 pssbb
        dsb   #5                        @ f3bf 8f45 dsb nshld
        dsb   unst                      @ f3bf 8f46 dsb unst
        dsb   un                        @ f3bf 8f47 dsb un
        dsb   #8                        @ f3bf 8f48 dsb #8
        dsb   #9                        @ f3bf 8f49 dsb ishld
        dsb   ishst                     @ f3bf 8f4a dsb ishst
        dsb   ish                       @ f3bf 8f4b dsb ish
        dsb   #12                       @ f3bf 8f4c dfb
        dsb   #13                       @ f3bf 8f4d dsb ld
        dsb   st                        @ f3bf 8f4e dsb st
        dsb   sy                        @ f3bf 8f4f dsb sy

        dmb   #0                        @ f3bf 8f50 dmb #0
        dmb   #1                        @ f3bf 8f51 dmb oshld
        dmb   oshst                     @ f3bf 8f52 dmb oshst
        dmb   osh                       @ f3bf 8f53 dmb osh
        dmb   #4                        @ f3bf 8f54 dmb #4
        dmb   #5                        @ f3bf 8f55 dmb nshld
        dmb   unst                      @ f3bf 8f56 dmb unst
        dmb   un                        @ f3bf 8f57 dmb un
        dmb   #8                        @ f3bf 8f58 dmb #8
        dmb   #9                        @ f3bf 8f59 dmb ishld
        dmb   ishst                     @ f3bf 8f5a dmb ishst
        dmb   ish                       @ f3bf 8f5b dmb ish
        dmb   #12                       @ f3bf 8f5c dmb #12
        dmb   #13                       @ f3bf 8f5d dmb ld
        dmb   st                        @ f3bf 8f5e dmb st
        dmb   sy                        @ f3bf 8f5f dmb sy

        isb   #0                        @ f3bf 8f60 isb #0
        isb   #1                        @ f3bf 8f61 isb #1
        isb   #2                        @ f3bf 8f62 isb #2
        isb   #3                        @ f3bf 8f63 isb #3
        isb   #4                        @ f3bf 8f64 isb #4
        isb   #5                        @ f3bf 8f65 isb #5
        isb   #6                        @ f3bf 8f66 isb #6
        isb   #7                        @ f3bf 8f67 isb #7
        isb   #8                        @ f3bf 8f68 isb #8
        isb   #9                        @ f3bf 8f69 isb #9
        isb   #10                       @ f3bf 8f6a isb #10
        isb   #11                       @ f3bf 8f6b isb #11
        isb   #12                       @ f3bf 8f6c isb #12
        isb   #13                       @ f3bf 8f6d isb #13
        isb   #14                       @ f3bf 8f6e isb #14
        isb   sy                        @ f3bf 8f6f isb sy

        mrs   r0, APSR                  @ f3ef 8000 mrs r0, CPSR
        mrs   r0, IAPSR                 @ f3ef 8001 mrs r0, IAPSR
        mrs   r0, EAPSR                 @ f3ef 8002 mrs r0, EAPSR
        mrs   r0, PSR                   @ f3ef 8003 mrs r0, PSR
        mrs   r0, IPSR                  @ f3ef 8005 mrs r0, IPSR
        mrs   r0, EPSR                  @ f3ef 8006 mrs r0, EPSR
        mrs   r0, IEPSR                 @ f3ef 8007 mrs r0, IEPSR
        mrs   r0, MSP                   @ f3ef 8008 mrs r0, MSP
        mrs   r0, PSP                   @ f3ef 8009 mrs r0, PSP
        mrs   r0, MSPLIM                @ f3ef 800a mrs r0, MSPLIM
        mrs   r0, PSPLIM                @ f3ef 800b mrs r0, PSPLIM
        mrs   r0, PRIMASK               @ f3ef 8010 mrs r0, PRIMASK
        mrs   r0, BASEPRI               @ f3ef 8011 mrs r0, BASEPRI
        mrs   r0, BASEPRI_MAX           @ f3ef 8012 mrs r0, BASEPRI_MAX
        mrs   r0, FAULTMASK             @ f3ef 8013 mrs r0, FAULTMASK
        mrs   r0, CONTROL               @ f3ef 8014 mrs r0, CONTROL
        mrs   r0, MSP_NS                @ f3ef 8088 mrs r0, MSP_NS
        mrs   r0, PSP_NS                @ f3ef 8089 mrs r0, PSP_NS
        mrs   r0, MSPLIM_NS             @ f3ef 808a mrs r0, MSPLIM_NS
        mrs   r0, PSPLIM_NS             @ f3ef 808b mrs r0, PSPLIM_NS
        mrs   r0, PRIMASK_NS            @ f3ef 8090 mrs r0, PRIMASK_NS
        mrs   r0, BASEPRI_NS            @ f3ef 8091 mrs r0, BASEPRI_NS
        mrs   r0, FAULTMASK_NS          @ f3ef 8093 mrs r0, FAULTMASK_NS
        mrs   r0, CONTROL_NS            @ f3ef 8094 mrs r0, CONTROL_NS
        mrs   r0, SP_NS                 @ f3ef 8098 mrs r0, SP_NS
        mrs   r1, APSR                  @ f3ef 8100 mrs r1, CPSR
        mrs   r2, APSR                  @ f3ef 8200 mrs r2, CPSR
        mrs   r3, APSR                  @ f3ef 8300 mrs r3, CPSR
        mrs   r4, APSR                  @ f3ef 8400 mrs r4, CPSR
        mrs   r5, APSR                  @ f3ef 8500 mrs r5, CPSR
        mrs   r6, APSR                  @ f3ef 8600 mrs r6, CPSR
        mrs   r7, APSR                  @ f3ef 8700 mrs r7, CPSR
        mrs   r8, APSR                  @ f3ef 8800 mrs r8, CPSR
        mrs   r9, APSR                  @ f3ef 8900 mrs r9, CPSR
        mrs   sl, APSR                  @ f3ef 8a00 mrs sl, CPSR
        mrs   fp, APSR                  @ f3ef 8b00 mrs fp, CPSR
        mrs   ip, APSR                  @ f3ef 8c00 mrs ip, CPSR
        mrs   lr, APSR                  @ f3ef 8e00 mrs lr, CPSR

        bl    .                         @ f000 d000 bl 0x00c00980
        bl    .+1                       @ f000 d001 bl 0x00c00986
        bl    0x00c0098c                @ f000 d002 bl 0x00c0098c
        bl    0x00c00994                @ f000 d004 bl 0x00c00994
        bl    0x00c009a0                @ f000 d008 bl 0x00c009a0
        bl    0x00c009b4                @ f000 d010 bl 0x00c009b4
        bl    0x00c009d8                @ f000 d020 bl 0x00c009d8
        bl    0x00c00a1c                @ f000 d040 bl 0x00c00a1c
        bl    0x00c00aa0                @ f000 d080 bl 0x00c00aa0
        bl    0x00c00ba4                @ f000 d100 bl 0x00c00ba4
        bl    0x00c00da8                @ f000 d200 bl 0x00c00da8
        bl    0x00c011ac                @ f000 d400 bl 0x00c011ac
        bl    0x008009b0                @ f000 d800 bl 0x008009b0
        bl    0x00c019b4                @ f001 d000 bl 0x00c019b4
        bl    0x00c029b8                @ f002 d000 bl 0x00c029b8
        bl    0x00c049bc                @ f004 d000 bl 0x00c049bc
        bl    0x00c089c0                @ f008 d000 bl 0x00c089c0
        bl    0x00c109c4                @ f010 d000 bl 0x00c109c4
        bl    0x00c209c8                @ f020 d000 bl 0x00c209c8
        bl    0x00c409cc                @ f040 d000 bl 0x00c409cc
        bl    0x00c809d0                @ f080 d000 bl 0x00c809d0
        bl    0x00d009d4                @ f100 d000 bl 0x00d009d4
        bl    0x00e009d8                @ f200 d000 bl 0x00e009d8
        bl    0xffffffffff0009dc        @ f400 d000 bl 0xffffffffff0009dc
        bl    0x004009e0                @ f000 f000 bl 0x004009e0
        bl    0x004009e6                @ f000 f001 bl 0x004009e6
        bl    0x004009ec                @ f000 f002 bl 0x004009ec
        bl    0x004009f4                @ f000 f004 bl 0x004009f4
        bl    0x00400a00                @ f000 f008 bl 0x00400a00
        bl    0x00400a14                @ f000 f010 bl 0x00400a14
        bl    0x00400a38                @ f000 f020 bl 0x00400a38
        bl    0x00400a7c                @ f000 f040 bl 0x00400a7c
        bl    0x00400b00                @ f000 f080 bl 0x00400b00
        bl    0x00400c04                @ f000 f100 bl 0x00400c04
        bl    0x00400e08                @ f000 f200 bl 0x00400e08
        bl    0x0040120c                @ f000 f400 bl 0x0040120c
        bl    0x00000a10                @ f000 f800 bl 0x00000a10
        bl    0x00401a14                @ f001 f000 bl 0x00401a14
        bl    0x00402a18                @ f002 f000 bl 0x00402a18
        bl    0x00404a1c                @ f004 f000 bl 0x00404a1c
        bl    0x00408a20                @ f008 f000 bl 0x00408a20
        bl    0x00410a24                @ f010 f000 bl 0x00410a24
        bl    0x00420a28                @ f020 f000 bl 0x00420a28
        bl    0x00440a2c                @ f040 f000 bl 0x00440a2c
        bl    0x00480a30                @ f080 f000 bl 0x00480a30
        bl    0x00500a34                @ f100 f000 bl 0x00500a34
        bl    0x00600a38                @ f200 f000 bl 0x00600a38
        bl    0xffffffffff800a3c        @ f400 f000 bl 0xffffffffff800a3c
