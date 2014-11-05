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
/* Methods of mufco_ctr.h 						      */
/******************************************************************************/
#include "mufco_ctr.h"

/******************************************************************************/
/* CONSTRUCTOR */
CMUFCOCtr::CMUFCOCtr ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode ) : CController::CController ( sSimCnf , pcNode ) {	
	string attr;
	attr = cnf->Attribute("K");
	m_fK = atof( attr.c_str() );
	attr = cnf->Attribute("P");
	m_fP = atof( attr.c_str() );
	attr = cnf->Attribute("R");
	m_nR = atoi( attr.c_str() );

	if ( cnf->Attribute("type") ){
		attr    = cnf->Attribute("type");
		m_CType = atoi( attr.c_str() );
	}
	else{
		m_CType = 3;
	}
	if ( cnf->Attribute("ini_cmp") ){
		attr      = cnf->Attribute("ini_cmp");
		m_nIniCmp = atoi( attr.c_str() );
	}
	else{
		m_nIniCmp = 0;
	}
	if ( cnf->Attribute("offset") ){
		attr      = cnf->Attribute("offset");
		m_fOffset = atof( attr.c_str() );
	}
	else{
		m_fOffset = 0.0;
	}
	if ( cnf->Attribute("amp") ){
		attr   = cnf->Attribute("amp");
		m_fAmp = atof( attr.c_str() );
	}
	else{
		m_fAmp = 1.0;
	}

};	

/******************************************************************************/
/* DESTRUCTOR */
CMUFCOCtr::~CMUFCOCtr ( void ){
	
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CMUFCOCtr::restart ( void ){
	// Reset initial variables
	m_fPhase  = m_sSimCnf->pcRandom->nextDouble( - M_PI , M_PI );		
	if ( m_nIniCmp > 0 ){
		m_nComp     = m_nIniCmp;
	}
	else{
		m_nComp     = m_sSimCnf->pcRandom->nextInt( m_nR + 1 , m_sSimCnf->nFFTsize/2 );
	}
	m_fPeriod   = float ( m_sSimCnf->nSampling ) * float( m_sSimCnf->nFFTsize  )/float(m_nComp);
	m_fOmega    = TWO_PI / m_fPeriod;	
	m_fArgument = m_fPhase ;
	return;
};

/******************************************************************************/
/* Execution Step */
void CMUFCOCtr::executionStep( void ){

	// Oscilator
	//float fArgument  = m_fOmega * float ( m_sSimCnf->nSimStep ) + m_fPhase ;
	//float fPower = 0.0 + cos ( fArgument );

	m_fArgument  = m_fOmega * float ( m_sSimCnf->nSimStep ) + m_fPhase ;
	float fPower = m_fAmp * ( m_fOffset + cos ( m_fArgument ) );

	m_pcNode->getLoad()->setDirectPower( fPower );
		
	return;
};

/******************************************************************************/
void CMUFCOCtr::sendFFT ( TVFreqCmp* vFFT , sFFTeval* FFTeval ){
	_mufcoStep ( vFFT , FFTeval );
	return;
};

/******************************************************************************/
int  CMUFCOCtr::_actionTime ( void ){
	int result;
	TVFloat prob;
	for ( int i = 0 ; i < 1440 ; i++ ){
		prob.push_back( 1.0 + cos ( m_fOmega * float ( m_sSimCnf->nSimStep + i ) + m_fPhase ) );
		//cout << prob.back() << endl;
	}	
	result = m_sSimCnf->pcRandom->roulette ( &prob ) + m_sSimCnf->nSimStep;
	return result;
};

/******************************************************************************/
TVFloat CMUFCOCtr::getStatus     ( void ){
	TVFloat result;	
	result.push_back( m_fArgument );
	return result;
};

/******************************************************************************/
void CMUFCOCtr::_mufcoStep ( TVFreqCmp* vFFT , sFFTeval* FFTeval ){
	
	// Component Switching
	float random = m_sSimCnf->pcRandom->nextDouble();
	//cout << random << " " << FFTeval->FFTrel << endl;
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
			m_fArgument = TWO_PI * float ( m_sSimCnf->nSimStep - 1 ) / m_fPeriod + m_fPhase ;
		}				
	}	
	// Kuramoto Coupling	
	if ( m_nComp != 0 ){	
		//m_fArgument  = m_fOmega * float ( m_sSimCnf->nSimStep - 1.0  ) + m_fPhase ;  // CHECK	
		float ph_mod = 0;				
		switch ( m_CType ){
			case 1: 
				ph_mod = m_fK * FFTeval->R[m_nComp] * sin ( FFTeval->Phi[m_nComp] - m_fArgument );					
				break;
			case 2:
				ph_mod = m_fK * ( vFFT->at(m_nComp).amp / ( FFTeval->Nrm[m_nComp] )  ) * sin ( vFFT->at(m_nComp).phs - m_fArgument );
				break;
			case 3:
				ph_mod = m_fK * ( vFFT->at(m_nComp).amp / FFTeval->FFTmax ) * sin ( vFFT->at(m_nComp).phs - m_fArgument );
				break;
		}					
		m_fPhase += ph_mod;
		if ( m_fPhase > M_PI )
			m_fPhase -= TWO_PI;
		else if ( m_fPhase < - M_PI )
			m_fPhase += TWO_PI;
		//cout << m_fPhase << " " << vFFT->at(m_nComp).phs << " " << vFFT->at(m_nComp).amp << " " << m_nComp << endl;
	}
	// Oscilator
	//m_fArgument = m_fOmega * float ( m_sSimCnf->nSimStep ) + m_fPhase ;
	//cout << cos ( m_fArgument ) << endl;	
	//m_fPower    = m_fOffset + cos ( m_fArgument );	
	


	return;
};


/******************************************************************************/
void CMUFCOCtr::setParameters ( TVFloat input ){
	if ( input.size() > 0 ){
		m_fK = input.at(0);
	}
	if ( input.size() > 1 ){
		m_fP = input.at(1);
	}
	if ( input.size() > 2 ){
		m_nR = int(input.at(2));
	}
	return;
};















