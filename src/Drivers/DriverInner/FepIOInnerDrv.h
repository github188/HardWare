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
		*	\brief	��ʼ����
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		virtual int StartWork(void);

		/*!
		*	\brief	ֹͣ����
		*
		*	\retval	����ֵ��0--�ɹ�������ʧ��
		*/
		virtual int StopWork();

		/*!
		*	\brief	 ά���߳�
		*
		*	����У��ÿ���豸����������״̬����ֹ��ǩƷ����Ч
		*/
		static ACE_THR_FUNC_RETURN maintance_thread ( void* p );

		//! �Ƿ��ڹ���
		bool IsWorking()
		{
			return m_bIsWorking;
		}
	private:
		//! �Ƿ�ʼ����
		bool m_bIsWorking;
		//! ά���߳�ID
		ACE_thread_t m_maintanceThreadId;
		//! ACE_ThreadManager
		ACE_Thread_Manager* m_pACEThreadMgr;
	};
}
#endif
