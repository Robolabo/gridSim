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
/* LIBRARY LOAD								      */
/* It is the class of the LOAD system	                   		      */
/******************************************************************************/
#ifndef LOAD_H_
#define LOAD_H_

/* LIBRARIES*/
#include "common.h"
#include "airconditioner.h"
#include "deferrable_load.h"

using namespace std;

/******************************************************************************/

class CLoad {
	public:
	/* FUNCTIONS */
	CLoad  ( sSimCnf*  sSimCnf , XMLElement* cnf );
	~CLoad ( void );	
	
	void             executionStep     ( void );
	void             restart           ( void );

	float            getPower          ( void ){return m_fPower;};	
	float            getDefPower       ( void ){return m_fDefPower;};
	float            getNDefPower      ( void ){return m_fNDefPower;};
	float            getAmp            ( void ){return m_fAmp;};
	CAirConditioner* getAirConditioner ( void ){return m_pcAir;};
	TVCDefLoad*      getDefLoads       ( void ){return &m_vDefLoad;};	
	
	void             setDefLoad        ( CDefLoad* input ){m_vDefLoad.push_back( input );};	
	void             setDirectPower    ( float );

	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;
	TVFloat   m_vFixC;

	/* Elements */
	CAirConditioner* m_pcAir;

	/* VARIABLES */		
	float      m_fPower;
	bool       m_bDPower;
	float      m_fDPower;
	float      m_fDefPower;
	float      m_fNDefPower;
	TVCDefLoad m_vDefLoad;

	float      m_fAmp;
	float      m_fAmp_ND;
	int        m_nType_ND;

	/* FUNCTIONS */	
	void  _read_FixedC_house ( string );
};

#endif






















