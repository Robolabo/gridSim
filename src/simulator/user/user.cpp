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

	//m_sParamsFile  = sFilename;
	//m_nSimStep     = nStep;
	//m_pcController = NULL;
	

	//TXML_StringV tmp_pattern;
	//tmp_pattern.push_back ( "USER" );
	//m_nProfile = int ( readParameter ( m_sParamsFile->c_str() , "PROFILE" , &tmp_pattern ) );	
	m_nProfile = 0;

};	

/******************************************************************************/
void CUser::restart( void ){


	return;
};

/******************************************************************************/
//void CUser::linkGrid ( CGrid*   input ){
//	m_pcGrid   = input;
	//TXML_StringV tmp_pattern;
	//tmp_pattern.push_back ( "GRID" );
	//if ( int ( readParameter ( m_sParamsFile->c_str() , "TYPE" , &tmp_pattern ) ) == 2 ){
	//	m_vInputProfile = m_pcGrid->getDailyProfile();
	//}
//	return;
//};

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
		tmp_dload->setIDLoad    ( 1 );	
		m_pcNode->getLoad()->setDefLoad( tmp_dload );	

		tmp_dload = new CDefLoad ( m_sSimCnf );
		ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
		tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
		tmp_dload->setIDLoad    ( 2 );	
		m_pcNode->getLoad()->setDefLoad( tmp_dload );

		tmp_dload = new CDefLoad ( m_sSimCnf );
		ac_time   = m_sSimCnf->pcRandom->roulette( &next_day );
		tmp_dload->setStartTime ( m_sSimCnf->nSimStep + ac_time );
		tmp_dload->setIDLoad    ( 3 );	
		m_pcNode->getLoad()->setDefLoad( tmp_dload );	
	}
	

	/*
	if ( (m_sSimCnf->nSimStep)%1440 == 0 ){
		sDefLoad tmp_load;
		int      tmp_pos;		
		
		tmp_pos          = _action ( 0 );
		tmp_load.ac_time = m_sSimCnf->nSimStep + tmp_pos;
		tmp_load.type    = 0;
		tmp_load.window  = 1440;
		m_vToDo.push_back( tmp_load );

		tmp_pos          = _action ( 1 );
		tmp_load.ac_time = m_sSimCnf->nSimStep + tmp_pos;
		tmp_load.type    = 1;
		tmp_load.window  = 1440;
		m_vToDo.push_back( tmp_load );

		tmp_pos          = _action ( 2 );
		tmp_load.ac_time = m_sSimCnf->nSimStep + tmp_pos;
		tmp_load.type    = 2;
		tmp_load.window  = 1440;
		m_vToDo.push_back( tmp_load );		
	}	

	bool Exit = false;
	int  cnt  = 0;
	while (!Exit){
		if ( cnt >= m_vToDo.size() )
			Exit = true;
		else if ( m_vToDo[cnt].ac_time <= m_sSimCnf->nSimStep ){
			if ( m_pcController ){
				m_pcController->setDefLoad      ( m_vToDo[cnt] );
			}
			else{
				m_pcNode->getLoad()->setDefLoad ( m_vToDo[cnt] );
			}
			m_vToDo.erase( m_vToDo.begin() + cnt );
		}
		else
			cnt++;
	}
	*/

	return;
};

/******************************************************************************/
int CUser::_action ( int type ){

	/*
	int result;
	switch ( m_nProfile ){
		case 0:
			if ( m_pcController )		
				result = NULL;
			else
				result = 1200 + 60 * type;	
			break;
		case 1:
			result = _roulette ( m_vInputProfile );
			break;	
		default:
			result  = 0;
			break;
	}
	return result;
	*/
	return 0;

};

