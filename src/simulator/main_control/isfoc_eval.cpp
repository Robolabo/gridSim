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
/* Methods of isfoc_eval.h 						      */
/******************************************************************************/
#include "isfoc_eval.h"

/******************************************************************************/
/* CONSTRUCTOR */
CIsfocEval::CIsfocEval ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf ) : CMainControl::CMainControl ( sSimCnf , pcGrid , vCtr ) {	
	
};	

/******************************************************************************/
/* RESTART */
void CIsfocEval::restart  ( void ){
	/* Link node */
	m_pcNode = m_vCtr->at(0)->getNode();

	/* Initialize variables */
	m_sNEnergy.pv           = 0.0;
	m_sNEnergy.pv_wasted    = 0.0;     
	m_sNEnergy.load         = 0.0;
	m_sNEnergy.bat_exp      = 0.0; //from node to bat
	m_sNEnergy.bat_imp      = 0.0; //from bat to node
	m_sNEnergy.grid_exp     = 0.0; //from node to grid
	m_sNEnergy.grid_imp     = 0.0; //from grid to node	
	m_sNEnergy.num_bo       = 0;

	m_vPV.clear();
	m_vGrid.clear();
	m_vLoad.clear();
	m_vBat.clear();
	m_vSC.clear();

	/* Set plotter */
	if ( m_sSimCnf->pcPlotter ){	
		m_sSimCnf->pcPlotter->setData( 0 , &m_vGrid );
		m_sSimCnf->pcPlotter->setData( 1 , &m_vPV );
		m_sSimCnf->pcPlotter->setData( 2 , &m_vLoad );
		m_sSimCnf->pcPlotter->setData( 3 , &m_vBat );
		m_sSimCnf->pcPlotter->setData( 4 , &m_vSC );
	}
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CIsfocEval::~CIsfocEval ( void ){

};

/******************************************************************************/
void CIsfocEval::setEnvironment ( TVFloat input ){

	if ( input.size() > 0 )
		m_pcNode->getPV()->setPAmp( input[0] );	
	if ( input.size() > 1 )
		m_pcNode->getStorage()->setCapacity( input[1] );

	//cout << input[0] << " " << input[1] << endl;
	
	return;
};	

/******************************************************************************/
void CIsfocEval::setParameters  ( TVFloat input ){

	if ( input.size() > 0 )
		m_vCtr->at(0)->setSoC_dst( input[0] );
	if ( input.size() > 1 )
		m_vCtr->at(0)->setTmpStep( input[1] );

	return;
};

/******************************************************************************/
TVFloat* CIsfocEval::getEvaluation  ( void ){
	m_vResult.clear();
	m_vResult.push_back(m_vSC.back());
	m_vResult.push_back( float( m_vCtr->at(0)->getCycles() ) );
	return &m_vResult;
};	

/******************************************************************************/
/* Execution Step */
void CIsfocEval::executionStep( void ){
	sNPower* sPower = m_pcNode->getNPower();
	m_vGrid.push_back ( sPower->grid );
	m_vPV.push_back   ( sPower->pv   );
	m_vLoad.push_back ( sPower->load );
	m_vBat.push_back  ( sPower->bat  );

	m_sNEnergy.pv           += sPower->pv / 60.0;
	m_sNEnergy.pv_wasted    += sPower->pv_wasted / 60.0;
	m_sNEnergy.load         += sPower->load / 60.0;
	if ( sPower->bat > 0.0 )
		m_sNEnergy.bat_exp      += sPower->bat / 60.0; //from node to bat
	else
		m_sNEnergy.bat_imp      -= sPower->bat / 60.0; //from bat to node
	if ( sPower->grid < 0.0 )
		m_sNEnergy.grid_exp     -= sPower->grid / 60.0; //from node to grid
	else
		m_sNEnergy.grid_imp     += sPower->grid / 60.0; //from grid to node
	if ( sPower->blackout )
		m_sNEnergy.num_bo++;

	float sc;
	if ( m_sNEnergy.load > 0 )
		sc = (m_sNEnergy.load - m_sNEnergy.grid_imp)/m_sNEnergy.load;
	else
		sc = 0.0;

	m_vSC.push_back( sc );

	if ( m_sSimCnf->pcWriter ){
		//m_sSimCnf->pcWriter->push_buffer( m_pcNode->getLoad()->getAirConditioner()->getOutdoorTemp( )   );
		//m_sSimCnf->pcWriter->push_buffer( m_pcNode->getLoad()->getAirConditioner()->getIndoorTemp( )    );
		//m_sSimCnf->pcWriter->push_buffer( m_pcNode->getLoad()->getAirConditioner()->getReferenceTemp( ) );
	}

	return;
};
