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
#ifndef MUFCO_CONTROLLER_H_
#define MUFCO_CONTROLLER_H_

/* LIBRARIES*/
#include "controller.h"

/******************************************************************************/
class CMUFCOCtr : public CController {

	public:
	/* FUNCTIONS */
	CMUFCOCtr   ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode );
	~CMUFCOCtr  ( void );		
	
	void executionStep ( void );
	void restart       ( void );

	void sendFFT       ( TVFreqCmp* , sFFTeval* );

	int     getCmp        ( void ){ return m_nComp; };
	int     getFCreject   ( void ){ return m_nR; };
	TVFloat getStatus     ( void );

	void setParameters ( TVFloat );


	private:
	/* VARIABLES */
	TVInt m_vSetLoads;
	int   m_nIniCmp;
	/* MUFCO parameter */
	int   m_CType;	
	float m_fK;	
	float m_fP;
	int   m_nR;

	float m_fAmp;
	float m_fOffset;

	/* State Variables */
	int   m_nComp;
	float m_fPeriod;
	float m_fOmega;
	float m_fArgument;
 	float m_fPhase;
	

	/* FUNCTIONS */	
	void _mufcoStep  ( TVFreqCmp* , sFFTeval* );
	int  _actionTime ( void );

};

#endif

