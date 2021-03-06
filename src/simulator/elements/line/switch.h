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
/* LIBRARY SWITCH							      */
/* It is the class of the switchs	                   		      */
/******************************************************************************/
#ifndef SWITCH_H_
#define SWITCH_H_

/* LIBRARIES*/
#include "common.h"

using namespace std;

/******************************************************************************/


class CSwitch {

	public:
	/* FUNCTIONS */
	CSwitch  ( void ){m_bOpen=false;};
	~CSwitch ( void ){};	

	void  restart ( void ){m_bOpen=false;};

	float isOpen    ( void ){return m_bOpen;};
	void  setStatus ( bool input ){m_bOpen=input;};

	private:
	/* VARIABLES */
	bool m_bOpen;

	/* FUNCTIONS */	
};

#endif

