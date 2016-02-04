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
/* LIBRARY DEFAULT MAIN CONTROL						      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef DEFAULT_MC_H_
#define DEFAULT_MC_H_

/* LIBRARIES*/
#include "main_control.h"

/******************************************************************************/
class CDefault_MC : public CMainControl {

	public:
	/* FUNCTIONS */
	CDefault_MC   ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf );
	~CDefault_MC  ( void );		
	
	void  executionStep  ( void );
	void  assessmentStep ( void );
	void  restart        ( void );		

	TVFloat* getEvaluation  ( void );
	void     setEnvironment ( TVFloat );

	private:

	float    m_fPVenergy;

	TVFloat  m_vTimeSignal;
	TVFloat  m_vSampledSig;
	TVFloat  m_vLineSignal;	

	TVFloat  m_vEvaluation;
		
};

#endif




