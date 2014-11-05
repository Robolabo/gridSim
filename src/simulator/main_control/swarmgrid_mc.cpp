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
/* Methods of swarmgrid_mc.h 						      */
/******************************************************************************/
#include "swarmgrid_mc.h"

/******************************************************************************/
/* CONSTRUCTOR */
CSwarmGrid_MC::CSwarmGrid_MC ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf ) : CMainControl::CMainControl ( sSimCnf , pcGrid , vCtr ) {	
	string attr;
	if ( cnf->Attribute("wt_beg") ){
		attr    = cnf->Attribute("wt_beg");
		m_nWbeg = atoi( attr.c_str() );
	}
	else{
		m_nWbeg = 0;
	}
	if ( cnf->Attribute("wt_end") ){
		attr    = cnf->Attribute("wt_end");
		m_nWend = atoi( attr.c_str() );
	}
	else{
		m_nWend = -1;
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
void CSwarmGrid_MC::restart  ( void ){

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

	m_fBEne  = 0.0;
	m_fTEne  = 0.0;
	m_fNEne  = 0.0;
	m_fLEne  = 0.0;
	m_fPVEne = 0.0;

	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CSwarmGrid_MC::~CSwarmGrid_MC ( void ){

};


/******************************************************************************/
TVFloat* CSwarmGrid_MC::getEvaluation  ( void ){
	_evaluateAC();	
	float sc_ave = 0.0;
	for ( int i = 0 ; i < m_vCtr->size() ; i++ ){
		sc_ave += m_vCtr->at(i)->getSelfC( );
	}	
	sc_ave /= float( m_vCtr->size() );
	if (sc_ave < 0.0){
		sc_ave = 0.0;
	}
	m_vResult.push_back( sc_ave );
	return &m_vResult;
};	

/******************************************************************************/
float CSwarmGrid_MC::getAssessment  ( void ){
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
void CSwarmGrid_MC::_evaluateAC  ( void ){
	m_vResult.clear();
	_calculateCF();
	return;
};

/******************************************************************************/
void CSwarmGrid_MC::_calculateCF   ( void ){	
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
		// Last year analysis
		float   act_power; 
		for ( int i = m_vTimeSignal.size() - 525600 ; i < m_vTimeSignal.size() ; i++ ){
			// Negative power = 0
			if ( m_vTimeSignal[i] < 0.0 ){
				act_power = 0.0;
			}
			else{
				act_power = m_vTimeSignal[i];
			}
			// Year
			if ( act_power > year_max ){
				year_max = act_power;
			}
			if ( act_power < year_min ){
				year_min = act_power;
			}
			year_rms += pow( act_power , 2 );
			// Month
			if ( i % 43800 == 43799 ){
				vmonth_max.push_back( month_max );
				vmonth_min.push_back( month_min );
				vmonth_rms.push_back( month_rms );
				month_max = act_power;
				month_min = act_power;
				month_rms = 0.0;
			}
			else{
				if ( act_power > month_max ){
					month_max = act_power;
				}
				if ( act_power < month_min ){
					month_min = act_power;
				}
				month_rms += pow( act_power , 2 );
			}
			// Week
			if ( i % 10080 == 10079 ){
				vweek_max.push_back( week_max );
				vweek_min.push_back( week_min );
				vweek_rms.push_back( week_rms );
				week_max = act_power;
				week_min = act_power;
				week_rms = 0.0;
			}
			else{
				if ( act_power > week_max ){
					week_max = act_power;
				}
				if ( act_power < week_min ){
					week_min = act_power;
				}
				week_rms += pow( act_power , 2 );
			}
			// Day
			if ( i % 1440 == 1439 ){
				vday_max.push_back( day_max );
				vday_min.push_back( day_min );
				vday_rms.push_back( day_rms );
				day_max = act_power;
				day_min = act_power;
				day_rms = 0.0;
			}
			else{
				if ( act_power > day_max ){
					day_max = act_power;
				}
				if ( act_power < day_min ){
					day_min = act_power;
				}
				day_rms += pow( act_power , 2 );
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
			if ( vday_rms[i] <= 0.0 ){
				day_par += 1.0;
			}
			else{
				day_par += pow( vday_max[i] , 1) / pow ( sqrt ( vday_rms[i] / 1440 ) , 1 );
			}			
		}
		m_vResult.push_back ( day_par / float(vday_max.size()));		
	}
	return;
};


/******************************************************************************/
/* Execution Step */
void CSwarmGrid_MC::executionStep( void ){	

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
/* Assessment Step */
void CSwarmGrid_MC::assessmentStep ( void ){

	/* Get Info */
	m_vTimeSignal.push_back( m_pcGrid->getPower() );
	m_vGridSig.push_back( m_pcGrid->getPower_grid() );
	if ( m_pcGrid->is_Sample() ){		
		m_vSampledSig.push_back( m_pcGrid->getPower_sampled() );
	}
	m_vNodeSignal.push_back( m_pcGrid->getPower_lines() );

	/* Writter */
	if ( m_sSimCnf->pcWriter && ( (m_sSimCnf->nSimStep % 60) == 0 ) && m_sSimCnf->nSimStep != 0 ){ 
	if ( ( m_nWend < 0 || m_sSimCnf->nSimStep < m_nWend ) && m_sSimCnf->nSimStep >= m_nWbeg ){ //262800	
		// Time Signal hourly average
		float fTS_ave   = 0.0;
		float fGS_ave   = 0.0;
		float fNS_ave   = 0.0;
		
		int   ref_ptr = m_vTimeSignal.size() - 1;
		for ( int i = 0 ; i < 60 ; i++ ){
			fTS_ave   += m_vTimeSignal.at  ( ref_ptr - i );
			fGS_ave   += m_vGridSig.at     ( ref_ptr - i );
			fNS_ave   += m_vNodeSignal.at  ( ref_ptr - i );
			
		}
		fTS_ave   /= 60.0;
		fGS_ave   /= 60.0;
		fNS_ave   /= 60.0;

		TVFloat tmp_vec;
		float pattern = 0.0;
		float consump = 0.0;
		float pv      = 0.0;		
		
		for ( int i = 0 ; i < m_vCtr->size() ; i++ ){
			tmp_vec = m_vCtr->at(i)->getStatus();
			pattern += tmp_vec[0];
			consump += tmp_vec[1];
			pv      += tmp_vec[2];
		}		

		m_sSimCnf->pcWriter->push_buffer( fTS_ave );
		m_sSimCnf->pcWriter->push_buffer( fGS_ave );
		m_sSimCnf->pcWriter->push_buffer( fNS_ave );
		m_sSimCnf->pcWriter->push_buffer( consump );
		m_sSimCnf->pcWriter->push_buffer( pv );
		m_sSimCnf->pcWriter->push_buffer( pattern );

		m_fTEne  += fTS_ave;
		m_fBEne  += fGS_ave;		
		m_fNEne  += fNS_ave;
		m_fLEne  += consump;
		m_fPVEne += pv;

		m_sSimCnf->pcWriter->push_buffer( m_fTEne  );
		m_sSimCnf->pcWriter->push_buffer( m_fBEne  );
		m_sSimCnf->pcWriter->push_buffer( m_fNEne  );
		m_sSimCnf->pcWriter->push_buffer( m_fLEne  );
		m_sSimCnf->pcWriter->push_buffer( m_fPVEne );
	}
	}



	return;
};

/******************************************************************************/
void CSwarmGrid_MC::_evaluateFFT ( TVFreqCmp* input ){
	/* FORM FACTOR EQUATION */	
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
void CSwarmGrid_MC::setParameters  ( TVFloat input ){
	for (int i = 0 ; i < m_vCtr->size() ; i++ ){
		m_vCtr->at(i)->setParameters( input );
	}
	return;
};

/******************************************************************************/
void CSwarmGrid_MC::setEnvironment ( TVFloat input ){
	/* PV sampling */
	if ( input.size() > 0 ){
		float tmp_power;
		tmp_power = 240.0 * input[0];	
		TVCLine* tmp_lines;  
		tmp_lines = m_pcGrid->getLines();
		TVCNode* tmp_nodes;
		for ( int i = 0 ; i < tmp_lines->size() ; i++ ){
			tmp_nodes = tmp_lines->at(i)->getNodes();
			for ( int j = 0 ; j < tmp_nodes->size() ; j++ ){
				tmp_nodes->at(j)->getPV()->setPAmp( tmp_power ); 
			}
		}
	}
	/* Battery sampling */
	if ( input.size() > 1 ){
		float tmp_cap;
		tmp_cap = 50.0 * input[1];	
		TVCLine* tmp_lines;  
		tmp_lines = m_pcGrid->getLines();
		TVCNode* tmp_nodes;
		for ( int i = 0 ; i < tmp_lines->size() ; i++ ){
			tmp_nodes = tmp_lines->at(i)->getNodes();
			for ( int j = 0 ; j < tmp_nodes->size() ; j++ ){
				if ( tmp_nodes->at(j)->getStorage() ){
					tmp_nodes->at(j)->getStorage()->setCapacity( tmp_cap ); 
				}
			}
		}
	}
	/* Percentaje of ADSM controllers per line */
	if ( input.size() > 2 ){
		float number_active;
		TVCLine* tmp_lines;  
		tmp_lines = m_pcGrid->getLines();
		TVCNode* tmp_nodes;
		for ( int i = 0 ; i < tmp_lines->size() ; i++ ){
			tmp_nodes     = tmp_lines->at(i)->getNodes();
			number_active = int ( input[2] * float ( tmp_nodes->size() ) );
			for ( int j = 0 ; j < tmp_nodes->size() ; j++ ){
				if ( j < number_active ){
					tmp_nodes->at(j)->getCtr()->flagLoadCtr(true);					
				}
				else{
					tmp_nodes->at(j)->getCtr()->flagLoadCtr(false);
				}				
			}
		}
	}
	return;
};





























