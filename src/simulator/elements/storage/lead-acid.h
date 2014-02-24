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
/* LIBRARY LEAD-ACID							      */
/* It is the class of the lead-acid batteries               		      */
/******************************************************************************/
#ifndef LEADACID_H_
#define LEADACID_H_

/* LIBRARIES */
#include "common.h"

//#include <fstream>
//#include <gsl/gsl_rng.h>
//#include "parser.h"

using namespace std;

/******************************************************************************/

class CLeadAcid {

	public:
	/* FUNCTIONS */
	CLeadAcid  ( void );
	~CLeadAcid ( void );	
	
	void  restart         ( void );	
	
	double AcceptedPower  ( void );
	double AvailablePower ( void );
	double receiveEnergy  ( double fsurplus );
	double supplyEnergy   ( double fNeeded );

	double getSoC ( void ){return m_fSoCpercent;};

	void   setCapacity ( float Cap );

	protected:
	/* VARIABLES */

	/* FUNCTIONS */	

	private:
	/* VARIABLES */	
	unsigned int m_unSimStep;
	double       m_fSoC;          // Unit -> Ah
	double       m_fSoCpercent;   // Unit -> %
	double       m_frealTimeStep; // Unit -> s
	double       m_nominalV;      // Unit -> V
	double       m_fNorm1;        // m_frealTimeStep / (3600.0 * m_nominalV)
	double       m_fNorm2;        // m_fCap / ( 100.0 * m_fNorm1 )
	double       m_fCap;
	double       m_fDCap;	// m_fCap / 10.0
	double       m_fF100Cap; // 100.0  / m_fCap; 

	/* FUNCTIONS */		
	void   givePower(double fpower);
	void   takePower(double fpower);
	double FaradayYield(double fpower);

};

#endif
