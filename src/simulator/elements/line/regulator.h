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
/* LIBRARY REGULATOR							      */
/* It is the class of the regulators	                   		      */
/******************************************************************************/
#ifndef REGULATOR_H_
#define REGULATOR_H_

/* LIBRARIES*/
#include "common.h"

using namespace std;

/******************************************************************************/

class CRegulator {

	public:
	/* FUNCTIONS */
	CRegulator  ( void ){m_fPowerLimit=1000000000.0;};
	~CRegulator ( void ){};	

	void  restart ( void ){m_fPowerLimit=1000000000.0;};

	float getPowerLimit ( void ){return m_fPowerLimit;};
	void  setPowerLimit ( float PowerLimit ){m_fPowerLimit = PowerLimit;};

	private:
	/* VARIABLES */
	char* m_pchParamsFile;
	float m_fPowerLimit;

	/* FUNCTIONS */	
};

#endif

