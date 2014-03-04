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

/****************************************************************/
/* GRID CLASS							*/
/****************************************************************/
#ifndef GRID_H_
#define GRID_H_

class CGrid;

/****************************************************************/
/* LIBRARIES */
#include "common.h"
#include "line.h"

class CGrid {
	public:
	CGrid  ( sSimCnf*  sSimCnf );
	~CGrid ( void    );
	
	void       executionStep ( void );
	void       restart       ( void );

	void       addLine          ( CLine* input ) { m_vLines.push_back(input); };

	TVFloat*   getTimeSignal    ( void )         { return &m_vTimeSignal; };
	TVFloat*   getTimeSample    ( void )         { return &m_vSampledSignal; };
	TVFloat*   getFreqSignalAmp ( void )         { return &m_vFreqSignal_amp; };
	TVFreqCmp* getFreqSignal    ( void )         { return &m_vFreqSignal; };
	
	TVFloat*   getLinesSignal   ( void )         { return &m_vLinesSignal; };

	TVFloat*   getGridSignal    ( void )         { return &m_vGGridSignal; };	

	TVCLine*   getLines         ( void )         { return &m_vLines;};	
 
	bool       is_FFT           ( void )         { return m_bNewFFT; };	

	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;

	CFFT*     m_pcFFT;	
	TVFreqCmp m_vFreqSignal;
	TVFloat   m_vFreqSignal_amp;	
	bool      m_bNewFFT;	

	TVFloat   m_vTimeSignal;
	TVFloat   m_vSampledSignal;
	TVFloat   m_vLinesSignal;
	TVFloat   m_vGGridSignal;

	TVCLine   m_vLines;	

	/* FUNCTIONS */		
	void  _SignalFFT           ( void );
	float _nextGridPower       ( void );
	
};
#endif






