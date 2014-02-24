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
/* Methods of storage.h	 						      */
/******************************************************************************/
#include "storage.h"

/******************************************************************************/
/* CONSTRUCTOR */
CStorage::CStorage ( sSimCnf*  sSimCnf , XMLElement* cnf ){
	string attr;
	/* General Simulator Configuration */
	m_sSimCnf = sSimCnf;	
	/* Create battery inverters */
	attr           = cnf->Attribute("num_inv");
	m_unNInverters = atoi( attr.c_str() );
	CBattInverter* tmp_batInv;
	for ( int i = 0 ; i < m_unNInverters ; i++ ){
		tmp_batInv = new CBattInverter ( ); // CHECK USE !!!
		m_VBatInverter.push_back( tmp_batInv );
	}
	/* Set capacity */
	attr        = cnf->Attribute("cap");
	setCapacity ( atof( attr.c_str() ) );

	return;
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CStorage::restart ( void ){
	/* Restart inverters */
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ ){
		m_VBatInverter[i]->restart();
	}
	return;
};

/******************************************************************************/
/* Set the storage capacity */
void CStorage::setCapacity ( float Cap ){
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )				
		m_VBatInverter[i]->setCapacity ( Cap );	
	return;
};

/******************************************************************************/
void CStorage::setChargePowLimit ( double fchargepow ){
	double fpower_ind = fchargepow / float( m_VBatInverter.size() );
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )				
		m_VBatInverter[i]->setChargePowLimit ( fpower_ind );	
	return;
};	

/******************************************************************************/
/* DESTRUCTOR */
CStorage::~CStorage ( void ){
	if ( m_VBatInverter.size() > 0 ){
		TBattInverterVite i;
   	 	for (i = m_VBatInverter.begin(); i != m_VBatInverter.end(); i++){        
    		    	delete (*i);
    		}
		m_VBatInverter.clear();
	}
	return;
};

/******************************************************************************/
double CStorage::getSoC ( void ){
	double result = 0.0;
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )
		result += m_VBatInverter[i]->getSoC();
	result /= float ( m_VBatInverter.size() );	
	return result;
};

/******************************************************************************/
double CStorage::getAvailablePower ( void ){
	double result = 0.0;
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )		
		result += m_VBatInverter[i]->getAvailablePower();	
	return result;
};

/******************************************************************************/
double CStorage::getAcceptedPower  ( void ){
	double result = 0.0;
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )		
		result += m_VBatInverter[i]->getAcceptedPower();	
	return result;
};

/******************************************************************************/
// From the battery
double CStorage::importPower ( double fpower ){
	double result     = 0.0;
	double fpower_ind = fpower / float( m_VBatInverter.size() );
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )
		result += m_VBatInverter[i]->importPower( fpower_ind );
	return result;
}; 

/******************************************************************************/
// To the battery
double CStorage::exportPower( double fpower ){
	double result     = 0.0;
	double fpower_ind = fpower / float( m_VBatInverter.size() );
	for ( int i = 0 ; i < m_VBatInverter.size() ; i++ )
		result += m_VBatInverter[i]->exportPower( fpower_ind );
	return result;
};
