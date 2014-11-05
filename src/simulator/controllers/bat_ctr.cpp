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
/* Methods of bat_ctr.h 						      */
/******************************************************************************/
#include "bat_ctr.h"

/******************************************************************************/
/* CONSTRUCTOR */
CBatCtr::CBatCtr ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode ) : CController::CController ( sSimCnf , pcNode ) {	
	m_bOverDC = true;
	m_nCycle  = 0;
	if ( cnf ){
		m_fSoC_min_ini = atof( cnf->Attribute("soc_min") );
		m_fSoC_max_ini = atof( cnf->Attribute("soc_max") );
		m_nBmode       = atoi( cnf->Attribute("b_mode")  );
	}
	m_fSoC_min = m_fSoC_min_ini;
	m_fSoC_max = m_fSoC_max_ini;
};

	

/******************************************************************************/
/* DESTRUCTOR */
CBatCtr::~CBatCtr ( void ){
	
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CBatCtr::restart ( void ){
	m_bOverDC  = true;
	m_nCycle   = 0;
	m_fSoC_min = m_fSoC_min_ini;
	m_fSoC_max = m_fSoC_max_ini;
	return;
};

/******************************************************************************/
/* Execution Step */
void CBatCtr::executionStep( void ){
	double fPV, fLoad, fSoC;
	/* Get data */
	fPV   = m_pcNode->getPV()->getPower();
	fLoad = m_pcNode->getLoad()->getPower();	
	fSoC  = m_pcNode->getStorage()->getSoC();
	/* Calculate Over Discharge state */	
	if ( fSoC > m_fSoC_max ){
		if ( m_bOverDC ){				
			m_nCycle++;
		}
		m_bOverDC = false;
	}
	else if ( fSoC < m_fSoC_min )
		m_bOverDC = true;
	/* Calculate operation */
	float fChargePower, fGridPower;
	bool  bSw_st;
	if ( m_bOverDC ){	
		bSw_st = false;
		fGridPower = 1000000000.0;	
		if ( fPV > fLoad )			
			fChargePower = fPV - fLoad;		
		else
			fChargePower = 0.0;			
		
	}
	else {
		bSw_st = true;
		fGridPower = fPV;
		if ( fPV > fLoad )			
			fChargePower = fPV - fLoad;		
		else
			fChargePower = 0.0;		
	}
	/* Set bat inverter, regulator and switch */
	if ( m_nBmode == 0 || m_nBmode == 2 )
		m_pcNode->getRegulator()->setPowerLimit   ( fGridPower   );
	if ( m_nBmode == 1 || m_nBmode == 2 )
		m_pcNode->getSwitch()->setStatus( bSw_st );	
	m_pcNode->getStorage()->setChargePowLimit ( fChargePower );		
	return;
};

/******************************************************************************/
TVFloat CBatCtr::getStatus ( void ){
	TVFloat result;	
	result.push_back( m_pcNode->getPV()->getPower() );
	result.push_back( m_pcNode->getLoad()->getPower() );
	result.push_back( m_pcNode->getStorage()->getSoC() );
	return result;
};













