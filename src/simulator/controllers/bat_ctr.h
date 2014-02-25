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
/* LIBRARY CONTROLLERS							      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef BATCONTROLLER_H_
#define BATCONTROLLER_H_

/* LIBRARIES*/
#include "controller.h"

/******************************************************************************/
class CBatCtr : public CController {

	public:
	/* FUNCTIONS */
	CBatCtr   ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode );
	~CBatCtr  ( void );		
	
	void executionStep ( void );
	void restart       ( void );

	int  getCycles     ( void ){return m_nCycle;};

	void setSoC_limits ( float l_min , float l_max ){m_fSoC_min=l_min;m_fSoC_max=l_max;};		

	private:
	/* VARIABLES */
	bool   m_bOverDC;
	float  m_fSoC_min;
	float  m_fSoC_max;
	float  m_fSoC_min_ini;
	float  m_fSoC_max_ini;	
	int    m_nBmode; // 0 - regulator, 1 - switch, 2 - both
	int    m_nCycle;

	/* FUNCTIONS */	
};

#endif

