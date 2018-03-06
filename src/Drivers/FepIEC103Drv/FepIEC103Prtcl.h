/*!
*	\file	FepIEC103Prtcl.h
*
*	\brief	103主站驱动协议类头文件
*
*
*	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC103Drv/FepIEC103Prtcl.h 2     11-02-24 11:25a Zhangyuguo $
*	$Author: Zhangyuguo $
*	$Date: 11-02-24 11:25a $
*	$Revision: 2 $
*
*	\attention	Copyright (c) 2002-2010, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef _FEP_IEC103Prtcl_20110110_H_
#define _FEP_IEC103Prtcl_20110110_H_

#include "RTDB/Server/DBDriverFrame/IOPrtclParser.h"
#include "PublicFileH.h"

typedef struct DAILYTIME {
	OpcUa_Int32 lYear;					//! 年
	OpcUa_Int16 sMonth;				//! 月
	OpcUa_Int16 sDay;					//! 日
	OpcUa_Int16 sHour;				//! 时
	OpcUa_Int16 sMinute;				//! 分
	OpcUa_Int16 sSecond;				//! 秒
	OpcUa_UInt16 sMSecond;	//! 毫秒

	//!	时区（为0则是格林威治时间，东时区为负，西时区为正，例如北京时间为-8）
	OpcUa_Int16 sTimeZone;

	//! 构造时赋缺省值
	DAILYTIME()
	{
		lYear = 1970;
		sMonth = 1;
		sDay = 1;
		sHour = 0;
		sMinute = 0;
		sSecond = 0;
		sMSecond = 0;
		sTimeZone = -8;
	};
}DAILYTIME;


namespace MACS_SCADA_SUD
{
	class CFepIEC103Device;
	class CIOTimer;

	class FEPIEC103_API CFepIEC103Prtcl:
		public CIOPrtclParser
	{
		friend void OnTimeOut( void*pParent );			
		friend void CheckTimePeriodProc( void* pParent );
		friend void GIPeriodProc( void* pParent );
	public:
		//构造、析构
		CFepIEC103Prtcl();
		~CFepIEC103Prtcl();

		static std::string s_ClassName;
		std::string GetClassName()
		{
			return s_ClassName;
		}

		virtual OpcUa_Int32 RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen );


		OpcUa_Int32 GetTagAddr( OpcUa_Byte Group, OpcUa_Byte Entry )
		{
			return Group*1000+Entry;
		}

		//检查是否为主站发送的报文
		bool IsMasterMsg( OpcUa_Byte* pCtrl )
		{
			return ((CONTROL_FIELD*)pCtrl)->Prm == 1;
		}

		//根据设备地址获取设备索引号
		bool GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo );

		//激活设备总召唤
		void ActiveWholeCall( CFepIEC103Device* pDevice );

		/*!
		*	\brief	组请求帧并发送
		*	本接口由驱动器在发送数据前调用
		*	缺省实现是判断通道是否被禁止了，如果被禁止则返回-2。
		*
		*	\param	pbyData: 要发送的数据
		*	\param	nDataLen: 要发送的数据的长度
		*
		*	\retval　0为成功，其他失败
		*
		*	\note	
		*/
		virtual OpcUa_Int32 BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32&nDataLen );

		/*!
		*	\brief	组请求帧
		*	本接口由驱动器在发送数据前调用
		*	缺省实现是判断通道是否被禁止了，如果被禁止则返回-2。
		*
		*	\param	pbyData: 要发送的数据
		*	\param	nDataLen: 要发送的数据的长度
		*
		*	\retval　0为成功，其他失败
		*
		*	\note	
		*/
		OpcUa_Int32 BuildRequestEx( OpcUa_Byte* pbyData, OpcUa_Int32&nDataLen );

		//! 给指定设备组通道诊断数据帧
		/*!
		*	\brief	给指定设备组通道诊断数据帧
		*
		*	\param	pDevice:要操作的设备指针
		*	\param	pbyData: 组好的通道诊断数据帧的首指针
		*	\param	nReturnLen:数据帧的总长度
		*
		*	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		*
		*	\note		注意事项（可选）
		*/
		void MakeChannelDiagFrame( CIODeviceBase* pDevice, OpcUa_Byte* pbyData, OpcUa_Int32& nReturnLen );

		/*!
		*	\brief	构造所需的帧
		*	
		*	\param	参数	OpcUa_Byte* pBuf: 返回的数据包
		*	\param	参数	OpcUa_Int32& iReturnLen: 返回数据包的长度	 
		*	\param	参数	CFepIEC103Device* pDevice: 所属设备	 
		*	\param	参数	bool IsLocal: 是否为本地请求	 
		*
		*	\retval	返回值	无
		*
		*	\note		
		*/
		void BuildFrame( OpcUa_Byte* pBuf, OpcUa_Int32& iReturnLen, CFepIEC103Device* pDevice, bool IsLocal = true );

		/*!
		*	\brief	组下发命令帧	
		*
		*	\param	参数	OpcUa_Byte* pFrame: 帧数据区	 
		*
		*	\retval	返回值	OpcUa_Int32 : 帧长度
		*
		*	\note		注意事项（可选）
		*/
		OpcUa_Int32 BuildCtlRequest( OpcUa_Byte* sndBuf ,OpcUa_Int32& iDataLen);
		
		//准备退出
		void Destroy();

		//读取配置文件
		void InitConfigPar( OpcUa_CharA* strAppName );
		
		OpcUa_CharA* MakeLog(OpcUa_Byte* pBuf, OpcUa_Int32 iLen, bool bSend=true);
		OpcUa_Byte Get8SumCode( OpcUa_Byte * Buf, OpcUa_Int32 iByteLen );
		

		CFepIEC103Device* GetNextRequestDev( void );
		OpcUa_Int32 OnConnSuccess(std::string pPeerIP );

								/*!
		*	\brief	把时间转换成毫秒数
		*
		*	\param	strTime	要转换的时间	
		*
		*	\retval	OpcUa_Int64	毫秒数，为-1则表示转换失败
		*/
		OpcUa_Int64 CaculateMSNum( DAILYTIME strTime );

		//检查是否链路离线，即是否所有设备都离线。
		bool IsLinkOffline();

		//检查是否链路不在线，即没有一个设备在线（所有设备都离线或者处于初始态）。
		bool IsLinkNotOnline();

		//检查是否链路可疑，即是否所有设备都可疑或离线。
		bool IsLinkSuspicious();

		/*!
		*	\brief	设备是否需要录波功能
		*/
		bool IsEnableFaultWave()
		{
			return m_bEnableFaultWave;
		}

	protected:
		/*!
		*	\brief	 分析出接收数据包
		*	
		*	\param	参数	OpcUa_Byte* pPacketBuffer: 分析出的数据包
		*	\param	参数	SHORT* pnPacketLength: 数据包长度	
		*
		*	\retval	返回值	bool : 成功true，否则false
		*
		*	\note		注意事项（可选）
		*/
		bool ExtractPacket( OpcUa_Byte* pPacketBuffer, OpcUa_Int32* pnPacketLength);

		//处理提取出来的接收数据帧，填写交换区，必要时发送变位
		void ProcessRcvdPacket(	OpcUa_Byte* pPacketBuffer, OpcUa_Int32 nPacketLength, bool bFromLocal);

		//计算广播校时时间
		OpcUa_Int32 CaculateBroadPeriod( void );
	public:	
		CIOTimer*			m_pTimeOutTimer;								//超时定时器
		CIOTimer*			m_pPeriodTimer;									//周期定时器
		//CIOTimer*			m_pFaultLogTimer;								//故障录波获取时强制记录日志和通信报文的生命周期。
		OpcUa_Int32	m_msFaultLogLifeCycle;							//故障录波日志及报文保存的延续时间，毫秒。0表示不保存日志及报文。
		

		//IOScheduleSendPacket	m_pSendPeerPacket;							//包发送函数指针	
		bool					m_bIsAskForSendPacket;						//!存在另机包发送请求		
	
		
		//bool				m_bSendPacketIsFromPeer;						//区别发送包是本地组的还是另机组的		
		OpcUa_Int32				m_iDevIndex;									//设备逻辑单元号: 0 ~ Channel[0]->nMaxUnit-1	
		OpcUa_Int32				m_iDevLastIndex;

	
		//定义几个配置参数，通过读取配置文件MacsDriver.ini进行初始化
		OpcUa_Int32 m_nRcvTimeOut;							//接收超时
		OpcUa_Int32 m_nCheckTimePeriod;						//对时周期, 秒
		OpcUa_Int32 m_nWholeCallPeriodASDU7;				//ASDU7总召唤周期, 秒. 0表示不进行该类总召唤
		OpcUa_Int32 m_nWholeCallPeriodASDU21;				//ASDU21总召唤周期, 秒. 0表示不进行该类总召唤
		bool m_bEnableFaultWave;					//是否需要故障录波功能
		OpcUa_Int32 m_nMaxOfflineCount;						//离线计数器
		OpcUa_Int32 m_nPeerTimeOut;							//网络包超时
		OpcUa_Int32 m_nDORelayTime;							//遥控选择延迟时间
		OpcUa_Int32 m_nSendDelay;							//发送间延迟，毫秒
		//	OpcUa_Int32 m_iBroadPeriod;						//广播校时周期
		bool m_bBroadCheckTime;						//是否使用广播对时。1-广播；0-点对点对时。默认1。


		bool m_bIsTimeForBroadCheckTime;			//广播对时周期到了。
		bool m_bIsBroadingCheckTime;				//正在进行广播校时

		//超时计数器
		OpcUa_Int32 m_nTimeOutCounter;
		//上次访问的设备地址
		OpcUa_Byte m_byLastDevAddr;	
		OpcUa_Byte m_byLastPeerDevAddr;	//从接收帧解析出的主站发送帧（另机发送）中对应的设备地址

		tIOCtrlRemoteCmd m_tCtrlRemoteCmd;	//! 当前处理的遥控遥调命令
		
		OpcUa_Byte m_byPrtclType;
		OpcUa_Int32 m_iConnTimeOut;
		OpcUa_Int32 m_iMaxReConn;

		OpcUa_UInt32 m_nCounter;					//! 周期触发事件计数器. 
		OpcUa_Int32 m_nMaxLinkOffline;		//链路最大离线秒数：如果在此时间内没有收到任何设备报文，则断定该链路离线。

		OpcUa_Int32	  m_nMaxRetryPeriod;	//用于部分设备离线时，离线设备按大周期发送请求帧的处理

	private:	
		OpcUa_Int32	 m_iCurDevIndex;									//当前正在工作的设备索引号
		bool m_bRTUChannelTest;				//! 当前发送帧是否为通道诊断帧
		
	};


}
#endif	