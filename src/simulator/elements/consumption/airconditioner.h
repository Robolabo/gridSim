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
/* LIBRARY AIRCONDITIONER						      */
/* It is the class of the Airconditioner system                		      */
/******************************************************************************/
#ifndef AIRCONDITIONER_H_
#define AIRCONDITIONER_H_

#include "common.h"

class CAirConditioner;

typedef struct {
	float fInitTemp; 			// Initial Temp (oC)
	float fRefTemp;  			// Reference Temp (oC)
	float fEpsilonTemp; 	// Temperature difference at which the Fan does not starts (oC)
	float fFanIntensity;	// Fan Intensity [0.0 - 1.0]
	float fDecayFactor;   // Decay Factor for free dynamics with outdoor temp */
	float fMaxDeltaT; 		// Maximum temperature difference per timestep
	float fMaxPower;   		// Max power to consume
} sAirConditioner;

using namespace std;

class CAirConditioner {

	public:
		CAirConditioner 	 ( sSimCnf*  sSimCnf , XMLElement* cnf );

		void 	simulationStep	 ( void );
		void    restart          ( void );
	

		void 	setReferenceTemp ( float f_temp );
		float   getReferenceTemp ( void );
		
		void 	setFanIntensity  ( float f_value );
		float   getFanIntensity  ( void );

		float	getIndoorTemp	 ( void );
		float	getOutdoorTemp	 ( void ){return m_vfOutdoorTemp.at(m_sSimCnf->nSimStep%m_vfOutdoorTemp.size());};
		float   getInstantPower	 ( void );

		float   getNPower        ( void ){return m_sAirConditioner.fMaxPower;};


	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;

		//bool up;

		int m_nStatus; // 0 off - 1 up - 2 down

		sAirConditioner	m_sAirConditioner;
		sAirConditioner m_sIniCnf;

		float 		m_fIndoorTemp;
		float 		m_fPower;
		vector<float>	m_vfOutdoorTemp; //Cyclid vector for outdoor temp	
};

#endif
