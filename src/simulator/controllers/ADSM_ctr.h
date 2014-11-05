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
#ifndef ADSM_CONTROLLER_H_
#define ADSM_CONTROLLER_H_

/* LIBRARIES*/
#include "controller.h"

/******************************************************************************/
class CADSMCtr : public CController {

	public:
	/* FUNCTIONS */
	CADSMCtr   ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode );
	~CADSMCtr  ( void );		
	
	void executionStep ( void );
	void restart       ( void );

	TVFloat getStatus     ( void );

	void setParameters ( TVFloat );

	bool flagLoadCtr ( bool input ){ active_ADSM=input; return active_ADSM;};
	bool flagLoadCtr ( void )      { return active_ADSM;};

	bool flagBatCtr ( bool input ) { active_bat=input; return active_bat;};
	bool flagBatCtr ( void )       { return active_bat;};

	float    getSelfC      ( void ){return (1.0 - (m_fGridEne/m_fLoadEne));};


	private:
	/* VARIABLES */	
	bool  active_ADSM;
	bool  active_bat;
	bool  m_bOverDC;
	float m_fSoC_min;
	float m_fSoC_max;

	/* State Variables */	
	float m_fLoadEne;
	float m_fPVEne;
	float m_fGridEne;

	/* Constants */
	

	/* FUNCTIONS */	
	int  _actionTime    ( TVFloat* , CDefLoad* );
	void _modifyPattern ( TVFloat* , CDefLoad* , int );

};

#endif

