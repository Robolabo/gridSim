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
/* SIMULATOR METHODS						*/
/****************************************************************/
#include "simulator.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
CSimulator::CSimulator ( string pcParamsFile ){
	/* Get configuration file */
	m_sSimCnf.sParamsFile = pcParamsFile;	
	/* NULL pointers to objects */
	m_sSimCnf.pcRandom  = NULL;
	m_sSimCnf.pcWriter  = NULL;	
	m_sSimCnf.pcPlotter = NULL;	
	m_pcMainControl     = NULL;
	/* Initialize variables */
	m_bVisu             = false;
	/* Create Grid */
	m_pcGrid            = new CGrid ( &m_sSimCnf );
	/* Configure from XML file */
	XMLDocument conf;
    	conf.LoadFile( m_sSimCnf.sParamsFile.c_str() );
	XMLElement* root = conf.FirstChildElement();
	string elemName , attr;	
	for( XMLElement* elem = root->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "Simulation" ){
			attr                  = elem->Attribute("seed");
			m_nSeed               = atoi( attr.c_str() );
			attr                  = elem->Attribute("length");
			m_nStepLimit          = atoi( attr.c_str() );
			attr                  = elem->Attribute("sampling");
			m_sSimCnf.nSampling   = atoi( attr.c_str() );
			attr                  = elem->Attribute("fft_lng");
			m_sSimCnf.nFFTsize    = atoi( attr.c_str() );
			attr                  = elem->Attribute("data_folder");
			m_sSimCnf.sDataFolder = attr;
		}
		else if ( elemName == "Grid" ){
			_configureGrid ( elem );
		}
		else if ( elemName == "Visualization" ){
			attr    = elem->Attribute("active");
			m_bVisu = bool (atoi( attr.c_str() ));
		}
		else if ( elemName == "Structure" ){
			_configureStructure ( elem );			
		}	
		else if ( elemName == "Writer" ){
			m_sSimCnf.pcWriter = new CWriter ( elem );
		}	
		else if ( elemName == "Main_control" ){			
			m_pcMainControl = createMC ( &m_sSimCnf , m_pcGrid , &m_vCtr , elem );
		}
	}
	conf.Clear();
	/* Read DBs */
	_readLoadDB(); // CAREFUL WITH ABSOLUTE ADDRESSES	
	/* Initialize values */
	restart();
	
	return;
};

/****************************************************************/
void CSimulator::_configureStructure ( XMLElement* elem ){
	string elemName, attr;
	
	/* Create lines */
	attr       = elem->Attribute("lines");
	int nLines = atoi( attr.c_str() );
	for ( int i = 0 ; i < nLines ; i++ ){		
		CLine* tmp_line = new CLine ( &m_sSimCnf );
		for( XMLElement* line = elem->FirstChildElement() ; line != NULL ; line = line->NextSiblingElement() ){
			elemName = line->Value();
			stringstream tmp_name;
			tmp_name << "line_";
			tmp_name << (i+1);
			if ( elemName == tmp_name.str() ){
				/* Create Nodes */
				attr         = line->Attribute("nodes");
				int nNodes   = atoi( attr.c_str() );
				string sType = line->Attribute("type");
				for( XMLElement* node = elem->FirstChildElement() ; node != NULL ; node = node->NextSiblingElement() ){
					elemName = node->Value();
					if ( elemName == sType ){
						for ( int j = 0 ; j < nNodes ; j++ ){	
							tmp_line->addNode    ( _createNode ( node , tmp_line->getPVNrGen() , tmp_line->getPVNrFrc() ) );	
						}
					}
				}				
			}
		}
		m_pcGrid->addLine ( tmp_line );
	}
	return;
};

/****************************************************************/
CNode* CSimulator::_createNode ( XMLElement* elem , TVFloat* pv_gen , TVFloat* pv_frc  ){
	string elemName, attr;
	/* Create the node */
	CNode*  tmp_node  = new CNode    ( &m_sSimCnf );
	/* Configure node */
	for( XMLElement* cnf = elem->FirstChildElement() ; cnf != NULL ; cnf = cnf->NextSiblingElement() ){
		elemName = cnf->Value();
		if ( elemName == "pv" ){
			CPV* tmp_PV = new CPV ( &m_sSimCnf , cnf , pv_gen , pv_frc );
			tmp_node->addPV       ( tmp_PV );
		}
		else if ( elemName == "bat" ){
			CStorage* tmp_storage = new CStorage  ( &m_sSimCnf , cnf );
			tmp_node->addStorage     ( tmp_storage );
		}
		else if ( elemName == "ctr" ){
			CController* tmp_ctr = createCtr ( &m_sSimCnf , tmp_node , cnf );
			tmp_node->addCtr     ( tmp_ctr );
			m_vCtr.push_back     ( tmp_ctr );	
		}
		else if ( elemName == "user" ){
			CUser* tmp_usr       = new CUser ( &m_sSimCnf , tmp_node , cnf );
			m_vUsers.push_back   ( tmp_usr );			
		}
		else if ( elemName == "load" ){
			CLoad*  tmp_Load  = new CLoad ( &m_sSimCnf , cnf );			
			tmp_node->addLoad             ( tmp_Load );
		}
	}
	return tmp_node;
};

/****************************************************************/
void CSimulator::_configureVisu ( void ){
	if ( m_sSimCnf.pcPlotter )
		delete m_sSimCnf.pcPlotter;				
	m_sSimCnf.pcPlotter = new CPlotter ( );
	XMLDocument conf;
    	conf.LoadFile( m_sSimCnf.sParamsFile.c_str() );
	XMLElement* root = conf.FirstChildElement();
	string  elemName , attr;	
	for( XMLElement* elem = root->FirstChildElement() ; elem != NULL ; elem = elem->NextSiblingElement() ){
		elemName = elem->Value();
		if ( elemName == "Visualization" ){
			attr       = elem->Attribute("rf_rate");
			m_sSimCnf.pcPlotter->setRefreshRate ( atoi( attr.c_str() ) );
			int cnt = 0;
			int type;
			for( XMLElement* e = elem->FirstChildElement() ; e != NULL ; e = e->NextSiblingElement() ){
				attr  = e->Attribute("title");
				m_sSimCnf.pcPlotter->createDisplay ( NULL , NULL , 1 , attr );
				attr   = e->Attribute("x_lng");
				m_sSimCnf.pcPlotter->setXrange     ( cnt , atoi( attr.c_str() ) , "Time [k]" );
				int y_ini, y_end;
				attr   = e->Attribute("y_ini");
				y_ini  = atoi( attr.c_str() );
				attr   = e->Attribute("y_end");
				y_end  = atoi( attr.c_str() );
				m_sSimCnf.pcPlotter->setYrange     ( cnt , y_ini , y_end , "Power (W)" );
				cnt++;				
			}			
		}
	}
	conf.Clear();
	return;
};

/****************************************************************/
void CSimulator::_configureGrid ( XMLElement* elem ){
	string attr;
	// Get Grid maximum amplitude
	if ( elem->Attribute("amp") ){
		attr = elem->Attribute("amp");
		m_sSimCnf.GridProfile.amp = atof( attr.c_str() );
	} 
	else{
		m_sSimCnf.GridProfile.amp = 0.0;
	}
	// Configure grid sinusiodal components	
	int nCmp;
	if ( elem->Attribute("cmp") ){
		attr = elem->Attribute("cmp");
		nCmp = atoi( attr.c_str() );
	}	
	else{
		nCmp = 0;
	}
	if ( nCmp > 0 ){
		m_pcGrid->setSCmp ( elem );
	}
	// Get Profile
	if ( elem->Attribute("file") ){
		string file_name;
		attr = elem->Attribute("file");
		file_name.assign   ( m_sSimCnf.sDataFolder );
		file_name.append   ( attr );
		ifstream inputFile ( file_name.c_str() );
		if ( inputFile.is_open() ){
			TVFloat  profile;
			float tmp_float, tmp_float_old, slope;		
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile >> tmp_float_old;
			inputFile.ignore   ( 256, '\n' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			inputFile.ignore   ( 256, ' ' );
			while( !inputFile.eof() ){				
				inputFile >> tmp_float;				
				slope = (tmp_float - tmp_float_old)/60.0;
				for ( int j = 0 ; j  < 60 ; j++ ){
					profile.push_back( tmp_float_old + slope * j );
				}
				tmp_float_old = tmp_float;
				inputFile.ignore   ( 256, '\n' );
				inputFile.ignore   ( 256, ' ' );
				inputFile.ignore   ( 256, ' ' );
				inputFile.ignore   ( 256, ' ' );
				inputFile.ignore   ( 256, ' ' );
				inputFile.ignore   ( 256, ' ' );
			}
			inputFile.close();
			m_sSimCnf.GridProfile.dur     = profile.size();
			m_sSimCnf.GridProfile.profile = profile;
			// Normalized profile
			TVFloat  nr_profile;
			float max = 0.0;
			for ( int i = 0 ; i < profile.size() ; i++ ){
				if ( profile[i] > max ){
					max = profile[i];
				}			
			}
			for ( int i = 0 ; i < profile.size() ; i++ ){
				nr_profile.push_back( profile[i] / max );
			}
			m_sSimCnf.GridProfile.nr_profile = nr_profile;
		}
		else{
			m_sSimCnf.GridProfile.dur = 0;
		}		
	}
	else{
		m_sSimCnf.GridProfile.dur = 0;
	}


	return;
};

/****************************************************************/
void CSimulator::_readLoadDB        ( void ){

	ifstream inputFile;
	TVFloat  tmp_shape;
	float    tmp_power;
	sDefLoad tmp_def_load;
	string   file_name; 

	file_name.assign ( m_sSimCnf.sDataFolder );
	file_name.append ( "def_load_profiles/WM_60_1200" );
	inputFile.open   ( file_name.c_str() );
	tmp_shape.clear  ( );
	while ( !inputFile.eof() ){
		inputFile.ignore( 256 , ' ' );
		inputFile >> tmp_power;
		tmp_shape.push_back( tmp_power );
	}	
	tmp_def_load.id      = 1;
	tmp_def_load.dur     = tmp_shape.size();
	tmp_def_load.profile = tmp_shape; 
	m_sSimCnf.LoadDB.def_load.push_back( tmp_def_load );
	inputFile.close();

	file_name.assign ( m_sSimCnf.sDataFolder );
	file_name.append ( "def_load_profiles/DR_RP_1200" );
	inputFile.open   ( file_name.c_str() );
	tmp_shape.clear  ( );
	while ( !inputFile.eof() ){
		inputFile.ignore( 256 , ' ' );
		inputFile >> tmp_power;
		tmp_shape.push_back( tmp_power );
	}
	tmp_def_load.id      = 2;
	tmp_def_load.dur     = tmp_shape.size();
	tmp_def_load.profile = tmp_shape; 
	m_sSimCnf.LoadDB.def_load.push_back( tmp_def_load );
	inputFile.close();

	file_name.assign ( m_sSimCnf.sDataFolder );
	file_name.append ( "def_load_profiles/DW_RP" );
	inputFile.open   ( file_name.c_str() );	
	tmp_shape.clear();
	while ( !inputFile.eof() ){
		inputFile.ignore( 256 , ' ' );
		inputFile >> tmp_power;
		tmp_shape.push_back( tmp_power );
	}
	tmp_def_load.id      = 3;
	tmp_def_load.dur     = tmp_shape.size();
	tmp_def_load.profile = tmp_shape; 
	m_sSimCnf.LoadDB.def_load.push_back( tmp_def_load );	
	inputFile.close();

	tmp_shape.clear();
	for ( int i = 0 ; i < 60 ; i++ )
		tmp_shape.push_back( 1.0 );
	tmp_def_load.id      = 4;
	tmp_def_load.dur     = tmp_shape.size();
	tmp_def_load.profile = tmp_shape; 
	m_sSimCnf.LoadDB.def_load.push_back( tmp_def_load );

	return;
};

/****************************************************************/
/* RESTART SIMULATION						*/
/****************************************************************/
void CSimulator::restart ( void ){
	/* Random */
	if ( m_sSimCnf.pcRandom )
		delete m_sSimCnf.pcRandom;	
	if ( m_nSeed < 0 )	
		m_sSimCnf.pcRandom = new CRandom ();
	else
		m_sSimCnf.pcRandom = new CRandom ( m_nSeed );
	/* Clock */
	m_sSimCnf.nSimStep       = 0;	
	/* Create Visualization */
	if ( m_bVisu ){
		_configureVisu ( );
	}
	/* Restart objects */				
	m_pcGrid->restart();
	for ( int i = 0 ; i < m_vCtr.size() ; i++ )
		m_vCtr[i]->restart();
	for ( int i = 0 ; i < m_vUsers.size() ; i++ )
		m_vUsers[i]->restart();
	if ( m_pcMainControl )
		m_pcMainControl->restart();
	return;
};

/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
CSimulator::~CSimulator ( void ){
	if ( m_sSimCnf.pcRandom ){		
		delete m_sSimCnf.pcRandom;		
	}
	delete m_pcGrid;
	if ( m_pcMainControl )
		delete m_pcMainControl;	
	for ( int i = 0 ; i < m_vUsers.size() ; i++ )
		delete m_vUsers[i];
	return;
};

/****************************************************************/
/* MAIN FUNCTION						*/
/****************************************************************/
void CSimulator::ExecuteSimulation ( void ){	
	bool  SimFinished = false;
	/* EXECUTION */	
	while (!SimFinished){	
		/* MAIN CONTROL: EXECUTION */	
		if ( m_pcMainControl ){
			m_pcMainControl->executionStep();
		}		
		/* USERS */		
		for ( int i = 0 ; i < m_vUsers.size() ; i++ ){
			m_vUsers[i]->executionStep();
		}			
		/* CONTROLLERS */		
		for ( int i = 0 ; i < m_vCtr.size() ; i++ ){
			m_vCtr[i]->executionStep();
		}		
		/* GRID */		
		m_pcGrid->executionStep();		
		/* MAIN CONTROL: ASSESSMENT */		
		if ( m_pcMainControl )
			m_pcMainControl->assessmentStep();		
		/* WRITE */		
		if ( m_sSimCnf.pcWriter )
			m_sSimCnf.pcWriter->write_buffer();		
		/* VISUALIZATION */
		if ( m_bVisu  )
			m_sSimCnf.pcPlotter->updateDisplay ();	
		
		/* STEP */
		m_sSimCnf.nSimStep++;		
		/* FINAL CONDITION */
		if ( ( m_bVisu && m_sSimCnf.pcPlotter->closed() ) || ( m_nStepLimit > 0 && m_sSimCnf.nSimStep > m_nStepLimit ) ){
			SimFinished = true;
		}
	}
	return;
};
















