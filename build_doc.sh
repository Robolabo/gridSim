#!/bin/bash

#Create folders if needed
if [ ! -d build ]; then
  mkdir build
fi

if [ ! -d build/doc ]; then
  mkdir build/doc
fi

#Go into building folder
cd build/doc

#Copy source files
cp ../../doc/manual_latex/* .

#Clean old version
if [[ -e gridsim_manual.dvi ]]; then
	make clean
fi

#Common make
make

#Copy executable in the main folder 
cp gridsim_manual.pdf ../..
