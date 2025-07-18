# components-platform.sh
#
# Copyright The Mbed TLS Contributors
# SPDX-License-Identifier: Apache-2.0

# This file contains test components that are executed by all.sh

################################################################
#### Platform Testing
################################################################

support_test_aesni () {
    # Check that gcc targets x86_64 (we can build AESNI), and check for
    # AESNI support on the host (we can run AESNI).
    #
    # The name of this function is possibly slightly misleading, but needs to align
    # with the name of the corresponding test, component_test_aesni.
    #
    # In principle 32-bit x86 can support AESNI, but our implementation does not
    # support 32-bit x86, so we check for x86-64.
    # We can only grep /proc/cpuinfo on Linux, so this also checks for Linux
    (gcc -v 2>&1 | grep Target | grep -q x86_64) &&
        [[ "$HOSTTYPE" == "x86_64" && "$OSTYPE" == "linux-gnu" ]] &&
        (lscpu | grep -qw aes)
}

component_test_aesni () { # ~ 60s
    # This tests the two AESNI implementations (intrinsics and assembly), and also the plain C
    # fallback. It also tests the logic that is used to select which implementation(s) to build.
    #
    # This test does not require the host to have support for AESNI (if it doesn't, the run-time
    # AESNI detection will fallback to the plain C implementation, so the tests will instead
    # exercise the plain C impl).

    msg "build: default config with different AES implementations"
    scripts/config.py set MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY
    scripts/config.py set MBEDTLS_HAVE_ASM

    # test the intrinsics implementation
    msg "AES tests, test intrinsics"
    make clean
    make CC=gcc CFLAGS='-Werror -Wall -Wextra -mpclmul -msse2 -maes'
    # check that we built intrinsics - this should be used by default when supported by the compiler
    ./programs/test/selftest aes | grep "AESNI code" | grep -q "intrinsics"

    # test the asm implementation
    msg "AES tests, test assembly"
    make clean
    make CC=gcc CFLAGS='-Werror -Wall -Wextra -mno-pclmul -mno-sse2 -mno-aes'
    # check that we built assembly - this should be built if the compiler does not support intrinsics
    ./programs/test/selftest aes | grep "AESNI code" | grep -q "assembly"

    # test the plain C implementation
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY
    msg "AES tests, plain C"
    make clean
    make CC=gcc CFLAGS='-O2 -Werror'
    # check that there is no AESNI code present
    ./programs/test/selftest aes | not grep -q "AESNI code"
    not grep -q "AES note: using AESNI" ./programs/test/selftest
    grep -q "AES note: built-in implementation." ./programs/test/selftest

    # test the intrinsics implementation
    scripts/config.py set MBEDTLS_AESNI_C
    scripts/config.py set MBEDTLS_AES_USE_HARDWARE_ONLY
    msg "AES tests, test AESNI only"
    make clean
    make CC=gcc CFLAGS='-Werror -Wall -Wextra -mpclmul -msse2 -maes'
    ./programs/test/selftest aes | grep -q "AES note: using AESNI"
    ./programs/test/selftest aes | not grep -q "AES note: built-in implementation."
    grep -q "AES note: using AESNI" ./programs/test/selftest
    not grep -q "AES note: built-in implementation." ./programs/test/selftest
}

support_test_aesni_m32 () {
    support_test_m32_no_asm && (lscpu | grep -qw aes)
}

component_test_aesni_m32 () { # ~ 60s
    # This tests are duplicated from component_test_aesni for i386 target
    #
    # AESNI intrinsic code supports i386 and assembly code does not support it.

    msg "build: default config with different AES implementations"
    scripts/config.py set MBEDTLS_AESNI_C
    scripts/config.py set MBEDTLS_PADLOCK_C
    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY
    scripts/config.py set MBEDTLS_HAVE_ASM

    # test the intrinsics implementation with gcc
    msg "AES tests, test intrinsics (gcc)"
    make clean
    make CC=gcc CFLAGS='-m32 -Werror -Wall -Wextra' LDFLAGS='-m32'
    # check that we built intrinsics - this should be used by default when supported by the compiler
    ./programs/test/selftest aes | grep "AESNI code" | grep -q "intrinsics"
    grep -q "AES note: using AESNI" ./programs/test/selftest
    grep -q "AES note: built-in implementation." ./programs/test/selftest
    grep -q "AES note: using VIA Padlock" ./programs/test/selftest
    grep -q mbedtls_aesni_has_support ./programs/test/selftest

    scripts/config.py set MBEDTLS_AESNI_C
    scripts/config.py unset MBEDTLS_PADLOCK_C
    scripts/config.py set MBEDTLS_AES_USE_HARDWARE_ONLY
    msg "AES tests, test AESNI only"
    make clean
    make CC=gcc CFLAGS='-m32 -Werror -Wall -Wextra -mpclmul -msse2 -maes' LDFLAGS='-m32'
    ./programs/test/selftest aes | grep -q "AES note: using AESNI"
    ./programs/test/selftest aes | not grep -q "AES note: built-in implementation."
    grep -q "AES note: using AESNI" ./programs/test/selftest
    not grep -q "AES note: built-in implementation." ./programs/test/selftest
    not grep -q "AES note: using VIA Padlock" ./programs/test/selftest
    not grep -q mbedtls_aesni_has_support ./programs/test/selftest
}

support_test_aesni_m32_clang () {
    # clang >= 4 is required to build with target attributes
    support_test_aesni_m32 && [[ $(clang_version) -ge 4 ]]
}

component_test_aesni_m32_clang () {

    scripts/config.py set MBEDTLS_AESNI_C
    scripts/config.py set MBEDTLS_PADLOCK_C
    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY
    scripts/config.py set MBEDTLS_HAVE_ASM

    # test the intrinsics implementation with clang
    msg "AES tests, test intrinsics (clang)"
    make clean
    make CC=clang CFLAGS='-m32 -Werror -Wall -Wextra' LDFLAGS='-m32'
    # check that we built intrinsics - this should be used by default when supported by the compiler
    ./programs/test/selftest aes | grep "AESNI code" | grep -q "intrinsics"
    grep -q "AES note: using AESNI" ./programs/test/selftest
    grep -q "AES note: built-in implementation." ./programs/test/selftest
    grep -q "AES note: using VIA Padlock" ./programs/test/selftest
    grep -q mbedtls_aesni_has_support ./programs/test/selftest
}

support_build_aes_armce () {
    # clang >= 11 is required to build with AES extensions
    [[ $(clang_version) -ge 11 ]]
}

component_build_aes_armce () {
    # Test variations of AES with Armv8 crypto extensions
    scripts/config.py set MBEDTLS_AESCE_C
    scripts/config.py set MBEDTLS_AES_USE_HARDWARE_ONLY

    msg "MBEDTLS_AES_USE_HARDWARE_ONLY, clang, aarch64"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=aarch64-linux-gnu -march=armv8-a+crypto"
    msg "clang, test aarch64 crypto instructions built"
    grep -E 'aes[a-z]+\s*[qv]' library/aesce.s

    msg "MBEDTLS_AES_USE_HARDWARE_ONLY, clang, arm"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a72+crypto -marm"
    msg "clang, test A32 crypto instructions built"
    grep -E 'aes[0-9a-z]+.[0-9]\s*[qv]' library/aesce.s

    msg "MBEDTLS_AES_USE_HARDWARE_ONLY, clang, thumb"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a32+crypto -mthumb"
    msg "clang, test T32 crypto instructions built"
    grep -E 'aes[0-9a-z]+.[0-9]\s*[qv]' library/aesce.s

    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY

    msg "MBEDTLS_AES_USE_both, clang, aarch64"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=aarch64-linux-gnu -march=armv8-a+crypto"
    msg "clang, test aarch64 crypto instructions built"
    grep -E 'aes[a-z]+\s*[qv]' library/aesce.s

    msg "MBEDTLS_AES_USE_both, clang, arm"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a72+crypto -marm"
    msg "clang, test A32 crypto instructions built"
    grep -E 'aes[0-9a-z]+.[0-9]\s*[qv]' library/aesce.s

    msg "MBEDTLS_AES_USE_both, clang, thumb"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a32+crypto -mthumb"
    msg "clang, test T32 crypto instructions built"
    grep -E 'aes[0-9a-z]+.[0-9]\s*[qv]' library/aesce.s

    scripts/config.py unset MBEDTLS_AESCE_C

    msg "no MBEDTLS_AESCE_C, clang, aarch64"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=aarch64-linux-gnu -march=armv8-a"
    msg "clang, test aarch64 crypto instructions not built"
    not grep -E 'aes[a-z]+\s*[qv]' library/aesce.s

    msg "no MBEDTLS_AESCE_C, clang, arm"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a72 -marm"
    msg "clang, test A32 crypto instructions not built"
    not grep -E 'aes[0-9a-z]+.[0-9]\s*[qv]' library/aesce.s

    msg "no MBEDTLS_AESCE_C, clang, thumb"
    make -B library/aesce.o library/aesce.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a32 -mthumb"
    msg "clang, test T32 crypto instructions not built"
    not grep -E 'aes[0-9a-z]+.[0-9]\s*[qv]' library/aesce.s
}

support_build_sha_armce () {
    # clang >= 4 is required to build with SHA extensions
    [[ $(clang_version) -ge 4 ]]
}

component_build_sha_armce () {
    scripts/config.py unset MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT

    # Test variations of SHA256 Armv8 crypto extensions
    scripts/config.py set MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY
        msg "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY clang, aarch64"
        make -B library/sha256.o library/sha256.s CC=clang CFLAGS="--target=aarch64-linux-gnu -march=armv8-a+crypto"
        msg "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY clang, test aarch64 crypto instructions built"
        grep -E 'sha256[a-z0-9]+\s+[qv]' library/sha256.s

        msg "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY clang, arm"
        make -B library/sha256.o library/sha256.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a72+crypto -marm"
        msg "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY clang, test A32 crypto instructions built"
        grep -E 'sha256[a-z0-9]+.32\s+[qv]' library/sha256.s
    scripts/config.py unset MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY


    # test the deprecated form of the config option
    scripts/config.py set MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY
        msg "MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY clang, thumb"
        make -B library/sha256.o library/sha256.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a32+crypto -mthumb"
        msg "MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY clang, test T32 crypto instructions built"
        grep -E 'sha256[a-z0-9]+.32\s+[qv]' library/sha256.s
    scripts/config.py unset MBEDTLS_SHA256_USE_A64_CRYPTO_ONLY

    scripts/config.py set MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT
        msg "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT clang, aarch64"
        make -B library/sha256.o library/sha256.s CC=clang CFLAGS="--target=aarch64-linux-gnu -march=armv8-a+crypto"
        msg "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT clang, test aarch64 crypto instructions built"
        grep -E 'sha256[a-z0-9]+\s+[qv]' library/sha256.s
    scripts/config.py unset MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT


    # test the deprecated form of the config option
    scripts/config.py set MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT
        msg "MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT clang, arm"
        make -B library/sha256.o library/sha256.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a72+crypto -marm -std=c99"

        msg "MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT clang, thumb"
        make -B library/sha256.o library/sha256.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a32+crypto -mthumb"
        msg "MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT clang, test T32 crypto instructions built"
        grep -E 'sha256[a-z0-9]+.32\s+[qv]' library/sha256.s
    scripts/config.py unset MBEDTLS_SHA256_USE_A64_CRYPTO_IF_PRESENT

    # examine the disassembly for absence of SHA instructions
    msg "clang, test A32 crypto instructions not built"
    make -B library/sha256.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a72 -marm"
    not grep -E 'sha256[a-z0-9]+.32\s+[qv]' library/sha256.s

    msg "clang, test T32 crypto instructions not built"
    make -B library/sha256.s CC=clang CFLAGS="--target=arm-linux-gnueabihf -mcpu=cortex-a32 -mthumb"
    not grep -E 'sha256[a-z0-9]+.32\s+[qv]' library/sha256.s

    msg "clang, test aarch64 crypto instructions not built"
    make -B library/sha256.s CC=clang CFLAGS="--target=aarch64-linux-gnu -march=armv8-a"
    not grep -E 'sha256[a-z0-9]+\s+[qv]' library/sha256.s
}

component_test_m32_no_asm () {
    # Build without assembly, so as to use portable C code (in a 32-bit
    # build) and not the i386-specific inline assembly.
    #
    # Note that we require gcc, because clang Asan builds fail to link for
    # this target (cannot find libclang_rt.lsan-i386.a - this is a known clang issue).
    msg "build: i386, make, gcc, no asm (ASan build)" # ~ 30s
    scripts/config.py full
    scripts/config.py unset MBEDTLS_HAVE_ASM
    scripts/config.py unset MBEDTLS_PADLOCK_C
    scripts/config.py unset MBEDTLS_AESNI_C # AESNI for 32-bit is tested in test_aesni_m32
    make CC=gcc CFLAGS="$ASAN_CFLAGS -m32" LDFLAGS="-m32 $ASAN_CFLAGS"

    msg "test: i386, make, gcc, no asm (ASan build)"
    make test
}

support_test_m32_no_asm () {
    case $(uname -m) in
        amd64|x86_64) true;;
        *) false;;
    esac
}

component_test_m32_o2 () {
    # Build with optimization, to use the i386 specific inline assembly
    # and go faster for tests.
    msg "build: i386, make, gcc -O2 (ASan build)" # ~ 30s
    scripts/config.py full
    scripts/config.py unset MBEDTLS_AESNI_C # AESNI for 32-bit is tested in test_aesni_m32
    make CC=gcc CFLAGS="$ASAN_CFLAGS -m32" LDFLAGS="-m32 $ASAN_CFLAGS"

    msg "test: i386, make, gcc -O2 (ASan build)"
    make test

    msg "test ssl-opt.sh, i386, make, gcc-O2"
    tests/ssl-opt.sh
}

support_test_m32_o2 () {
    support_test_m32_no_asm "$@"
}

component_test_m32_everest () {
    msg "build: i386, Everest ECDH context (ASan build)" # ~ 6 min
    scripts/config.py set MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED
    scripts/config.py unset MBEDTLS_AESNI_C # AESNI for 32-bit is tested in test_aesni_m32
    make CC=gcc CFLAGS="$ASAN_CFLAGS -m32" LDFLAGS="-m32 $ASAN_CFLAGS"

    msg "test: i386, Everest ECDH context - main suites (inc. selftests) (ASan build)" # ~ 50s
    make test

    msg "test: i386, Everest ECDH context - ECDH-related part of ssl-opt.sh (ASan build)" # ~ 5s
    tests/ssl-opt.sh -f ECDH

    msg "test: i386, Everest ECDH context - compat.sh with some ECDH ciphersuites (ASan build)" # ~ 3 min
    # Exclude some symmetric ciphers that are redundant here to gain time.
    tests/compat.sh -f ECDH -V NO -e 'ARIA\|CAMELLIA\|CHACHA'
}

support_test_m32_everest () {
    support_test_m32_no_asm "$@"
}

component_test_mx32 () {
    msg "build: 64-bit ILP32, make, gcc" # ~ 30s
    scripts/config.py full
    make CC=gcc CFLAGS='-O2 -Werror -Wall -Wextra -mx32' LDFLAGS='-mx32'

    msg "test: 64-bit ILP32, make, gcc"
    make test
}

support_test_mx32 () {
    case $(uname -m) in
        amd64|x86_64) true;;
        *) false;;
    esac
}

component_test_arm_linux_gnueabi_gcc_arm5vte () {
    # Mimic Debian armel port
    msg "test: ${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc -march=arm5vte, default config" # ~4m
    make CC="${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc" AR="${ARM_LINUX_GNUEABI_GCC_PREFIX}ar" CFLAGS='-Werror -Wall -Wextra -march=armv5te -O1'

    msg "test: main suites make, default config (out-of-box)" # ~7m 40s
    make test

    msg "selftest: make, default config (out-of-box)" # ~0s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~0s
    tests/scripts/run_demos.py
}

support_test_arm_linux_gnueabi_gcc_arm5vte () {
    can_run_arm_linux_gnueabi
}

# The hard float ABI is not implemented for Thumb 1, so use gnueabi
# Some Thumb 1 asm is sensitive to optimisation level, so test both -O0 and -Os
component_test_arm_linux_gnueabi_gcc_thumb_1_opt_0 () {
    msg "test: ${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc -O0, thumb 1, default config" # ~2m 10s
    make CC="${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc" CFLAGS='-std=c99 -Werror -Wextra -O0 -mcpu=arm1136j-s -mthumb'

    msg "test: main suites make, default config (out-of-box)" # ~36m
    make test

    msg "selftest: make, default config (out-of-box)" # ~10s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~0s
    tests/scripts/run_demos.py
}

support_test_arm_linux_gnueabi_gcc_thumb_1_opt_0 () {
    can_run_arm_linux_gnueabi
}

component_test_arm_linux_gnueabi_gcc_thumb_1_opt_s () {
    msg "test: ${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc -Os, thumb 1, default config" # ~3m 10s
    make CC="${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc" CFLAGS='-std=c99 -Werror -Wextra -Os -mcpu=arm1136j-s -mthumb'

    msg "test: main suites make, default config (out-of-box)" # ~21m 10s
    make test

    msg "selftest: make, default config (out-of-box)" # ~2s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~0s
    tests/scripts/run_demos.py
}

support_test_arm_linux_gnueabi_gcc_thumb_1_opt_s () {
    can_run_arm_linux_gnueabi
}

component_test_arm_linux_gnueabihf_gcc_armv7 () {
    msg "test: ${ARM_LINUX_GNUEABIHF_GCC_PREFIX}gcc -O2, A32, default config" # ~4m 30s
    make CC="${ARM_LINUX_GNUEABIHF_GCC_PREFIX}gcc" CFLAGS='-std=c99 -Werror -Wextra -O2 -march=armv7-a -marm'

    msg "test: main suites make, default config (out-of-box)" # ~3m 30s
    make test

    msg "selftest: make, default config (out-of-box)" # ~0s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~0s
    tests/scripts/run_demos.py
}

support_test_arm_linux_gnueabihf_gcc_armv7 () {
    can_run_arm_linux_gnueabihf
}

component_test_arm_linux_gnueabihf_gcc_thumb_2 () {
    msg "test: ${ARM_LINUX_GNUEABIHF_GCC_PREFIX}gcc -Os, thumb 2, default config" # ~4m
    make CC="${ARM_LINUX_GNUEABIHF_GCC_PREFIX}gcc" CFLAGS='-std=c99 -Werror -Wextra -Os -march=armv7-a -mthumb'

    msg "test: main suites make, default config (out-of-box)" # ~3m 40s
    make test

    msg "selftest: make, default config (out-of-box)" # ~0s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~0s
    tests/scripts/run_demos.py
}

support_test_arm_linux_gnueabihf_gcc_thumb_2 () {
    can_run_arm_linux_gnueabihf
}

component_test_aarch64_linux_gnu_gcc () {
    msg "test: ${AARCH64_LINUX_GNU_GCC_PREFIX}gcc -O2, default config" # ~3m 50s
    make CC="${AARCH64_LINUX_GNU_GCC_PREFIX}gcc" CFLAGS='-std=c99 -Werror -Wextra -O2'

    msg "test: main suites make, default config (out-of-box)" # ~1m 50s
    make test

    msg "selftest: make, default config (out-of-box)" # ~0s
    programs/test/selftest

    msg "program demos: make, default config (out-of-box)" # ~0s
    tests/scripts/run_demos.py
}

support_test_aarch64_linux_gnu_gcc () {
    # Minimum version of GCC for MBEDTLS_AESCE_C is 6.0
    [ "$(gcc_version "${AARCH64_LINUX_GNU_GCC_PREFIX}gcc")" -ge 6 ] && can_run_aarch64_linux_gnu
}

component_build_arm_none_eabi_gcc () {
    msg "build: ${ARM_NONE_EABI_GCC_PREFIX}gcc -O1, baremetal+debug" # ~ 10s
    scripts/config.py baremetal
    make CC="${ARM_NONE_EABI_GCC_PREFIX}gcc" AR="${ARM_NONE_EABI_GCC_PREFIX}ar" LD="${ARM_NONE_EABI_GCC_PREFIX}ld" CFLAGS='-std=c99 -Werror -Wall -Wextra -O1' lib

    msg "size: ${ARM_NONE_EABI_GCC_PREFIX}gcc -O1, baremetal+debug"
    ${ARM_NONE_EABI_GCC_PREFIX}size -t library/*.o
}

component_build_arm_linux_gnueabi_gcc_arm5vte () {
    msg "build: ${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc -march=arm5vte, baremetal+debug" # ~ 10s
    scripts/config.py baremetal
    # Build for a target platform that's close to what Debian uses
    # for its "armel" distribution (https://wiki.debian.org/ArmEabiPort).
    # See https://github.com/Mbed-TLS/mbedtls/pull/2169 and comments.
    # Build everything including programs, see for example
    # https://github.com/Mbed-TLS/mbedtls/pull/3449#issuecomment-675313720
    make CC="${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc" AR="${ARM_LINUX_GNUEABI_GCC_PREFIX}ar" CFLAGS='-Werror -Wall -Wextra -march=armv5te -O1' LDFLAGS='-march=armv5te'

    msg "size: ${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc -march=armv5te -O1, baremetal+debug"
    ${ARM_LINUX_GNUEABI_GCC_PREFIX}size -t library/*.o
}

support_build_arm_linux_gnueabi_gcc_arm5vte () {
    type ${ARM_LINUX_GNUEABI_GCC_PREFIX}gcc >/dev/null 2>&1
}

component_build_arm_none_eabi_gcc_arm5vte () {
    msg "build: ${ARM_NONE_EABI_GCC_PREFIX}gcc -march=arm5vte, baremetal+debug" # ~ 10s
    scripts/config.py baremetal
    # This is an imperfect substitute for
    # component_build_arm_linux_gnueabi_gcc_arm5vte
    # in case the gcc-arm-linux-gnueabi toolchain is not available
    make CC="${ARM_NONE_EABI_GCC_PREFIX}gcc" AR="${ARM_NONE_EABI_GCC_PREFIX}ar" CFLAGS='-std=c99 -Werror -Wall -Wextra -march=armv5te -O1' LDFLAGS='-march=armv5te' SHELL='sh -x' lib

    msg "size: ${ARM_NONE_EABI_GCC_PREFIX}gcc -march=armv5te -O1, baremetal+debug"
    ${ARM_NONE_EABI_GCC_PREFIX}size -t library/*.o
}

component_build_arm_none_eabi_gcc_m0plus () {
    msg "build: ${ARM_NONE_EABI_GCC_PREFIX}gcc -mthumb -mcpu=cortex-m0plus, baremetal_size" # ~ 10s
    scripts/config.py baremetal_size
    make CC="${ARM_NONE_EABI_GCC_PREFIX}gcc" AR="${ARM_NONE_EABI_GCC_PREFIX}ar" LD="${ARM_NONE_EABI_GCC_PREFIX}ld" CFLAGS='-std=c99 -Werror -Wall -Wextra -mthumb -mcpu=cortex-m0plus -Os' lib

    msg "size: ${ARM_NONE_EABI_GCC_PREFIX}gcc -mthumb -mcpu=cortex-m0plus -Os, baremetal_size"
    ${ARM_NONE_EABI_GCC_PREFIX}size -t library/*.o
    for lib in library/*.a; do
        echo "$lib:"
        ${ARM_NONE_EABI_GCC_PREFIX}size -t $lib | grep TOTALS
    done
}

component_build_arm_none_eabi_gcc_no_udbl_division () {
    msg "build: ${ARM_NONE_EABI_GCC_PREFIX}gcc -DMBEDTLS_NO_UDBL_DIVISION, make" # ~ 10s
    scripts/config.py baremetal
    scripts/config.py set MBEDTLS_NO_UDBL_DIVISION
    make CC="${ARM_NONE_EABI_GCC_PREFIX}gcc" AR="${ARM_NONE_EABI_GCC_PREFIX}ar" LD="${ARM_NONE_EABI_GCC_PREFIX}ld" CFLAGS='-std=c99 -Werror -Wall -Wextra' lib
    echo "Checking that software 64-bit division is not required"
    not grep __aeabi_uldiv library/*.o
}

component_build_arm_none_eabi_gcc_no_64bit_multiplication () {
    msg "build: ${ARM_NONE_EABI_GCC_PREFIX}gcc MBEDTLS_NO_64BIT_MULTIPLICATION, make" # ~ 10s
    scripts/config.py baremetal
    scripts/config.py set MBEDTLS_NO_64BIT_MULTIPLICATION
    make CC="${ARM_NONE_EABI_GCC_PREFIX}gcc" AR="${ARM_NONE_EABI_GCC_PREFIX}ar" LD="${ARM_NONE_EABI_GCC_PREFIX}ld" CFLAGS='-std=c99 -Werror -O1 -march=armv6-m -mthumb' lib
    echo "Checking that software 64-bit multiplication is not required"
    not grep __aeabi_lmul library/*.o
}

component_build_arm_clang_thumb () {
    # ~ 30s

    scripts/config.py baremetal

    msg "build: clang thumb 2, make"
    make clean
    make CC="clang" CFLAGS='-std=c99 -Werror -Os --target=arm-linux-gnueabihf -march=armv7-m -mthumb' lib

    # Some Thumb 1 asm is sensitive to optimisation level, so test both -O0 and -Os
    msg "build: clang thumb 1 -O0, make"
    make clean
    make CC="clang" CFLAGS='-std=c99 -Werror -O0 --target=arm-linux-gnueabihf -mcpu=arm1136j-s -mthumb' lib

    msg "build: clang thumb 1 -Os, make"
    make clean
    make CC="clang" CFLAGS='-std=c99 -Werror -Os --target=arm-linux-gnueabihf -mcpu=arm1136j-s -mthumb' lib
}

component_build_armcc () {
    # Common configuration for all the builds below
    scripts/config.py baremetal

    # armc[56] don't support SHA-512 intrinsics
    scripts/config.py unset MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT

    # older versions of armcc/armclang don't support AESCE_C on 32-bit Arm
    scripts/config.py unset MBEDTLS_AESCE_C

    # Stop armclang warning about feature detection for A64_CRYPTO.
    # With this enabled, the library does build correctly under armclang,
    # but in baremetal builds (as tested here), feature detection is
    # unavailable, and the user is notified via a #warning. So enabling
    # this feature would prevent us from building with -Werror on
    # armclang. Tracked in #7198.
    scripts/config.py unset MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT

    scripts/config.py set MBEDTLS_HAVE_ASM

    # Compile mostly with -O1 since some Arm inline assembly is disabled for -O0.

    # ARM Compiler 6 - Target ARMv7-A
    helper_armc6_build_test "-O1 --target=arm-arm-none-eabi -march=armv7-a"

    # ARM Compiler 6 - Target ARMv7-M
    helper_armc6_build_test "-O1 --target=arm-arm-none-eabi -march=armv7-m"

    # ARM Compiler 6 - Target ARMv7-M+DSP
    helper_armc6_build_test "-O1 --target=arm-arm-none-eabi -march=armv7-m+dsp"

    # ARM Compiler 6 - Target ARMv8-A - AArch32
    helper_armc6_build_test "-O1 --target=arm-arm-none-eabi -march=armv8.2-a"

    # ARM Compiler 6 - Target ARMv8-M
    helper_armc6_build_test "-O1 --target=arm-arm-none-eabi -march=armv8-m.main"

    # ARM Compiler 6 - Target Cortex-M0 - no optimisation
    helper_armc6_build_test "-O0 --target=arm-arm-none-eabi -mcpu=cortex-m0"

    # ARM Compiler 6 - Target Cortex-M0
    helper_armc6_build_test "-Os --target=arm-arm-none-eabi -mcpu=cortex-m0"

    # ARM Compiler 6 - Target ARMv8.2-A - AArch64
    #
    # Re-enable MBEDTLS_AESCE_C as this should be supported by the version of armclang
    # that we have in our CI
    scripts/config.py set MBEDTLS_AESCE_C
    helper_armc6_build_test "-O1 --target=aarch64-arm-none-eabi -march=armv8.2-a+crypto"
}

support_build_armcc () {
    armc6_cc="$ARMC6_BIN_DIR/armclang"
    (check_tools "$armc6_cc" > /dev/null 2>&1)
}

# For timebeing, no VIA Padlock platform available.
component_build_aes_via_padlock () {

    msg "AES:VIA PadLock, build with default configuration."
    scripts/config.py unset MBEDTLS_AESNI_C
    scripts/config.py set MBEDTLS_PADLOCK_C
    scripts/config.py unset MBEDTLS_AES_USE_HARDWARE_ONLY
    make CC=gcc CFLAGS="$ASAN_CFLAGS -m32" LDFLAGS="-m32 $ASAN_CFLAGS"
    grep -q mbedtls_padlock_has_support ./programs/test/selftest

}

support_build_aes_via_padlock_only () {
    ( [ "$MBEDTLS_TEST_PLATFORM" == "Linux-x86_64" ] || \
        [ "$MBEDTLS_TEST_PLATFORM" == "Linux-amd64" ] ) && \
    [ "`dpkg --print-foreign-architectures`" == "i386" ]
}
