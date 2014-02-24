CPPFLAGS             = -I../../src -I../../src/common  
lib_LTLIBRARIES      = libcommon.la
libcommon_la_SOURCES = ../../src/common/common.cpp \ 
		       ../../src/common/tinyxml2.cpp \  
		       ../../src/common/random.cpp \		       
		       ../../src/common/fft.cpp		       
libcommon_la_LIBADD  = -lgsl -lgslcblas -lX11 -L/usr/X11R6/lib -lpthread
