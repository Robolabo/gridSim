/*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*    Copyright (C) 2014  Manuel Castillo Cagigal
*
*    Authors: Manuel Castillo Cagigal <manuel.castillo@upm.es>
*             Eduardo Matallanas de Avila <eduardo.matallanas@upm.es>
*	      Alvaro Gutierrez Martin <aguti@etsit.upm.es>
*
*    Developed in Robolabo <http://www.robolabo.etsit.upm.es/>
*/

/******************************************************************************/
/* LIBRARY COMMON 							      */
/* It contains common data types and structures				      */
/******************************************************************************/
#ifndef COMMON_H_
#define COMMON_H_

/****************************************************************/
/* MACROS */

/****************************************************************/
/* LIBRARIES */
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <complex>
#include <algorithm>
#include "tinyxml2.h"
#include "fft.h"
#include "random.h"
#include "datatypes.h"
#include "graphics_engine/plotter.h"

using namespace std;
using namespace tinyxml2;

class CWriter;

struct sGridProfile{
	int     dur;
	float   amp;
	TVFloat profile;
	TVFloat nr_profile;
};


struct sGridSCmp {
	int   cmp;
	float amp;
	float omega;
	float arg;
};
typedef vector<sGridSCmp> TVGridSCmp;

struct sDefLoad{
	int     id;
	int     dur;
	TVFloat profile;
};

typedef vector<sDefLoad> TVDefLoad;

struct sLoadDB{
	TVDefLoad def_load;
};

struct sSimCnf {	
	string       sParamsFile;
	string       sDataFolder;
	sLoadDB      LoadDB;
	sGridProfile GridProfile;
	CRandom*     pcRandom;	
	CWriter*     pcWriter;
	CPlotter*    pcPlotter;
	int          nSimStep;	
	int          nSampling;
	int          nFFTsize;
};

struct sStructure {
	int nLines;
	int nNodes;
};

struct sFFTeval {
	float   FFTave;
	float   FFTvar;
	float   FFTrel;
	float   FFTmax;
	TVFloat R, Phi, Nrm;
};

class CWriter {
	public:
	CWriter  ( XMLElement* cnf );
	~CWriter ( void );

	void write_buffer ( void );
	void push_buffer  ( float input ){ m_vBuffer.push_back( input );};

	private:
	string   m_sFileName;
	ofstream m_sOutStream;

	TVFloat  m_vBuffer;

};

#endif
















