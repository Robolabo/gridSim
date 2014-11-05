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
/* LIBRARY DEFERRABLE_LOAD						      */
/* It is the class of the deferrable loads              		      */
/******************************************************************************/
#ifndef DEFERRABLE_LOAD_H_
#define DEFERRABLE_LOAD_H_

#include "common.h"

class CDefLoad;

using namespace std;

typedef vector<CDefLoad*> TVCDefLoad;

class CDefLoad {

	public:
	CDefLoad  ( sSimCnf*  sSimCnf );
	~CDefLoad ( void );

	void 	simulationStep	 ( void );
	void    restart          ( void );

	void    setStartTime     ( int  input ){ m_nStartTime = input;};	
	void    setLoadType      ( int  input );
	void    setRange         ( int  input ){ m_nRange = input;};
	void	setCtrFlag       ( bool input ){ m_bCtrFlag = input;};
	
	bool    isFinished       ( void ){return m_bEnd;};
	bool    isCtrFlag        ( void ){return m_bCtrFlag;};

	float     getPower         ( void ){return m_fPower;};	
	int       getDur           ( void ){return m_sLoadInfo->dur;};  
	int       getRange         ( void ){return m_nRange;};
	int       getStartTime     ( void ){return m_nStartTime;};
	sDefLoad* getLoadInfo      ( void ){return m_sLoadInfo;};
	
	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;	
	int       m_nStartTime;	
	sDefLoad* m_sLoadInfo;
	bool      m_bEnd;
	float 	  m_fPower;
	int       m_nRange;

	bool      m_bCtrFlag;
	
};

#endif




