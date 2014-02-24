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

	m_sSimCnf = sSimCnf;

	//m_sParamsFile = sParamsFile;
	//m_nSimStep    = nStep;

	m_nGGType     = 0;
	m_sSourceFile = "/home/manu/repo/simulator/gridSim/gridSim_1/input/profile";

	/*
	TXML_StringV tmp_pattern;
	tmp_pattern.push_back ( "GRID" );
	m_nGGType     = readParameter ( m_sParamsFile->c_str() , "TYPE" , &tmp_pattern );
	m_sSourceFile = readString    ( m_sParamsFile->c_str() , "SRC"  , &tmp_pattern );		
	*/
	

	switch( m_nGGType ){
		case 1:
			_configureSinusoidal ( ); 			
			break;
		case 2:
			_configureProfile   ( ); 
			break;
		default:						
			break;
	}	

	TVFloat   tmp_vfloat;
	TVFreqCmp tmp_frq;
	sFreqCmp  tmp_freqcmp;	

	/* FFT */	
	m_pcFFT        = new CFFT();

	for ( int i = 0 ; i < (m_sSimCnf->nFFTsize)/2 + 1 ; i++ ){
		m_vFreqSignal.push_back     ( tmp_freqcmp );
		m_vFreqSignal_amp.push_back ( 0.0 );
	}	
	m_bNewFFT = false;

};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CGrid::restart ( void ){

	m_bNewFFT = false;

	m_vTimeSignal.clear();
	m_vSampledSignal.clear();

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
	float fPower = _gridPowerStep();		
	m_vGGridSignal.push_back( fPower );
	/* Execute Lines */
	for ( int i = 0 ; i < m_vLines.size() ; i++ )
		m_vLines[i]->executionStep ();
	/* Get Lines Power */
	fPower = 0.0;
	for ( int i = 0 ; i < m_vLines.size() ; i++ )
		fPower += m_vLines[i]->getPower ();
	
	fPower *= 1e-3;

	m_vLinesSignal.push_back( fPower );

	/* Global Power */
	m_vTimeSignal.push_back( m_vGGridSignal.back() + m_vLinesSignal.back() );	
	if ( (m_sSimCnf->nSimStep)%(m_sSimCnf->nSampling) == 0 ){		
		m_vSampledSignal.push_back( m_vTimeSignal.back() );
		/* FFT */	
		_SignalFFT ( );	
	}
	else{
		m_bNewFFT = false;
	}
	return;
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
		m_bNewFFT = true;			
	}	
	return;
};

/****************************************************************/
float CGrid::_gridPowerStep            ( void ){
	float fresult;
	switch( m_nGGType ){		
		case 1:
			fresult = _nextInputSinusoidal( );
			break;
		case 2:
			fresult  = _nextInputProfile( );			
			break;
		default:
			fresult = 0.0;			
			break;
	}
	return fresult;
};


/****************************************************************/
float CGrid::_nextInputProfile( void ){
	float  result = m_vInputProfile[ (m_sSimCnf->nSimStep)%m_vInputProfile.size() ];
	float  next_p = m_vInputProfile[ (m_sSimCnf->nSimStep + 1440 )%m_vInputProfile.size() ];
	m_vDailyProfile.push_back( next_p );
	m_vDailyProfile.erase( m_vDailyProfile.begin() );	
	return result;
};

/****************************************************************/
float CGrid::_nextInputSinusoidal( void ){
	float result = 0.0;	
	for ( int i = 0 ; i < m_vSinusoidalInput.size() ; i++ ){
		result += m_vSinusoidalInput[i].amp * ( 1.0 + cos( TWO_PI * float(m_sSimCnf->nSimStep)/m_vSinusoidalInput[i].period + m_vSinusoidalInput[i].phs ) );
	}	
	return result;
};

/****************************************************************/
void CGrid::_configureProfile    ( void ){
	ifstream InputFile    ( m_sSourceFile.c_str() );
	m_vInputProfile.clear ( );
	float tmp_float, slope;
	InputFile.ignore   ( 256, ' ' );
	InputFile.ignore   ( 256, ' ' );
	InputFile.ignore   ( 256, ' ' );
	InputFile >> tmp_float;
	m_vInputProfile.push_back( 10000000.0 * tmp_float );
	InputFile.ignore   ( 256, '\n' );
	for ( int i = 1 ; i < 720 ; i++ ){
		InputFile.ignore   ( 256, ' ' );
		InputFile.ignore   ( 256, ' ' );
		InputFile.ignore   ( 256, ' ' );
		InputFile >> tmp_float;
		tmp_float *= 10000000.0;
		slope = (tmp_float - m_vInputProfile.back())/60.0;
		for ( int j = 0 ; j  < 60 ; j++ ){
			m_vInputProfile.push_back( m_vInputProfile.back() + slope );
		}
		InputFile.ignore   ( 256, '\n' );
	}
	InputFile.close();
	/* Create DailyProfile */
	for ( int i = 0 ; i < 1440 ; i++)
		m_vDailyProfile.push_back( m_vInputProfile[i] );

	return;
};

/****************************************************************/
void CGrid::_configureSinusoidal ( void ){ 	
	//int numberFC = int ( readParameter ( m_sSourceFile.c_str() , "FCOMP", NULL));
	/*
	string       tmp_data;	
	sFreqCmp     tmp_FC;	
	for (int i = 1 ; i <= numberFC ; i++ ){	
		ostringstream tmp_str;			
		tmp_str << i;
		tmp_data.assign("FCPr_");		
		tmp_data.append( tmp_str.str() );				
		tmp_FC.period    = readParameter ( m_sSourceFile.c_str() , tmp_data.c_str(), NULL );
			
		tmp_data.assign("FCAm_");
		tmp_data.append( tmp_str.str() );
		tmp_FC.amp       = readParameter ( m_sSourceFile.c_str() , tmp_data.c_str(), NULL);
		
		tmp_data.assign("FCPh_");
		tmp_data.append( tmp_str.str() );
		tmp_FC.phs       = readParameter ( "param_file/signalComp.xml" , tmp_data.c_str(), NULL);
			
		m_vSinusoidalInput.push_back ( tmp_FC );
	}
	*/
	return;
};



