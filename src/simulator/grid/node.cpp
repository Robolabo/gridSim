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
/* NODE METHODS							*/
/****************************************************************/
#include "node.h"

/****************************************************************/
/* CONSTRUCTOR 							*/
/****************************************************************/
CNode::CNode ( sSimCnf*  sSimCnf ){
	/* Simulator stuff */
	m_sSimCnf = sSimCnf;
	/* Initialize elements */
	m_pcPV        = NULL;
	m_pcLoad      = NULL;
	m_pcStorage   = NULL;
	m_pcRegulator = new CRegulator ();
	m_pcSwitch    = new CSwitch    ();	
};

/****************************************************************/
/* DESTRUCTOR 							*/
/****************************************************************/
CNode::~CNode ( void ){		
	if ( m_pcPV )
		delete m_pcPV;
	if ( m_pcLoad )
		delete m_pcLoad;
	if ( m_pcStorage )
		delete m_pcStorage;
	delete m_pcRegulator;
	delete m_pcSwitch;	
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CNode::restart ( void ){
	/* Initialize powers */
	m_sPower.pv        = 0.0;
	m_sPower.pv_wasted = 0.0;
	m_sPower.load      = 0.0;
	m_sPower.bat       = 0.0;
	m_sPower.grid      = 0.0;
	m_sPower.blackout  = false;
	/* Restart elements */
	m_pcPV->restart();
	m_pcLoad->restart();
	m_pcStorage->restart();
	m_pcRegulator->restart(); 
	m_pcSwitch->restart();   
	return;
};


/****************************************************************/
/* EXECUTION FUNCTION						*/
/****************************************************************/
int CNode::executionStep ( void ){
	int nerror = 0;
	/* Update basic elements and get power */
	/* Update PV power*/	
	if ( m_pcPV ){
		m_pcPV->executionStep  ();
		m_sPower.pv = m_pcPV->getPower ();
	}
	else{
		m_sPower.pv = 0.0;
	}
	/* Update Load power */
	if ( m_pcLoad ){
		m_pcLoad->executionStep  ();
		m_sPower.load = m_pcLoad->getPower ();
	}
	else{
		m_sPower.load = 0.0;		
	}
	/* Power Balance */
        //float fRegulatorLimit = m_pcRegulator->getPowerLimit();
	float fGenPower;
	/* If the system is isolated */
	if ( m_pcSwitch->isOpen() ){ 
		m_sPower.grid = 0.0;
		// Calculate usable power and wasted
		if ( m_sPower.pv > m_pcRegulator->getPowerLimit() ){
			m_sPower.pv_wasted = m_sPower.pv - m_pcRegulator->getPowerLimit();
			fGenPower          = m_pcRegulator->getPowerLimit();
		}
		else{
			m_sPower.pv_wasted = 0.0;
			fGenPower          = m_sPower.pv;		
		}		
		// With storage
		if ( m_pcStorage ){
			float  faccepted  = m_pcStorage->getAcceptedPower();
			double favailable = m_pcStorage->getAvailablePower();
			if ( fGenPower < m_sPower.load ){
				if ( favailable >= m_sPower.load - fGenPower ){
					m_sPower.blackout   = false;
					m_sPower.bat        = - m_pcStorage->importPower( m_sPower.load - fGenPower );
				}
				else{
					m_sPower.blackout = true;
					m_sPower.bat      = - m_pcStorage->importPower( favailable );
				}
			}
			else{
				m_sPower.blackout   = false;
				if ( faccepted >= fGenPower - m_sPower.load )				
					m_sPower.bat        = m_pcStorage->exportPower( fGenPower - m_sPower.load );		
				else{
					m_sPower.bat        = m_pcStorage->exportPower( faccepted );
					m_sPower.pv_wasted += fGenPower - m_sPower.load - faccepted;
				}
			}		
		}
		// Without storage
		else{
			m_sPower.bat = 0.0;
			if ( fGenPower < m_sPower.load ){
				m_sPower.blackout = true;
			}	
			else{
				m_sPower.blackout   = false;
				m_sPower.pv_wasted += fGenPower - m_sPower.load;
			}	
		}		
	}
	/* If the system is conected to the grid. System balance: m_sPower.grid = m_sPower.load - m_sPower.pv + m_sPower.bat */
	else{
		fGenPower          = m_pcRegulator->getPowerLimit();
		m_sPower.pv_wasted = 0.0;
		// With storage
		if ( m_pcStorage ){
			float  faccepted  = m_pcStorage->getAcceptedPower();
			double favailable = m_pcStorage->getAvailablePower();
			if ( fGenPower < m_sPower.load ){
				m_sPower.grid = fGenPower - m_sPower.pv;		
				if ( favailable >= m_sPower.load - fGenPower ){
					m_sPower.blackout = false;
					m_sPower.bat      = - m_pcStorage->importPower( m_sPower.load - fGenPower );					
				}
				else{
					m_sPower.blackout = true;
					m_sPower.bat = - m_pcStorage->importPower( favailable );					
				}
			}
			else{
				m_sPower.blackout = false;
				if ( faccepted >= fGenPower - m_sPower.load ){				
					m_sPower.bat  = m_pcStorage->exportPower( fGenPower - m_sPower.load );
					m_sPower.grid = fGenPower - m_sPower.pv; 
				}		
				else{
					m_sPower.bat  = m_pcStorage->exportPower( faccepted );
					m_sPower.grid = m_sPower.load + m_sPower.bat - m_sPower.pv;
				}
			}		
		}
		// Without storage
		else{
			m_sPower.bat = 0.0;
			if ( fGenPower < m_sPower.load ){
				m_sPower.blackout = true;
				m_sPower.grid = fGenPower - m_sPower.pv;
			}
			else{
				m_sPower.blackout = false;
				m_sPower.grid = m_sPower.load - m_sPower.pv;
			}
		}
	}
	return nerror;
};



