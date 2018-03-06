#include "FepIOInnerDrv.h"
#include "RTDBDriverFrame/IODeviceBase.h"
#include "ace/Thread.h"
#include "ace/Synch.h"


namespace MACS_SCADA_SUD
{	
	extern "C" MACS_DLL_EXPORT int InitDll()
	{
		//初始化类工厂
		GENERIC_REGISTER_CLASS( CIODriverBase, CFepIOInnerDrv, &g_IODriverFactory );
		return 0;
	}

	extern "C" MACS_DLL_EXPORT int UnInitDll()
	{
		GENERIC_UNREGISTER_CLASS( CIODriverBase, CFepIOInnerDrv, &g_IODriverFactory );
		return 0;
	}

	std::string CFepIOInnerDrv::s_ClassName("CFepIOInnerDrv");

	CFepIOInnerDrv::CFepIOInnerDrv(void)
		: m_bIsWorking(false)
		, m_pACEThreadMgr(NULL)
	{

	}

	CFepIOInnerDrv::~CFepIOInnerDrv(void)
	{
		//! 关闭维护线程
		m_bIsWorking = false;
	}

	std::string CFepIOInnerDrv::GetClassName()
	{
		return s_ClassName;
	}

	int CFepIOInnerDrv::WriteDCB(tIORemoteCtrl& tRemote)
	{
		CIODriver::WriteDCB( tRemote );
		if ( !m_IORemoteCtrl.IsEmpty() )
		{
			tIOCtrlRemoteCmd pRemote;
			int nSize = static_cast<int>( sizeof( tIOCtrlRemoteCmd ) );
			m_IORemoteCtrl.DeQueue( (char*)&pRemote, nSize );
			if ( !nSize )
			{
				return -1;
			}
			CIODeviceBase* pDevice = GetDeviceByName( pRemote.pchUnitName );
			if ( !pDevice )
			{
				return -2;
			}

			CIOCfgUnitType* pUnitType = pDevice->GetIOCfgUnit()->GetUnitTypeByID( pRemote.byUnitType );
			if ( !pUnitType )
			{
				return -3;
			}

			pDevice->SetVal( pRemote.byUnitType, pRemote.lTagAddress, pRemote.byData, pRemote.nDataLen, 0, true );

			//! 写日志
			char chLog[255];
			T_mapFieldPoints lstFieldPoints;
			CIOCfgTag* pIOCfgTag = pDevice->GetTagByIndex( pRemote.byUnitType, pRemote.lTagAddress );
			if (pIOCfgTag)
			{
				lstFieldPoints = pIOCfgTag->GetFieldPoints();
				if(lstFieldPoints.size() > 0)
				{
					T_mapFieldPoints::iterator it;
					for (it = lstFieldPoints.begin(); it != lstFieldPoints.end(); it++)
					{
						FieldPointType* pFieldPoint = it->second;
						if(pFieldPoint)
						{
							sprintf(chLog,"Device<%s> Tag<%s> Value Change!", pRemote.pchUnitName, pFieldPoint->getOffset().c_str() );
							WriteLog( chLog );
						}
					}
				}	
			}				
		}
		return 0;
	}

	int CFepIOInnerDrv::StartWork(void)
	{
		CIODriver::StartWork();

		//! 获取设备总数
		int nCount = GetDeviceCount();

		//! 按序号得到设备指针,序号从0开始
		CIODeviceBase* pDevice = NULL;
		for ( int i=0; i<nCount; i++ )
		{
			pDevice = GetDeviceByIndex( i );
			if ( pDevice )
			{
				pDevice->SetOnLine( DEVICE_ONLINE );
			}
		}


		m_bIsWorking = true;

		//! 创建定期维护线程
		m_pACEThreadMgr = ACE_Thread_Manager::instance();
		if (m_pACEThreadMgr)
		{
			m_pACEThreadMgr->spawn( 
				maintance_thread, this, THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED, &m_maintanceThreadId );
		}
		return 0;
	}

	int CFepIOInnerDrv::StopWork()
	{
		int nResult;
		//!停止线程
		if (m_pACEThreadMgr)
		{
			nResult = m_pACEThreadMgr->cancel(m_maintanceThreadId);
			if (nResult != 0)
			{
				 MACS_ERROR((ERROR_PERFIX ACE_TEXT("The IOInnerDrv:%s cancel Thread failed!"), this->GetDriverName().c_str()));
			}
			ACE_OS::sleep(ACE_Time_Value(1));
			//m_pACEThreadMgr->wait();
		}
		nResult = CIODriver::StopWork();
		return nResult;
	}

	ACE_THR_FUNC_RETURN CFepIOInnerDrv::maintance_thread ( void* p )
	{
		CFepIOInnerDrv* pDriver = ( CFepIOInnerDrv * )p;
		while( pDriver->IsWorking() )
		{
			CIODeviceBase* pDevice = NULL;
			for ( int i = 0; i < pDriver->GetDeviceCount(); i++ )
			{
				pDevice = pDriver->GetDeviceByIndex( i );
				if ( pDevice && !pDevice->IsBlock())
				{
					pDevice->SetQualityFirstFlag(true);
					pDevice->SetOnLine( DEVICE_ONLINE );
				}
			}

			ACE_OS::sleep( 1 );	//!< 每个维护周期维护一次
		}

		return 0;
	}

}
