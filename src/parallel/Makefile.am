CPPFLAGS                 = -I../../src -I../../src/simulator -I../../src/common -I../../src/graphics_engine
bin_PROGRAMS             = gridSim_parallel
gridSim_parallel_SOURCES = ../../src/parallel/main.cpp ../../src/common/tinyxml2.cpp
gridSim_parallel_LDADD   = -L../simulator -lsimulator
gridSim_parallel_LDFLAGS = -static
