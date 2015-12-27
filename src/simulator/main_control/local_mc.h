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
/* LIBRARY SWARMGRID MAIN CONTROL					      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef LOCAL_MC_H_
#define LOCAL_MC_H_

/* LIBRARIES*/
#include "main_control.h"

/******************************************************************************/
class CLocal_MC : public CMainControl {

	public:
	/* FUNCTIONS */
	CLocal_MC   ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf );
	~CLocal_MC  ( void );		
	
	void  executionStep  ( void );
	void  assessmentStep ( void );
	void  restart        ( void );		

	TVFloat* getEvaluation  ( void );
	float    getAssessment  ( void );

	private:
	TVFloat  m_vTimeSignal;
	TVFloat  m_vSampledSig;
	TVFloat  m_vNodeSignal;		
	TVFloat  m_vGridSig;

	int   m_nWbeg, m_nWend;

	TVFloat m_Result;


	/* Writer */
	float   m_Wgrid;
	float   m_Wnode;
	float   m_Wgrid_base;
	TVFloat m_Wstatus;
	TVFloat m_Wstatus1;
	TVFloat m_Wstatus2;


	/* FUNCTIONS */	
	void _getInfo ( void );
	
		
};

#endif




