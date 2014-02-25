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
/* LIBRARY ISFOC EVALUATION						      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef ISFOC_EVAL_H_
#define ISFOC_EVAL_H_

/* LIBRARIES*/
#include "main_control.h"

/******************************************************************************/
class CIsfocEval : public CMainControl {

	public:
	/* FUNCTIONS */
	CIsfocEval   ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf );
	~CIsfocEval  ( void );		
	
	void  executionStep  ( void );
	void  restart        ( void );	

	void    setEnvironment ( TVFloat input );	
	void    setParameters  ( TVFloat input );	

	TVFloat* getEvaluation  ( void );//{return m_vSC.back();};

	private:
	/* VARIABLES */
	sNEnergy m_sNEnergy;
	TVFloat  m_vPV;
	TVFloat  m_vGrid;
	TVFloat  m_vLoad;
	TVFloat  m_vBat;
	TVFloat  m_vSC;

	TVFloat  m_vResult;

	CNode*   m_pcNode;

	/* FUNCTIONS */	
};

#endif

