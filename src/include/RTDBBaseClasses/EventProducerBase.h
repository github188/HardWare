/*!
 *	\file	EventProducerBase.h
 *
 *	\brief	�¼���������
 *
 *
 *	\author wangzan
 *
 *	\date	2014.04.01
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADAϵͳ������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _EVENT_PRODUCER_BASE_H_
#define _EVENT_PRODUCER_BASE_H_

#include "RTDBCommon/KiwiVariant.h"

#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif


namespace MACS_SCADA_SUD
{
	typedef struct EventParam
	{
		void*			pNode;							//! �¼�Դ�ڵ�
		void* pConditionNode;	//! �����Ӷ���ڵ�,��ͨ�¼���NULL
		unsigned int	uiType;							//! �¼����
		unsigned int	uiSeverity;						//! �¼��ȼ�
		long	    dtTime;							//! ʱ�䷢��ʱ��
		std::string strMessage;						//! �¼�����
		unsigned int	uiChgCode;						//! ����״̬��
		std::string		opsName;						//! ����Ա��
		std::string		opsComment;						//! ����Ա����
		std::string		opsCmdPosition;					//! ��λ

		//! �����ֶ����¼�Դ�ڵ㲻���ڵ������ʹ��
		std::string	 strPointName;	//! ����
		std::string	 strDevName;	//! �豸��
		std::string	 strArea1;		//! ����1
		std::string	 strArea2;		//! ����2
		std::string	 strArea3;		//! ����3
		std::string     strDomain;     //! ������


		EventParam()
		{
			uiType = 0;
			uiSeverity = 0;
			pNode = NULL;
			pConditionNode = NULL;
			uiChgCode = 0;
		}
	} ;

	/*!
	 *	\class	CEventProducerBase
	 *
	 *	\brief	�¼�������
	 *
	 *	��ʵʱ�����
	 */
	class DBBASECLASSES_API CEventProducerBase /* parasoft-suppress  CODSTA-CPP-19 "����ӿڣ�����Ҫ���캯��" */ /* parasoft-suppress  OOP-23 "����ӿڶ��壬û���κγ�Ա����˲���Ҫ����/��������" */
	{
	public:
		/*!
		 *	\brief	�¼������ӿ�,ʵʱ���������
		 *
		 *	\param	[in]pEventParam���¼�����
		 *
		 *	\retval	0:�ɹ�;��0:ʧ��
		 *
		 *	\note	ע�������ѡ��
		 */
		virtual unsigned int FireEvent(EventParam* pEventParam) = 0;
	};

	extern DBBASECLASSES_API CEventProducerBase* g_pEventProducer;
}

#endif
