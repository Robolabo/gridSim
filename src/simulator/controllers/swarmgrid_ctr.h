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
#ifndef SWARMGRID_CONTROLLER_H_
#define SWARMGRID_CONTROLLER_H_

/* LIBRARIES*/
#include "controller.h"

#define SW_COS_DEF 4096

/******************************************************************************/
class CSwarmGridCtr : public CController {

	public:
	/* FUNCTIONS */
	CSwarmGridCtr   ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode );
	~CSwarmGridCtr  ( void );		
	
	void executionStep ( void );
	void restart       ( void );

	void sendFFT       ( TVFreqCmp* , sFFTeval* );

	int     getCmp        ( void ){ return m_nComp; };
	int     getFCreject   ( void ){ return m_nR; };

	TVFloat getStatus     ( void );

	float   getSelfC      ( void ){return (1.0 - (m_fGridEne/m_fLoadEne));};

	void setParameters ( TVFloat );

	bool flagLoadCtr ( bool input ){ m_bAct=input; return m_bAct;};
	bool flagLoadCtr ( void )      { return m_bAct;};

	bool flagBatCtr ( bool input ) { active_bat=input; return active_bat;};
	bool flagBatCtr ( void )       { return active_bat;};


	private:
	/* VARIABLES */	
	/* MUFCO parameter */
	float m_fK;	
	float m_fP;
	float m_fB;
	int   m_nR;

	bool m_bAct;
	bool  active_bat;

	bool  m_bOverDC;
	float m_fSoC_min;
	float m_fSoC_max;

	sFFTeval* m_sFFTst; 

	/* State Variables */
	int   m_nComp;
	float m_fPeriod;
	float m_fOmega;
	float m_fArgument;
 	float m_fPhase;

	float m_fLoadEne;
	float m_fPVEne;
	float m_fGridEne;

	/* Constants */
	float COS[SW_COS_DEF];
	float M_NRM;

	/* FUNCTIONS */	
	void _mufcoStep   ( TVFreqCmp* , sFFTeval* );
	int  _actionTime  ( TVFloat* );
	void _genGPattern ( TVFloat* , int , int);
	void _genPattern  ( TVFloat* , TVFloat* , TVFloat* );

};

#endif

