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
/* METHODS OF PV	 						      */
/******************************************************************************/
#include "PV.h"

/******************************************************************************/
/* CONSTRUCTOR */
CPV::CPV ( sSimCnf*  sSimCnf , XMLElement* cnf , TVFloat* pv_gen , TVFloat* pv_frc ){
	/* General Simulator Configuration */
	m_sSimCnf = sSimCnf;
	/* Get input files (hourly data) */
	m_sPwGenFile = cnf->Attribute("gen");
	m_sPwFrcFile = cnf->Attribute("frc");
	m_fPAmp      = atof(cnf->Attribute("power"));
	/* Get profiles */
	if ( pv_gen->size() == 0 ){
		_readAll( &m_sPwGenFile , pv_gen );
	}
	if ( pv_frc->size() == 0 ){
		_readAll( &m_sPwFrcFile , pv_frc );
	}
	m_vPwGen = pv_gen;
	m_vPwFrc = pv_frc;
	return;
};

/******************************************************************************/
void CPV::restart ( void ){
	m_fPower = 0.0;
	return;
};	

/******************************************************************************/
/* DESTRUCTOR */
CPV::~CPV ( void ){
	return;
};

/******************************************************************************/
/* Step execution */
void CPV::executionStep( void ){	
	m_fPower = m_fPAmp * m_vPwGen->at( m_sSimCnf->nSimStep % m_vPwGen->size() );
	return;
};

/******************************************************************************/
float CPV::getNextHourFrc ( void ){
	float result;
	result = ( m_vPwFrc->at( ( m_sSimCnf->nSimStep + 60 ) % m_vPwFrc->size() ) + m_vPwFrc->at( m_sSimCnf->nSimStep % m_vPwFrc->size() ) ) / 2.0;
	return result;
};

/******************************************************************************/
TVFloat CPV::getNextDayFrc   ( int time_ref ){
	TVFloat result;
	result = getNextDayNrFrc( time_ref );
	for ( int i = 0 ; i < result.size() ; i++ ){
		result[i] *= m_fPAmp;
	}
	return result;
};

/******************************************************************************/
TVFloat CPV::getNextDayNrFrc ( int time_ref ){
	TVFloat result;
	for ( int i = 0 ; i < 1440 ; i++ ){
		result.push_back( m_vPwFrc->at( ( i + time_ref ) % m_vPwFrc->size() ) );
	}
	return result;
};

/******************************************************************************/
TVFloat CPV::getRangeNrFrc ( int begin , int end ){
	TVFloat result;
	for ( int i = begin ; i < end ; i++ ){
		result.push_back( m_vPwFrc->at ( ( i ) % m_vPwFrc->size() ) );
	}
	return result;
};

/******************************************************************************/
void CPV::_readAll ( string* file_name , TVFloat* output ){
	// Read from file and interpolate
	string file_address;	
	file_address.assign ( m_sSimCnf->sDataFolder );
	file_address.append ( *file_name );
	ifstream  inputFile ( file_address.c_str() );
	float     tmp_power, tmp_power_old, tmp_power_step;
	inputFile >> tmp_power_old;		
	inputFile.ignore(256, '\n');
	while( !inputFile.eof() ){	
		inputFile >> tmp_power;
		tmp_power_step = (tmp_power - tmp_power_old)/60.0;
		for ( int i = 0 ; i < 60 ; i++ )	
			output->push_back( tmp_power_old + tmp_power_step * float(i) );
		inputFile.ignore(256, '\n');
		tmp_power_old = tmp_power;
	}
	inputFile.close();
	// Normalize
	float max = 0.0;
	for ( int i = 0 ; i < output->size() ; i++ ){
		if ( output->at(i) > max ){
			max = output->at(i);
		}
	}
	for ( int i = 0 ; i < output->size() ; i++ ){
		output->at(i) /= max;
	}
	return;
};

























