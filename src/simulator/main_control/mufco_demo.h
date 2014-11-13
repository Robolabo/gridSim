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
/* LIBRARY MUFCO MAIN CONTROL						      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef MUFCO_DEMO_MC_H_
#define MUFCO_DEMO_MC_H_

/* LIBRARIES*/
#include "main_control.h"

/******************************************************************************/
class CMUFCO_demo : public CMainControl {

	public:
	/* FUNCTIONS */
	CMUFCO_demo   ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf );
	~CMUFCO_demo  ( void );		
	
	void  executionStep  ( void );
	void  assessmentStep ( void );
	void  restart        ( void );		

	TVFloat* getEvaluation  ( void );
	float    getAssessment  ( void );

	void setParameters  ( TVFloat );	

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
	int   m_nResCmp;
	int   m_nResType;
	int   m_nAssType;	
	
	int   m_nCtrActTime;

	int   m_nWbeg, m_nWend;

	/* FUNCTIONS */
	void       _evaluateFFT   ( TVFreqCmp* );
	void       _evaluateAC    ( void );
	void       _calculateCF   ( void );
	void  _SignalFFT           ( void );	

};

#endif




