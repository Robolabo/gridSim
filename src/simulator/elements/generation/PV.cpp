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
CPV::CPV ( sSimCnf*  sSimCnf , XMLElement* cnf ){
	/* General Simulator Configuration */
	m_sSimCnf = sSimCnf;
	/* Get input files (hourly data) */
	m_sPwGenFile = cnf->Attribute("gen");
	m_sPwFrcFile = cnf->Attribute("frc");
	m_fPAmp      = atof(cnf->Attribute("power"));

	_readAll( &m_sPwGenFile , &m_vPwGen );
	_readAll( &m_sPwFrcFile , &m_vPwFrc );
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
	m_fPower = m_fPAmp * m_vPwGen[(m_sSimCnf->nSimStep)%m_vPwGen.size()];	
	return;
};

/******************************************************************************/
float CPV::getNextHourFrc ( void ){
	float result;
	result = ( m_vPwFrc[ ( m_sSimCnf->nSimStep + 60 ) % m_vPwFrc.size() ] + m_vPwFrc[ m_sSimCnf->nSimStep % m_vPwFrc.size() ] ) / 2.0;
	return result;
};

/******************************************************************************/
void CPV::_readAll ( string* file_name , TVFloat* output ){
	ifstream inputFile ( file_name->c_str() );
	float    tmp_power, tmp_power_step;
	inputFile >> tmp_power;		
	output->push_back( tmp_power );		
	inputFile.ignore(256, '\n');
	for( int i = 1 ; i < 8760 ; i++ ){
		inputFile >> tmp_power;
		tmp_power_step = (tmp_power - output->back())/60.0;
		for ( int j = 0 ; j < 60 ; j++ )	
			output->push_back( output->back() + tmp_power_step );
		inputFile.ignore(256, '\n');
	}
	inputFile.close();
	return;
};


