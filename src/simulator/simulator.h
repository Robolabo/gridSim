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
/* GRID SIMULATOR HEADER					*/
/****************************************************************/
#ifndef SIMULATOR_H_
#define SIMULATOR_H_

class CSimulator;

/****************************************************************/
/* LIBRARIES */
#include "common.h"
#include "plotter.h"
#include "user/user.h"
#include "grid/grid.h"
#include "elements/generation/PV.h"
#include "elements/storage/storage.h"
#include "controllers/controllers_list.h"
#include "main_control/main_control_list.h"

class CSimulator {
	public:
	CSimulator  ( string );
	~CSimulator ( void );

	void    ExecuteSimulation ( void );
	void    setSeed           ( int seed ){m_nSeed = seed;};
	void    restart           ( void );	

	CMainControl* getMainControl ( void ){return m_pcMainControl;};
		
	
	private:
	/* Simulator stuff */	
	sSimCnf    m_sSimCnf;
	sStructure m_sStructure;
	int        m_nStepLimit;
	int        m_nSeed;
	/* Visualization */
	bool      m_bVisu;	
	/* Grid */
	CGrid*        m_pcGrid;
	/* Main Control */
	CMainControl* m_pcMainControl;
	/* Controllers */
	TVCController m_vCtr;
	/* Users */
	TVCUser       m_vUsers;		
	/* Data Base */
	TMFloat   m_mLoadDB;
	/* FUNCTIONS */
	void   _createCtr     ( XMLElement* elem , CNode* pcNode );	
	void   _createMainCtr ( XMLElement* elem );
	void   _readLoadDB         ( void );
	void   _configureVisu      ( void );
	void   _configureStructure ( XMLElement* elem );
	CNode* _createNode         ( XMLElement* elem );

};
#endif




