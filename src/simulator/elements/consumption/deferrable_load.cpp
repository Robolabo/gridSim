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

#include "deferrable_load.h"

/******************************************************************************/
/* CONSTRUCTOR */
CDefLoad::CDefLoad ( sSimCnf*  sSimCnf ) {
	m_sSimCnf = sSimCnf;

	m_fPower       = 0.0;	
	m_nStartTime   = 0;
	m_bEnd         = false;

	m_sLoadInfo    = NULL;

};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CDefLoad::restart ( void ){

	return;
};

/****************************************************************/
CDefLoad::~CDefLoad ( void ){	
};

/**************************************************
 **************************************************/
void CDefLoad::simulationStep ( void ){
	m_fPower       = 0.0;
	if ( m_sLoadInfo ){
		if ( m_sSimCnf->nSimStep >= m_nStartTime ){
			if ( m_sSimCnf->nSimStep >= m_nStartTime + m_sLoadInfo->dur ){
				m_bEnd   = true;
			}
			else{
				m_fPower = m_sLoadInfo->profile[ m_sSimCnf->nSimStep - m_nStartTime ];
			}
		}
	}
	else{
		m_bEnd   = true;
	}
};

/****************************************************************/
void CDefLoad::setIDLoad ( int input ){
	sDefLoad* info_ptr;
	for ( int i = 0 ; i < m_sSimCnf->LoadDB.def_load.size() ; i++ ){		
		if ( m_sSimCnf->LoadDB.def_load[i].id == input )
			info_ptr = &(m_sSimCnf->LoadDB.def_load[i]);
	}
	m_sLoadInfo = info_ptr;
	return;
};

 




