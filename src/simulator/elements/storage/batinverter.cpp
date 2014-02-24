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
/* Methods of battery inverter.h 					      */
/******************************************************************************/
#include "batinverter.h"

/******************************************************************************/
/* CONSTRUCTOR */
CBattInverter::CBattInverter ( void ) {

	m_pcBattery = new CLeadAcid( );

	m_fChargePowLimit = 5000.0;

	m_sfBatInvParamInput.k0 = 0.0023;
	m_sfBatInvParamInput.k1 = 0.0103;
	m_sfBatInvParamInput.k2 = 0.0908;

	m_sfBatInvParamOut.k0 = 0.0027;
	m_sfBatInvParamOut.k1 = 0.0158;
	m_sfBatInvParamOut.k2 = 0.0619;

	m_fPnom = 5000.0;

	/* Nominales */
	m_fChargeMaxPow    = 5000.0;
	m_fDisChargeMaxPow = 5000.0;

	return;
};

/******************************************************************************/
/* DESTRUCTOR */
CBattInverter::~CBattInverter ( void ){
	if (m_pcBattery)
		delete m_pcBattery;
	return;
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CBattInverter::restart ( void ){
	m_fChargePowLimit = 5000.0;
	m_pcBattery->restart();
	return;
};

/******************************************************************************/
/* Maximun accepted power  */
double CBattInverter::AcceptedPower( void ){
	double fresult,fAuxPow;
	fresult = m_pcBattery->AcceptedPower();
	if ( fresult > m_fChargePowLimit )
		fresult = m_fChargePowLimit;
	fAuxPow = InputEfficiency(fresult,1);
	fresult = fAuxPow;
	if ( fresult > m_fChargePowLimit )
		fresult = m_fChargePowLimit;
	return fresult;
};

/******************************************************************************/
/* Maximun available power  */
double CBattInverter::AvailablePower( void ){
	double fresult, fAuxPow, fMax;
	fresult = m_pcBattery->AvailablePower();
	fMax    = OutputEfficiency( m_fDisChargeMaxPow, 1 );
	if (fresult > fMax )
		fresult = fMax;	
	fAuxPow = OutputEfficiency(fresult,0);
	fresult = fAuxPow;
	if (fresult > m_fDisChargeMaxPow)
		fresult = m_fDisChargeMaxPow;	
	return fresult;
};

/*******************************************************************************/
/* Process the real export to the battery                                      */ 
double CBattInverter::exportPower( double fpower ){
	double fresult, fEpower, fAuxPow;	
	if ( fpower > m_fChargePowLimit )
		fEpower = m_fChargePowLimit;
	else
		fEpower = fpower;
	fAuxPow = InputEfficiency(fEpower,0);
	fresult = m_pcBattery->receiveEnergy(fAuxPow);	
	fAuxPow = InputEfficiency(fresult,1);
	fresult = fAuxPow;
	return fresult;	
};

/*******************************************************************************/
/* Process the real import from the battery                                    */ 
double CBattInverter::importPower(double fpower){
	double fresult, fIpower, fAuxPow;
	if ( fpower > m_fDisChargeMaxPow )
		fIpower = m_fDisChargeMaxPow;
	else
		fIpower = fpower;
	fAuxPow = OutputEfficiency(fIpower,1); 
	fresult = m_pcBattery->supplyEnergy(fAuxPow);
	fAuxPow = OutputEfficiency(fresult,0);  
	fresult = fAuxPow;	
	return fresult;	
};

/*******************************************************************************/
/* Set charge current */
void CBattInverter::setChargePowLimit ( double fchargepow ){
	if( fchargepow < 0.0 )
		m_fChargePowLimit = 0.0;
	else if ( fchargepow > m_fChargeMaxPow)
		m_fChargePowLimit = m_fChargeMaxPow;	
	else
		m_fChargePowLimit = fchargepow;
	return;
};
/*******************************************************************************/
/* Input Effienciency */
double CBattInverter::InputEfficiency ( double f_power, int n_mode){
	double fpo, fpi, fPower;	
	double fk0 = m_sfBatInvParamInput.k0;
	double fk1 = m_sfBatInvParamInput.k1;
	double fk2 = m_sfBatInvParamInput.k2;	
	/*f_power is the input power*/
	if(n_mode == 0){
		fpi=f_power/m_fPnom;		
		if (fpi < fk0)
			fpo = 0;
		else
			fpo = (-(1+fk1)+ sqrt((1+fk1)*(1+fk1)-4*fk2*(fk0-fpi)))/(2*fk2);
		fPower = fpo*m_fPnom;
	}
	/*f_power is the output power*/
	else{
		fpo=f_power/m_fPnom;				
		fpi=fk2*fpo*fpo+(fk1+1)*fpo+fk0;		
		fPower = fpi*m_fPnom;		
	}	
	return fPower;
};
/*******************************************************************************/
/* Output Effienciency */
double CBattInverter::OutputEfficiency ( double f_power, int n_mode){
	double fpo, fpi, fPower;	
	double fk0 = m_sfBatInvParamOut.k0;
	double fk1 = m_sfBatInvParamOut.k1;
	double fk2 = m_sfBatInvParamOut.k2;	
	/*f_power is the input power*/
	if(n_mode == 0){
		fpi=f_power/m_fPnom;		
		if (fpi < fk0)
			fpo = 0;
		else
			fpo = (-(1+fk1)+ sqrt((1+fk1)*(1+fk1)-4*fk2*(fk0-fpi)))/(2*fk2);
		fPower = fpo*m_fPnom;
	}
	/*f_power is the output power*/
	else{
		fpo=f_power/m_fPnom;				
		fpi=fk2*fpo*fpo+(fk1+1)*fpo+fk0;
		fPower = fpi*m_fPnom;
	}
	return fPower;
};
