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
/* LIBRARY MAIN CONTROL							      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef MAIN_CONTROL_H_
#define MAIN_CONTROL_H_

class CMainControl;

/* LIBRARIES*/
#include "common.h"
#include "grid/grid.h"
#include "controllers/controller.h"

/******************************************************************************/
class CMainControl {

	public:
	/* FUNCTIONS */
	CMainControl  ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr );
	virtual ~CMainControl ( void );	
	
	virtual void    executionStep ( void ){};
	virtual void    restart       ( void ){};

	virtual TVFloat getEvaluation ( void ){};

	virtual void    setEnvironment ( TVFloat ){};
	virtual void    setParameters  ( TVFloat ){};

	protected:
	/* Simulator stuff */
	sSimCnf*       m_sSimCnf;
	CGrid*         m_pcGrid;
	TVCController* m_vCtr;

	private:
};

#endif

