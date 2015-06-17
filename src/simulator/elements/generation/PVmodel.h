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
/* LIBRARY PV MODEL							      */
/* It is the class of the PV model	                   		      */
/******************************************************************************/
#ifndef PVMODEL_H_
#define PVMODEL_H_

/* LIBRARIES*/
#include "common.h"

using namespace std;

typedef struct{ 
  double k0;
	double k1;
	double k2;
}__attribute__((__packed__)) PVinvParam;

typedef struct{ 
  double height;
	double width;
	double thickness;
}__attribute__((__packed__)) dimension;

typedef struct{ 
  double latitude;
	double longitude;
	double timeZone;
}__attribute__((__packed__)) geoCoord;

typedef struct{ 
  double alfa;
	double beta;
}__attribute__((__packed__)) orientation;

typedef struct{ 
  int hour;
	int day;
	int month;
	int year;
}__attribute__((__packed__)) datePV;

#define ISTC 1000
#define GTEMP 800
#define TSTC 25
#define TEMP 20

#define BO 1367
#define SOLMINUTE 30
#define SPRINGDAY 80
#define AUTUMNDAY 294
#define HOURS 24
#define MADRIDLATITUDE 40.45
#define HALFANGLE 25
#define YEARHOUR 8760

const double DEG_TO_RAD = M_PI / 180.0;
const double RAD_TO_DEG = 180.0 / M_PI;

/******************************************************************************/

class CPVmodel {

	public:
	/* FUNCTIONS */
	CPVmodel  ( sSimCnf*  sSimCnf , XMLElement* cnf );
	~CPVmodel ( void );	

	void    executionStep ( void );
	void    restart ( void );

	float    getPower       ( void ){return  m_fPower;};

	protected:
	/* VARIABLES */


	/* FUNCTIONS */
	

	private:
	/* Simulator stuff */
	sSimCnf*  m_sSimCnf;

	/* VARIABLES */
	float m_fPower;

	/* Inverter */	
	float      m_fACPower;
	double     m_fPnom;
	PVinvParam m_sfInvParam;
	double     m_fInvVL;
	
	/* Generator */
	float m_fDCPower;
	float m_fGenVL; // Voltage losses
	int   m_nNs; // Serial modules
	int   m_nNp; // Parallel modules	
	float m_fDF; // Dispersion factor
	float m_fPnomGen;

	/* Module */
	float m_fPnomMod;
	float m_fGamma;
	float m_fTONC;
	dimension m_sDimension;
	float m_fDirtFactor;
	float m_fModVL;

	/* Radiation */
	float m_pfGTH; // Tilded radiation


	/* FUNCTIONS */
	void _ACPower( float fDCPower );
	void _DCPower( double f_G , double f_Ta );	

	void   _GlobalRadiation           ( float fGobs , datePV s_date , geoCoord s_coord , orientation sf_orientation , double f_reflectivity , double f_dirt_factor );
	double _Gamma			  ( int n_day );
	double _Delta			  ( double f_gamma );
	int    _TZDiffHour		  ( int n_day);
	bool   _ComparisonDouble          ( double f_x , double f_y , double f_epsylon );
	double _ChiC			  ( double f_thetaZS, double f_thetaS );
	double _ChiH			  ( double f_thetaZS );
	double _PsiC			  ( double f_thetaS );
	double _PsiH			  ( double f_thetaZS );
	double _TrueSolarOmega            ( datePV sn_date, int n_day, geoCoord sf_coord, double f_gamma );
	double _ThetaZS                   ( double f_delta, double f_omega, double f_latitude);
	double _ExtraAtmRad               ( double f_gamma , double f_thetaZS );
	double _IndexCorrelation          ( double f_latitude, double f_kth );
	double _ThetaS                    ( double f_delta, double f_omega, double f_latitude, orientation sf_orientation );
	double _TiltedDirectRadiation     ( double f_Bh0, double f_thetaZS, double f_thetaS );
	double _TiltedAlbedoRadiation     ( double f_Gh0, orientation sf_orientation, double f_reflectivity);
	void   _DiffusePerezCoefficients  ( double f_Bh0, double f_Dh0, double f_thetaZS, double* f_k);
	double _TiltedDiffuseCSRadiation  ( double f_Dh0, double f_k3, double f_thetaZS, double f_thetaS);
	double _TiltedDiffuseIsoRadiation ( double f_Dh0, orientation sf_orientation, double f_k3);
	double _TiltedDiffuseHorRadiation ( double f_Dh0, orientation sf_orientation, double f_k4);
	void   _DirtIncAngLosses          ( double f_thetaS, double f_beta,double f_dirt_factor, double* pf_DIALF);



};

#endif

