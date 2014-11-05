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
	m_pcAir = NULL;
	/* General Simulator Configuration */
	m_sSimCnf = sSimCnf;	
	/* Setup elements */
	string elemName, attr;
	if ( cnf->Attribute("amp") ){
		attr   = cnf->Attribute("amp");
		m_fAmp = atof( attr.c_str() );
	}
	else{
		m_fAmp = 1.0;
	}	
	if ( cnf->Attribute("amp_nd") ){
		attr      = cnf->Attribute("amp_nd");
		m_fAmp_ND = atof( attr.c_str() );
	}
	else{
		m_fAmp_ND = 0.0;
	}
	if ( cnf->Attribute("type_nd") ){
		attr       = cnf->Attribute("type_nd");
		m_nType_ND = atoi( attr.c_str() );
		if ( m_nType_ND == 1 ){
			_read_FixedC_house ();
		}
	}
	else{
		m_nType_ND = 0;
	}
	for( XMLElement* elem = cnf->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "airconditioner" ){
			m_pcAir = new CAirConditioner( m_sSimCnf , elem );

		}
	}
	return;
};

/******************************************************************************/
void CLoad::_read_FixedC_house ( void ){
	float fNDcomp, tmp_float;
	ifstream cooker ( "data/def_load_profiles/cooker" );
	ifstream fridge ( "data/def_load_profiles/fridge" );

	for ( int i = 0 ; i < 1440 ; i++ ){
		fNDcomp  = 200.0;
		cooker >> tmp_float;
		fNDcomp += tmp_float;
		fridge >> tmp_float;
		fNDcomp += tmp_float;
		if ( ( i > 1259 ) && ( i < 1385 ) )
			fNDcomp  += 360.0;
		m_vFixedC.push_back( fNDcomp );
	}
	return;
}


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
	m_fPower     = 0.0;
	m_fDefPower  = 0.0;
	m_fNDefPower = 0.0;
	m_fDPower    = 0.0;
	m_bDPower    = false;
	if ( m_pcAir )
		m_pcAir->restart();
	for ( int i = 0 ; i < m_vDefLoad.size() ; i++ ){
		delete m_vDefLoad[i];
	}
	m_vDefLoad.clear();
	return;
};

/******************************************************************************/
/* Step execution */
void CLoad::executionStep( void ){
	m_fPower     = 0.0;	
	m_fDefPower  = 0.0;
	m_fNDefPower = 0.0;
	/* Air Conditioner */
	if ( m_pcAir ){
		m_pcAir->simulationStep( );
		m_fPower += m_pcAir->getInstantPower();
	}
	/* Deferrable Loads */	
	bool checking = true;
	int  cnt = 0;
	while(checking){
		if ( cnt >= m_vDefLoad.size() ){
			checking = false;
		}
		else{
			m_vDefLoad[cnt]->simulationStep();
			if ( m_vDefLoad[cnt]->isFinished() ){				
				delete m_vDefLoad[cnt];
				m_vDefLoad.erase( m_vDefLoad.begin() + cnt );
			}
			else{
				m_fDefPower += m_vDefLoad[cnt]->getPower();
				cnt++;
			}
		}
	}
	/* Power DEF amplitude */
	m_fDefPower *= m_fAmp;
	m_fPower    += m_fDefPower;

	/* No deferrable load */
	switch( m_nType_ND ){
		case 0:
			m_fNDefPower  = m_fAmp_ND;
			break;
		case 1:
			m_fNDefPower  = m_fAmp_ND * m_vFixedC[ ( m_sSimCnf->nSimStep )%( m_vFixedC.size() ) ];
			break;
		case 2:
			m_fNDefPower  = m_fAmp_ND * m_sSimCnf->GridProfile.nr_profile[ ( m_sSimCnf->nSimStep )%( m_sSimCnf->GridProfile.dur ) ];
			break;

		default:
			m_fNDefPower = 0.0;
			break;

	}
	m_fPower     += m_fNDefPower;
	/* Direct power */
	if ( m_bDPower ){
		m_fPower  = m_fDPower;
		m_bDPower = false;
	}	
	return;
};

/******************************************************************************/
void CLoad::setDirectPower    ( float fPower ){
	m_fDPower = fPower;
	m_bDPower = true;
	return;
};


