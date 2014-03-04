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
/* LIBRARY USER								      */
/* It is the class of the users		                   		      */
/******************************************************************************/
#ifndef USER_H_
#define USER_H_

class CUser;

/* LIBRARIES*/
#include "common.h"
#include "grid/node.h"
#include "elements/consumption/load.h"
//#include "controllers/controller.h"

typedef vector<CUser*> TVCUser;

/******************************************************************************/
class CUser {

	public:
	/* FUNCTIONS */
	CUser  ( sSimCnf*  sSimCnf , CNode* pcNode , XMLElement* cnf );
	~CUser ( void );	

	void restart( void );
	
	void executionStep  ( void );
	//void linkNode       ( CNode* input ){m_pcNode = input;};	
	//void linkGrid       ( CGrid* );	
	void setID          ( int      input ){m_nID      = input;};
	//void linkController ( CController* input ){m_pcController = input;};		

	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;

	/* VARIABLES */		
	CNode*       m_pcNode;
	//CGrid*       m_pcGrid;	
	//CController* m_pcController;	
	TVFloat*     m_vInputProfile;	

	//TVDefLoad    m_vToDo;
	int          m_nID;
	int          m_nProfile;

	/* FUNCTIONS */		
	int _action   ( int );
	

};

#endif

