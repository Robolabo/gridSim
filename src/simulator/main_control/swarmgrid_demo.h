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
/* LIBRARY SWARMGRID MAIN CONTROL					      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef SWARMGRID_DEMO_H_
#define SWARMGRID_DEMO_H_

/* LIBRARIES*/
#include "main_control.h"

/******************************************************************************/
class CSwarmGrid_demo : public CMainControl {

	public:
	/* FUNCTIONS */
	CSwarmGrid_demo   ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf );
	~CSwarmGrid_demo  ( void );		
	
	void  executionStep  ( void );
	void  assessmentStep ( void );
	void  restart        ( void );		

	TVFloat* getEvaluation  ( void );
	float    getAssessment  ( void );

	void setParameters  ( TVFloat );
	void setEnvironment ( TVFloat );	

	private:

	TVFloat  m_vTimeSignal;
	TVFloat  m_vSampledSig;
	TVFloat  m_vNodeSignal;		
	TVFloat  m_vGridSig;
	TVFloat  m_vCmp;		
	TVFloat  m_vResult;

	CFFT*      m_pcFFT;	
	TVFreqCmp  m_vFreqSignal;
	TVFloat    m_vFreqSignal_amp;
	TVFloat    m_vSampForFFT;

	TMFloat m_mHist_Cmp, m_mHist_Frq; 

	sFFTeval m_sFFTst;

	int   m_nFCreject;
	int   m_nAssType;
	int   m_nWbeg, m_nWend;

	int   m_nCtrActTime;

	float m_fBEne;
	float m_fTEne;
	float m_fNEne;

	float m_fLEne;
	float m_fPVEne;
	

	/* FUNCTIONS */
	void       _evaluateFFT   ( TVFreqCmp* );
	void       _evaluateAC    ( void );
	void       _calculateCF   ( void );

	void  _SignalFFT           ( void );	
			
};

#endif




