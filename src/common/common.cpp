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

#include "common.h"


/******************************************************************************/
/* CONSTRUCTOR */
CWriter::CWriter ( XMLElement* cnf ){
	/* Open data flush */
	m_sFileName = cnf->Attribute("file");
	m_sOutStream.open( m_sFileName.c_str() );
};

/******************************************************************************/
/* DESTRUCTOR */
CWriter::~CWriter( void ){
	m_sOutStream.close();
};

/******************************************************************************/
void CWriter::write_buffer ( void ){
	if ( m_vBuffer.size() > 0 ){
		for ( int i = 0 ; i < m_vBuffer.size() ; i++ ){
			m_sOutStream << m_vBuffer[i] << " ";
		}
		m_sOutStream << endl;	
	}
	m_vBuffer.clear();
	return;
};
