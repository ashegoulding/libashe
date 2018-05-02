DEBUG_FLAGS =
OPTI_FLAGS =
if DEBUG
DEBUG_FLAGS += -g
OPTI_FLAGS += -O0
else
DEBUG_FLAGS += -g -Wno-unused-parameter -Wno-unused-value
OPTI_FLAGS += -O3
endif

lib_LTLIBRARIES = libashe.la
libashe_la_LDFLAGS = -version-info 1:0:0
libashe_la_CXXFLAGS = -Wall $(DEBUG_FLAGS) $(OPTI_FLAGS) -std=c++11

libashe_la_SOURCES =\
    __internal.cpp\
    __openssl.cpp\
    __regex.cpp\
    Base64Decoder.cpp\
    Base64Encoder.cpp\
    Buffer.cpp\
    Class.cpp\
    Endian.cpp\
    Exception.cpp\
    FilterInterface.cpp\
    helper.cpp\
    HelperException.cpp\
    MessageDigest.cpp\
    ModuleException.cpp\
    Regex.cpp\
    Semaphore.cpp\
    UUID.cpp\
    LAsheException.cpp

nobase_include_HEADERS =\
    libashe/Base64Encoder.hpp\
    libashe/Regex.hpp\
    libashe/FilterInterface.hpp\
    libashe/helper.hpp\
    libashe/ModuleException.hpp\
    libashe/Base64Decoder.hpp\
    libashe/Buffer.hpp\
    libashe/StringUtils.hpp\
    libashe/Exception.hpp\
    libashe/UUID.hpp\
    libashe/Class.hpp\
    libashe/HelperException.hpp\
    libashe/Endian.hpp\
    libashe/MessageDigest.hpp\
    libashe/pub.hpp\
    libashe/Semaphore.hpp\
    libashe/OSCode.hpp\
    libashe/LAsheException.hpp

pkgconfigdir = @libdir@/pkgconfig
pkgconfig_DATA = libashe.pc


if TESTS
    GTEST_LIBS = -lgtest -lpthread -ldl
    GTEST_MAIN_LIBS = -lgtest -lgtest_main -lpthread -ldl
    GTEST_CXXFLAGS = -Wall -g $(OPTI_FLAGS) -std=c++11
    GTEST_LDADD = libashe.la

    bin_PROGRAMS =\
        lashetest_lib-init\
        lashetest_all\
        lashetest_noability\
        lashetest_openssl

    lashetest_lib_init_SOURCES = tests/lib-init.cpp
    lashetest_lib_init_LDFLAGS = $(GTEST_MAIN_LIBS)
    lashetest_lib_init_CXXFLAGS = $(GTEST_CXXFLAGS)
    lashetest_lib_init_LDADD = $(GTEST_LDADD)

    lashetest_all_SOURCES = tests/all.cpp
    lashetest_all_LDFLAGS = $(GTEST_LIBS)
    lashetest_all_CXXFLAGS = $(GTEST_CXXFLAGS)
    lashetest_all_LDADD = $(GTEST_LDADD)

    lashetest_noability_SOURCES = tests/noability.cpp
    lashetest_noability_LDFLAGS = $(GTEST_LIBS)
    lashetest_noability_CXXFLAGS = $(GTEST_CXXFLAGS)
    lashetest_noability_LDADD = $(GTEST_LDADD)

    lashetest_openssl_SOURCES = tests/openssl.cpp
    lashetest_openssl_LDFLAGS = $(GTEST_LIBS)
    lashetest_openssl_CXXFLAGS = $(GTEST_CXXFLAGS)
    lashetest_openssl_LDADD = $(GTEST_LDADD)
endif
