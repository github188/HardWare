/*!
 *	\file	FepModbusPrtcl.h
 *
 *	\brief	通用Modbus驱动的协议类头文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusPrtcl.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_PRTCL_20080513_H_
#define _FEP_MODBUS_PRTCL_20080513_H_

#include "ace/Timer_Heap.h"
#include "ace/Timer_Queue_Adapters.h"
#include "RTDBDriverFrame/IOPrtclParser.h"
#include "FepModbusDrvH.h"
#include "FepModbusDriver.h"

namespace MACS_SCADA_SUD
{	
	class CFepModbusDevice;
	/*!
	*	\class	CFepModbusPrtcl
	*
	*	\brief	TEP-I-F直流屏微机监控装置驱动的协议类
	*/
	class FEPMODBUSDRV_API CFepModbusPrtcl:
		public CIOPrtclParser
	{
	public:
		/*!
		*	\brief	缺省构造函数
		*/
		CFepModbusPrtcl(void);
		/*!
		*	\brief	缺省析构函数
		*/
		~CFepModbusPrtcl(void);

	public:
		/*!
		*	\brief	初始化工作
		*/
		virtual void Init( Byte byPrtclType, int nMaxReSend );

		virtual int RxDataProc( Byte* pDataBuf, int& nLen );

		/*!
		*	\brief	组请求帧
		*
		*	\param	pbyData		帧首地址
		*	\param	nDataLen	帧长
		*
		*	\retval	int	0表示成功，否则表示失败
		*/
		virtual int BuildRequest( Byte* pbyData, int& nDataLen );

		/*!
		*	\brief	备份并发送请求
		*
		*	\param	pDevice		发送帧的设备
		*	\param	pbyData		数据包首地址
		*	\param	nDataLen	数据包长度
		*
		*	\retval	bool	成功true，否则false
		*/
		int BackupAndSendRequest( 
			CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen );

		/*!
		*	\brief	得到最小的包长度
		*
		*	\retval	int	最小包长
		*/
		virtual int GetMinPacketLength();

		//! 连接成功
		virtual int OnConnSuccess(std::string pPeerIP );

		/*!
		*	\brief	得到最近发送帧的设备地址
		*
		*	\retval	Byte 最近发送帧的设备地址
		*/
		inline Byte GetLastAddr()
		{
			return m_bySendAddr;
		};

		//! 当前发送帧是否为通道诊断帧
		inline bool IsRTUChannelTest()
		{
			return m_bRTUChannelTest;
		};

		//! 设置通道诊断帧标志
		inline void SetRTUChannelTest(bool bRTUChannelTest)
		{
			m_bRTUChannelTest = bRTUChannelTest;
		};

		//! 设置遥控伴随帧长度
		inline void SetCtrlInsertLen(int nCtrlInsertLen)
		{
			m_nCtrlInsertLen = nCtrlInsertLen;
		};

		//! 获得将要发送的遥控伴随帧长度
		inline int GetCtrlInsertTempLen()
		{
			return m_nCtrlInsertTempLen;
		};

		//! 设置将要发送的遥控伴随帧长度
		inline void SetCtrlInsertTempLen( int nCtrlInsertTempLen )
		{
			m_nCtrlInsertTempLen = nCtrlInsertTempLen;
		};

		//! 获得协议是否为MODBUS TCP
		inline bool IsModbusTCP()
		{
			return ( m_byPrtclType == 1 );
		}

		//! 获得协议是否为MODBUS TCP
		inline bool IsNetConn()
		{
			return ( m_byPrtclType == 1 || m_byPrtclType == 2 );
		}

		//! 设置命令类型标志
		inline void SetCmdType( Byte byCmdType)
		{
			m_bycmdType = byCmdType;
		};
		ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>* GetTimerMgr(){return m_pTimerMgr;};
		//! 根据报文数据获得输出字符串（byRcvOrSnd：1为Rcv，0为Snd）
		std::string GetCommData( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd );

		//! 获得类名
		std::string GetClassName();

		//! 声明友元函数以使这些函数能访问本类的私有或保护型数据成员
		friend void OnTimeOut(void* p);
		friend void PeriodProc(void* p);
		friend void InsertDelayProc(void* p);

	protected:
		/*!
		*	\brief	生成事务单元标识
		*
		*	\param	High	标识高位
		*	\param	Low		标识低位
		*
		*	\retval	int	Modbus TCP请求大小
		*/
		void GetPacketNo( Byte& High, Byte& Low );

		/*!
		*	\brief	把Modbus RTU请求转化成Modbus TCP请求
		*
		*	\param	pRTU		RTU请求
		*	\param	iRTULen		RTU请求大小
		*	\param	pTCP		TCP请求
		*
		*	\retval	int	Modbus TCP请求大小
		*/
		int MBRTU2MBTCPRequest( Byte* pRTU, int iRTULen, Byte* pTCP ); 

		/*!
		*	\brief	分析出接收数据包
		*
		*	从接收缓冲区中提取一个帧，提取的帧数据保存在pPacketBuffer中
		*
		*	\param	pPacketBuffer	分析出的数据包帧头
		*	\param	pnPacketLength	被处理掉数据的长度
		*
		*	\retval	bool	成功true，否则false
		*/
		bool ExtractPacket( Byte* pPacketBuffer, int* pnPacketLength);

		/*!
		*	\brief	分析轮询帧返回包
		*
		*	检查以pFrame开始的数据缓存区中是否有完整的轮询帧返回包
		*
		*	\param	pFrame		帧头指针
		*	\param	nCorrectLen	如有解析出正确的遥控返信包，
		*						则置该参数为正确帧的长度
		*
		*	\retval	int			含义如下：
		*		FLAG_NONE				0		不是轮询帧返回包
		*		FLAG_UNCOMPLETE			1		是轮询帧返回包，但没接收完整
		*		FLAG_CORRECTFRAME		2		是正确的轮询帧返回包
		*		FLAG_ERROR				3		解析出错，说明给的帧头位置不对
		*/
		int ExtractMsgPacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	分析遥控返信包
		*
		*	检查以pFrame开始的数据缓存区中是否有完整的遥控返信包
		*
		*	\param	pFrame		帧头指针
		*	\param	nCorrectLen	如有解析出正确的遥控返信包，
		*						则置该参数为正确帧的长度
		*
		*	\retval	int			含义如下：
		*		FLAG_NONE				0		不是遥控返信帧
		*		FLAG_UNCOMPLETE			1		是遥控返信帧，但没接收完整
		*		FLAG_CORRECTFRAME		2		是正确的遥控返信帧
		*		FLAG_ERROR				3		解析出错，说明给的帧头位置不对
		*/
		int ExtractCtrlPacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	解析遥文件操作返信包
		*
		*	检查以pFrame开始的数据缓存区中是否有完整的文件操作返信包
		*
		*	\param	pFrame		帧头指针
		*	\param	nCorrectLen	如有解析出正确的遥控返信包，
		*						则置该参数为正确帧的长度
		*
		*	\retval	int			含义如下：
		*		FLAG_NONE				0		不是遥控返信帧
		*		FLAG_UNCOMPLETE			1		是遥控返信帧，但没接收完整
		*		FLAG_CORRECTFRAME		2		是正确的遥控返信帧
		*		FLAG_ERROR				3		解析出错，说明给的帧头位置不对
		*/
		int ExtractFilePacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	解析其他特殊的返信包
		*
		*	检查以pFrame开始的数据缓存区中是否有完整的其他特殊的返信包
		*
		*	\param	pFrame		帧头指针
		*	\param	nCorrectLen	如有解析出正确的遥控返信包，
		*						则置该参数为正确帧的长度
		*
		*	\retval	int			含义如下：
		*		FLAG_NONE				0		不是遥控返信帧
		*		FLAG_UNCOMPLETE			1		是遥控返信帧，但没接收完整
		*		FLAG_CORRECTFRAME		2		是正确的遥控返信帧
		*		FLAG_ERROR				3		解析出错，说明给的帧头位置不对
		*/
		virtual int ExtractOtherPacket( const Byte* pFrame, int &nCorrectLen );

		/*!
		*	\brief	处理完整的数据包
		*
		*	处理提取出来的接收数据帧，填写交换区，必要时发送变位
		*
		*	\param	pPacketBuffer	数据包
		*	\param	nPacketLength	数据包长度
		*	\param	bFromLocal		区分是本机来的还是另机来的
		*
		*	\retval	void
		*/		
		virtual void ProcessRcvdPacket(	Byte* pPacketBuffer, int nPacketLength, bool bFromLocal );

		/*!
		*	\brief	处理特殊的数据包（已经转化为RTU格式的数据包）
		*
		*	\param	pPacketBuffer	数据包
		*	\param	nPacketLength	数据包长度
		*	\param	bFromLocal		区分是本机来的还是另机来的
		*
		*	\retval	void
		*/		
		virtual void ProcessRcvdSplPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal);

		/*!
		*	\brief	计算16位数据校验和--CRC
		*
		*	1、	置一16位寄存器位全1；
		*	2、	将报文数据的高位字节异或寄存器的低八位，存入寄存器；
		*	3、	右移寄存器，最高位置0，移出的低八位存入标志位；
		*	4、	如标志位是1，则用A001异或寄存器；如标志位是0，继续步骤3；
		*	5、	重复步骤3和4，直至移位八位；
		*	6、	异或下一位字节与寄存器；
		*	7、	重复步骤3至5，直至所有报文数据均与寄存器异或并移位8次；
		*	8、	此时寄存器中即为CRC校验码，最低位先发送；
		*
		*	\param	datapt		数据起始位置
		*	\param	bytecount	字节个数
		*
		*	\retval	WORD	16位的数据校验和
		*/
		WORD CRC(const Byte * datapt,int bytecount);

		/*!
		*	\brief	根据设备地址获取设备索引号
		*
		*	\param	Addr	设备地址
		*	\param	DevNo	设备地址对应的设备索引号
		*
		*	\retval	bool	true表示找到，false表示没有
		*/
		bool GetDeviceNoByAddr( int Addr, int& DevNo );

		/*!
		*	\brief	记录上次发送帧中的重要数据
		*
		*	\retval	void
		*/
		void BackupLastRequest();

		/*!
		*	\brief	组遥控遥调帧
		*
		*	\param	pbyData		帧首地址
		*	\param	nDataLen	帧长
		*	\param	nTimeOut	超时时间
		*	\param	fCmdVal		发送的命令值
		*
		*	\retval	int	0表示成功，否则表示失败
		*/
		virtual int BuildCtrlFrame( 
			Byte* pbyData, int& nDataLen, int& nTimeOut, float& fCmdVal );

		/*!
		*	\brief	组遥控伴随帧
		*
		*	\param	pDevice		设备
		*	\param	lTagAddr	遥控标签地址
		*
		*	\retval	int	0表示成功，否则表示失败
		*/
		int BuildCtrlFwReq( CFepModbusDevice* pDevice, int lTagAddr );

		/*!
		*	\brief	发送请求
		*
		*	\param	pDevice		发送帧的设备
		*	\param	pbyData		数据包首地址
		*	\param	nDataLen	数据包长度
		*
		*	\retval	bool	成功true，否则false
		*/
		virtual int SendRequest(
			CFepModbusDevice* pDevice, Byte* pbyData, int nDataLen );

		/*!
		*	\brief	给指定设备组通道诊断数据帧
		*
		*	\param	pDevice:要操作的设备指针
		*	\param	pbyData: 组好的通道诊断数据帧的首指针
		*	\param	nReturnLen:数据帧的总长度
		*
		*	\retval	void
		*/
		void MakeChannelDiagFrame( 
			CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen );


		//! 读取配置文件
		int ReadIniCfg(std::string csDriverName);

		//!定时器停止工作
		virtual int StopTimer();


	public:
		//! 类名
		static std::string s_ClassName;	

		//! 记录本次发送数据的长度
		int m_nSndLen;

		CIOTimer* m_pPeriodTimer;			//! 周期定时器	

		
	protected:
		//! 定时器
		ACE_Thread_Timer_Queue_Adapter<ACE_Timer_Heap>* m_pTimerMgr;
		CIOTimer* m_pIOTimer;				//! 超时定时器
		CIOTimer* m_pInsertDelayTimer;		//! 返信请求延时定时器

		//! 协议类型：0-Modbus RTU；1-Modbus TCP；2-基于网络的Modbus RTU
		Byte m_byPrtclType;

		bool m_bRTUChannelTest;				//! 当前发送帧是否为通道诊断帧

		//! 上次发送遥控帧记录
		Byte m_pbyCtrlSendBuffer[MB_RTU_MAX_SEND_SIZE];	
		int	 m_nCtrlSendBufferLen;	//! 上次发送遥控帧有效长度

		/*
		 *	最大重发次数。0，1，2，。。。。
		 *	0表示不重复，1表示重发一次。
		 *	注意，设备离线不重发，广播不重发，目前暂定非遥控不重发
		 */
		int	  m_nMaxResend;

		int	  m_nCurResend;			//! 当前重复次数		

		Byte m_byActiveAddr;			//! 当前接收数据中的设备地址（正在解析的帧）
		Byte m_byActiveFunc;			//! 当前接收数据中的功能码（正在解析的帧）	

		Byte m_bycmdType;				//! 命令类型标志，1为遥控，2为遥调
		float m_fCmdVal;				//! 遥控遥调时发送的命令值
		tIOCtrlRemoteCmd m_tCtrlRemoteCmd;	//! 当前处理的遥控遥调命令		

		//! TCP/IP Modbus专用
		WORD m_wdPacketNo;							//! 事务单元标识（包序号）
		/*
		 *	最近发送的下行数据关键部分缓存.下标为包的序列号(0~65535)
		 *	在加载 TCP/IP Modbus驱动的时候为其开辟空间
		 */
		SENDDATA* m_pSendData;

		//! 以下四项在每次发送请求后立即记录之。
		Byte m_bySendAddr;		//! 最近发送帧设备地址
		Byte m_bySendFunc;		//! 最近发送帧功能码
		WORD m_wdSendStartAddr;			//! 最近发送帧起始地址
		WORD m_wdSendPoints;			//! 最近发送帧点数（AI，为寄存器数；DI，为bit数）

		//! 遥控伴随帧（发送遥控后紧跟着的下一个请求）
		int  m_nCtrlInsertTempLen;					//! 
		int  m_nCtrlInsertLen;						//! 大小. 每次发送后清零
		Byte m_pbyCtrlInsertBuf[MB_RTU_MAX_SEND_SIZE];

		bool m_bIsFirstTime;

		bool m_bIsBroadFrame;						//! 当前帧为广播帧，判断是否需要启动定时器
	public:
		unsigned int m_nCounter;;					//! 校时计数
		unsigned int gCountSetting;					//! 定值组计数
		ACE_Time_Value gDateTime;					//! 当前的时间，用于定点校时的时间比对 Add by: Wangyanchao
	};
}
#endif
