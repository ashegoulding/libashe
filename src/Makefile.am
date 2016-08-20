DEBUG_FLAGS =
OPTI_FLAGS =
if DEBUG
DEBUG_FLAGS += -g
OPTI_FLAGS += -O0
else
DEBUG_FLAGS += -g
OPTI_FLAGS += -O3
endif

lib_LTLIBRARIES = libashe.la
libashe_la_LDFLAGS = -version-info 0:0:0
libashe_la_CXXFLAGS = -Wall -Wextra $(DEBUG_FLAGS) $(OPTI_FLAGS) -std=c++11
libashe_la_SOURCES =\
	AsheClass.cpp\
	AsheException.cpp\
	helper.cpp\
	__internal.cpp\
	__openssl.cpp\
	UUID.cpp\
	Endian.cpp\
	FilterInterface.cpp\
	Base64Encoder.cpp\
	Base64Decoder.cpp\
	MessageDigest.cpp\
	Process.cpp

nobase_include_HEADERS =\
	libashe/UUID.h\
	libashe/pub.h\
	libashe/AsheClass.h\
	libashe/AsheException.h\
	libashe/helper.h\
	libashe/OSCode.h\
	libashe/Endian.h\
	libashe/FilterInterface.h\
	libashe/Base64Encoder.h\
	libashe/Base64Decoder.h\
	libashe/MessageDigest.h\
	libashe/Process.h
	
pkgconfigdir = @libdir@/pkgconfig
pkgconfig_DATA = libashe.pc
