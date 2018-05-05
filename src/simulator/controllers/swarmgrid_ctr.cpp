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
/* Methods of swamrgrid_PV_ctr.h					      */
/******************************************************************************/
#include "swarmgrid_ctr.h"

/******************************************************************************/

/* CONSTRUCTOR */
CSwarmGridCtr::CSwarmGridCtr ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode ) : CController::CController ( sSimCnf , pcNode ) {	
	string attr;
	attr = cnf->Attribute("K");
	m_fK = atof( attr.c_str() );
	attr = cnf->Attribute("P");
	m_fP = atof( attr.c_str() );
	attr = cnf->Attribute("R");
	m_nR = atof( attr.c_str() );
	if ( cnf->Attribute("B") ){
		attr = cnf->Attribute("B");
		m_fB = atof( attr.c_str() );
	}	
	else{
		m_fB = 0.0;
	}
	if ( cnf->Attribute("act") ){
		attr   = cnf->Attribute("act");
		m_bAct = atof( attr.c_str() );
	}	
	else{
		m_bAct = false;
	}
	
	// Bat conf
	if ( cnf->Attribute("active_bat") ){		
		active_bat = bool(atoi( cnf->Attribute("active_bat") ));
		if ( active_bat ){
			if (cnf->Attribute("soc_min")){
				m_fSoC_min = atof( cnf->Attribute("soc_min") );
			}
			else{
				m_fSoC_min = 40.0;
			}	
			if (cnf->Attribute("soc_max")){
				m_fSoC_max = atof( cnf->Attribute("soc_max") );
			}
			else{
				m_fSoC_max = 60.0;
			}
		}
	}	
	else{
		active_bat = false;
	}		

	//Load cosine constant
	M_NRM = float(SW_COS_DEF) / TWO_PI;
	for ( int i = 0 ; i < SW_COS_DEF; i++ ){
		COS[i] = 1.0 + cos( i / M_NRM );
	}

	


};	

/******************************************************************************/
/* DESTRUCTOR */
CSwarmGridCtr::~CSwarmGridCtr ( void ){
	
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CSwarmGridCtr::restart ( void ){
	// Reset initial variables
	m_fPhase  = m_sSimCnf->pcRandom->nextDouble( - M_PI , M_PI );		
	m_nComp     = m_sSimCnf->pcRandom->nextInt( m_nR + 1 , m_sSimCnf->nFFTsize/2 );
	m_fPeriod   = float ( m_sSimCnf->nSampling ) * float( m_sSimCnf->nFFTsize  )/float(m_nComp);
	m_fOmega    = TWO_PI / m_fPeriod;	
	m_fArgument = m_fPhase ;

	// Reset energies
	m_fLoadEne = 0.0;
	m_fGridEne = 0.0;
	m_fPVEne   = 0.0;	
	return;
};

/******************************************************************************/
/* Execution Step */
void CSwarmGridCtr::executionStep( void ){
	/* SC calculation */
	sNPower* power_info = m_pcNode->getNPower ();
	m_fLoadEne += power_info->load;	
	m_fPVEne   += power_info->pv;
	if ( power_info->grid > 0){
		m_fGridEne += power_info->grid;
	}

	/* Battery controller */
	if ( active_bat && m_pcNode->getStorage() ){
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

	/* SG algorithm */
	if ( m_bAct ){
		// Get Loads
		TVCDefLoad* vDefLoad = m_pcNode->getLoad()->getDefLoads();	
		// Execute the not set loads			
		int  ac_time;
		TVFloat vGPattern, vPVPattern, vPattern;
		for ( int i = 0 ; i < vDefLoad->size() ; i++ ){
			// Check if the load has been set
			if ( !vDefLoad->at(i)->isCtrFlag() ){		
				if ( m_fB == 0.0 ){ //Only grid
					_genGPattern( &vPattern , vDefLoad->at(i)->getDur() , vDefLoad->at(i)->getRange() );									
				}
				else if ( m_fB == 1.0 ){ // Only PV
					int time_ref = m_sSimCnf->nSimStep + ( vDefLoad->at(i)->getDur() / 2 );
					vPattern = m_pcNode->getPV()->getRangeNrFrc( time_ref , time_ref + vDefLoad->at(i)->getRange() );
				}
				else{
					_genGPattern( &vGPattern , vDefLoad->at(i)->getDur() , vDefLoad->at(i)->getRange() );
					int time_ref = m_sSimCnf->nSimStep + ( vDefLoad->at(i)->getDur() / 2 );
					vPVPattern = m_pcNode->getPV()->getRangeNrFrc( time_ref , time_ref + vDefLoad->at(i)->getRange() );
					_genPattern ( &vGPattern, &vPVPattern, &vPattern );
				}			
				// Select run time				
				int ac_time = _actionTime( &vPattern );
				vDefLoad->at(i)->setStartTime ( ac_time );
				vDefLoad->at(i)->setCtrFlag   ( true );					
			}
		}
	}	
	return;
};

/******************************************************************************/
TVFloat CSwarmGridCtr::getStatus ( void ){
	TVFloat result;
	TVFloat vPVPattern;
	if ( m_pcNode->getPV() )
		vPVPattern = m_pcNode->getPV()->getNextDayNrFrc( m_sSimCnf->nSimStep + 45 );
	else
		vPVPattern.push_back(0);
	int   time_ref = m_sSimCnf->nSimStep + 45;	
	float phase    = m_fOmega * float ( time_ref ) + m_fPhase;
	phase          = fmod( double(phase) , double(TWO_PI) );	
	float PG       = COS[ int ( M_NRM * phase ) ];//1.0 + cos ( phase );
	float pattern  = ( 1.0 - m_fB ) * ( PG ) + m_fB * vPVPattern.at(0);
	float consump  = m_pcNode->getLoad()->getPower();
	float pv;
	if ( m_pcNode->getPV() )
      		pv = m_pcNode->getPV()->getPower();
	else
		pv = 0.0;
	result.push_back( pattern );
	result.push_back( consump );
	result.push_back( pv );	

	return result;
};

/******************************************************************************/
void CSwarmGridCtr::_genPattern  ( TVFloat* inputG , TVFloat* inputPV , TVFloat* output ){
	output->clear();
	for ( int i = 0 ; i < inputG->size() ; i++ ){
		output->push_back  ( m_fB * inputPV->at(i) + ( 1.0 - m_fB ) * inputG->at(i) );
	}
	return;
};

/******************************************************************************/
void CSwarmGridCtr::_genGPattern( TVFloat* output, int dur , int range ){
	output->clear();
	int time_ref = m_sSimCnf->nSimStep + ( dur / 2 );	
	float phase  = m_fOmega * float ( time_ref ) + m_fPhase;
	phase = fmod( double(phase) , double(TWO_PI) );	
	for ( int i = 0 ; i < range ; i++ ){		
		output->push_back( COS[ int ( M_NRM * phase ) ] );
		//output->push_back( 1.0 + cos ( phase ) );
		phase += m_fOmega;	
		if ( phase > TWO_PI )
			phase -= TWO_PI;	
	}
	return;
};


/******************************************************************************/
int  CSwarmGridCtr::_actionTime ( TVFloat* prob ){
	int result = m_sSimCnf->nSimStep;
	if ( prob->size() > 0 )
		result += m_sSimCnf->pcRandom->roulette ( prob );
	return result;
};

/******************************************************************************/
void CSwarmGridCtr::sendFFT ( TVFreqCmp* vFFT , sFFTeval* FFTeval ){
	if ( m_bAct ){
		_mufcoStep ( vFFT , FFTeval );
	}
	return;
};

/******************************************************************************/
void CSwarmGridCtr::_mufcoStep ( TVFreqCmp* vFFT , sFFTeval* FFTeval ){
	// Component Switching
	float random = m_sSimCnf->pcRandom->nextDouble();	
	if ( random < m_fP * FFTeval->FFTrel ){			
		TVFloat tmp_vfloat;
		for ( int i = m_nR + 1 ; i < vFFT->size() ; i++ ){						
			tmp_vfloat.push_back( pow ( vFFT->at(i).amp , 2 ) );
		}				
		int new_cmp = m_sSimCnf->pcRandom->roulette ( &tmp_vfloat ) + m_nR + 1; // 0 is taken into account
		if ( m_nComp != new_cmp ){				
			m_nComp     = new_cmp;										
			m_fPeriod   = float ( m_sSimCnf->nSampling ) * vFFT->at(m_nComp).period;
			m_fOmega    = TWO_PI / m_fPeriod;							
			m_fPhase    = m_sSimCnf->pcRandom->nextDouble( - M_PI , M_PI );	
			m_fArgument = TWO_PI * float ( m_sSimCnf->nSimStep - 1 ) / m_fPeriod + m_fPhase ; // LAST COMPONENT NOT REPRESENTED!!!!!!!!!!!!!!!!!!!!!!!
		}				
	}	
	// Kuramoto Coupling	
	if ( m_nComp != 0 ){
		m_fArgument  = m_fOmega * float ( m_sSimCnf->nSimStep - 1.0  ) + m_fPhase ; 
		float ph_mod = m_fK * ( vFFT->at(m_nComp).amp / FFTeval->FFTmax ) * sin ( vFFT->at(m_nComp).phs - m_fArgument );
		m_fPhase += ph_mod;
		if ( m_fPhase > M_PI )
			m_fPhase -= TWO_PI;
		else if ( m_fPhase < - M_PI )
			m_fPhase += TWO_PI;		
	}
	return;
};

/******************************************************************************/
void CSwarmGridCtr::setParameters ( TVFloat input ){
	if ( input.size() > 0 ){
		m_fK = input.at(0);
	}
	if ( input.size() > 1 ){
		m_fP = input.at(1);
	}
	if ( input.size() > 2 ){
		m_nR = int(input.at(2));
	}
	if ( input.size() > 3 ){
		m_fB = input.at(3);
	}
	return;
};















