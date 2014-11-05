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
/* LIBRARY BATTERY INVERTER     					      */
/* It is the class of the batteries inverter               		      */
/******************************************************************************/
#ifndef BATTINVERTER_H_
#define BATTINVERTER_H_

/* LIBRARIES*/
#include "common.h"
#include "lead-acid.h"

using namespace std;

typedef struct{ 
  double k0;
	double k1;
	double k2;
}__attribute__((__packed__)) invParam;

class CBattInverter;

typedef vector<CBattInverter*> TBattInverterV;
typedef vector<CBattInverter*>::iterator TBattInverterVite;

/******************************************************************************/

class CBattInverter {

	public:
	/* FUNCTIONS */
	CBattInverter  ( void );
	~CBattInverter ( void );	

	void  restart       ( void );

	void   setCapacity       ( float Cap ){m_pcBattery->setCapacity( Cap );};	
	void   setChargePowLimit ( double fchargepow );

	double getSoC            ( void ){return m_pcBattery->getSoC();};
	double getCap            ( void ){return m_pcBattery->getCap();};
	double getAvailablePower ( void ){return AvailablePower();};
	double getAcceptedPower  ( void ){return AcceptedPower();};

	double importPower(double fpower); // From the battery
	double exportPower(double fpower); // To the battery	

	protected:
	/* VARIABLES */	
	CLeadAcid* m_pcBattery;
	/* FUNCTIONS */

	private:
	/* VARIABLES */
	invParam m_sfBatInvParamInput;
	invParam m_sfBatInvParamOut;
	double m_fPnom;	
	
	double m_fChargePowLimit;	
	double m_fChargeMaxPow;	
	double m_fDisChargeMaxPow;
	
	/* FUNCTIONS */
	double AcceptedPower( void );
	double AvailablePower( void );	
	double InputEfficiency ( double f_power, int n_mode);
	double OutputEfficiency ( double f_power, int n_mode);
};

#endif
