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
/* DATA TYPES	 							      */
/* It contains common data types and structures				      */
/******************************************************************************/
#ifndef DATATYPES_H
#define DATATYPES_H

/* MACROS */
#ifndef TWO_PI
#define TWO_PI 6.2831853071795864769252867665590057683943L
#endif

/* STRUCTURES */
#include <vector>
#include <complex>
#include <iostream>

using namespace std;

typedef vector<int>      TVInt;

typedef vector<float>    TVFloat;
typedef vector<TVFloat>  TMFloat;
typedef vector<TVFloat*> TMFloat_pt;

struct sFreqCmp {	
	float amp;
	float phs;
	float period;
	complex<double> c_number;
};
typedef vector<sFreqCmp>  TVFreqCmp;
typedef vector<TVFreqCmp> TMFreqCmp;

struct sDefLoad {
	int ac_time;
	int type;
	int window;
};
typedef vector<sDefLoad> TVDefLoad;

struct sNPower {
	float pv;        // PV power 
	float pv_wasted; // PV power that can not be used
	float load;
	float bat;       // + from node to bat
	float grid;      // + from grid to node
	bool  blackout;  // If load has no enough power
};
typedef vector<sNPower> TVNPower;

struct sNEnergy {
	float pv;  
	float pv_wasted;     
	float load;
	float bat_exp;  //from node to bat
	float bat_imp;  //from bat to node
	float grid_exp; //from node to grid
	float grid_imp; //from grid to node
	int   num_bo;	//number of blackouts
};





#endif










