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
/* LIST OF MAIN CONTROL LIBRARIES 					      */
/******************************************************************************/
#ifndef MAIN_CONTROL_LIST_H_
#define MAIN_CONTROL_LIST_H_

/* LIBRARIES*/
#include "main_control.h"
#include "default_mc.h"
#include "mufco_mc.h"
#include "mufco_demo.h"
#include "swarmgrid_mc.h"
#include "swarmgrid_demo.h"
#include "local_mc.h"

/* Functions */
CMainControl* createMC ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf );

#endif


