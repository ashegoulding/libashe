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
# 컴파일할 소스.
libashe_la_SOURCES =\
	ColourSpace.cpp\
	DescendantPipe.cpp\
	DescendantProcess.cpp\
	Fjord.cpp\
	Frelish.cpp\
	Icebank.cpp\
	StrongRune.cpp\
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
	libashe/Icebank.h\
	libashe/MFCLinkage.h\
	libashe/ManagedBinary.h\
	libashe/Number.h\
	libashe/SectionBlock2D.h\
	libashe/Semaphore.h\
	libashe/StrongRune.h\
	libashe/ThreadPool.h\
	libashe/TransitiveInterface.h\
	libashe/UUID.h\
	libashe/WeakRune.h\
	libashe/WebSocket.h

# pkg-config
pkgconfigdir = @libdir@/pkgconfig
pkgconfig_DATA = libashe.pc
