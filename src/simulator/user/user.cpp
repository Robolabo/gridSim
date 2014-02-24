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
CUser::CUser ( sSimCnf*  sSimCnf ){

	m_sSimCnf = sSimCnf;	

	//m_sParamsFile  = sFilename;
	//m_nSimStep     = nStep;
	m_pcController = NULL;
	m_pcNode       = NULL;

	//TXML_StringV tmp_pattern;
	//tmp_pattern.push_back ( "USER" );
	//m_nProfile = int ( readParameter ( m_sParamsFile->c_str() , "PROFILE" , &tmp_pattern ) );	
	m_nProfile = 0;

};	

/******************************************************************************/
void CUser::linkGrid ( CGrid*   input ){
	m_pcGrid   = input;

	//TXML_StringV tmp_pattern;
	//tmp_pattern.push_back ( "GRID" );
	//if ( int ( readParameter ( m_sParamsFile->c_str() , "TYPE" , &tmp_pattern ) ) == 2 ){
	//	m_vInputProfile = m_pcGrid->getDailyProfile();
	//}

	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CUser::~CUser ( void ){
	
};

/******************************************************************************/
void CUser::executionStep ( void ){

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

/******************************************************************************/
int CUser::_roulette ( TVFloat* prob ){

	/*
	int     result;	
	TVFloat roulette;
	roulette.push_back( (*prob)[0] );
	for ( int i = 1 ; i < prob->size() ; i++ ){
		roulette.push_back( (*prob)[i] +  roulette.back() );
	}	
	float rand = m_sSimCnf->pcRandom->nextDouble( roulette.back() );	
	bool  Exit = false;
	int   cnt  = 0;
	while (!Exit){
		if ( roulette[cnt] > rand ){
			result = cnt;
			Exit   = true;
		}
		else{
			cnt++;
		}
	}
	return result;
	*/
	return 0;
	
};
