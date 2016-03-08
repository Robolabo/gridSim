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
/* Methods of default_mc.h 						      */
/******************************************************************************/
#include "default_mc.h"

/******************************************************************************/
/* CONSTRUCTOR */
CDefault_MC::CDefault_MC ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf ) : CMainControl::CMainControl ( sSimCnf , pcGrid , vCtr ) {	
	
};	

/******************************************************************************/
/* RESTART */
void CDefault_MC::restart  ( void ){		
	/* Set plotter */
	if ( m_sSimCnf->pcPlotter ){	
		m_sSimCnf->pcPlotter->setData    ( 0 , &m_vTimeSignal                 );
		m_sSimCnf->pcPlotter->setMarks   ( 0 , &m_vSampledSig                 );
		m_sSimCnf->pcPlotter->setMarksSp ( 0 , m_sSimCnf->nSampling           );
		m_sSimCnf->pcPlotter->setData    ( 1 , &m_vLineSignal                 );		
	}
	/* Restart variables */
	m_fPVenergy = 0.0;

	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CDefault_MC::~CDefault_MC ( void ){

};


/******************************************************************************/
TVFloat* CDefault_MC::getEvaluation  ( void ){	
	m_vEvaluation.clear();
	m_vEvaluation.push_back( m_fPVenergy );
	return &m_vEvaluation;
};	

/******************************************************************************/
/* Execution Step */
void CDefault_MC::executionStep( void ){
	
	return;
};

/******************************************************************************/
/* Assessment Step */
void CDefault_MC::assessmentStep ( void ){
	/* Store info for plotter */
	if ( m_sSimCnf->pcPlotter ){
		m_vTimeSignal.push_back( m_pcGrid->getPower() );
		if ( m_pcGrid->is_Sample() ){		
			m_vSampledSig.push_back( m_pcGrid->getPower_sampled() );
		}
		m_vLineSignal.push_back( m_pcGrid->getPower_lines() );
	}
	/* Fill writer buffer */
	if ( m_sSimCnf->pcWriter ){
		m_sSimCnf->pcWriter->push_buffer( m_pcGrid->getPower() );
		m_sSimCnf->pcWriter->push_buffer( m_pcGrid->getLines()->front()->getNodes()->front()->getNPower()->grid );
		m_sSimCnf->pcWriter->push_buffer( m_pcGrid->getLines()->front()->getNodes()->front()->getNPower()->pv   );	
		m_sSimCnf->pcWriter->push_buffer( m_pcGrid->getLines()->front()->getNodes()->front()->getNPower()->load );	
		m_sSimCnf->pcWriter->push_buffer( m_pcGrid->getLines()->front()->getNodes()->front()->getNPower()->bat  );
		CStorage* tmp_storage = m_pcGrid->getLines()->front()->getNodes()->front()->getStorage();
		if ( tmp_storage ){
			m_sSimCnf->pcWriter->push_buffer(  tmp_storage->getSoC() );	
		}
		else{
			m_sSimCnf->pcWriter->push_buffer( 0.0 );	
		}	
	}
	/* Store PV energy information */
	m_fPVenergy += m_pcGrid->getLines()->front()->getNodes()->front()->getNPower()->pv / 60.0;

	return;
};

/******************************************************************************/
void CDefault_MC::setEnvironment ( TVFloat input ){
	/* PV sampling */
	if ( input.size() > 0 ){
		float tmp_power;
		tmp_power = 240.0 * input[0];	// 240W is the maximum generation
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
	return;
};









