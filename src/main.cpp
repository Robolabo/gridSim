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
/* GRID SIMULATOR (gridSim) MAIN FILE                                         */
/******************************************************************************/



/******************************************************************************/
/* LIBRARIES */
#include "simulator.h"

/******************************************************************************/

/* MAIN */
int main(int argc, char** argv) {	

	string pcExperimentFilename;

	for(int i=1; i<argc; i++) { //loop on command line arguments
		if(argv[i][0] != '-') { // Type error
			cout << "./gridSim [-p <parameters file>]" << endl;
			exit(0);
		}
		switch(argv[i][1]) { //switch on current argument
			/* Get Input File */
			case 'p':
				++i;
				pcExperimentFilename.assign(argv[i]);
				cout << " Filename: " << pcExperimentFilename << " CHOOSEN." << endl;
				break;					
			/* Option not recognized*/
			default:
				cout << "ERROR: Option not recognized" << endl;
				break;
		}
	}

	/* EXECUTION */	
	int start_time = time(0);
	CSimulator *pcSimulator = new CSimulator ( pcExperimentFilename );
	pcSimulator->ExecuteSimulation ( );
	// Get results
	CMainControl *pcMainControl = pcSimulator->getMainControl();
	if ( pcMainControl ){
		TVFloat* result = pcMainControl->getEvaluation();	
		cout << " RESULT:     ";
		if ( result ){
			for ( int i = 0 ; i < result->size() ; i++ ){
				cout << result->at(i) << " ";
			}
		}
		cout << endl;
		cout << " ASSESSMENT: " << pcMainControl->getAssessment() << endl;
	}
	delete pcSimulator;
	cout << " EXCT TIME:  "<< time(0) - start_time << endl;
	return 0;		
};




