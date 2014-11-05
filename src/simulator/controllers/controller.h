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
/* LIBRARY CONTROLLERS							      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

class CController;

/* LIBRARIES*/
#include "common.h"
#include "grid/node.h"
//#include "grid/grid.h"

typedef vector<CController*> TVCController;

/******************************************************************************/
class CController {

	public:
	/* FUNCTIONS */
	CController  ( sSimCnf*  sSimCnf , CNode* pcNode );
	~CController ( void );	
	
	virtual void executionStep ( void ){};
	virtual void restart       ( void ){};

	//virtual void setDefLoad    ( sDefLoad ){};
	//virtual int  getFComp      ( void ){return 0;};
	virtual void setParameters ( TVFloat ){};

	virtual void setTmpStep    ( float ){ };
	virtual void setSoC_dst    ( float ){ };

	virtual void sendFFT       ( TVFreqCmp* i1, sFFTeval* i2 ){};
	
	virtual int     getCycles     ( void ){return 0;};
	virtual int     getCmp        ( void ){return 0;};
	virtual int     getFCreject   ( void ){return 0;};
	virtual TVFloat getStatus     ( void ){};
	virtual float   getSelfC      ( void ){return 0.0;};

	virtual bool flagLoadCtr      ( bool input ){ return false;};
	virtual bool flagLoadCtr      ( void )      { return false;};

	virtual bool flagBatCtr      ( bool input ){ return false;};
	virtual bool flagBatCtr      ( void )      { return false;};
	

	//void   linkNode   ( CNode*   input ){m_pcNode   = input;};	
	//void   linkGrid   ( CGrid*   input ){m_pcGrid   = input;};

	CNode* getNode    ( void ){return m_pcNode;};
	void   setID      ( int      input ){m_nID      = input;};

	protected:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;

	/* VARIABLES */
	CNode*   m_pcNode;
	//CGrid*   m_pcGrid;

	int     m_nID;

	private:
};

#endif

