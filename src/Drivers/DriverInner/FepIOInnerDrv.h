#ifndef _FEPIOINNERDRV_20080426_H_
#define _FEPIOINNERDRV_20080426_H_

#ifdef DRIVERINNER_EXPORTS
#define FEPIOINNERDRV_API MACS_DLL_EXPORT
#else
#define FEPIOINNERDRV_API MACS_DLL_IMPORT
#endif

#include "../../RTDBDriverFrame/IODriverFrameH.h"
#include "RTDBDriverFrame/IODriver.h"
#include "ace/Thread_Manager.h"

namespace MACS_SCADA_SUD
{	

	class FEPIOINNERDRV_API CFepIOInnerDrv : public CIODriver
	{
	public:
		CFepIOInnerDrv(void);
		virtual ~CFepIOInnerDrv(void);

	public:
		static std::string s_ClassName;
		std::string GetClassName();
		virtual int WriteDCB(tIORemoteCtrl& tRemote);

		/*!
		*	\brief	开始工作
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		virtual int StartWork(void);

		/*!
		*	\brief	停止工作
		*
		*	\retval	返回值：0--成功，其它失败
		*/
		virtual int StopWork();

		/*!
		*	\brief	 维护线程
		*
		*	定期校正每个设备的在线离线状态，防止标签品质无效
		*/
		static ACE_THR_FUNC_RETURN maintance_thread ( void* p );

		//! 是否在工作
		bool IsWorking()
		{
			return m_bIsWorking;
		}
	private:
		//! 是否开始工作
		bool m_bIsWorking;
		//! 维护线程ID
		ACE_thread_t m_maintanceThreadId;
		//! ACE_ThreadManager
		ACE_Thread_Manager* m_pACEThreadMgr;
	};
}
#endif
