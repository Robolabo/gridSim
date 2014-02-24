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
			/*
			sAirConditioner strAirConditioner;
			strAirConditioner.fInitTemp 	= 25.0;
			strAirConditioner.fRefTemp 	= 25.0;
			strAirConditioner.fEpsilonTemp 	= 0.5;
			strAirConditioner.fFanIntensity	= 0.3;
			strAirConditioner.fDecayFactor 	= 0.001;
			strAirConditioner.fMaxDeltaT 	= 0.05; 
			strAirConditioner.fMaxPower	= 10000;
			*/
			m_pcAir = new CAirConditioner( m_sSimCnf , elem );

		}
	}	
	

	//m_fPeriod   = 0.0;
	//m_fPhase    = 0.0;
	//m_fArgument = 0.0;

	return;
};

/******************************************************************************/
/*
void CLoad::setLoadDB  ( TMFloat* input ){ 
	m_mLoadDB = input; 
	// No Deferrable Load
	float tmp_power;
	for ( int i = 0 ; i < 1440 ; i++ ){
		tmp_power = 200;	
		// Lights
		if ( i >= 1259 && i <= 1385 )
			tmp_power += 360.0;
		// Fridge
		tmp_power += (*m_mLoadDB)[ 3 ][ i ];
		// Oven
		tmp_power += (*m_mLoadDB)[ 4 ][ i ];

		m_vNDefLoad.push_back( tmp_power );
	}
};	
*/
/******************************************************************************/
/* DESTRUCTOR */
CLoad::~CLoad ( void ){
	return;
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CLoad::restart           ( void ){

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

	if ( m_pcAir ){
		m_pcAir->simulationStep( );
		m_fPower += m_pcAir->getInstantPower();
	}

	return;
};

/******************************************************************************/
/*
void CLoad::setDefLoad ( sDefLoad input ){
	if ( (*m_mLoadDB)[ input.type ].size() < m_vDefLoad.size() ){
		for ( int i = 0 ; i < (*m_mLoadDB)[ input.type ].size() ; i++ )
			 m_vDefLoad[i] += (*m_mLoadDB)[ input.type ][ i ];
	}
	else{
		for ( int i = 0 ; i < m_vDefLoad.size() ; i++ )
			 m_vDefLoad[ i ] += (*m_mLoadDB)[ input.type ][ i ];
		for ( int i = m_vDefLoad.size() ; i < (*m_mLoadDB)[ input.type ].size() ; i++ )
			 m_vDefLoad.push_back ( (*m_mLoadDB)[ input.type ][ i ] );
	}
	return;
};
*/
/******************************************************************************/
/*
float CLoad::_executeProfiles   ( void ){
	float  result = 0.0;		
	// No Deferrable 
	result += m_vNDefLoad[(m_sSimCnf->nSimStep)%1440];
	
	// Deferrable 		
	if ( m_vDefLoad.size() > 0 ){
		result += m_vDefLoad.front();
		m_vDefLoad.erase( m_vDefLoad.begin() );
	}
	return result;
};
*/
/******************************************************************************/	
/*
float CLoad::_executeSinusoidal ( void ){
	float  result;
	if ( m_fPeriod == 0.0 ){
		m_fArgument = 0.0;
		result      = 1000.0;
	}	
	else{
		m_fArgument = TWO_PI * ( float ( m_sSimCnf->nSimStep ) / m_fPeriod - int ( float ( m_sSimCnf->nSimStep ) / m_fPeriod ) ) + m_fPhase;
		if ( m_fArgument > M_PI )
			m_fArgument -= TWO_PI;
		else if ( m_fArgument < -M_PI )
			m_fArgument += TWO_PI;
		result      = 1000.0 * ( 1.0 + cos ( m_fArgument ) );
	}

	return result;
};
*/





