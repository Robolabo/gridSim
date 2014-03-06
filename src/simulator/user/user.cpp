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
/* Methods of user.h	 						      */
/******************************************************************************/
#include "user.h"

/******************************************************************************/
/* CONSTRUCTOR */
CUser::CUser ( sSimCnf*  sSimCnf , CNode* pcNode , XMLElement* cnf ){
	/* Simulator stuff */	
	m_sSimCnf = sSimCnf;	
	m_pcNode  = pcNode;
};	

/******************************************************************************/
void CUser::restart( void ){
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CUser::~CUser ( void ){
	
};

/******************************************************************************/
void CUser::executionStep ( void ){	

	CDefLoad* tmp_dload;
	if ( (m_sSimCnf->nSimStep)%1440 == 0 ){

		TVFloat next_day;
		for ( int i = 0 ; i < 1440 ; i++ ){
			if ( m_sSimCnf->GridProfile.dur == 0 )
				next_day.push_back(1.0);
			else
				next_day.push_back(  m_sSimCnf->GridProfile.profile[ (m_sSimCnf->nSimStep + i)%(m_sSimCnf->GridProfile.dur) ] );
		}
		
		int ac_time; 

		tmp_dload = new CDefLoad ( m_sSimCnf );
		ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
		tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
		tmp_dload->setLoadType  ( 1 );	
		tmp_dload->setLoadID    ( 10 * m_sSimCnf->nSimStep + 1 );
		m_pcNode->getLoad()->setDefLoad( tmp_dload );	

		tmp_dload = new CDefLoad ( m_sSimCnf );
		ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
		tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
		tmp_dload->setLoadType  ( 2 );	
		tmp_dload->setLoadID    ( 10 * m_sSimCnf->nSimStep + 2 );
		m_pcNode->getLoad()->setDefLoad( tmp_dload );

		tmp_dload = new CDefLoad ( m_sSimCnf );
		ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
		tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
		tmp_dload->setLoadType  ( 3 );	
		tmp_dload->setLoadID    ( 10 * m_sSimCnf->nSimStep + 3 );
		m_pcNode->getLoad()->setDefLoad( tmp_dload );	
	}

	return;
};



