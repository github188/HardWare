//CIOTimer.cpp
#include "RTDBCommon/IOTimer.h"
#include "ace/High_Res_Timer.h"

namespace MACS_SCADA_SUD
{
	CIOTimer::CIOTimer( OnIOTimer OnTimer, void* pParent, bool bOneShot ,ActiveTimer* pTimerMgr)
		: m_pTimerMgr(pTimerMgr), m_lTimerID(0), m_pParent(NULL), m_bOneShot( bOneShot )
	{
		m_lTimerID = -1;
		m_OnTimer = OnTimer;
		m_pParent = pParent;
		if ( NULL == m_pTimerMgr )
		{
			m_bOnlyHandler = false;
			m_pTimerMgr = new ActiveTimer();
			ACE_High_Res_Timer::global_scale_factor (); 
			m_pTimerMgr->timer_queue()->gettimeofday( ACE_High_Res_Timer::gettimeofday_hr );

			m_pTimerMgr->activate(THR_NEW_LWP|THR_DETACHED|THR_INHERIT_SCHED);
		}
		else
			m_bOnlyHandler = true;
	}
	CIOTimer::~CIOTimer(void)
	{
		if(m_bOnlyHandler == false)
		{
			m_pTimerMgr->deactivate();
			delete m_pTimerMgr;
			m_pTimerMgr = NULL;
		}
	}

	int CIOTimer::handle_timeout(const ACE_Time_Value& tv, const void* arg)
	{
		m_OnTimer( m_pParent );
		return 0;
	}

	//! 启动定时器
	int CIOTimer::SetTimer( unsigned int nElapse)
	{
		if( NULL == m_pTimerMgr )
			return -1;

		//const ACE_Time_Value curt_tv = ACE_OS::gettimeofday();
		const ACE_Time_Value curt_tv = ACE_High_Res_Timer::gettimeofday_hr(); //ACE_OS::gettimeofday();
		ACE_Time_Value interval = ACE_Time_Value(nElapse/1000L,nElapse%1000*1000);

		if ( m_lTimerID < 0 )
		{
			if ( m_bOneShot )
			{
				m_lTimerID = m_pTimerMgr->schedule( this, m_pParent, curt_tv + interval );
			}
			else
			{
				m_lTimerID = m_pTimerMgr->schedule( this, m_pParent, curt_tv + interval, interval );
			}
			if ( m_lTimerID!=-1 )
			{
				return 0;
			}
			else
			{
				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOTimer::SetTimer Failed TimerID=%d!"), m_lTimerID ));
			}
		}
		return -1;
	}

	//! 关闭定时器
	int CIOTimer::KillTimer()
	{
		if( NULL == m_pTimerMgr )
			return -1;

		int nRet = -1;
		if ( m_lTimerID >= 0 )
		{
			nRet = m_pTimerMgr->cancel( m_lTimerID );

			//! 如果关闭定时器成功,则将TimerID复位
			if ( nRet >= 0 )
			{
//				MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOTimer::KillTimer Success TimerID=%d, Return=%d!"), m_lTimerID, nRet ));
				m_lTimerID = -1;
			}
			else
			{
				//MACS_DEBUG(( DEBUG_PERFIX ACE_TEXT( "CIOTimer::KillTimer Failed TimerID=%d!"), m_lTimerID ));
			}
		}
		return nRet;
	}

}
