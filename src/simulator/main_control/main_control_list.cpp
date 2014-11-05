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

#include "main_control_list.h"

/******************************************************************************/
CMainControl* createMC ( sSimCnf*  sSimCnf , CGrid* pcGrid , TVCController* vCtr , XMLElement* cnf ){
	CMainControl* result = NULL;
	string attr = cnf->Attribute("name");
	if ( attr == "default" ){
		result = new CDefault_MC ( sSimCnf , pcGrid , vCtr , cnf );
	}
	else if ( attr == "local" ){
		result = new CLocal_MC ( sSimCnf , pcGrid , vCtr , cnf );
	}
	else if ( attr == "mufco" ){
		result = new CMUFCO_MC ( sSimCnf , pcGrid , vCtr , cnf );
	}
	else if ( attr == "swarmgrid" ){
		result = new CSwarmGrid_MC ( sSimCnf , pcGrid , vCtr , cnf );
	}
	else{
		cout << "ERROR: MAIN CONTROL NAME NOT RECOGNIZED "<< endl;
	}
	return result;
};
