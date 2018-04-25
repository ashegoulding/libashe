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
libashe_la_LDFLAGS = -version-info 0:0:0
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
    UUID.cpp

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
    libashe/OSCode.hpp

pkgconfigdir = @libdir@/pkgconfig
pkgconfig_DATA = libashe.pc
