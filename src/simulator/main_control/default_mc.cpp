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
		m_sSimCnf->pcPlotter->setData    ( 0 , m_pcGrid->getTimeSignal ( )    );
		m_sSimCnf->pcPlotter->setMarks   ( 0 , m_pcGrid->getTimeSample ( )    );
		m_sSimCnf->pcPlotter->setMarksSp ( 0 , m_sSimCnf->nSampling           );
		m_sSimCnf->pcPlotter->setMarks   ( 1 , m_pcGrid->getFreqSignalAmp ( ) );		
	}
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CDefault_MC::~CDefault_MC ( void ){

};


/******************************************************************************/
TVFloat* CDefault_MC::getEvaluation  ( void ){	
	return NULL;
};	

/******************************************************************************/
/* Execution Step */
void CDefault_MC::executionStep( void ){
	if ( m_sSimCnf->pcWriter ){
		if ( m_pcGrid->getTimeSignal()->size() > 0 )
			m_sSimCnf->pcWriter->push_buffer( m_pcGrid->getTimeSignal()->back() );		
	}
	return;
};






