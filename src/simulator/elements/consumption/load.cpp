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
/* METHODS OF LOAD	 						      */
/******************************************************************************/
#include "load.h"

/******************************************************************************/
/* CONSTRUCTOR */
CLoad::CLoad ( sSimCnf*  sSimCnf , XMLElement* cnf ){
	string elemName, attr;

	m_pcAir = NULL;

	/* General Simulator Configuration */
	m_sSimCnf = sSimCnf;
	/* Configure Load */
	attr   = cnf->Attribute("type");
	m_nType = atoi( attr.c_str() );

	/* Setup elements */
	for( XMLElement* elem = cnf->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "airconditioner" ){
			m_pcAir = new CAirConditioner( m_sSimCnf , elem );

		}
	}
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CLoad::~CLoad ( void ){

	for ( int i = 0 ; i < m_vDefLoad.size() ; i++ )
		delete m_vDefLoad[i];
	m_vDefLoad.clear();

	return;
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CLoad::restart           ( void ){
	if ( m_pcAir )
		m_pcAir->restart();
	return;
};

/******************************************************************************/
/* Step execution */
void CLoad::executionStep( void ){

	switch ( m_nType ){
		/*		
		case 1:
      			m_fPower = _executeSinusoidal();
			break;
		case 2:
			m_fPower = _executeProfiles(); 
			break;
		*/	
		default:
			m_fPower = 0.0;
			break;		
	}	

	/* Air Conditioner */
	if ( m_pcAir ){
		m_pcAir->simulationStep( );
		m_fPower += m_pcAir->getInstantPower();
	}
	/* Deferrable Loads */	
	//cout << " LOAD: " << m_sSimCnf->GridProfile.profile[ (m_sSimCnf->nSimStep)%(m_sSimCnf->GridProfile.dur) ] << endl;
	for ( int i = 0 ; i < m_vDefLoad.size(); i++ ){
		m_vDefLoad[i]->simulationStep();
		if ( m_vDefLoad[i]->isFinished() ){
			delete m_vDefLoad[i];
			m_vDefLoad.erase( m_vDefLoad.begin() + i );
		}
		else
			m_fPower += m_vDefLoad[i]->getPower();
	}


	return;
};





