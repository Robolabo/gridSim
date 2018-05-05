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
	/* Setup elements */
	string attr = cnf->Attribute("loads");
	m_nLoads    = atoi( attr.c_str() );	
	attr = cnf->Attribute("type");
	m_nType     = atoi( attr.c_str() );
	attr = cnf->Attribute("pr_mod");
	m_fPrMod     = atof( attr.c_str() );
};	

/******************************************************************************/
void CUser::restart( void ){	
	m_nLoadCnt = 0;	
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CUser::~CUser ( void ){	
	//cout << m_nLoadCnt << endl;
};

/******************************************************************************/
void CUser::executionStep ( void ){
	int ref_time;
	if ( m_nType == 0 ){
		ref_time = m_sSimCnf->nSimStep;
		for ( int i = 0 ; i < m_nLoads ; i++ ){
			if ( ref_time % 1440 == 0 ){
				CDefLoad* tmp_dload;
				TVFloat next_day;
				for ( int j = 0 ; j < 1440 ; j++ ){
					if ( m_sSimCnf->GridProfile.dur == 0 )
						next_day.push_back(1.0);
					else
						next_day.push_back(  m_sSimCnf->GridProfile.nr_profile[ ( ref_time + j )%(m_sSimCnf->GridProfile.dur) ] );
				}		
				int ac_time;			
				tmp_dload = new CDefLoad ( m_sSimCnf );
//				ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
				ac_time   = 1200 + i*60;
				tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
				tmp_dload->setLoadType  ( i + 1 );	
				tmp_dload->setRange     ( 1440 );			
				m_pcNode->getLoad()->setDefLoad( tmp_dload );
					
			}
		}
	}
	else if ( m_nType == 1 ){
		ref_time = m_sSimCnf->nSimStep + 30;//45;
		if ( ref_time % 1440 == 0 ){
			CDefLoad* tmp_dload;
			TVFloat next_day;
			for ( int i = 0 ; i < 1440 ; i++ ){
				if ( m_sSimCnf->GridProfile.dur == 0 )
					next_day.push_back(1.0);
				else
					next_day.push_back(  m_sSimCnf->GridProfile.nr_profile[ ( ref_time + i )%(m_sSimCnf->GridProfile.dur) ] );
			}		
			int ac_time; 		
			for ( int i = 0 ; i < m_nLoads ; i++ ){
				tmp_dload = new CDefLoad ( m_sSimCnf );
				ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
				tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
				tmp_dload->setLoadType  ( 4 );	
				tmp_dload->setRange     ( 1440 );			
				m_pcNode->getLoad()->setDefLoad( tmp_dload );
			}		
		}
	}
	else if ( m_nType == 2 ){
		ref_time = m_sSimCnf->nSimStep + 30;// 45;
		float prob   = m_fPrMod * m_sSimCnf->GridProfile.nr_profile[ ( ref_time )%(m_sSimCnf->GridProfile.dur) ];
		float random = m_sSimCnf->pcRandom->nextDouble();
		if ( random < prob ){
			m_nLoadCnt ++;
			CDefLoad* tmp_dload;
			tmp_dload = new CDefLoad ( m_sSimCnf );		
			tmp_dload->setStartTime ( m_sSimCnf->nSimStep );
			tmp_dload->setLoadType  ( 4 );	
			tmp_dload->setRange     ( 1440 );//( m_sSimCnf->pcRandom->nextInt( 2880 ) );			
			m_pcNode->getLoad()->setDefLoad( tmp_dload );
		}
	}
	else if ( m_nType == 3 ){
		ref_time = m_sSimCnf->nSimStep + 30;// 45;
		if ( ref_time % 1440 == 0 || m_sSimCnf->nSimStep == 0 ){
			float prob, random;			
			for ( int i = 0 ; i < 1440 ; i++ ){
				prob   = m_fPrMod * m_sSimCnf->GridProfile.nr_profile[ ( ref_time + i )%(m_sSimCnf->GridProfile.dur) ];
				random = m_sSimCnf->pcRandom->nextDouble();
				if ( random < prob ){
					m_nLoadCnt ++;
					CDefLoad* tmp_dload;
					tmp_dload = new CDefLoad ( m_sSimCnf );		
					tmp_dload->setStartTime ( m_sSimCnf->nSimStep + i );
					tmp_dload->setLoadType  ( 4 );	
					tmp_dload->setRange     ( 1440 );//( m_sSimCnf->pcRandom->nextInt( 2880 ) );			
					m_pcNode->getLoad()->setDefLoad( tmp_dload );					
				}
			}			
		}
	}
	return;
};






		/*
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
		*/


















