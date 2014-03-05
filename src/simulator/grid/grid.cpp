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

/****************************************************************/
/* GRID METHODS							*/
/****************************************************************/
#include "grid.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
CGrid::CGrid ( sSimCnf*  sSimCnf ){
	/* Simulator stuff */	
	m_sSimCnf = sSimCnf;
	/* FFT */	
	sFreqCmp  tmp_freqcmp;	
	m_pcFFT        = new CFFT();
	for ( int i = 0 ; i < (m_sSimCnf->nFFTsize)/2 + 1 ; i++ ){
		m_vFreqSignal.push_back     ( tmp_freqcmp );
		m_vFreqSignal_amp.push_back ( 0.0 );
	}
	/* Clear variables */	
	m_bNewSample   = false;
	m_fPower       = 0.0;
	m_fPower_lines = 0.0;
	m_fPower_egrid = 0.0;
	m_vSampledSignal.clear();
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CGrid::restart ( void ){
	/* Clear variables */
	m_bNewSample   = false;	
	m_fPower       = 0.0;
	m_fPower_lines = 0.0;
	m_fPower_egrid = 0.0;
	m_vSampledSignal.clear();
	/* Restart lines */
	for ( int i = 0 ; i <  m_vLines.size() ; i++ ){
		m_vLines[i]->restart();
	}
	return;
};

/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
CGrid::~CGrid ( void ){	
	for ( int i = 0 ; i < m_vLines.size() ; i++ )
		delete m_vLines[i];
	m_vLines.clear();
	delete m_pcFFT;	
};

/****************************************************************/
/* EXECUTION FUNCTION						*/
/****************************************************************/
void CGrid::executionStep ( void ){	
	/* Get General Grid Power */
	m_fPower_egrid = _nextGridPower();	
	/* Execute Lines */
	for ( int i = 0 ; i < m_vLines.size() ; i++ )
		m_vLines[i]->executionStep ();
	/* Get Lines Power */
	m_fPower_lines = 0.0;
	for ( int i = 0 ; i < m_vLines.size() ; i++ )
		m_fPower_lines += m_vLines[i]->getPower ();
	/* Global Power */	
	m_fPower = m_fPower_egrid + m_fPower_lines;
	if ( (m_sSimCnf->nSimStep)%(m_sSimCnf->nSampling) == 0 ){		
		m_vSampledSignal.push_back( m_fPower );	
		m_bNewSample = true;
		// FFT 	
		_SignalFFT ( );			
		// Clear sampled signal
		if ( m_vSampledSignal.size() >= m_sSimCnf->nFFTsize )
			m_vSampledSignal.erase( m_vSampledSignal.begin() );
	}
	else{
		m_bNewSample = false;
	}
	return;
};

/****************************************************************/
float CGrid::_nextGridPower       ( void ){
	float result;
	if ( m_sSimCnf->GridProfile.dur == 0 )	
		result = m_sSimCnf->GridProfile.amp;
	else
		result = m_sSimCnf->GridProfile.amp * m_sSimCnf->GridProfile.profile[ (m_sSimCnf->nSimStep)%(m_sSimCnf->GridProfile.dur) ];
	return result;
};

/****************************************************************/
void CGrid::_SignalFFT ( void ){	
	if ( m_vSampledSignal.size() >= m_sSimCnf->nFFTsize ){
		sFreqCmp         tmp_freqcmp;		
		m_vFreqSignal.clear();
		m_vFreqSignal_amp.clear();
		double time_domain  [m_sSimCnf->nFFTsize][2];
		double frec_domain  [m_sSimCnf->nFFTsize][2];							
		for ( int j = 0 ; j < m_sSimCnf->nFFTsize ; j++ ){
			time_domain[j][0] = m_vSampledSignal[ m_vSampledSignal.size() + j - m_sSimCnf->nFFTsize ];
			time_domain[j][1] = 0.0;				
		}						
		m_pcFFT->fft ( m_sSimCnf->nFFTsize, &time_domain[0], &frec_domain[0] );							
		for (int j = 0 ; j < (m_sSimCnf->nFFTsize)/2 + 1 ; j++){
			complex<double> tmp_complex (frec_domain[j][0],frec_domain[j][1]);			
			tmp_freqcmp.c_number = tmp_complex;				
			tmp_freqcmp.phs      = arg(tmp_complex);
			if ( j == 0 ){
				tmp_freqcmp.amp      = abs(tmp_complex)/float(m_sSimCnf->nFFTsize);
				tmp_freqcmp.period   = 0.0;
			}
			else{
				tmp_freqcmp.amp      = 2.0 * abs(tmp_complex)/float(m_sSimCnf->nFFTsize);	
				tmp_freqcmp.period   = float(m_sSimCnf->nFFTsize)/float(j);						
			}
			m_vFreqSignal.push_back     ( tmp_freqcmp );	
			m_vFreqSignal_amp.push_back ( tmp_freqcmp.amp );							
		}
		//m_bNewFFT = true;			
	}	
	return;
};







