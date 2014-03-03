#!/bin/bash

#Create folders if needed
if [ ! -d build ]; then
  mkdir build
fi

if [ ! -d build/common ]; then
  mkdir build/common
fi

if [ ! -d build/simulator ]; then
  mkdir build/simulator
fi

if [ ! -d build/simulator/grid ]; then
  mkdir build/simulator/grid
fi

if [ ! -d build/simulator/elements ]; then
  mkdir build/simulator/elements
fi

if [ ! -d build/simulator/controllers ]; then
  mkdir build/simulator/controllers
fi

if [ ! -d build/simulator/user ]; then
  mkdir build/simulator/user
fi

if [ ! -d build/simulator/main_control ]; then
  mkdir build/simulator/main_control
fi

if [ ! -d build/graphics_engine ]; then
  mkdir build/graphics_engine
fi

#Go into building folder
cd build

#Copy autotools files
if [[ ! -e configure.ac ]]; then
	cp ../src/configure.ac       .
fi
cp ../src/Makefile.am                        .
cp ../src/common/Makefile.am                 common
cp ../src/simulator/Makefile.am              simulator
cp ../src/simulator/grid/Makefile.am         simulator/grid
cp ../src/simulator/elements/Makefile.am     simulator/elements
cp ../src/simulator/controllers/Makefile.am  simulator/controllers
cp ../src/simulator/main_control/Makefile.am simulator/main_control
cp ../src/simulator/user/Makefile.am         simulator/user
cp ../src/graphics_engine/Makefile.am        graphics_engine

#First installation

if [[ ! -e ltmain.sh ]]; then
        libtoolize
fi
if [[ ! -e configure ]]; then
	autoreconf
fi
if [[ ! -e Makefile.in ]]; then
	automake --include-deps --add-missing --foreign
fi
if [[ ! -e Makefile ]]; then
	./configure
fi

#Remove the binary
if [[ -e gridSim ]]; then
	rm gridSim
fi

#Common make
make

#Copy executable in the main folder 
cp gridSim ..
