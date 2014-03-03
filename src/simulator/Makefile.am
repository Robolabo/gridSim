CPPFLAGS                = -I../../src -I../../src/common -I../../src/simulator -I../../src/graphics_engine
lib_LTLIBRARIES         = libsimulator.la
libsimulator_la_SOURCES = ../../src/simulator/simulator.cpp
libsimulator_la_LIBADD  = -L../common -lcommon ../graphics_engine/libGEngine.la elements/libelements.la grid/libgrid.la controllers/libcontrollers.la user/libuser.la main_control/libmain_control.la
			  	
