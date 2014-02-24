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
/* LINE CLASS							*/
/****************************************************************/
#ifndef LINE_H_
#define LINE_H_

class CLine;

/****************************************************************/
/* LIBRARIES */
#include "common.h"
#include "node.h"

typedef vector<CLine*> TVCLine;

class CLine {
	public:
	CLine  ( sSimCnf*  sSimCnf );
	~CLine ( void    );
	
	void  executionStep ( void );
	void  restart       ( void );
	

	float getPower      ( void )         {return m_fPower;};
	void  addNode       ( CNode* input ) {m_vNodes.push_back(input);};

	TVCNode* getNodes   ( void ){return &m_vNodes;};           
	
	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;
	/* Nodes vector */
	TVCNode m_vNodes;
	/* Current power */
	float   m_fPower;	
};
#endif
