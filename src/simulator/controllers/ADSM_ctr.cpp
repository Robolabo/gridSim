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
/* Methods of ADSM_ctr.h						      */
/******************************************************************************/
#include "ADSM_ctr.h"

/******************************************************************************/
/* CONSTRUCTOR */
CADSMCtr::CADSMCtr ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode ) : CController::CController ( sSimCnf , pcNode ) {	
	if ( cnf->Attribute("active_ADSM") ){		
		active_ADSM = bool(atoi( cnf->Attribute("active_ADSM") ));
	}	
	else{
		active_ADSM = false;
	}
	if ( cnf->Attribute("active_bat") ){		
		active_bat = bool(atoi( cnf->Attribute("active_bat") ));
		if ( active_bat ){
			m_fSoC_min = atof( cnf->Attribute("soc_min") );
			m_fSoC_max = atof( cnf->Attribute("soc_max") );
		}
	}	
	else{
		active_bat = false;
	}
};	

/******************************************************************************/
/* DESTRUCTOR */
CADSMCtr::~CADSMCtr ( void ){
	
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CADSMCtr::restart ( void ){
	// Reset energies
	m_fLoadEne = 0.0;
	m_fGridEne = 0.0;
	m_fPVEne   = 0.0;
	return;
};

/******************************************************************************/
/* Execution Step */
void CADSMCtr::executionStep( void ){
	/* SC calculation */
	sNPower* power_info = m_pcNode->getNPower ();
	m_fLoadEne += power_info->load;	
	m_fPVEne   += power_info->pv;
	if ( power_info->grid > 0){
		m_fGridEne += power_info->grid;
	}

	// ADSM controller
	if ( active_ADSM ){
		// Get Loads
		TVCDefLoad* vDefLoad = m_pcNode->getLoad()->getDefLoads();	
		// Execute the not set loads
//		int     ac_time;
//		TVFloat vPattern = m_pcNode->getPV()->getNextDayFrc( m_sSimCnf->nSimStep );
//		cout << "DL: ";
//		for ( int i = 0 ; i < vDefLoad->size() ; i++ ){
//			if ( !vDefLoad->at(i)->isCtrFlag() ){
//				ac_time = _actionTime( &vPattern , vDefLoad->at(i) );
//				_modifyPattern( &vPattern , vDefLoad->at(i) , ac_time );
//				vDefLoad->at(i)->setStartTime ( m_sSimCnf->nSimStep + ac_time );
//				vDefLoad->at(i)->setCtrFlag   ( true );
//				cout << m_sSimCnf->nSimStep + ac_time << " ";
//			}
//		}
//		cout << endl;
		if ( vDefLoad->size() > 0 ){
		if ( !vDefLoad->at(0)->isCtrFlag() ){
			int     ac_time = 0;
			TVFloat vPattern = m_pcNode->getPV()->getNextDayFrc( m_sSimCnf->nSimStep );			
			for ( int i = 0 ; i < vDefLoad->size() ; i++ ){
				if ( !vDefLoad->at(i)->isCtrFlag() ){
					ac_time = _actionTime( &vPattern , vDefLoad->at(i) );
					_modifyPattern( &vPattern , vDefLoad->at(i) , ac_time );
					vDefLoad->at(i)->setStartTime ( m_sSimCnf->nSimStep + ac_time );
					vDefLoad->at(i)->setCtrFlag   ( true );					
				}
			}		
		}
		}
	}
	// Bat controller
	if ( active_bat ){
		double fPV, fLoad, fSoC;
		// Get data 
		fPV   = m_pcNode->getPV()->getPower();
		fLoad = m_pcNode->getLoad()->getPower();
		fSoC  = m_pcNode->getStorage()->getSoC();
		// Calculate Over Discharge state 	
		if ( fSoC > m_fSoC_max ){			
			m_bOverDC = false;
		}
		else if ( fSoC < m_fSoC_min ){
			m_bOverDC = true;
		}
		// Calculate operation 
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
		// Set bat inverter, regulator and switch 
		m_pcNode->getRegulator()->setPowerLimit   ( fGridPower   );
		m_pcNode->getStorage()->setChargePowLimit ( fChargePower );
	}
	return;
};

/******************************************************************************/
TVFloat CADSMCtr::getStatus ( void ){
	TVFloat result;

	sNPower* power = m_pcNode-> getNPower(); 
	result.push_back( power->load );
	result.push_back( power->pv   );
	result.push_back( power->bat  );
	result.push_back( power->grid );

	/* PV SELF-CONSUMPTION */
	float self_consumption;	
	self_consumption = min ( power->pv , power->load );
	result.push_back( self_consumption );
	
//	if ( m_pcNode->getPV() ){
//		result.push_back( m_pcNode->getPV()->getPower() );
//	}
//	result.push_back( m_pcNode->getLoad()->getPower() );
//	result.push_back( m_pcNode->getLoad()->getDefPower() );
//	result.push_back( m_pcNode->getLoad()->getNDefPower() );
//	if ( m_pcNode->getStorage() ){
//		result.push_back( m_pcNode->getStorage()->getPower() );
//	}
	return result;
};

/******************************************************************************/
int CADSMCtr::_actionTime( TVFloat* vec , CDefLoad* def_load ){  // Relative to the beginning of the day
	int result = 0;
	sDefLoad* load_info = def_load->getLoadInfo();
	float fAmp      = m_pcNode->getLoad()->getAmp();
	float max_sc, tmp_sc;
	for ( int i = 0 ; i < ( vec->size() - load_info->dur  ); i++ ){
		tmp_sc = 0.0;
		for ( int j = 0 ; j < load_info->dur ; j++ ){
			//tmp_sc += min( vec->at(i+j), fAmp * load_info->profile[j] );
			tmp_sc += vec->at(i+j);
		}
		if ( i == 0 ){
			max_sc = tmp_sc;
		}
		else if ( tmp_sc > max_sc ){
			max_sc = tmp_sc;
			result = i;
		}	
	}	
	return result;
};

/******************************************************************************/
void CADSMCtr::_modifyPattern ( TVFloat* pattern , CDefLoad* def_load , int ac_time ){
	sDefLoad* load_info = def_load->getLoadInfo();
	float     fAmp      = m_pcNode->getLoad()->getAmp();
	if ( load_info->id == 4 ){
		for ( int i = 0 ; i < load_info->dur ; i++ ){
			pattern->at( ac_time + i ) -= fAmp;
		}
	}
	else{
		for ( int i = 0 ; i < load_info->dur ; i++ ){
			pattern->at( ac_time + i ) -= fAmp * load_info->profile[i];
		}
	}
	return;
};

/******************************************************************************/
void CADSMCtr::setParameters ( TVFloat input ){
	
	return;
};















