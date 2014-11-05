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
/* Methods of mufco_mc.h 						      */
/******************************************************************************/
#include "mufco_mc.h"

/******************************************************************************/
/* CONSTRUCTOR */
CMUFCO_MC::CMUFCO_MC ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf ) : CMainControl::CMainControl ( sSimCnf , pcGrid , vCtr ) {
	string attr;
	if ( cnf->Attribute("res_cmp") ){
		attr      = cnf->Attribute("res_cmp");
		m_nResCmp = atoi( attr.c_str() );
	}
	else{
		m_nResCmp = 0;
	}
	if ( cnf->Attribute("res_type") ){
		attr       = cnf->Attribute("res_type");
		m_nResType = atoi( attr.c_str() );
	}
	else{
		m_nResType = 1;
	}
	if ( cnf->Attribute("ass_type") ){
		attr       = cnf->Attribute("ass_type");
		m_nAssType = atoi( attr.c_str() );
	}
	else{
		m_nAssType = 1;
	}



	

	
};	

/******************************************************************************/
/* RESTART */
void CMUFCO_MC::restart  ( void ){

	if ( m_vCtr->size() > 0 )
		m_nFCreject = m_vCtr->at(0)->getFCreject();
	else	
		m_nFCreject = 0;

	/* Variables initial values */
	m_sFFTst.FFTave = 0.0;
	m_sFFTst.FFTvar = 0.0;	
	m_sFFTst.FFTmax = 0.0001; // IT COULD BE ZERO BUT CONTROLLERS SHOULD TAKE IT INTO ACCOUNT. TO REDUCE COMPUTATION BETTER START WITH A LOW VAUE
	m_sFFTst.FFTrel = 0.0;			
	/* Set plotter */
	if ( m_sSimCnf->pcPlotter ){	
		m_sSimCnf->pcPlotter->setData    ( 0 , &m_vTimeSignal                 );
		m_sSimCnf->pcPlotter->setMarks   ( 0 , &m_vSampledSig                 );
		m_sSimCnf->pcPlotter->setMarksSp ( 0 , m_sSimCnf->nSampling           );				
		m_sSimCnf->pcPlotter->setMarks   ( 1 , m_pcGrid->getFreqSignalAmp ( ) );		
		m_sSimCnf->pcPlotter->setData    ( 2 , &m_vNodeSignal                 );		
		m_sSimCnf->pcPlotter->setMarks   ( 3 , &m_vCmp );		
	}

	/* Clean vectors */
	m_vTimeSignal.clear();
	m_vSampledSig.clear();
	m_vNodeSignal.clear();
	m_vGridSig.clear();		
	m_vResult.clear();

	m_vCmp.clear();
	for ( int i = 0 ; i < m_sSimCnf->nFFTsize/2 + 1 ; i++ )
		m_vCmp.push_back(0.0);


	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CMUFCO_MC::~CMUFCO_MC ( void ){

};


/******************************************************************************/
TVFloat* CMUFCO_MC::getEvaluation  ( void ){
	_evaluateAC();	
	return &m_vResult;
};	

/******************************************************************************/
float CMUFCO_MC::getAssessment  ( void ){
	_evaluateAC();	
	float  result;
	switch( m_nAssType ){		
		case 1: //DAY
			result = m_vResult[3];
			break;
		case 2: //WEEK
			result = m_vResult[2];
			break;

		case 3: //DAY-WEEK
			result = 0.5 * ( m_vResult[2] + m_vResult[3] );
			break;
		default:
			break;
	}	 
	return result;
};

/******************************************************************************/
void CMUFCO_MC::_evaluateAC  ( void ){
	m_vResult.clear();
	switch(m_nResType){
		case 1:
			m_vResult.push_back( m_sFFTst.R[m_nResCmp] );
			break;
		case 2:
			_calculateCF   ( );
			break;
		default:
			break;
	}
	return;
};

/******************************************************************************/
void CMUFCO_MC::_calculateCF   ( void ){	
	// If there is enough data
	if ( m_vTimeSignal.size() >= 525600 ){
		// Year data
		float   year_max  = m_vTimeSignal[0];
		float   year_min  = m_vTimeSignal[0];
		float   year_rms  = 0.0;
		// Month data
		float   month_max = m_vTimeSignal[0];
		float   month_min = m_vTimeSignal[0];
		float   month_rms = 0.0;
		TVFloat vmonth_max, vmonth_min, vmonth_rms;
		// Week data		
		float   week_max = m_vTimeSignal[0];
		float   week_min = m_vTimeSignal[0];
		float   week_rms = 0.0;
		TVFloat vweek_max, vweek_min, vweek_rms;
		// Day data		
		float   day_max   = m_vTimeSignal[0];
		float   day_min   = m_vTimeSignal[0];
		float   day_rms   = 0.0;
		TVFloat vday_max, vday_min, vday_rms;
		// Second year analysis
		for ( int i = m_vTimeSignal.size() - 525600 ; i < m_vTimeSignal.size() ; i++ ){
			// Year
			if ( m_vTimeSignal[i] > year_max ){
				year_max = m_vTimeSignal[i];
			}
			if ( m_vTimeSignal[i] < year_min ){
				year_min = m_vTimeSignal[i];
			}
			year_rms += pow( m_vTimeSignal[i] , 2 );
			// Month
			if ( i % 43800 == 43799 ){
				vmonth_max.push_back( month_max );
				vmonth_min.push_back( month_min );
				vmonth_rms.push_back( month_rms );
				month_max = m_vTimeSignal[i];
				month_min = m_vTimeSignal[i];
				month_rms = 0.0;
			}
			else{
				if ( m_vTimeSignal[i] > month_max ){
					month_max = m_vTimeSignal[i];
				}
				if ( m_vTimeSignal[i] < month_min ){
					month_min = m_vTimeSignal[i];
				}
				month_rms += pow( m_vTimeSignal[i] , 2 );
			}
			// Week
			if ( i % 10080 == 10079 ){
				vweek_max.push_back( week_max );
				vweek_min.push_back( week_min );
				vweek_rms.push_back( week_rms );
				week_max = m_vTimeSignal[i];
				week_min = m_vTimeSignal[i];
				week_rms = 0.0;
			}
			else{
				if ( m_vTimeSignal[i] > week_max ){
					week_max = m_vTimeSignal[i];
				}
				if ( m_vTimeSignal[i] < week_min ){
					week_min = m_vTimeSignal[i];
				}
				week_rms += pow( m_vTimeSignal[i] , 2 );
			}
			// Day
			if ( i % 1440 == 1439 ){
				vday_max.push_back( day_max );
				vday_min.push_back( day_min );
				vday_rms.push_back( day_rms );
				day_max = m_vTimeSignal[i];
				day_min = m_vTimeSignal[i];
				day_rms = 0.0;
			}
			else{
				if ( m_vTimeSignal[i] > day_max ){
					day_max = m_vTimeSignal[i];
				}
				if ( m_vTimeSignal[i] < day_min ){
					day_min = m_vTimeSignal[i];
				}
				day_rms += pow( m_vTimeSignal[i] , 2 );
			}
		}
		// Year
		year_rms = sqrt( year_rms / 525600 );		
		m_vResult.push_back ( pow( year_max            , 1) / pow ( year_rms , 1 ) ); // pow1 -> crest factor | pow2 -> PAPR 		
		// Month
		float month_par = 0.0;
		float month_ptp = 0.0;
		for ( int i = 0 ; i < vmonth_max.size() ; i++ ){
			month_par += ( pow( vmonth_max[i]                 , 1) / pow ( sqrt( vmonth_rms[i] / 43800 ) , 1 ) );			
		}
		m_vResult.push_back ( month_par / float(vmonth_max.size()));
		// Week
		float week_par = 0.0;
		float week_ptp = 0.0;
		for ( int i = 0 ; i < vweek_max.size() ; i++ ){
			week_par += ( pow( vweek_max[i]                 , 1) / pow ( sqrt( vweek_rms[i] / 10080 ) , 1 ) );			
		}
		m_vResult.push_back ( week_par / float(vweek_max.size()));		
		// Day
		float day_par = 0.0;
		float day_ptp = 0.0;
		for ( int i = 0 ; i < vday_max.size() ; i++ ){
			day_par += pow( vday_max[i]               , 1) / pow ( sqrt ( vday_rms[i] / 1440 ) , 1 );
			
		}
		m_vResult.push_back ( day_par / float(vday_max.size()));		
	}
	return;
};


/******************************************************************************/
/* Execution Step */
void CMUFCO_MC::executionStep( void ){	

	/* Communication with the controllers - FFT send */
	if ( m_pcGrid->is_FFT ( ) ){
		TVFreqCmp* tmp_FFT     = m_pcGrid->getFreqSignal();
		// Evaluate FFT
		_evaluateFFT ( tmp_FFT );
		// Send FFT and evaluation		
		for ( int i = 0 ; i < m_vCtr->size() ; i++ ){
			m_vCtr->at(i)->sendFFT ( tmp_FFT , &m_sFFTst );
		}
		for ( int i = 0 ; i < m_vCmp.size() ; i++ )
			m_vCmp[i] = 0.0;
		int cmp;
		for ( int i = 0 ; i < m_vCtr->size() ; i++ ){
			cmp = m_vCtr->at(i)->getCmp ( );
			m_vCmp[cmp]++;
		}
	}
	
	return;
};

/******************************************************************************/
void CMUFCO_MC::_evaluateFFT ( TVFreqCmp* input ){
	/* ORDER PARAMETER */
	TVGridSCmp* tmp_GCmp = m_pcGrid->getSCmp();	
	// Generate empty vectors
	TVFloat  tmp_real, tmp_img, tmp_Gnrm;
	TVInt    tmp_pop;
	for ( int i = 0 ; i < m_sSimCnf->nFFTsize/2 + 1 ; i++ ){
		tmp_real.push_back ( 0.0 );
		tmp_img.push_back  ( 0.0 );
		tmp_Gnrm.push_back ( 0.0 );
		tmp_pop.push_back  ( 0 );
	}
	// Get non-controllable arguments
	for ( int i = 0 ; i < tmp_GCmp->size() ; i++ ){
		tmp_real [ tmp_GCmp->at(i).cmp ] = tmp_GCmp->at(i).amp * cos ( tmp_GCmp->at(i).arg );
		tmp_img  [ tmp_GCmp->at(i).cmp ] = tmp_GCmp->at(i).amp * sin ( tmp_GCmp->at(i).arg );	
		tmp_Gnrm [ tmp_GCmp->at(i).cmp ] = tmp_GCmp->at(i).amp;    
	}						
	// Get oscillators arguments	
	TVFloat tmp_status;
	for ( int i = 0 ; i < m_vCtr->size() ; i++ ){
		tmp_status = m_vCtr->at(i)->getStatus();
		tmp_real[ m_vCtr->at(i)->getCmp() ] += cos ( tmp_status[0] );
		tmp_img [ m_vCtr->at(i)->getCmp() ] += sin ( tmp_status[0] );
		tmp_pop [ m_vCtr->at(i)->getCmp() ]++;
	}
	// Calculate R and Phi
	m_sFFTst.R.clear();
	m_sFFTst.Phi.clear();
	m_sFFTst.Nrm.clear();
	for ( int i = 0 ; i < m_sSimCnf->nFFTsize/2 + 1 ; i++ ){
		m_sFFTst.Nrm.push_back( float(tmp_pop[i]) + tmp_Gnrm[i] );
		if ( m_sFFTst.Nrm.back() > 1e-3 ){ 
			tmp_real[i] /= ( m_sFFTst.Nrm.back() );
			tmp_img [i] /= ( m_sFFTst.Nrm.back() );
			complex<float> tmp_complex ( tmp_real[i] , tmp_img[i] );
			m_sFFTst.R.push_back   ( abs( tmp_complex ) );
			m_sFFTst.Phi.push_back ( arg( tmp_complex ) );								
		}
		else{			
			m_sFFTst.R.push_back(0.0);
			m_sFFTst.Phi.push_back(0.0);
		}		
	}
	/* FORM FACTOR EQUATION */
	//m_sFFTst.FFTmax = 0.0; // ERROR MUST BE GLOBAL
	for ( int i = m_nFCreject + 1 ; i < input->size() ; i++ ){
		if ( input->at(i).amp > m_sFFTst.FFTmax )
			m_sFFTst.FFTmax   = input->at(i).amp;
	}	
	m_sFFTst.FFTrel = 0.0;
	for ( int i = m_nFCreject + 1 ; i < input->size() ; i++ ){
		m_sFFTst.FFTrel +=  pow ( input->at(i).amp , 4 );
	}	
	m_sFFTst.FFTrel /= float( input->size() - m_nFCreject - 1 ) * pow( m_sFFTst.FFTmax  , 4 ); 
	return;
};

/******************************************************************************/
/* Assessment Step */
void CMUFCO_MC::assessmentStep ( void ){

	/* Get Info */
	m_vTimeSignal.push_back( m_pcGrid->getPower() );
	m_vGridSig.push_back( m_pcGrid->getPower_grid() );
	if ( m_pcGrid->is_Sample() ){		
		m_vSampledSig.push_back( m_pcGrid->getPower_sampled() );
	}
	m_vNodeSignal.push_back( m_pcGrid->getPower_lines() );

	/* Writter */
	if ( m_sSimCnf->pcWriter && ( (m_sSimCnf->nSimStep % 60) == 0 ) && ( m_sSimCnf->nSimStep > 262800 ) ){ //262800
		// Time Signal hourly average
		float fTS_ave   = 0.0;				
		int   ref_ptr = m_vTimeSignal.size() - 1;
		for ( int i = 0 ; i < 60 ; i++ ){
			fTS_ave   += m_vTimeSignal.at  ( ref_ptr - i );			
		}
		fTS_ave   /= 60.0;
		m_sSimCnf->pcWriter->push_buffer( fTS_ave );		
	}
	return;
};

/******************************************************************************/
void CMUFCO_MC::setParameters  ( TVFloat input ){
	for (int i = 0 ; i < m_vCtr->size() ; i++ ){
		m_vCtr->at(i)->setParameters( input );
	}
	return;
};






