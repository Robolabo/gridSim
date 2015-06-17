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
/* METHODS OF PV MODEL	 						      */
/******************************************************************************/
#include "PVmodel.h"

/******************************************************************************/
/* CONSTRUCTOR */
CPVmodel::CPVmodel ( sSimCnf*  sSimCnf , XMLElement* cnf ){
	/* General Simulator Configuration */
	m_sSimCnf = sSimCnf;
	/* Configure model */

	/* Module */	
	m_fPnomMod             = 0.01;
	m_fGamma               = 0.004;
	m_fTONC	               = 47;
	m_sDimension.height    = 1310;
	m_sDimension.width     = 652;
	m_sDimension.thickness = 35;
	m_fDirtFactor          = 0.98;
	m_fModVL               = 0.0;

	/* Inverter */
	m_fPnom         = 0.009;
	m_sfInvParam.k0	= 0.005163226;
	m_sfInvParam.k1 = 0.010096562;
	m_sfInvParam.k2 = 0.021009642;
	m_fInvVL	= 0.005;

	/* Generator */
	m_fGenVL = 0.01;
	m_nNs    = 1; // Serial modules
	m_nNp    = 1; // Parallel modules	
	m_fDF    = 0.0;
	m_fPnomGen = m_nNs * m_nNp * m_fPnomMod * ( 1 - m_fModVL ) * (1 - m_fDF);

	return;
};

/******************************************************************************/
void CPVmodel::restart ( void ){
	m_fPower = 0.0;
	return;
};	

/******************************************************************************/
/* DESTRUCTOR */
CPVmodel::~CPVmodel ( void ){
	return;
};

/******************************************************************************/
/* Step execution */
void CPVmodel::executionStep( void ){	
	m_fPower = 0.0;

	return;
};


/******************************************************************************/
/* AC POWER FROM DC POWER */
void CPVmodel::_ACPower( float fDCPower ){	

	double fk0 = m_sfInvParam.k0;
	double fk1 = m_sfInvParam.k1;
	double fk2 = m_sfInvParam.k2;

	double fpi;
	double fpo;

	fpi = fDCPower/m_fPnom;
		
	if (fpi < fk0){
		fpo = 0;
	}
	else{
		fpo = (-(1+fk1)+ sqrt((1+fk1)*(1+fk1)-4*fk2*(fk0-fpi)))/(2*fk2);
	}

	m_fACPower = fpo * m_fPnom * (1 - m_fInvVL);

	return;
};

/******************************************************************************/
/* DC POWER */
void CPVmodel::_DCPower( double f_G , double f_Ta ){

	double fTONC   = m_fTONC;
	double fMGamma = m_fGamma; 
	double fTc;

	fTc        = f_Ta + ( fTONC - (double)TEMP ) * f_G / (double)GTEMP;		
	m_fDCPower = m_fPnomGen * ( f_G / ISTC ) * (1 - fMGamma * ( fTc - (double)TSTC )) * ( 1 - m_fGenVL );
	
	return;
};


/******************************************************************************/
/* RADIATION */
void CPVmodel::_GlobalRadiation( float fGobs , datePV s_date , geoCoord s_coord , orientation sf_orientation , double f_reflectivity , double f_dirt_factor ){

	int nDayYear;

	double fGh0;     //Horizontal Global radiation
	double fBoh;     //Horizontal Extra-atmospherical radiation
	double fBh0;     //Horizontal Direct radiation
	double fDh0;     //Horizontal Diffuse radiation
	
	double fGhab;    //Tilted Global radiation
	double fBhab;    //Tilted Direct radiation
	double fALhab;   //Tilted Reflected radiation
	double fDhISOab; //Tilted Isotropic Diffuse radiation
	double fDhCSab;  //Tilted Circunsolar Diffuse radiation
	double fDhHORab; //Tilted Horizontal Diffuse radiation
	
	//Angular variables
	double fGamma;
	double fDelta;
	double fOmega;
	double fThetaZS;
	double fThetaS;

	//Horizontal relationships
	double fKth;
	double fKdh;

	//Perez Coefficients
	double* pfK=new double[2];

	//Dirt and incidence Loss Factor 
	double* pfDIALF=new double[3];

//	DayForwardTime (s_date.year,m_nDayForward);	
//	DayDelayTime (s_date.year,m_nDayDelay);	

	nDayYear = 1;//DayOfYear (s_date); // Day of the year

	fGamma = _Gamma( nDayYear );
	fDelta = _Delta( fGamma );
	
	fGh0        = fGobs;

	//Check if the irradiance is cero	
	if ( fGh0 == 0 ){
		m_pfGTH = 0;			
	}
	else{
	
		fOmega   = _TrueSolarOmega ( s_date , nDayYear , s_coord , fGamma );

		fThetaZS = _ThetaZS        ( fDelta , fOmega , s_coord.latitude );

		//Calculate the extra-atmospheric radiation
		fBoh     = _ExtraAtmRad ( fGamma , fThetaZS );

		//Clarity Index
		fKth = fGh0 / fBoh;

		//Calculate components of the horizontal global radiation
		if ( (fKth < 0 || fKth > 0.9) || ( fKth > 0.7 && fGh0 < 200 ) ) {
			m_pfGTH = 0;				
		}			
		else {
			fKdh = _IndexCorrelation( s_coord.latitude , fKth );
			fDh0 = fKdh * fGh0;
			fBh0 = fGh0 - fDh0;
			
			fThetaS = _ThetaS ( fDelta , fOmega , s_coord.latitude , sf_orientation );
			
			//Calculate the tilted components of the radiation
			fBhab   = _TiltedDirectRadiation ( fBh0 , fThetaZS , fThetaS );
				
			fALhab  = _TiltedAlbedoRadiation ( fGh0 , sf_orientation , f_reflectivity );
	
			_DiffusePerezCoefficients ( fBh0 , fDh0 , fThetaZS , pfK );
	
			fDhCSab  = _TiltedDiffuseCSRadiation  ( fDh0 , pfK[0] , fThetaZS , fThetaS); 
			fDhISOab = _TiltedDiffuseIsoRadiation ( fDh0 , sf_orientation , pfK[0]); 
			fDhHORab = _TiltedDiffuseHorRadiation ( fDh0 , sf_orientation , pfK[1]); 	
			
			//Shadows loss factors REMOVED
		
			//Losses of the incidence angle and dirtiness
			_DirtIncAngLosses ( fThetaS , sf_orientation.beta , f_dirt_factor , pfDIALF );

			//Final Radiation Values
			fBhab    = fBhab    * pfDIALF[0];
			fALhab   = fALhab   * pfDIALF[2];
			fDhCSab  = fDhCSab  * pfDIALF[0];
			fDhISOab = fDhISOab * pfDIALF[1];
			fDhHORab = fDhHORab * pfDIALF[1];	
				
			m_pfGTH = fBhab + fDhCSab + fDhISOab + fDhHORab + fALhab;
		}
	}	
	
	delete[] pfK;	
	delete[] pfDIALF;
	
	return;
};

/******************************************************************************/
/*
 *	Gamma of the year
 */
/******************************************************************************/
double CPVmodel::_Gamma(int n_day){
	double fGamma;	
	fGamma = 2 * M_PI * ((double) n_day - 1) / 365;
	return fGamma;
};

/******************************************************************************/
/*
 *	Declination angle
 */
/******************************************************************************/
double CPVmodel::_Delta(double f_gamma){
	double fDelta;
	fDelta = 0.006918 - 0.399912 * cos(f_gamma) + 0.070257 * sin(f_gamma) - 0.006758 * cos(2 * f_gamma) + 0.000907 * sin(2 * f_gamma) - 0.002697 * cos(3 * f_gamma) + 0.00148 * sin(3 * f_gamma);
	return fDelta;
};

/******************************************************************************/
/*
 *	True solar hour
 */
/******************************************************************************/
double CPVmodel::_TrueSolarOmega ( datePV sn_date, int n_day, geoCoord sf_coord, double f_gamma ){
	double fTE;
	double fSolH;

	fTE = (229.18/60) * (-0.000075 + 0.001868 * cos(f_gamma) - 0.032077 * sin(f_gamma) - 0.014615 * cos(2 * f_gamma) - 0.04089 * sin(2 * f_gamma));
	fSolH = (double)sn_date.hour + ((double) SOLMINUTE) / 60 - 12.0 + fTE - (double)_TZDiffHour(n_day) - (sf_coord.timeZone - sf_coord.longitude) / 15;
	
	return (DEG_TO_RAD * 15 * fSolH);
};

/******************************************************************************/
/*
 *	Time Zone difference in hours
 */
/******************************************************************************/
int CPVmodel::_TZDiffHour(int n_day){
//	if (n_day <= m_nDayForward || n_day>= m_nDayDelay)
//		return 1;
//	else
//		return 2;
	return 1;
};

/******************************************************************************/
/*
 *	Zenital Distance
 */
/******************************************************************************/
double CPVmodel::_ThetaZS(double f_delta, double f_omega, double f_latitude){
	double fPhi;
	double fThetaZS;
	fPhi     = DEG_TO_RAD * f_latitude;
	fThetaZS = acos(sin(f_delta) * sin(fPhi) + cos(f_delta) * cos(fPhi) * cos(f_omega));
	return fThetaZS;
};

/******************************************************************************/
/*
 * Extra Atmospheric Radiation
 */
/******************************************************************************/
double CPVmodel::_ExtraAtmRad ( double f_gamma , double f_thetaZS ){
	double fEpsilon;
	double fBoh; 
	fEpsilon = 1.00011 + 0.034221 * cos(f_gamma) + 0.00128 * sin(f_gamma) + 0.000719 * cos(2 * f_gamma) + 0.000077 * sin(2 * f_gamma);
	fBoh = (double)BO * fEpsilon * cos(f_thetaZS);
	return fBoh;
};


/******************************************************************************/
/*
 * Correlation between clarity index and diffusive fraction
 */
/******************************************************************************/
double CPVmodel::_IndexCorrelation(double f_latitude, double f_kth){
	double fCorrelation;
	//Macagnan Correlation
	if( _ComparisonDouble( f_latitude , (double)MADRIDLATITUDE , 0.2 ) ){
		if (f_kth < 0.2) 
		{
			fCorrelation = 0.996 + 0.0424 * f_kth - 0.586 * pow(f_kth,2);
		}else if (f_kth < 0.7)
		{
			fCorrelation = 1.11 - 0.203 * f_kth - 2.52 * pow(f_kth,2) + 0.617 * pow(f_kth,3) + 1.063 * pow(f_kth,4);
		}else
			fCorrelation = -0.0169 - 0.99 * f_kth + 1.63 * pow(f_kth,2);
	}
	//Erbs Correlation
	else {
		if (f_kth <= 0.22) 
		{
			fCorrelation = 1 - 0.09 * f_kth; 
		}else if (f_kth <= 0.8)
		{
			fCorrelation = 0.9511 - 0.1604 * f_kth + 4.388 * pow(f_kth,2) - 16.638 * pow(f_kth,3) + 12.336 * pow(f_kth,4);
		}else
			fCorrelation = 0.165;
	}
	return fCorrelation;
};

/******************************************************************************/
//Comparison of two double.
/******************************************************************************/
bool CPVmodel::_ComparisonDouble( double f_x, double f_y, double f_epsylon ){
	return fabs( f_x - f_y ) <= f_epsylon;
};

/******************************************************************************/
/*
 *	Contirbution to the solid angle of the circumsolar region
 */
/******************************************************************************/
double CPVmodel::_ChiC(double f_thetaZS, double f_thetaS){
	double fChiC;
	double falfa = (double)DEG_TO_RAD*HALFANGLE;
	double fPsiC;
	double fPsiH;
	if (f_thetaS > (M_PI/2 + falfa)){
		fChiC = 0;
	}
	else if (f_thetaS > (M_PI/2 - falfa)) {
		fPsiC = _PsiC( f_thetaS );
		fPsiH = _PsiH( f_thetaZS );
		fChiC = fPsiC * fPsiH * sin( fPsiC * falfa );
	}
	else {	
		fPsiH = _PsiH( f_thetaZS );
		fChiC = fPsiH * cos( f_thetaS );
	}
	return fChiC;
};

/******************************************************************************/
/*
 *	Contirbution to the solid angle of the circumsolar region
 */
/******************************************************************************/
double CPVmodel::_ChiH(double f_thetaZS){
	double fChiH;
	double falfa = (double)DEG_TO_RAD*HALFANGLE;
	double fPsiH;
	if(f_thetaZS < (M_PI/2 - falfa)) {
		fChiH = cos(f_thetaZS);
	}
	else {
		fPsiH = _PsiH( f_thetaZS );
		fChiH = fPsiH * sin(fPsiH * falfa);
	}
	return fChiH;
};

/******************************************************************************/
/*
 *	Contirbution to the solid angle of the circumsolar region
 */
/******************************************************************************/
double CPVmodel::_PsiC( double f_thetaS ){
	double fPsiC;
	double falfa = (double)DEG_TO_RAD*HALFANGLE;
	fPsiC = (M_PI/2 - f_thetaS + falfa)/(2*falfa);
	return fPsiC;
};

/******************************************************************************/
/*
 *	Contirbution to the solid angle of the circumsolar region
 */
/******************************************************************************/
double CPVmodel::_PsiH( double f_thetaZS ){
	double fPsiH;
	double falfa = (double)DEG_TO_RAD*HALFANGLE;
	if(f_thetaZS > (M_PI/2 - falfa)){
		fPsiH = (M_PI/2 - f_thetaZS + falfa)/(2*falfa);
	}
	else{
		fPsiH = 1;
	}
	return fPsiH;
};

/******************************************************************************/
/*
 *	Incidence Angle
 */
/******************************************************************************/
double CPVmodel::_ThetaS(double f_delta, double f_omega, double f_latitude, orientation sf_orientation){
	double fPhi;
	double fAlfa;
	double fBeta;
	double fThetaS;
	fPhi    = DEG_TO_RAD * f_latitude;
	fAlfa   = sf_orientation.alfa;
	fBeta   = sf_orientation.beta;
	fThetaS = acos(sin(f_delta)*sin(fPhi)*cos(fBeta) - sin(f_delta)*cos(fPhi)*sin(fBeta)*cos(fAlfa) + cos(f_delta)*cos(fPhi)*cos(fBeta)*cos(f_omega) + cos(f_delta)*sin(fPhi)*sin(fBeta)*cos(fAlfa)*cos(f_omega)+cos(f_delta)*sin(fAlfa)*sin(f_omega)*sin(fBeta));
	return fThetaS;
};

/******************************************************************************/
/*
 * Direct tilted Radiation
 */
/******************************************************************************/
double CPVmodel::_TiltedDirectRadiation (double f_Bh0, double f_thetaZS, double f_thetaS){
	double fBab;
	fBab = f_Bh0 * max(0.0,cos(f_thetaS)) / cos(f_thetaZS);
	if(fBab < 0){
		fBab = 0;
	}
	return fBab;
};
/******************************************************************************/
/*
 * Albedo Radiation
 */
/******************************************************************************/
double CPVmodel::_TiltedAlbedoRadiation (double f_Gh0, orientation sf_orientation, double f_reflectivity){
	double fBeta;
	double fALab;
	fBeta=sf_orientation.beta;
	fALab=f_Gh0*(1-cos(fBeta))*(f_reflectivity/2);
	if(fALab < 0){
		fALab = 0;
	}
	return fALab;
};


/******************************************************************************/
/*
 * Diffuse Perez Coefficients
 */
/******************************************************************************/
void CPVmodel::_DiffusePerezCoefficients (double f_Bh0, double f_Dh0, double f_thetaZS, double* f_k){
	double fEpsilon;
	double fDelta;
	
	fEpsilon = 1 + f_Bh0/f_Dh0;
	fDelta = f_Dh0/((double)BO * cos(f_thetaZS));
	
	if((int) HALFANGLE == 25) {
		if(fEpsilon < 1.056) {
			f_k[0] = -0.011 + 0.748 * fDelta - 0.080 * f_thetaZS;
			f_k[1] = -0.048 + 0.073 * fDelta - 0.024 * f_thetaZS;
		}
		else if (fEpsilon < 1.253) {
			f_k[0] = -0.038 + 1.115 * fDelta - 0.109 * f_thetaZS;
			f_k[1] = -0.023 + 0.106 * fDelta - 0.037 * f_thetaZS;
		}
		else if (fEpsilon < 1.586) {
			f_k[0] = 0.166 + 0.909 * fDelta - 0.179 * f_thetaZS;
			f_k[1] = 0.062 - 0.021 * fDelta - 0.050 * f_thetaZS;
		}
		else if (fEpsilon < 2.134) {
			f_k[0] = 0.419 + 0.646 * fDelta - 0.262 * f_thetaZS;
			f_k[1] = 0.140 - 0.167 * fDelta - 0.042 * f_thetaZS;
		}
		else if (fEpsilon < 3.230) {
			f_k[0] = 0.710 + 0.025 * fDelta - 0.290 * f_thetaZS;
			f_k[1] = 0.243 - 0.511 * fDelta - 0.004 * f_thetaZS;
		}
		else if (fEpsilon < 5.980) {
			f_k[0] = 0.857 - 0.370 * fDelta - 0.279 * f_thetaZS;
			f_k[1] = 0.267 - 0.792 * fDelta + 0.076 * f_thetaZS;
		}
		else if (fEpsilon < 10.080) {
			f_k[0] = 0.734 - 0.073 * fDelta - 0.228 * f_thetaZS;
			f_k[1] = 0.231 - 1.180 * fDelta + 0.199 * f_thetaZS;
		}
		else {
			f_k[0] = 0.421 - 0.661 * fDelta + 0.097 * f_thetaZS;
			f_k[1] = 0.119 - 2.125 * fDelta + 0.446 * f_thetaZS;
		}
	}
	else { //Point source circumsolar model	
		if(fEpsilon < 1.056) {
			f_k[0] =  0.041 + 0.621 * fDelta - 0.105 * f_thetaZS;
			f_k[1] = -0.040 + 0.074 * fDelta - 0.031 * f_thetaZS;
		}
		else if (fEpsilon < 1.253) {
			f_k[0] = -0.054 + 0.966 * fDelta - 0.166 * f_thetaZS;
			f_k[1] = -0.016 + 0.114 * fDelta - 0.045 * f_thetaZS;
		}
		else if (fEpsilon < 1.586) {
			f_k[0] = 0.227 + 0.866 * fDelta - 0.250 * f_thetaZS;
			f_k[1] = 0.069 - 0.002 * fDelta - 0.062 * f_thetaZS;
		}
		else if (fEpsilon < 2.134) {
			f_k[0] = 0.486 + 0.670 * fDelta - 0.373 * f_thetaZS;
			f_k[1] = 0.148 - 0.137 * fDelta - 0.056 * f_thetaZS;
		}
		else if (fEpsilon < 3.230) {
			f_k[0] = 0.819 + 0.106 * fDelta - 0.465 * f_thetaZS;
			f_k[1] = 0.268 - 0.497 * fDelta - 0.029 * f_thetaZS;
		}
		else if (fEpsilon < 5.980) {
			f_k[0] = 1.020 - 0.260 * fDelta - 0.514 * f_thetaZS;
			f_k[1] = 0.306 - 0.804 * fDelta + 0.046 * f_thetaZS;
		}
		else if (fEpsilon < 10.080) {
			f_k[0] = 1.009 - 0.708 * fDelta - 0.433 * f_thetaZS;
			f_k[1] = 0.287 - 1.286 * fDelta + 0.166 * f_thetaZS;
		}
		else {
			f_k[0] = 0.936 - 1.121 * fDelta - 0.352 * f_thetaZS;
			f_k[1] = 0.226 - 2.449 * fDelta + 0.383 * f_thetaZS;
		}
	}
	return;
};

/******************************************************************************/
/*
 * Diffuse Circunsolar Radiation
 */
/******************************************************************************/
double CPVmodel::_TiltedDiffuseCSRadiation (double f_Dh0, double f_k3, double f_thetaZS, double f_thetaS){
	double fDCSab;
	double a;
	double c;

	a = 2 * (1 - cos((double) DEG_TO_RAD * HALFANGLE))* _ChiC( f_thetaZS , f_thetaS);
	c = 2 * (1 - cos((double) DEG_TO_RAD * HALFANGLE))* _ChiH( f_thetaZS );

	fDCSab = f_Dh0 * f_k3 * a / c; 

	if( fDCSab < 0 ) {
		fDCSab = 0;
	}
	return fDCSab;
};

/******************************************************************************/
/*
 * Diffuse Isotropic Radiation
 */
/******************************************************************************/
double CPVmodel::_TiltedDiffuseIsoRadiation (double f_Dh0, orientation sf_orientation, double f_k3){
	double fDIsoab;
	fDIsoab = f_Dh0 * (1 + cos(sf_orientation.beta)) * (1 - f_k3) /2;
	if( fDIsoab < 0 ){
		fDIsoab = 0;
	}
	return fDIsoab;
};

/******************************************************************************/
/*
 * Diffuse Horizontal Radiation
 */
/******************************************************************************/
double CPVmodel::_TiltedDiffuseHorRadiation (double f_Dh0, orientation sf_orientation, double f_k4){
	double fDHab;
	fDHab = f_Dh0 * sin(sf_orientation.beta) * f_k4;
	if( fDHab < 0 ) {
		fDHab = 0;
	}
	return fDHab;
};

/******************************************************************************/
/*
 * Incidence Angle and dirtiness Losses Factors
 * double* pf_DIALF[3]
 * 		0: Dirt and Incidence Angle Loss Factor of Direct and Diffuse CircunSolar Radiation 
 * 		1: Dirt and Incidence Angle Loss Factor of Diffuse Isotropic Radiation
 * 		2: Dirt and Incidence Angle Loss Factor of Diffuse Horizontal and Albedo Radiation
 */
/******************************************************************************/
void CPVmodel::_DirtIncAngLosses(double f_thetaS, double f_beta,double f_dirt_factor, double* pf_DIALF){
	//Params of the dirt model
	double far = -1.23*f_dirt_factor + 1.403;
	double fc1 = 4 / (3 * M_PI);
	double fc2 = -0.561 *f_dirt_factor + 0.494;
	//Auxiliar Factor
	double fAuxIsoH;
	double fAuxAL;
	//Direct and Diffuse CircunSolar Radiation Loss Factor
	pf_DIALF[0] = f_dirt_factor *(1 - ((exp(-cos(f_thetaS) / far) - exp(-1 / far)) / (1 - exp(-1 / far))));
	//Diffuse Isotropic Radiation and Diffuse Horizontal Loss Factor
	fAuxIsoH = sin(f_beta) + (M_PI - f_beta - sin(f_beta)) / (1 + cos(f_beta));
	pf_DIALF[1] = f_dirt_factor * (1 - exp(-(fc1 / far) * fAuxIsoH - (fc2 / far) * fAuxIsoH * fAuxIsoH));
	//Albedo Radiation Loss Factor
	fAuxAL = sin(f_beta) + (f_beta - sin(f_beta)) / (1 - cos(f_beta));
	pf_DIALF[2] = f_dirt_factor * (1 - exp(-(fc1 / far) * fAuxAL - (fc2 / far) * fAuxAL * fAuxAL));
	return;
};



















