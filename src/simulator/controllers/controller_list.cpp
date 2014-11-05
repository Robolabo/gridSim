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

#include "controller_list.h"

/******************************************************************************/
CController* createCtr ( sSimCnf*  sSimCnf , CNode* pcNode , XMLElement* cnf ){
	CController* result = NULL;
	string attr = cnf->Attribute("name");
	if ( attr == "bat_ctr" ){
		result = new CBatCtr ( sSimCnf , cnf , pcNode );
	}
	else if ( attr == "ADSM" ){
		result = new CADSMCtr ( sSimCnf , cnf , pcNode );
	}
	else if ( attr == "mufco" ){
		result = new CMUFCOCtr ( sSimCnf , cnf , pcNode );
	}
	else if ( attr == "swarmgrid" ){
		result = new CSwarmGridCtr ( sSimCnf , cnf , pcNode );
	}	
	else if ( attr == "default" ){
		result = new CController ( sSimCnf , pcNode );
	}
	else{
		cout << "ERROR: MAIN CONTROL NAME NOT RECOGNIZED "<< endl;
	}
	return result;
};
