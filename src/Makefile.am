SUBDIRS         = common graphics_engine simulator/main_control simulator/user simulator/controllers simulator/elements simulator/grid simulator 
CPPFLAGS        = -I../src -I../src/simulator -I../src/common -I../src/graphics_engine
bin_PROGRAMS    = gridSim
gridSim_SOURCES = ../src/main.cpp
gridSim_LDADD   = -Lsimulator -lsimulator
gridSim_LDFLAGS = -static
