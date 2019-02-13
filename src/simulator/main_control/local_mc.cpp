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
/* Methods of local_mc.h 						      */
/******************************************************************************/
#include "local_mc.h"

/******************************************************************************/
/* CONSTRUCTOR */
CLocal_MC::CLocal_MC ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf ) : CMainControl::CMainControl ( sSimCnf , pcGrid , vCtr ) {	
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
};	

/******************************************************************************/
/* RESTART */
void CLocal_MC::restart  ( void ){
	/* Variables initial values */
	m_vTimeSignal.clear();
	m_vSampledSig.clear();
	m_vNodeSignal.clear();	
	/* Set plotter */
	if ( m_sSimCnf->pcPlotter ){	
		m_sSimCnf->pcPlotter->setData    ( 0 , &m_vTimeSignal                 );
		m_sSimCnf->pcPlotter->setMarks   ( 0 , &m_vSampledSig                 );
		m_sSimCnf->pcPlotter->setMarksSp ( 0 , m_sSimCnf->nSampling           );				
		m_sSimCnf->pcPlotter->setMarks   ( 1 , m_pcGrid->getFreqSignalAmp ( ) );		
		m_sSimCnf->pcPlotter->setData    ( 2 , &m_vNodeSignal                 );		
			
	}
	/* Writer */
	m_Wgrid      = 0.0;
	m_Wnode      = 0.0;
	m_Wgrid_base = 0.0;
	m_Wstatus.clear();
	m_Wstatus1.clear();
	m_Wstatus2.clear();	
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CLocal_MC::~CLocal_MC ( void ){

};

/******************************************************************************/
/* Execution Step */
void CLocal_MC::executionStep( void ){		
	return;
};

/******************************************************************************/
/* Assessment Step */
void CLocal_MC::assessmentStep ( void ){
	/* Get Info */
	if ( m_pcGrid->getPower() > 0.0 ){
		m_vTimeSignal.push_back( m_pcGrid->getPower() );
	}
	else{
		m_vTimeSignal.push_back( 0.0 );
	}		
	if ( m_pcGrid->is_Sample() ){		
		m_vSampledSig.push_back( m_pcGrid->getPower_sampled() );
	}
	m_vNodeSignal.push_back( m_pcGrid->getPower_lines() );
	m_vGridSig.push_back( m_pcGrid->getPower_grid() );
	/* Writter */
	if ( m_sSimCnf->pcWriter && m_sSimCnf->nSimStep != 0 ){ // ( (m_sSimCnf->nSimStep % 60) == 0 )
	if ( ( m_nWend < 0 || m_sSimCnf->nSimStep < m_nWend ) && m_sSimCnf->nSimStep >= m_nWbeg ){ 		
		_getInfo();		
		if ( (m_sSimCnf->nSimStep % 15) == 0 ){
			m_sSimCnf->pcWriter->push_buffer( m_Wgrid      / 15.0 );
			m_sSimCnf->pcWriter->push_buffer( m_Wgrid_base / 15.0 );
			m_sSimCnf->pcWriter->push_buffer( m_Wnode      / 15.0 );								
			for ( int i = 0 ; i < m_Wstatus1.size() ; i++ ){
				m_sSimCnf->pcWriter->push_buffer( m_Wstatus1[i] / 15.0 );
			}
			for ( int i = 0 ; i < m_Wstatus2.size() ; i++ ){
				m_sSimCnf->pcWriter->push_buffer( m_Wstatus2[i] / 15.0 );
			}
			// Clean info
			m_Wgrid      = 0.0;
			m_Wnode      = 0.0;
			m_Wgrid_base = 0.0;
			m_Wstatus.clear();
			m_Wstatus1.clear();
			m_Wstatus2.clear();
		}
				
	}
	}
	return;
};

/******************************************************************************/
void CLocal_MC::_getInfo ( void ){
	m_Wgrid      += m_vTimeSignal.back();
	m_Wnode      += m_vNodeSignal.back();
	m_Wgrid_base += m_vGridSig.back();			
	TVFloat tmp_vec, status_info;
	// AGGREGATED STATUS
	for ( int i = 0 ; i < m_vCtr->size() ; i++ ){
		tmp_vec = m_vCtr->at(i)->getStatus();	
		if ( status_info.size() == 0 ){
			for ( int j = 0 ; j < tmp_vec.size() ; j++ ){
				status_info.push_back( tmp_vec.at(j) ); 
			}
		}
		else{
			for ( int j = 0 ; j < tmp_vec.size() ; j++ ){
				status_info.at(j) += tmp_vec.at(j); 
			}
		}				
	}
	if ( m_Wstatus.size() == 0 ){
		for ( int i = 0 ; i <  status_info.size() ; i++ ){
			m_Wstatus.push_back( status_info[i] );
		}
	}
	else{
		for ( int i = 0 ; i <  status_info.size() ; i++ ){
			m_Wstatus[i] += status_info[i];
		}
	}
	// NODE 1 STATUS
	if ( m_vCtr->size() > 0 ){
		status_info = m_vCtr->at(0)->getStatus();
		if ( m_Wstatus1.size() == 0 ){
			for ( int i = 0 ; i <  status_info.size() ; i++ ){
				m_Wstatus1.push_back( status_info[i] );
			}
		}
		else{
			for ( int i = 0 ; i <  status_info.size() ; i++ ){
				m_Wstatus1[i] += status_info[i];
			}
		}
	}	
	// NODE 2 STATUS
	if ( m_vCtr->size() > 1 ){
		status_info = m_vCtr->at(1)->getStatus();
		if ( m_Wstatus2.size() == 0 ){
			for ( int i = 0 ; i <  status_info.size() ; i++ ){
				m_Wstatus2.push_back( status_info[i] );
			}
		}
		else{
			for ( int i = 0 ; i <  status_info.size() ; i++ ){
				m_Wstatus2[i] += status_info[i];
			}
		}
	}

	return;
};

/******************************************************************************/
float CLocal_MC::getAssessment  ( void ){
	float  result = 0.0;
	return result;
};

/******************************************************************************/
TVFloat* CLocal_MC::getEvaluation  ( void ){
	// PAR
	if ( m_vTimeSignal.size() >= 525600 ){
		float   year_max  = m_vTimeSignal[0];
		float   year_min  = m_vTimeSignal[0];
		float   year_rms  = 0.0;
		float   month_max = m_vTimeSignal[0];
		float   month_min = m_vTimeSignal[0];
		float   month_rms = 0.0;
		TVFloat vmonth_max, vmonth_min, vmonth_rms;
		float   day_max   = m_vTimeSignal[0];
		float   day_min   = m_vTimeSignal[0];
		float   day_rms   = 0.0;
		TVFloat vday_max, vday_min, vday_rms;
		for ( int i = 0 ; i < 525600 ; i++ ){
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
		m_Result.push_back ( pow( year_max            , 1) / pow ( year_rms , 1 ) ); // pow1 -> crest factor | pow2 -> PAPR  
		// Month
		float month_par = 0.0;
		float month_ptp = 0.0;
		for ( int i = 0 ; i < vmonth_max.size() ; i++ ){
			month_par += ( pow( vmonth_max[i]                 , 1) / pow ( sqrt( vmonth_rms[i] / 43800 ) , 1 ) );
		}
		m_Result.push_back ( month_par / float(vmonth_max.size()));
		// Day
		float day_par = 0.0;
		float day_ptp = 0.0;
		for ( int i = 0 ; i < vday_max.size() ; i++ ){
			day_par += pow( vday_max[i]               , 1) / pow ( sqrt ( vday_rms[i] / 1440 ) , 1 );
		}
		m_Result.push_back ( day_par / float(vday_max.size()));
	}	
	return  &m_Result;
};
