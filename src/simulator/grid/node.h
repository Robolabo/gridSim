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
/* NODE CLASS							*/
/****************************************************************/
#ifndef NODE_H_
#define NODE_H_

/****************************************************************/
/* BASIC LIBRARIES */
#include "common.h"

class CNode;
typedef vector<CNode*> TVCNode;

/****************************************************************/
/* LIBRARIES */
#include "elements/generation/PV.h"
#include "elements/storage/storage.h"
#include "elements/line/regulator.h"
#include "elements/line/switch.h"
#include "elements/consumption/load.h"
#include "controllers/controller.h"

class CNode {
	public:
	CNode  ( sSimCnf*  sSimCnf );
	~CNode ( void );
	
	int         executionStep ( void );	
	void  restart       ( void );
	
	CPV*         getPV        ( void ){return m_pcPV;};
        CLoad*       getLoad      ( void ){return m_pcLoad;};	
	CStorage*    getStorage   ( void ){return m_pcStorage;};
	CRegulator*  getRegulator ( void ){return m_pcRegulator;};
	CSwitch*     getSwitch    ( void ){return m_pcSwitch;};
	CController* getCtr       ( void ){return m_pcController;};

	sNPower*    getNPower    ( void ){return &m_sPower;};

	void  addPV       ( CPV*         input ){m_pcPV         = input;};
	void  addLoad     ( CLoad*       input ){m_pcLoad       = input;};
	void  addStorage  ( CStorage*    input ){m_pcStorage    = input;};	
	void  addCtr      ( CController* input ){m_pcController = input;};	
	
	
	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;	
	/* Power Balance Status */	
	sNPower    m_sPower;
	/* Elements */
	CPV*        m_pcPV;
	CLoad*      m_pcLoad;
	CStorage*   m_pcStorage;
	CRegulator* m_pcRegulator; // Separate the battery and load of the grid and generation
	CSwitch*    m_pcSwitch;    // Isolate the system

	CController* m_pcController;	
};
#endif
