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

#include "airconditioner.h"

/******************************************************************************/
/* CONSTRUCTOR */
CAirConditioner::CAirConditioner ( sSimCnf*  sSimCnf , XMLElement* cnf ) {
	m_sSimCnf = sSimCnf;
	string attr;	
	/* Load init vars */
	attr                            = cnf->Attribute("ini_t");
	m_sAirConditioner.fInitTemp     = atof( attr.c_str() );
	attr                            = cnf->Attribute("ref_t");
	m_sAirConditioner.fRefTemp      = atof( attr.c_str() );
	attr                            = cnf->Attribute("eps_t");
	m_sAirConditioner.fEpsilonTemp  = atof( attr.c_str() );
	attr                            = cnf->Attribute("fan_i");
	m_sAirConditioner.fFanIntensity = atof( attr.c_str() );
	attr                            = cnf->Attribute("dec_f");
	m_sAirConditioner.fDecayFactor  = atof( attr.c_str() );
	attr                            = cnf->Attribute("del_t");
	m_sAirConditioner.fMaxDeltaT    = atof( attr.c_str() );
	attr                            = cnf->Attribute("power");
	m_sAirConditioner.fMaxPower     = atof( attr.c_str() );
	m_sIniCnf = m_sAirConditioner;
	m_nStatus = 0;
	
	/* Load outdoor temp */    	
	ifstream inputFile ( cnf->Attribute("ext_file") );	
	// Open file 
  	if (inputFile.is_open()){
		float    tmp_temp, tmp_temp_step;
		inputFile >> tmp_temp;		
		if ( tmp_temp < -25.0 )
			tmp_temp = 0.0;
		m_vfOutdoorTemp.push_back( tmp_temp );		
		inputFile.ignore(256, '\n');
		for( int i = 1 ; i < 8760 ; i++ ){
			inputFile >> tmp_temp;
			if ( tmp_temp < -25.0 )
				tmp_temp = m_vfOutdoorTemp.back();
			tmp_temp_step = (tmp_temp - m_vfOutdoorTemp.back())/60.0;
			for ( int j = 0 ; j < 60 ; j++ )	
				m_vfOutdoorTemp.push_back( m_vfOutdoorTemp.back() + tmp_temp_step );
			inputFile.ignore(256, '\n');
		}
		inputFile.close();
	}
	// If file not open, go for default value 
	else{
		m_vfOutdoorTemp.push_back(22.0);
	}
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CAirConditioner::restart ( void ){
	m_sAirConditioner = m_sIniCnf;
	m_nStatus         = 0;
	/* Check fan Intentensity is between bounds */
	setFanIntensity(m_sAirConditioner.fFanIntensity);
	/* Set indoor temp to init temp */
	m_fIndoorTemp = m_sAirConditioner.fInitTemp;
	return;
};


/**************************************************
 **************************************************/

void CAirConditioner::simulationStep ( void ){
	/* Get OutdoorTemp from vector. Remember it is a cyclid vector */
	float fOutdoorTemp = m_vfOutdoorTemp.at(m_sSimCnf->nSimStep%m_vfOutdoorTemp.size());

 	/* Calc free dynamics because of OutdoorTemp */	
	float tmp_mod;
	if ( fOutdoorTemp - m_fIndoorTemp > 0 )
		tmp_mod = m_sAirConditioner.fDecayFactor;// * sig( fOutdoorTemp - m_fIndoorTemp );
	else
		tmp_mod = -m_sAirConditioner.fDecayFactor;
	m_fIndoorTemp += tmp_mod;

	/* Calc temp and power because of HVAC */	
	/* Cal intensity because of fan and temperature difference and saturate to maxDeltaT*/	
	float fIntensity = m_sAirConditioner.fMaxDeltaT * m_sAirConditioner.fFanIntensity;
	/* Calculate Air Conditioner operation */
	float eps1 = m_sAirConditioner.fEpsilonTemp;
	float eps2 = m_sAirConditioner.fEpsilonTemp/2.0;

	if ( m_fIndoorTemp < m_sAirConditioner.fRefTemp - eps1 )
		m_nStatus = 1; // Go up
	else if ( m_fIndoorTemp < m_sAirConditioner.fRefTemp - eps2 ){
		if ( m_nStatus == 2 )
			m_nStatus = 0; // Stop
	}
	
	if ( m_fIndoorTemp > m_sAirConditioner.fRefTemp + eps1 )
		m_nStatus = 2; // Go down

	else if ( m_fIndoorTemp > m_sAirConditioner.fRefTemp + eps2  ){
		if ( m_nStatus == 1 )
			m_nStatus = 0; // 
	}
	
	if ( m_nStatus == 1 ){ // Going up
		m_fIndoorTemp += fIntensity;
		m_fPower       = m_sAirConditioner.fMaxPower * m_sAirConditioner.fFanIntensity;
	}
	else if ( m_nStatus == 2 ){ // Going down
		m_fIndoorTemp -= fIntensity;
		m_fPower       = m_sAirConditioner.fMaxPower * m_sAirConditioner.fFanIntensity; 
	}	
	else
		m_fPower       = 0;
}

/**************************************************
 **************************************************/

float CAirConditioner::getIndoorTemp ( void )
{
	return m_fIndoorTemp;
}

/**************************************************
 **************************************************/

float CAirConditioner::getReferenceTemp ( void )
{
	return m_sAirConditioner.fRefTemp;
}

/**************************************************
 **************************************************/

void CAirConditioner::setReferenceTemp ( float f_temp )
{

	m_sAirConditioner.fRefTemp = f_temp;
}

/**************************************************
 **************************************************/

float CAirConditioner::getFanIntensity ( void )
{
	return m_sAirConditioner.fFanIntensity;
}

/**************************************************
 **************************************************/

void CAirConditioner::setFanIntensity ( float f_value )
{
	if ( f_value > 1.0  )
		f_value = 1.0;
	if (f_value < 0.0 )
		f_value = 0.0;

	m_sAirConditioner.fFanIntensity = f_value;
}

/**************************************************
 **************************************************/

float CAirConditioner::getInstantPower			( void )
{
	return m_fPower;
}
 
