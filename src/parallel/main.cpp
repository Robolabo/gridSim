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
/* PARALLEL EXPERIMENTS DISPATCHER MAIN FILE                                  */
/******************************************************************************/




/******************************************************************************/

/* LIBRARIES */
#include <mpi.h>
#include "simulator.h"
#include "main_control/main_control_list.h"

using namespace std;

/******************************************************************************/
/* STRUCTS */
struct SParamConf{
	int   length;
	float offset;
	float step;
};
typedef vector<SParamConf> TVCParams; 

/******************************************************************************/
/* FUNCTIONS */
void   IRACE_slave         ( void );
void   MPI_slaveLoop       ( void );
void   MPI_masterLoop      ( void );
bool   experimentGenerator ( int *p, int *e );
void   parallelSetUp       ( void );

/******************************************************************************/
/* GLOBAL VARIABLES */
string g_sMainFilename; // Configuration File
string g_sOutputFile;   // Output File for MPI Master or I-RACE output
string g_sExpConf;      // Experiment Configuration File

int    g_nSeeds   = 1;
int    g_nParamN  = 1;
int    g_nEnvirN  = 1;
int    g_nExpSize = 1;
int    g_nResltN  = 1;

TVCParams g_vParams;
TVCParams g_vEnvirs;

int    g_ne1 = 6;
int    g_ne2 = 2;
float  g_fk  = 0.0;
float  g_fp  = 0.0;
int    g_nr  = 0;
float  g_fb  = 0.0;

int    g_nNumProcs;     // Number of process in MPI environment
int    g_nMyid;         // MPI ID of the process
int    g_nCurrentExp;   // Experiment ID for the experiment generator

/******************************************************************************/
/* MAIN 								      */
/******************************************************************************/
int main (int argc, char** argv) {	
	bool irace = false;
	/* Command line input */
	for(int i=1; i<argc; i++) { 
		if(argv[i][0] != '-') { // Type error
			cout << "./parallel [-p <parameters file>]" << endl;
			exit(0);
		}
		switch(argv[i][1]) { //switch on current argument
			/* Get Input File */
			case 'p':
				++i;
				g_sMainFilename.assign(argv[i]);
				//cout << " Filename: " << g_sMainFilename << " CHOOSEN." << endl;
				break;		
			case 'o':
				++i;
				g_sOutputFile.assign(argv[i]);
				//cout << " Filename: " << g_sMainFilename << " CHOOSEN." << endl;
				break;	
			case 'e':
				switch(argv[i][2]) {
					case '1':
						++i;	
						g_ne1 = atoi(argv[i]);				
						break;
					case '2':	
						++i;	
						g_ne2 = atoi(argv[i]);				
						break;					
					default:
						break;
				}
				irace = true;
				break;
			case 'k':
				++i;	
				g_fk = atof(argv[i]);	
				irace = true;					
				break;
			case 's':
				++i;	
				g_fp = atof(argv[i]);	
				irace = true;					
				break;		
			case 'r':
				++i;	
				g_nr = atoi(argv[i]);	
				irace = true;					
				break;	
			case 'b':
				++i;	
				g_fb = atof(argv[i]);	
				irace = true;					
				break;
			/* Option not recognized*/
			default:
				cout << "ERROR: Option not recognized" << endl;
				break;
		}
	}
	if ( irace ){
		IRACE_slave();
	}
	else{
		/* Configure Parallel Environment */
		parallelSetUp ( );
		/* EXECUTION */	
		/* MPI Execution */	
		/* Initialize MPI environment */	
		int argc2    = 0;
		char **argv2 = NULL;
		MPI_Init      ( &argc2, &argv2 );
		MPI_Comm_size ( MPI_COMM_WORLD, &g_nNumProcs );
       		MPI_Comm_rank ( MPI_COMM_WORLD, &g_nMyid );	
		/* Launch master or slave mode */		
		if ( g_nMyid == 0 )
			MPI_masterLoop();
		else
			MPI_slaveLoop();
		/* Close MPI environment */		
		MPI_Finalize();
		cout << "Process " << g_nMyid << " finalized " << endl;
	}	
	return 0;		
};

/******************************************************************************/
/* IRACE SLAVE LOOP 							      */
/******************************************************************************/
void IRACE_slave ( void ){
	/* Create Simulator and take over it */
	CSimulator   *pcSimulator   = new CSimulator ( g_sMainFilename );
	CMainControl *pcMainControl = pcSimulator->getMainControl();
	/* Set environment */	
	pcSimulator->setFFTsize  ( pow ( 2 , g_ne1 ) );
	pcSimulator->setSampling ( g_ne2 * 45 );					
	/* Set Controllers */	
	TVFloat param;
	param.push_back( - g_fk );
	param.push_back(   g_fp );
	param.push_back(   g_nr );
	param.push_back(   g_fb );	
	pcMainControl->setParameters ( param );							
	/* Restart Simulator */	
	pcSimulator->restart       (  );
	/* Execute the experiment */
	pcSimulator->ExecuteSimulation ( );
	/* Get the results */
	float   ave_res = pcMainControl->getAssessment();
	/* Write the output */
	ofstream output (g_sOutputFile.c_str());	
	output << ave_res << endl;
	output.close();
	/*	
	cout << " FITNESS: " << ave_res << endl;
	TVFloat* result = pcMainControl->getEvaluation();	
	cout << " RESULT: ";
	if ( result ){
		for ( int i = 0 ; i < result->size() ; i++ ){
			cout << result->at(i) << " ";
		}
	}
	cout << endl;
	*/
	/* Kill simulator */
	delete pcSimulator;
	return;
};

/******************************************************************************/
/* MPI SLAVE LOOP 							      */
/* Ask for job to the MPI Master 					      */
/* Set and execute the experiment					      */
/******************************************************************************/
void MPI_slaveLoop  ( void ){	
	cout << "Created process " << g_nMyid << " as SLAVE " << endl;
	/* Parallel stuff */
	int                flags_tx [1];
	int                flags_rx [1];
	int                nParams  [g_nParamN];
	int                nEnviro  [g_nEnvirN];
	float              fResults [g_nResltN];
	int                nSeed    [1];
	bool               Exit;
	MPI_Status         stat;	
	std::vector<float> tmp_vec;	
	flags_tx[0] = 0;	

	/* Create Simulator*/
	CSimulator   *pcSimulator   = new CSimulator ( g_sExpConf );
	CMainControl *pcMainControl = pcSimulator->getMainControl();

	/* SLAVE Main Loop - getting work and executing*/	
	Exit = false;
	while(!Exit){
		/* Ask for job */
		MPI_Send ( flags_tx   , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD );        		
		MPI_Recv ( flags_rx   , 1 , MPI_INT , 0 , 1 , MPI_COMM_WORLD, &stat ); 
		/* Flag to continue working or finish the process */
		if ( flags_rx[0] == 1 ){		
			/* Receives work */			
			MPI_Recv ( nParams , g_nParamN , MPI_INT  , 0 , 3 , MPI_COMM_WORLD, &stat );
			MPI_Recv ( nEnviro , g_nEnvirN , MPI_INT  , 0 , 8 , MPI_COMM_WORLD, &stat );
			MPI_Recv ( nSeed   , 1         , MPI_INT  , 0 , 6 , MPI_COMM_WORLD, &stat );										
			/* Restart Simulator */
			pcSimulator->setSeed       ( nSeed[0] );
			pcSimulator->restart       (  );	
			/* Set environment */	
			TVFloat enviro;
			for ( int i = 0 ; i < g_nEnvirN ; i++ ){
				enviro.push_back( g_vEnvirs[i].step * float(nEnviro[i]) + g_vEnvirs[i].offset );
			}
			//enviro.push_back( g_vEnvirs[0].step * float(nEnviro[0]) + g_vEnvirs[0].offset );
			//enviro.push_back( g_vEnvirs[1].step * float(nEnviro[1]) + g_vEnvirs[1].offset );
			pcMainControl->setEnvironment ( enviro );					
			/* Set Controllers */		
			TVFloat param;
			for ( int i = 0 ; i < g_nParamN ; i++ ){
				param.push_back( g_vParams[i].step * float(nParams[i]) + g_vParams[i].offset );
			}
			//param.push_back( g_vParams[0].step * float(nParams[0]) + g_vParams[0].offset );
			//param.push_back( g_vParams[1].step * float(nParams[1]) + g_vParams[1].offset );
			pcMainControl->setParameters ( param );			
			/* Execute the experiment */
			pcSimulator->ExecuteSimulation ( );
			/* Get the results */
			TVFloat* tmp_res = pcMainControl->getEvaluation();
			for ( int i = 0 ; i < g_nResltN ; i++ )
				fResults[i] = tmp_res->at(i);			
			/* Send the results to the MPI Master*/		
			MPI_Send ( nParams  , g_nParamN , MPI_INT   , 0 , 2 , MPI_COMM_WORLD );
			MPI_Send ( nEnviro  , g_nEnvirN , MPI_INT   , 0 , 7 , MPI_COMM_WORLD );	
			MPI_Send ( nSeed    , 1         , MPI_INT   , 0 , 5 , MPI_COMM_WORLD );		
			MPI_Send ( fResults , g_nResltN , MPI_FLOAT , 0 , 4 , MPI_COMM_WORLD );			
			flags_tx[0] = 1;					
		}
		else
			Exit = true;
	}
	/* Delete Simulator */
	delete pcSimulator;
	
	return;
};

/******************************************************************************/
/* MPI MASTER LOOP							      */
/* Manage the MPI execution						      */
/* Calls the experiment generator and send the experiment to the MPI salves   */
/* Receives the experiment results and stores it			      */
/* Finish the MPI execution when the experiments are finished		      */
/******************************************************************************/
void MPI_masterLoop ( void ){
	cout << "Created process " << g_nMyid << " as MASTER " << endl;	
	/* MPI communication variables */
	int         index;
	int         nParams_tx [g_nParamN]; 	// Algorithm Parameters Sent 
	int         nParams_rx [g_nParamN]; 	// Algorithm Parameters Received 
	int         nEnviro_tx [g_nEnvirN]; 	// Environment Parameters Sent
	int         nEnviro_rx [g_nEnvirN]; 	// Environment Parameters Received
	int         nSeed_tx   [1];       	// Seed Sent
	int         nSeed_rx   [1];	  	// Seed Received
	float       fResults   [g_nResltN]; 	// Results Received
	int         flags_tx   [1];       	// Flags Sent
	int         flags_rx   [1];	  	// Flags Received
	bool        buff_req   [g_nNumProcs-1]; // Slaves Request 
	MPI_Request request    [g_nNumProcs-1]; // Request Information
	MPI_Status  stat;                       // MPI status variable
	// Experiment ID initialized
	g_nCurrentExp = 0;
	/* Open and Initialize OutputFile */
	ofstream outFile( g_sOutputFile.c_str() );
	for ( int i = 0 ; i < g_nParamN ; i++ )
		outFile << "p" << i+1 << " ";
	for ( int i = 0 ; i < g_nEnvirN ; i++ )
		outFile << "e" << i+1 << " ";
	outFile << "seed ";	
	for ( int i = 0 ; i < g_nResltN ; i++ )
		outFile << "r" << i+1 << " ";
	outFile << endl;
	/* Set all slaves as no requested */
	for (int i = 0 ; i < g_nNumProcs - 1 ; i++)
		buff_req[i] = false;
	/* MASTER Main Loop : It send experiemnts if the experiment generator has experiments to be done */	
	while ( experimentGenerator( nParams_tx , nEnviro_tx ) ){
		/* SEED Loop : Each experiment is repeated for a different seed */
		for ( nSeed_tx[0]   = 1 ; nSeed_tx[0]  <= g_nSeeds ; nSeed_tx[0]++ ){  
			/* Request slaves if they are idle */
			for(int i = 0 ; i < g_nNumProcs - 1 ; i++){
        	        	if (!buff_req[i]){
        	        	        MPI_Irecv( flags_rx , 1 , MPI_INT , i + 1 , 0 , MPI_COMM_WORLD , &request[i] );
					buff_req[i] = true;
        	                }
        	        }		
			/* Wait until one slave is idle */
			MPI_Waitany( g_nNumProcs - 1 , request, &index, &stat);			
			/* If the slave has results, store it (flags_rx[0] == 1) */			
			if (flags_rx[0] == 1){
				MPI_Recv ( nParams_rx , g_nParamN , MPI_INT   , index+1 , 2 , MPI_COMM_WORLD, &stat );
				MPI_Recv ( nEnviro_rx , g_nEnvirN , MPI_INT   , index+1 , 7 , MPI_COMM_WORLD, &stat );
				MPI_Recv ( nSeed_rx   , 1 	  , MPI_INT   , index+1 , 5 , MPI_COMM_WORLD, &stat );
				MPI_Recv ( fResults   , g_nResltN , MPI_FLOAT , index+1 , 4 , MPI_COMM_WORLD, &stat );
				/* Print the experiment summary in the OutputFile */
				for ( int i = 0 ; i < g_nParamN ; i++ )
					outFile << nParams_rx[i] << " ";
				for ( int i = 0 ; i < g_nEnvirN ; i++ )
					outFile << nEnviro_rx[i] << " ";
				outFile << nSeed_rx[0] << " ";
				for ( int i = 0 ; i < g_nResltN ; i++ )
					outFile << fResults[i]   << " ";
				outFile << endl;
			}				
			/* If the slave is idle, send it work */
			flags_tx[0] = 1;
			MPI_Send ( flags_tx   , 1         , MPI_INT  , index + 1 , 1 , MPI_COMM_WORLD );
			MPI_Send ( nParams_tx , g_nParamN , MPI_INT  , index + 1 , 3 , MPI_COMM_WORLD );
			MPI_Send ( nEnviro_tx , g_nEnvirN , MPI_INT  , index + 1 , 8 , MPI_COMM_WORLD );	
			MPI_Send ( nSeed_tx   , 1         , MPI_INT  , index + 1 , 6 , MPI_COMM_WORLD );		
        	        buff_req[index] = false;
		}
	}	
	/* MASTER Last Results Reception : Wait until all slaves have finished */
	/* Request all slaves if they have finished the work */
	for(int i = 0 ; i < g_nNumProcs - 1 ; i++){
               	if (!buff_req[i]){
               	        MPI_Irecv( flags_rx , 1 , MPI_INT , i + 1 , 0 , MPI_COMM_WORLD , &request[i] );                                
                        buff_req[i] = true;
                }
        }	
	/* Wait until all slaves have finished : If the slave has finished, kill it */
	bool Continue = true;
	while(Continue){
		/* Wait until one slave is idle */
		MPI_Waitany( g_nNumProcs - 1 , request, &index, &stat);	
		/* If the slave has results, store it (flags_rx[0] == 1) */			
		if (flags_rx[0] == 1){
			MPI_Recv ( nParams_rx , g_nParamN , MPI_INT   , index+1 , 2 , MPI_COMM_WORLD, &stat );
			MPI_Recv ( nEnviro_rx , g_nEnvirN , MPI_INT   , index+1 , 7 , MPI_COMM_WORLD, &stat );
			MPI_Recv ( nSeed_rx   , 1 	  , MPI_INT   , index+1 , 5 , MPI_COMM_WORLD, &stat );
			MPI_Recv ( fResults   , g_nResltN , MPI_FLOAT , index+1 , 4 , MPI_COMM_WORLD, &stat );
			/* Print the experiment summary in the OutputFile */
			for ( int i = 0 ; i < g_nParamN ; i++ )
				outFile << nParams_rx[i] << " ";
			for ( int i = 0 ; i < g_nEnvirN ; i++ )
				outFile << nEnviro_rx[i] << " ";
			outFile << nSeed_rx[0] << " ";
			for ( int i = 0 ; i < g_nResltN ; i++ )
				outFile << fResults[i]   << " ";
			outFile << endl;			
		}				
		buff_req[index] = false;
		/* Kill slave */
		flags_tx[0] = 0;
		MPI_Send ( flags_tx   , 1 , MPI_INT  , index + 1 , 1 , MPI_COMM_WORLD );
		/* Check if all slaves have finished */
		Continue = buff_req[0];
		for (int i = 1 ; i < g_nNumProcs - 1 ; i++ )
			Continue = Continue || buff_req[i];
	}	
	outFile.close();
	return;
};

/******************************************************************************/
/* EXPERIMENT GENERATOR							      */
/* Set the algorithm and environment parameters experiment  		      */
/* Experiment configuration values is an integer number 		      */
/* The range of this parameters [0,LNG_PARAM-1] or [0,LNG_ENVIR-1]            */
/* respectively.							      */
/* NOTE: The experiment design file used range is [1,LNG_PARAM] 	      */
/*       Because of this, the readed value is modified			      */
/******************************************************************************/
bool experimentGenerator ( int *p, int *e ){
	int      tmp_exp;
	bool     result;
	/* Full Factorial Design */
	tmp_exp  = g_nCurrentExp;
	g_nCurrentExp++;
	if (g_nCurrentExp <= g_nExpSize ){
		for ( int i = 0 ; i < g_nEnvirN ; i++ ){
			e[g_nEnvirN - 1 - i] = tmp_exp%g_vEnvirs[g_nEnvirN - 1 - i].length;
			tmp_exp -= e[g_nEnvirN - 1 - i];
			tmp_exp /= g_vEnvirs[g_nEnvirN - 1 - i].length;
		}
		for ( int i  = 0 ; i < g_nParamN ; i++ ){
			p[g_nParamN - 1 - i] = tmp_exp % g_vParams[g_nParamN - 1 - i].length;
			tmp_exp -= p[g_nParamN - 1 - i];
			tmp_exp /= g_vParams[g_nParamN - 1 - i].length;
		}
		result = true;
	}
	else
		result = false;

	return result;
};

/******************************************************************************/
/* SETUP THE PARALLEL ENVIRONMENT					      */
/******************************************************************************/
void parallelSetUp       ( void ){
	XMLDocument conf;
    	conf.LoadFile( g_sMainFilename.c_str() );
	XMLElement* root = conf.FirstChildElement();
	string elemName , attr;	

	for( XMLElement* elem = root->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();

		if ( elemName == "output" ){
			attr          = elem->Attribute("file");
			g_sOutputFile = attr;
		}
		else if ( elemName == "exp_conf"  ){
			attr       = elem->Attribute("file");
			g_sExpConf = attr;
		}
		else if ( elemName == "seed"){
			attr       = elem->Attribute("number");
			g_nSeeds   = atoi( attr.c_str() );
		}
		else if ( elemName == "prm"){
			SParamConf tmp_param;
			attr             = elem->Attribute("length");
			tmp_param.length = atoi( attr.c_str() );
			attr             = elem->Attribute("offset");	
			tmp_param.offset = atof( attr.c_str() );
			attr             = elem->Attribute("step");
			tmp_param.step   = atof( attr.c_str() );
			g_vParams.push_back( tmp_param );			
		}
		else if ( elemName == "env"){
			SParamConf tmp_envir;
			attr             = elem->Attribute("length");
			tmp_envir.length = atoi( attr.c_str() );
			attr             = elem->Attribute("offset");	
			tmp_envir.offset = atof( attr.c_str() );
			attr             = elem->Attribute("step");
			tmp_envir.step   = atof( attr.c_str() );
			g_vEnvirs.push_back( tmp_envir );			
		}
		else if ( elemName == "result"){
			attr       = elem->Attribute("length");
			g_nResltN  = atoi( attr.c_str() );			
		}
	}
	g_nParamN = g_vParams.size();
	g_nEnvirN = g_vEnvirs.size(); 
	for ( int i = 0 ; i < g_nParamN ; i++ )
		g_nExpSize *= g_vParams[i].length;
	for ( int i = 0 ; i < g_nEnvirN ; i++ )
		g_nExpSize *= g_vEnvirs[i].length;
	conf.Clear();	

	return;
};


