/*!
 *	\file	IOPrtclParserBase.h
 *
 *	\brief	Э�����ӿڶ����ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��8��	10:42
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOPRTCLPARSERBASE_H_
#define _IOPRTCLPARSERBASE_H_


#include "RTDBCommon/GenericFactory.h"
#include "utilities/fepcommondef.h"
#include "RTDBCommon/OS_Ext.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOPrtclParserBase
	 *
	 *	\brief	Э�鴦�������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class DBBASECLASSES_API CIOPrtclParserBase
	{

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOPrtclParserBase();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIOPrtclParserBase();

		/*!
		 *	\brief	
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pbyData		����ָ��
		 *	\param	nDataLen	���ݳ���
		 *
		 *	\retval	int	0Ϊ�ɹ�����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int BuildRequest(Byte* pbyData, int& nDataLen);

		/*!
		 *	\brief	������֡,������������
		 *
		 *	���ӿ����������ڷ�������ǰ����,ͨ������bTrigTimer�������Ƿ�������ʱ��ʱ��
		 *	�����ʹ��COMX���͵���Ҫ���ر��ӿ�
		 *
		 *	\param	pbyData		: Ҫ���͵�����
		 *	\param	nDataLen	: Ҫ���͵����ݵĳ���
		 *	\param	bTrigTimer	: ���ӿڷ��أ����������Ƿ�������ʱ��ʱ���ı�־��true��������ʱ��ʱ����false������
		 *	\param  nTimeOut	: ��ʱʱ��
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int BuildRequest(Byte* pbyData, int&nDataLen, bool& bTrigTimer, int& nTimeOut);

		/*!
		 *	\brief	�����������,Ϊ�������е����ݴ���
		 *
		 *	���ӿ����������ڽ��յ����ݺ����,ͨ������bStopTimer��bSendData������ֹͣ��ʱ���ͷ������ݵ�����
		 *	�����ʹ��COMX���͵���Ҫ���ر��ӿ�
		 *
		 *	\param	pDataBuf	: ���յ�������
		 *	\param	nLen		: ���յ������ݵĳ���
		 *	\param	bStopTimer	: ���ӿڷ��أ����������Ƿ�رճ�ʱ��ʱ���ı�־��true��رճ�ʱ��ʱ����false�򲻹ر�
		 *	\param	bSendData	: ���ӿڷ��أ����������Ƿ������ݵı�־��true�������ݣ�����BuildRequest��false�򲻷���
		 *
		 *	\retval	int	0Ϊ�ɹ�������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData);

		/*!
		 *	\brief	�����������
		 *
		 *	\param	pDataBuf: ���յ�������
		 *	\param	nLen	: ���յ������ݵĳ���
		 *
		 *	\retval	int	0Ϊ�ɹ�������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen);

		/*!
		 *	\brief	��ʱ����
		 *
		 *	���ӿ�����������ʱ����ʱ�����е���,ͨ������bSendData��bTrigTimer�������Ƿ������ݺ�������ʱ��ʱ��
		 *	�����ʹ��COMX���͵���Ҫ���ر��ӿ�
		 *
		 *	\param	bSendData	: ���ӿڵ��ú�,�������Ƿ�������,trueΪ��������,false������
		 *	\param	bTrigTimer	: ���ӿڵ��ú�,�������Ƿ�������ʱ��ʱ���ı�־��true��������ʱ��ʱ����false������
		 *	\param	nTimeOut	: ��ʱʱ��
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut);

		/*!
		 *	\brief	���յ�ͨ����ϰ�
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	ptPacket: ��ϰ�ָ��
		 *
		 *	\retval	����ֵ	int 0Ϊ�ɹ�����ʧ��
		 */
		virtual int RcvPcketCmd(tIOCtrlPacketCmd* ptPacket);

		/*!
		 *	\brief	���ӳɹ������������ص�����������������Э��
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pPeerIp		���Զ�IP
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int OnConnSuccess(std::string pPeerIP);

		/*!
		 *	\brief	��������
		 */
		std::string getClassName();

		//!���ö�TCP�����������·�����ָ��
		void SetHandle(void* pHandle);

		//!��ȡ��TCP�����������·�����ָ��
		void* GetHandle();

		//!������������ָ��
		void SetParam(void* pParam);

		//!�õ���������ָ��
		void* GetParam();

		/*!
		 *	\brief	ֹͣЭ��������еĶ�ʱ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int StopTimer();

	public:
		//!����
		static std::string s_ClassName;
			
	protected:
		//������ָ�룬һ��Ϊ��������ָ��
		void* m_pParam;

	private:
		//!��TCP�����������·�����ָ��(103G�����õ�)
		void* m_pHandle;
	};

	//!���ȫ�ֱ���
	extern DBBASECLASSES_API GenericFactory<CIOPrtclParserBase> g_DrvPrtclFactory;
}

#endif