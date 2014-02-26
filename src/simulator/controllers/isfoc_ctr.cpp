/******************************************************************************/
/* Methods of controller.h 						      */
/******************************************************************************/
#include "isfoc_ctr.h"

/******************************************************************************/
/* CONSTRUCTOR */
CIsfocCtr::CIsfocCtr ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode ) : CController::CController ( sSimCnf , pcNode ) {	
	/* Configure controller */
	m_fTmp_step = atof( cnf->Attribute("tmp_step") );
	m_fSoC_min  = atof( cnf->Attribute("soc_min")  );
	m_fSoC_max  = atof( cnf->Attribute("soc_max")  );
	m_fTmp_min  = atof( cnf->Attribute("t_min" )   );
	m_fTmp_max  = atof( cnf->Attribute("t_max" )   );
	m_nFmode    = atoi( cnf->Attribute("f_mode" )  );
	if ( cnf->Attribute("e_mode" ) )
		m_nEmode    = atoi( cnf->Attribute("e_mode" )  );
	else
		m_nEmode    = 0;
	/* Create Bat Controller */
	m_pcBatCrt = new CBatCtr ( m_sSimCnf , cnf , pcNode );	
	m_fPV_next_hour = 0.0;
};	

/******************************************************************************/
/* DESTRUCTOR */
CIsfocCtr::~CIsfocCtr ( void ){
	
};

/****************************************************************/
/* RESTART 							*/
/****************************************************************/
void CIsfocCtr::restart ( void ){
	m_pcBatCrt->restart();
	m_fPV_next_hour = 0.0;
	return;
};

/******************************************************************************/
void CIsfocCtr::setSoC_dst ( float input ){ 
	m_fSoC_max  = input*( 100.0 - m_fSoC_min ) + m_fSoC_min; 
	m_pcBatCrt->setSoC_limits( m_fSoC_min , m_fSoC_max );
	return;
};

/******************************************************************************/
/* Execution Step */
void CIsfocCtr::executionStep( void ){
	/* Battery controller execution */
	m_pcBatCrt->executionStep();
	/* Get sensors and actuators */
	CStorage* pcStorage;	
	pcStorage = m_pcNode->getStorage();
	CAirConditioner* pcAir;
	pcAir = m_pcNode->getLoad()->getAirConditioner();	
	float out_tmp = pcAir->getOutdoorTemp( );
	float in_tmp  = pcAir->getIndoorTemp( );
	float ref_tmp = pcAir->getReferenceTemp( );
	float air_np  = pcAir->getNPower();	
	if ( m_sSimCnf->nSimStep % 60 == 0 ){
		float pv_np     = m_pcNode->getPV()->getNPower();
		m_fPV_next_hour = pv_np * m_pcNode->getPV()->getNextHourFrc();			
	}
	/* Temperature variation */
	// Energy availability	
	float SoC_eval = ( pcStorage->getSoC() - m_fSoC_min )/m_fSoC_max; // [0:1]
	float Frc_eval;	
	if ( m_nEmode == 0 ){
		Frc_eval = 0.0;
	}
	else if ( m_nEmode == 1 ) {
		Frc_eval = m_fPV_next_hour / air_np; //[0:1]
		if ( Frc_eval > 1.0 )
			Frc_eval = 1.0;
	}
	// Maximum consumption temeprature
	float obj_tmp;
	if ( out_tmp < ( m_fTmp_max + m_fTmp_min )/2 )
		obj_tmp = m_fTmp_max;
	else
		obj_tmp = m_fTmp_min;
	// Reference temperature variation
	float tmp_var = 0.0;
	// To the maximum distance (maximum consumption)
	if ( ref_tmp < obj_tmp )
		tmp_var += ( SoC_eval + Frc_eval ) * m_fTmp_step ;
	else
		tmp_var -= ( SoC_eval + Frc_eval ) * m_fTmp_step ;
	// To the outdoor temperature (minimum consumption)
	if ( ref_tmp < out_tmp )
		tmp_var += (1.0 - SoC_eval ) * m_fTmp_step ;
	else
		tmp_var -= (1.0 - SoC_eval ) * m_fTmp_step ;
	float new_ref = ref_tmp + tmp_var;
	if ( new_ref > m_fTmp_max )
		new_ref = m_fTmp_max;
	else if ( new_ref < m_fTmp_min )
		new_ref = m_fTmp_min;
	pcAir->setReferenceTemp( new_ref );
	/* Fan intensity */
	if ( m_nFmode == 1 ){
		if ( SoC_eval < 0.33 )
			pcAir->setFanIntensity(0.33);
		else if ( SoC_eval < 0.66 )
			pcAir->setFanIntensity(0.66);
		else
			pcAir->setFanIntensity(1.0);
	}	
	else{
		if ( fabs( in_tmp - ref_tmp ) < 0.5 )
			pcAir->setFanIntensity(0.33);
		else if ( fabs( in_tmp - ref_tmp ) < 1.0 )
			pcAir->setFanIntensity(0.66);
		else
			pcAir->setFanIntensity(1.0);
	}
	return;
};
