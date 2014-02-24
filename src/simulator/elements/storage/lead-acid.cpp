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
/* Methods of lead-acid.h						      */
/******************************************************************************/
#include "lead-acid.h"

/******************************************************************************/
/* CONSTRUCTOR */
CLeadAcid::CLeadAcid ( void ){	
	m_fCap          = 800;  // Bat capacity in Ah
	m_fDCap         = m_fCap/10.0;
	m_fF100Cap      = 100.0/m_fCap;
	m_frealTimeStep = 60;   // Seconds of the time-base	
	m_nominalV      = 48.0;
	m_fNorm1        = m_frealTimeStep / (3600.0 * m_nominalV);
	m_fNorm2        = m_fCap / ( 100.0 * m_fNorm1 );
	
	m_fSoCpercent = 50;	// Initial SoC [0:100] 
	m_fSoC        = m_fSoCpercent * m_fCap / 100.0; // Ah		
	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CLeadAcid::~CLeadAcid ( void ){
	return;
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CLeadAcid::restart ( void ){
	m_fSoCpercent = 50;	// Initial SoC [0:100] 
	m_fSoC        = m_fSoCpercent * m_fCap / 100.0; // Ah
	return;
};

/******************************************************************************/
/* Maximun accepted power  */
double CLeadAcid::AcceptedPower ( void ){
	double fresult;
	//fresult  = (((100.0 - m_fSoCpercent) * m_fCap * m_nominalV * 3600.0) / (100.0 * m_frealTimeStep));
	fresult  = ( 100.0 - m_fSoCpercent ) * m_fNorm2;
	if (fresult <= 100.0)
		fresult = 100.0;
	return fresult;
};

/******************************************************************************/
/* Maximun available power  */
double CLeadAcid::AvailablePower ( void ){
	double fresult;
	//fresult = ((m_fSoCpercent * m_fCap * m_nominalV * 3600.0) / (100.0 * m_frealTimeStep));
	fresult = m_fSoCpercent * m_fNorm2;
	return fresult;
};

/*******************************************************************************/
/* Power aceptance of real battery                                             */ 
double CLeadAcid::receiveEnergy(double fsurplus){
	double fresult, fP_accepted;
	fP_accepted = AcceptedPower();
	if (fsurplus >= fP_accepted){
		givePower (fP_accepted);
		fresult = fP_accepted;
	}
	else {
		givePower (fsurplus);
		fresult = fsurplus;
	}	
	return fresult;
};

/*******************************************************************************/
/* Insert power during a time period on the battery                            */
void CLeadAcid::givePower(double fpower){
	//m_fSoC += ((fpower*FaradayYield(fpower)*m_frealTimeStep)/(3600.0*m_nominalV));
	m_fSoC += fpower * FaradayYield(fpower) * m_fNorm1;
	if (m_fSoC >= m_fCap )
		m_fSoC = m_fCap;
	m_fSoCpercent = m_fSoC * m_fF100Cap;
	return;
};

/*********************************************************************************/
/* Power supply of real battery                                                  */
/* Output indicates the amount of power which has to be supplied by other source */ 
double CLeadAcid::supplyEnergy (double fNeeded){
	double fresult, fP_available;	
	fP_available = AvailablePower();
	if (fNeeded >= fP_available){
		takePower (fP_available);
		fresult = fP_available;
	}
	else {
		takePower (fNeeded);
		fresult = fNeeded;
	}	
	return fresult;
};

/*******************************************************************************/
/* Extract power during a time period from the battery                         */
void CLeadAcid::takePower(double fpower){	
	//m_fSoC -= ( (fpower * m_frealTimeStep) / (3600.0 * m_nominalV) );
	m_fSoC -= fpower * m_fNorm1;	
	if (m_fSoC <= 0.0)
		m_fSoC = 0.0;	
	m_fSoCpercent = m_fSoC * m_fF100Cap;
	return;
};

/*******************************************************************************/
/* Calculate the Faraday Yield of the battery						                       */
double CLeadAcid::FaradayYield(double fpower){	
	//double etac;
	//etac = 1.0 - exp ( ( 20.73 * ( (m_fSoCpercent/100.0) - 1.0 ) ) / ( ( (fpower/m_nominalV) / (m_fcapacity/10.0) ) + 0.55 ) );
	//return etac; 
	return ( 1.0 - exp ( ( 0.2073 * m_fSoCpercent - 20.73 ) / ( ( ( fpower / m_nominalV ) / m_fDCap ) + 0.55 ) ) );
};

/******************************************************************************/
/* Set Capacity */
void   CLeadAcid::setCapacity ( float fCap ){
	m_fCap     = fCap;
	m_fDCap    = m_fCap/10.0; 
	m_fF100Cap = 100.0/m_fCap;
	m_fNorm2   = m_fCap / ( 100.0 * m_fNorm1 );
	m_fSoC     = m_fSoCpercent * m_fCap / 100.0; // Ah
	return;
};

