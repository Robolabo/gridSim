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

/****************************************************************/
/* LINE METHODS							*/
/****************************************************************/
#include "line.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
CLine::CLine ( sSimCnf*  sSimCnf ){
	/* Simulator stuff */
	m_sSimCnf = sSimCnf;	
	m_vPwGen.clear();
	m_vPwFrc.clear();
};

/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
CLine::~CLine ( void ){		
	for ( int i = 0 ; i < m_vNodes.size() ; i++ )
		delete m_vNodes[i];
	m_vNodes.clear();
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CLine::restart ( void ){	
	for ( int i = 0 ; i <  m_vNodes.size() ; i++ ){
		m_vNodes[i]->restart();
	}
	return;
};

/****************************************************************/
/* EXECUTION FUNCTION						*/
/****************************************************************/
void CLine::executionStep ( void ){	
	for ( int i = 0 ; i < m_vNodes.size() ; i++ )
		m_vNodes[i]->executionStep ();
	m_fPower = 0.0;
	for ( int i = 0 ; i < m_vNodes.size() ; i++ )
		m_fPower += m_vNodes[i]->getNPower()->grid;
	return;
};





