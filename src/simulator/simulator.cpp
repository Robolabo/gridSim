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
			attr                = elem->Attribute("seed");
			m_nSeed             = atoi( attr.c_str() );
			attr                = elem->Attribute("length");
			m_nStepLimit        = atoi( attr.c_str() );
			attr                = elem->Attribute("sampling");
			m_sSimCnf.nSampling = atoi( attr.c_str() );
			attr                = elem->Attribute("fft_lng");
			m_sSimCnf.nFFTsize  = atoi( attr.c_str() );
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
							tmp_line->addNode    ( _createNode ( node ) );	
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
CNode* CSimulator::_createNode ( XMLElement* elem ){
	string elemName, attr;
	/* Create the node */
	CNode*  tmp_node  = new CNode    ( &m_sSimCnf );
	/* Configure node */
	for( XMLElement* cnf = elem->FirstChildElement() ; cnf != NULL ; cnf = cnf->NextSiblingElement() ){
		elemName = cnf->Value();
		if ( elemName == "pv" ){
			CPV* tmp_PV = new CPV ( &m_sSimCnf , cnf );
			tmp_node->addPV       ( tmp_PV );
		}
		else if ( elemName == "bat" ){
			CStorage* tmp_storage = new CStorage  ( &m_sSimCnf , cnf );
			tmp_node->addStorage     ( tmp_storage );
		}
		else if ( elemName == "ctr" ){
			CController* tmp_ctr = createCtr ( &m_sSimCnf , tmp_node , cnf );
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
	if ( m_pcMainControl )
		m_pcMainControl->restart();		
	m_pcGrid->restart();
	for ( int i = 0 ; i < m_vCtr.size() ; i++ )
		m_vCtr[i]->restart();
	for ( int i = 0 ; i < m_vUsers.size() ; i++ )
		m_vUsers[i]->restart();
	return;
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
/* DESTRUCTOR 							*/
/****************************************************************/
CSimulator::~CSimulator ( void ){
	if ( m_sSimCnf.pcRandom ){		
		delete m_sSimCnf.pcRandom;		
	}
	delete m_pcGrid;
	delete m_pcMainControl;	
	return;
};

/****************************************************************/
/* MAIN FUNCTION						*/
/****************************************************************/
void CSimulator::ExecuteSimulation ( void ){	
	bool  SimFinished = false;
	while (!SimFinished){
		/* EXECUTION */	
		/* Main Control */
		if ( m_pcMainControl )
			m_pcMainControl->executionStep();
		/* Users */
		for ( int i = 0 ; i < m_vUsers.size() ; i++ ){
			m_vUsers[i]->executionStep();
		}		
		/* Controllers */
		for ( int i = 0 ; i < m_vCtr.size() ; i++ ){
			m_vCtr[i]->executionStep();
		}					
		/* GRID */
		m_pcGrid->executionStep();		
		/* WRITE */
		if ( m_sSimCnf.pcWriter )
			m_sSimCnf.pcWriter->write_buffer();
		/* VISUALIZATION */
		if ( m_bVisu  )
			m_sSimCnf.pcPlotter->updateDisplay ();
		/* FINAL CONDITION */
		if ( ( m_bVisu && m_sSimCnf.pcPlotter->closed() ) || ( m_nStepLimit > 0 && m_sSimCnf.nSimStep > m_nStepLimit ) ){
			SimFinished = true;
		}
		/* STEP */
		m_sSimCnf.nSimStep++;
	}
	return;
};

