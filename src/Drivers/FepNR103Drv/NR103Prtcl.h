/*!
*	\file	NR103Driver.h
*
*	\brief	NR103协议解析类
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

		//!组出请求帧并发送
		virtual OpcUa_Int32 BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!处理接收数据
		virtual OpcUa_Int32 RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen );
		
		//!从缓冲区中提取一帧完整报文
		virtual bool ExtractPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength );
		
		//!解析一包完整的数据帧
		virtual void ProcessRcvdPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal );

		//!与设备连接成功时处理
		virtual OpcUa_Int32 OnConnSuccess( std::string pPeerIP );

		std::string GetClassName();

		//!初始化驱动参数
		void InitConfig(std::string strDrvName);

		//!组请求帧
		OpcUa_Int32 BuildRequestFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!获取网络超时时间
		OpcUa_UInt32 GetConnTimeOut();

		//!获取最大重连次数
		OpcUa_UInt32 GetMaxReConn();

		//!发送遥控请求帧
		OpcUa_Int32 BuildCtrlRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!组设备请求帧
		OpcUa_Int32 BuildFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen, CNR103Device* pDevice );

		//!输出报文
		std::string GetCommData( OpcUa_Byte* pFrame, OpcUa_Int32 nPacketLen, OpcUa_Int32 byRcvOrSnd );
		
		/*!
		 *	\brief	解析ASDU帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDUFrame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );

		//获取上次请求设备索引
		OpcUa_Int32 GetLastDevIndex();

		//获取接收超时时间
		OpcUa_Int32 GetRcvTimeOut();

		//得到设备最大离线次数
		OpcUa_Int32 GetMaxOffLineCount();

	private:
		void WriteLog(UaString ustrMsg, OpcUa_Boolean isWriteAnyWay = OpcUa_False);
		
		/*!
		 *	\brief	解析ASDU5帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU5Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	解析ASDU5帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU6Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	解析ASDU10帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtracktASDU10Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	解析ASDU23帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU23Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );

		/*!
		 *	\brief	解析ASDU28帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU28Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	解析ASDU29帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU29Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	解析ASDU31帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU31Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );
		
		/*!
		 *	\brief	解析NR103 ASDU20远方复归反信帧
		 *
		 *	\param	pFrame	: 要发送的数据
		 *	\param	nlen	: 待分析数据区长度
		 *	\param(out)	nASDULen: ASDU帧长度
		 *
		 *	\retval	返回值	F_CONTINUE继续下次解析、F_BREAK退出解析、F_SUCCESS解析成功
		 */
		OpcUa_Int32 ExtractASDU1Frame( OpcUa_Byte* pFrame, OpcUa_Int32 nlen, OpcUa_Int32& nASDULen );

	public:
		static std::string s_ClassName;
		CIOTimer* m_pRcvOutTimer;			//!接收超时定时器
		OpcUa_UInt32 m_nPeriodCount;		//!周期定时器计数器
		OpcUa_UInt32 m_iWQInternal;			//!发送总召唤报文间隔
		OpcUa_UInt32 m_iCheckTPeriod;		//!周期校时间隔
		OpcUa_UInt32 m_iHBInternal;			//!发送心跳报文间隔
		OpcUa_UInt32 m_nOffDevCount;		//!离线设备个数

		OpcUa_UInt32 m_nErrPacketNum;		//!错误包数量

	private:
		CIOTimer* m_pPeriodTimer;			//!周期轮询定时器
		OpcUa_UInt32 m_iConnTimeOut;		//!网络连接超时
		OpcUa_UInt32 m_iMaxReConn;			//!最大重连次数
		OpcUa_UInt32 m_iRcvTimeOut;			//!设备响应超时
		OpcUa_UInt32 m_iMaxOffCount;		//!设备最大离线次数
		OpcUa_UInt32 m_nLogicalAddr;		//!当前轮询设备地址
		OpcUa_UInt32 m_nLastAddr;			//!上一次发送请求设备地址
		OpcUa_Int32 m_nSendDelay;			//!发送延时
		tIOCtrlRemoteCmd m_tRemoteCtrlCmd;	//!需要下发遥控命令

		UaMutex m_mutexDataSync;			//!互斥锁对象
		
	};
}

#endif