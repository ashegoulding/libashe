DEBUG_FLAGS =
OPTI_FLAGS =
if PROFILING
DEBUG_FLAGS += -p
endif
if DEBUG
DEBUG_FLAGS += -g3
OPTI_FLAGS += -O0
else
DEBUG_FLAGS += -g
OPTI_FLAGS += -O3
endif

AM_CXXFLAGS = $(WARNINGCFLAGS) $(DEBUG_FLAGS) $(OPTI_FLAGS) -std=c++11 -I..

lib_LTLIBRARIES = libdanu.la
# so 파일 버전. 순서는 거꾸로. 0.0.1이면, 1:0:0 으로 써야 함.
libdanu_la_LDFLAGS = -version-info 1:0:0
# 컴파일할 소스.
libdanu_la_SOURCES =\
	__coreutils/base64.c\
	__coreutils/sha1.c\
	ColourSpace.cpp\
	DescendantPipe.cpp\
	DescendantProcess.cpp\
	Fjord.cpp\
	Frelish.cpp\
	IPEndpoint.cpp\
	Icebank.cpp\
	Logger.cpp\
	StrongRune.cpp\
	TCPSocket.cpp\
	ThreadPool.cpp\
	TransitiveInterface.cpp\
	UUID.cpp\
	WeakRune.cpp\
	WebSocket.cpp

# install 시 내보낼 헤더들
nobase_include_HEADERS =\
	libashe/ColourSpace.h\
	libashe/DescendantPipe.h\
	libashe/DescendantProcess.h\
	libashe/Fjord.h\
	libashe/Frelish.h\
	libashe/HostEndian.h\
	libashe/IPEndpoint.h\
	libashe/Icebank.h\
	libashe/Logger.h\
	libashe/MFCLinkage.h\
	libashe/ManagedBinary.h\
	libashe/Number.h\
	libashe/SectionBlock2D.h\
	libashe/Semaphore.h\
	libashe/StrongRune.h\
	libashe/TCPSocket.h\
	libashe/ThreadPool.h\
	libashe/TransitiveInterface.h\
	libashe/UUID.h\
	libashe/WeakRune.h\
	libashe/WebSocket.h
