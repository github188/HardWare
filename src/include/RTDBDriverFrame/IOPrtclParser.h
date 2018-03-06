/*!
 *	\file	IOPrtclParser.h
 *
 *	\brief	Э���෽������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��8��	15:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOPRTCLPARSER_H_
#define _IOPRTCLPARSER_H_

#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "RTDBDriverFrame/IODriverH.h"

namespace MACS_SCADA_SUD
{
	class CIODeviceBase;

	class IODRIVERFRAME_API CIOPrtclParser : public CIOPrtclParserBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOPrtclParser();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIOPrtclParser();

	public:
		
		/*!
		 *	\brief	��ȡ����
		 *
		 *	\param	��
		 *
		 *	\retval	std::string	Э���������
		 */
		
		std::string GetClassName();


		/*!
		 *	\brief	������֡,����������������ǰ����
		 *
		 *	\param	pbyData	: Ҫ���͵�����
		 *	\param	nDataLen: Ҫ���͵����ݵĳ���
		 *
		 *	\retval	����ֵ	0Ϊ�ɹ�,����ʧ��
		 *
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
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData);

		/*!
		 *	\brief	�����������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pDataBuf: ���յ�������
		 *	\param	nLen	: ���յ������ݵĳ���
		 *
		 *	\retval	int	0Ϊ�ɹ�������ʧ��
		 *
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen);

		/*!
		 *	\brief	���յ�ͨ����ϰ�
		 *
		 *	\param	ptPacket	: ��ϰ�ָ��
		 *
		 *	\retval	����ֵ	int 0Ϊ�ɹ�����ʧ��
		 *
		 */
		virtual int RcvPcketCmd(tIOCtrlPacketCmd* ptPacket);

		/*!
		 *	\brief	�ӽ��ջ���������ȡһ֡�������ݰ�
		 *
		 *	��������Ӧ���ر��ӿڣ����ӿ��Ǹ��ݾ���Э����û������ڵķ����ӽ��ջ���������ȡһ����Ч֡
		 *
		 *	\param	pPacketBuffer	��	��ȡ��֡���ݵı�����
		 *	\param	pnPacketLength	��	Ϊ����������ݵĳ���
		 *
		 *	\retval	bool	trueΪ�ɹ�,falseΪʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual bool ExtractPacket(Byte* pPacketBuffer, int* pnPacketLength);

		/*!
		 *	\brief	������ȡ�������������ݰ�
		 *
		 *	\param	pPacketBuffer	����Ч���ݵ���ָ��
		 *	\param	nPacketLength	����Ч���ݵĳ���
		 *	\param	bFromLocal		���Ƿ�ӱ�����ȡ�ģ������Ƿ��ͣӣϣţ�
		 *	\param	bStopTimer		���Ƿ�ֹͣ��ʱ��ʱ����trueΪֹͣ;falseΪ��ֹͣ
		 *	\param	bSendData		���Ƿ������ݣ�trueΪ����;falseΪ������
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		���ӿ���ʹ��CIOComxBoard���͵�������ʱҪ���õĺ���,�����������ر��ӿڡ�
		 *			
		 */
		virtual void ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal, bool& bStopTimer, bool& bSendData);

		/*!
		 *	\brief	���ý��մ����е� �Ƿ�ֹͣ��ʱ����־ �� �Ƿ������ݵı�־
		 *
		 *	��������RxDataProc�е���
		 *	ȱʡʵ���ǣ�����豸Ϊģ��̬�ʹӻ����ñ�־Ϊfalse.
		 *
		 *	\param	pDevice		:�豸ָ��
		 *	\param	bStopTimer	:�Ƿ�ֹͣ��ʱ����־
		 *	\param	bSendData	:�Ƿ������ݵı�־
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note	���ӿ���ʹ��CIOComxBoard���͵�������ʱҪ���õĺ�����
		 *			���ȱʡʵ�ֲ�������Ҫ����������ء�
		 */
		virtual int SetRcvProcFlag( CIODeviceBase* pDevice, bool& bStopTimer, bool& bSendData );

		/*!
		 *	\brief	��ָ���豸��ͨ���������֡
		 *
		 *	\param	pDevice		:Ҫ�������豸ָ��
		 *	\param	pbyData		:��õ�ͨ���������֡����ָ��
		 *	\param	nReturnLen	:����֡���ܳ���
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 */
		virtual void MakeChannelDiagFrame(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen);

		/*!
		 *	\brief	������ȡ�������������ݰ�
		 *
		 *	\param	pPacketBuffer	����Ч���ݵ���ָ��
		 *	\param	nPacketLength	����Ч���ݵĳ���
		 *	\param	bFromLocal		���Ƿ�ӱ�����ȡ�ģ������Ƿ��ͣӣϣţ�
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 */
		virtual void ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal);

		/*!
		 *	\brief	�õ�֡���ݻ�����ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	Byte* ������ָ��
		 *
		 */
		Byte* GetPacketBuffer();

		/*!
		 *	\brief	�õ���С���ĳ���
		 *
		 *	\retval	int	��С���ĳ���
		 */
		virtual int GetMinPacketLength();

		/*!
		 *	\brief	ֹͣЭ��������еĶ�ʱ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int StopTimer();

	protected:
		/*!
		 *	\brief	����16λsumУ��
		 *
		 *	\param	Buf		: Ҫ�����������
		 *	\param	iWordLen: ������ֽ���
		 *
		 *	\retval	�������У���
		 */
		WORD Get16SumCode(Byte * Buf, int iWordLen);

		/*!
		 *	\brief	����16λWORD�ĸ��ֽ�ֵ
		 *
		 *	\param	wdCheck: Ҫ�������
		 *
		 *	\retval	���ֽ�ֵ
		 */
		Byte GetWordHigh(WORD wdCheck);

		/*!
		 *	\brief	����16λWORD�ĵ��ֽ�ֵ
		 *
		 *	\param	wdCheck: Ҫ�������
		 *
		 *	\retval	���ֽ�ֵ
		 */
		Byte GetWordLow(WORD wdCheck);

		/*!
		 *	\brief	������·���ʳ�ͻ,������͸����
		 *
		 *	���ӿ��ǵ���������·���ʳ�ͻʱ����,���ָ���豸������·������ݰ��������͸����
		 *
		 *	�߼����ж�˫��ͨ����������·���ʳ�ͻ��ָ���豸Ϊ��������״̬��Ϲ�����
		 *		����״̬��ϱ�־������ϰ������͸������
		 *
		 *	\param	pDevice		:Ҫ������豸��ָ��
		 *	\param	pbyData		:ͨ����ϰ�����ָ��
		 *	\param	nReturnLen	:ͨ����ϰ��ĳ���
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int DealLinkClash(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen);

		/*!
		 *	\brief	�����յ�����Ӧ���ݷ��͵����
		 *
		 *	�������������ķ��Ͱ�,�����ͺ�,���յ���Ӧ,����Ӧ���͸����
		 *
		 *	\param	bFromLocal	: �Ƿ��Ǳ����յ�����Ӧ
		 *	\param	pbyData		: �յ�������
		 *	\param	nDataLen	: �յ������ݳ���
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int DealRcvLink( bool bFromLocal, Byte* pbyData, int nDataLen ); 

		/*!
		 *	\brief	ת��longlong����Ϊʱ��
		 *
		 *	\param	const long:�����ͱ�����ʱ������			
		 *
		 *	\retval��std::string ʱ��,��ʽΪ ��/��/�� ʱ:��:��
		 */	
		std::string llNum2Time( const long llNum2Time );

		/*!
		 *	\brief	�Ѵ��������ȷŵ�m_pbyInnerBuf�У�����Ӵ��ڶ�һ��������ȡ��
		 *
		 *	Ϊ���ݻ������ڴ�������õĺ���
		 *
		 *	\param	pBuf:Ҫ�ӵ�����
		 *	\param	iLen:Ҫ�ӵ����ݵĳ���
		 *
		 *	\retval	void
		 */
		void InnerAddData( Byte* pBuf, int iLen );

		/*!
		 *	\brief	��ȡ�ڲ��������е�����,Ϊ�������ڷ���
		 *
		 *	\param	pBuf	:����������
		 *	\param	iBufLen	:ϣ�����ĳ���
		 *	\param	iReadLen:ʵ�ʶ������ݵĳ���
		 *
		 *	\retval	bool		�ɹ�����true������false
		 */
		bool InnerRead( Byte* pBuf, int iBufLen, int& iReadLen );


		
		virtual int TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut);
		
		virtual int OnConnSuccess(std::string pPeerIP);

	public:
		//!����
		static std::string s_ClassName;
		
		//!��Ч������С����
		int PRTCL_MIN_PACKET_LENGTH;

		//!��Ч������󳤶�
		int PRTCL_MAX_PACKET_LENGTH;

		//!�������ݻ���������󳤶�
		int PRTCL_RCV_BUFFER_LENGTH;

		//!�������ݻ���������󳤶�
		int PRTCL_SND_BUFFER_LENGTH;

		//!������������������󳤶�
		int PRTCL_MAX_INNER_LENGTH;

		//!��ʱʱ��
		int MACSDRIVER_MAX_TIMEOUT;

		//!���ͻ�����,����Ϊm_nSndBufLen
		Byte* m_pbySndBuffer;

	protected:
		//!Э������������,�ڲ���������ָ��
		Byte* m_pbyInnerBuf;

		//!Э������������,�ڲ����ݳ���
		int	 m_iInnerLen;

		//!���ջ�����,����Ϊm_nRcvBufLen
		Byte* m_pbyRevBuffer;
		
		//!֡���ݻ�����,����Ϊm_nMaxPacketLen
		Byte* m_sPacketBuffer;

		//!��ǰ���ջ���������Ч���ݵĳ���
		int m_nRevLength;

		//!�����Ͱ��Ǳ�����Ļ���������,trueΪ������͹�����
		bool m_bSendPacketIsFromPeer;

		//!�Է�����������ϰ�,�����ػ���,��BuildRequest�з���
		tIOCtrlPacketCmd m_cmdPacket;
	};

	//!�����ڴ�ռ�
#define InitPrtclHelper( nMinPacketLen, nMaxPacketLen, nRcvBufLen, nSndBufLen, nInnerBufLen ) {\
	PRTCL_MIN_PACKET_LENGTH = nMinPacketLen;\
	PRTCL_MAX_PACKET_LENGTH = nMaxPacketLen;\
	PRTCL_RCV_BUFFER_LENGTH = nRcvBufLen;\
	PRTCL_SND_BUFFER_LENGTH = nSndBufLen;\
	PRTCL_MAX_INNER_LENGTH  = nInnerBufLen;\
	m_pbyRevBuffer = new Byte[(unsigned int)PRTCL_RCV_BUFFER_LENGTH];\
	m_pbySndBuffer = new Byte[(unsigned int)PRTCL_SND_BUFFER_LENGTH];\
	m_sPacketBuffer = new Byte[(unsigned int)PRTCL_MAX_PACKET_LENGTH];\
	m_pbyInnerBuf = new Byte[(unsigned int)PRTCL_MAX_INNER_LENGTH]; }

}

#endif