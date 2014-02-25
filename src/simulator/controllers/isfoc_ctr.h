/******************************************************************************/
/* LIBRARY CONTROLLERS							      */
/* It is the class of the controllers	                   		      */
/******************************************************************************/
#ifndef ISFOC_CTR_H_
#define ISFOC_CTR_H_

/* LIBRARIES*/
#include "controller.h"
#include "bat_ctr.h"

/******************************************************************************/
class CIsfocCtr : public CController {

	public:
	/* FUNCTIONS */
	CIsfocCtr   ( sSimCnf*  sSimCnf , XMLElement* cnf , CNode* pcNode  );
	~CIsfocCtr  ( void );		
	
	void executionStep ( void );
	void restart       ( void );		

	void setTmpStep    ( float input ){ m_fTmp_step = input; };
	void setSoC_dst    ( float input );

	int  getCycles     ( void ){return m_pcBatCrt->getCycles();};

	private:
	/* VARIABLES */	

	float m_fTmp_step;
	float m_fSoC_min;
	float m_fSoC_max;
	
	float m_fTmp_min;
	float m_fTmp_max;

	float m_fPV_next_hour;

	int   m_nFmode;
	int   m_nEmode;

	CBatCtr* m_pcBatCrt;
	/* FUNCTIONS */	
};

#endif

