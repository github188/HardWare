/*!
*	\file	NR103Driver.h
*
*	\brief	NR103Э�������
*
*	$Date: 15-03-30$
*
*/
#ifndef _NR103PRCL_H_
#define _NR103PRCL_H_

#include "NR103CommDef.h"
#include "RTDB/Server/DBDriverFrame/IOPrtclParser.h"
#include "uamutex.h"

namespace MACS_SCADA_SUD
{
	class CNR103Device;

	class FEPNR103Drv_API CNR103Prtcl : public CIOPrtclParser
	{
	public:
		CNR103Prtcl();
		
		~CNR103Prtcl();

		//!�������֡������
		virtual OpcUa_Int32 BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!�����������
		virtual OpcUa_Int32 RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen );
		
		//!�ӻ���������ȡһ֡��������
		virtual bool ExtractPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength );
		
		//!����һ������������֡
		virtual void ProcessRcvdPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal );

		//!���豸���ӳɹ�ʱ����
		virtual OpcUa_Int32 OnConnSuccess( std::string pPeerIP );

		std::string GetClassName();

		//!��ʼ����������
		void InitConfig(std::string strDrvName);

		//!������֡
		OpcUa_Int32 BuildRequestFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!��ȡ���糬ʱʱ��
		OpcUa_UInt32 GetConnTimeOut();

		//!��ȡ�����������
		OpcUa_UInt32 GetMaxReConn();

		//!����ң������֡
		OpcUa_Int32 BuildCtrlRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!���豸����֡
		OpcUa_Int32 BuildFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen, CNR103Device* pDevice );

		//!�������
		std::string GetCommData( OpcUa_Byte* pFrame, OpcUa_Int32 nPacketLen, OpcUa_Int32 byRcvOrSnd );
		
		/*!
		 *	\brief	����ASDU֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDUFrame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );

		//��ȡ�ϴ������豸����
		OpcUa_Int32 GetLastDevIndex();

		//��ȡ���ճ�ʱʱ��
		OpcUa_Int32 GetRcvTimeOut();

		//�õ��豸������ߴ���
		OpcUa_Int32 GetMaxOffLineCount();

	private:
		void WriteLog(UaString ustrMsg, OpcUa_Boolean isWriteAnyWay = OpcUa_False);
		
		/*!
		 *	\brief	����ASDU5֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU5Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	����ASDU5֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU6Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	����ASDU10֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtracktASDU10Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	����ASDU23֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU23Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );

		/*!
		 *	\brief	����ASDU28֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU28Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	����ASDU29֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU29Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	����ASDU31֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU31Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	����NR103 ASDU20Զ�����鷴��֡
		 *
		 *	\param	pFrame	: Ҫ���͵�����
		 *	\param	nlen	: ����������������
		 *	\param(out)	nASDULen: ASDU֡����
		 *
		 *	\retval	����ֵ	F_CONTINUE�����´ν�����F_BREAK�˳�������F_SUCCESS�����ɹ�
		 */
		OpcUa_Int32 ExtractASDU1Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );

	public:
		static std::string s_ClassName;
		CIOTimer* m_pRcvOutTimer;			//!���ճ�ʱ��ʱ��
		OpcUa_UInt32 m_nPeriodCount;		//!���ڶ�ʱ��������
		OpcUa_UInt32 m_iWQInternal;			//!�������ٻ����ļ��
		OpcUa_UInt32 m_iCheckTPeriod;		//!����Уʱ���
		OpcUa_UInt32 m_iHBInternal;			//!�����������ļ��
		OpcUa_UInt32 m_nOffDevCount;		//!�����豸����

		OpcUa_UInt32 m_nErrPacketNum;		//!���������

	private:
		CIOTimer* m_pPeriodTimer;			//!������ѯ��ʱ��
		OpcUa_UInt32 m_iConnTimeOut;		//!�������ӳ�ʱ
		OpcUa_UInt32 m_iMaxReConn;			//!�����������
		OpcUa_UInt32 m_iRcvTimeOut;			//!�豸��Ӧ��ʱ
		OpcUa_UInt32 m_iMaxOffCount;		//!�豸������ߴ���
		OpcUa_UInt32 m_nLogicalAddr;		//!��ǰ��ѯ�豸��ַ
		OpcUa_UInt32 m_nLastAddr;			//!��һ�η��������豸��ַ
		OpcUa_Int32 m_nSendDelay;			//!������ʱ
		tIOCtrlRemoteCmd m_tRemoteCtrlCmd;	//!��Ҫ�·�ң������

		UaMutex m_mutexDataSync;			//!����������
		
	};
}

#endif