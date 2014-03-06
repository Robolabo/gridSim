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
	CAirConditioner* getAirConditioner ( void ){return m_pcAir;};
	TVCDefLoad*      getDefLoads       ( void ){return &m_vDefLoad;};
	
	
	void             setDefLoad        ( CDefLoad* input ){m_vDefLoad.push_back( input );};
	
	//TVFloat* getShape    ( int type ){return &((*m_mLoadDB)[ type ]);};

	//float    getPhase    ( void ){ return m_fPhase;    };
	//float    getPeriod   ( void ){ return m_fPeriod;   };
	//float    getArgument ( void ){ return m_fArgument; };
	//TMFloat* getLoadDB   ( void ){ return  m_mLoadDB;  };
	
	//void     setDefLoad  ( sDefLoad );	
	//void     setPeriod   ( float input ){ m_fPeriod = input; };
	//void     setPhase    ( float input ){ m_fPhase  = input; };
	//void     setLoadDB   ( TMFloat* );	

	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;

	/* Elements */
	CAirConditioner* m_pcAir;

	/* VARIABLES */
	int       m_nType;
	//float     m_fPeriod;
	//float     m_fPhase;
	//float     m_fArgument;

	//string    m_sSourceFile;	
	float     m_fPower;

	TVCDefLoad m_vDefLoad;

	//TMFloat*  m_mLoadDB;
	
	//TVFloat   m_vDefLoad;
	//TVFloat   m_vNDefLoad;

	/* FUNCTIONS */	
	
	//float _executeProfiles   ( void );	
	//float _executeSinusoidal ( void );

};

#endif

