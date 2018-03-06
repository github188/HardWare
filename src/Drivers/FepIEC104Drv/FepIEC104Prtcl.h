/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104主站协议类头文件
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Prtcl.h 6     09-09-28 16:41 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 09-09-28 16:41 $
 *	$Revision: 6 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOMACSIEC104PRTCL_20080812_H_
#define _IOMACSIEC104PRTCL_20080812_H_

#include "RTDB/Server/DBDriverFrame/IOPrtclParser.h"
#include "FepIEC104DrvH.h"

namespace MACS_SCADA_SUD
{	
	class CIODeviceBase;

	/*!
	 *	\class	CFepIEC104Prtcl
	 *
	 *	\brief	104主站协议类
	 */
	class IOMACSIEC104_API CFepIEC104Prtcl:
		public CIOPrtclParser
	{
	public:
		CFepIEC104Prtcl(void);
		~CFepIEC104Prtcl(void);

	public:

		/*!
		*	\brief	处理一个接收到数据包
		*
		*	\param	pBuf: 帧首地址
		*	\param	iLen: 帧长度
		*
		*	\retval bool true表示成功处理一个有效帧，false表示失败
		*
		*	\note	
		*/
		virtual OpcUa_Int32 RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen );

		/*!
		*	\brief	组请求帧
		*	本接口由驱动器在发送数据前调用,通过引用bTrigTimer来控制是否启动超时定时器
		*
		*	\param	pbyData: 要发送的数据
		*	\param	nDataLen: 要发送的数据的长度
		*	\param	bTrigTimer: 本接口返回０后，驱动器是否启动超时定时器的标志，
		*						true则启动超时定时器，false则不启动
		*
		*	\retval　0为成功，其他失败
		*/
		virtual OpcUa_Int32 BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );		

		/*!
		*	\brief	获得类名
		*
		*	\retval string	类名
		*
		*	\note	
		*/
		std::string GetClassName();

		/*!
		*	\brief	得到最小的包长度
		*
		*	\retval　OpcUa_Int32 最小包长
		*
		*	\note	
		*/
		virtual OpcUa_Int32 GetMinPacketLength();

		/*!
		*	\brief	连接成功后由驱动器回调，用于网络连接类协议
		*
		*	\param	pPeerIP: 对端 IP
		*
		*	\retval　0为成功，其他失败
		*
		*	\note	
		*/
		virtual OpcUa_Int32 OnConnSuccess(std::string pPeerIP );

		//! 根据报文数据获得输出字符串（byRcvOrSnd：1为Rcv，0为Snd）
		std::string GetCommData( OpcUa_Byte* pbyBuf, OpcUa_Int32 nDataLen, OpcUa_Byte byRcvOrSnd );


		//! 类名
		static std::string s_ClassName;

		//! 记录本次发送数据的长度
		OpcUa_Int32 m_nSndLen;

	private:
		/*!
		*	\brief	从接收缓冲区中提取一个帧
		*
		*	\retval　bool	true表示成功提取一个有效帧，false表示失败
		*/
		bool ExtractPacket();

		/*!
		*	\brief	处理提取出来的接收数据帧，填写交换区，必要时发送变位
		*
		*	\param	bFromLocal: 是否为本地发送帧的响应帧
		*
		*	\retval　void
		*
		*	\note	
		*/
		void ProcessRcvdPacket( bool bFromLocal );

	///////////////////////////////////////////////////////////////
	public:


		/*!
		*	\brief	构造本地链路在线时的本地下行帧(不含遥控)
		*
		*	\param	pBuf: 帧首地址
		*
		*	\retval OpcUa_Int32 组成帧的长度
		*
		*	\note	
		*/
		OpcUa_Int32 BuildLocalNormalFrame( OpcUa_Byte* pBuf );


		/*!
		*	\brief	构造本地链路在线时的本地从机下行帧(不含遥控)
		*
		*	\param	pBuf: 帧首地址
		*
		*	\retval OpcUa_Int32 组成帧的长度
		*
		*	\note	
		*/
		OpcUa_Int32 BuildLocalStandbyFrame( OpcUa_Byte* pBuf );

		/*!
		*	\brief	验证是否为有效IP
		*
		*	\param	ip: 要验证的IP字符串首指针
		*
		*	\retval bool true表示ip为一个有效的IP地址，false表示ip无效
		*
		*	\note	
		*/
		bool IsValidIP( const char* ip );

		/*!
		*	\brief	接收序号重新归零
		*
		*	\retval void
		*
		*	\note	
		*/
		void ResetAllDeviceNr(); 

		/*!
		*	\brief	发送序号重新归零
		*
		*	\retval void
		*
		*	\note	
		*/
		void ResetAllDeviceNs();

		/*!
		*	\brief	更新所有设备离在线状态
		*
		*	\retval void
		*
		*	\note	
		*/
		void UpdateDevState();

		/*!
		*	\brief	根据设备地址获取设备索引号
		*
		*	\param	Addr: 设备地址
		*	\param	DevNo: 设备索引号（引用）
		*
		*	\retval bool true表示获取成功，false表示获取失败
		*
		*	\note	
		*/
		bool GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo );

		/*!
		*	\brief	获得链路状态
		*
		*	\retval bool true--至少有一个设备在线；false--所有设备均离线
		*
		*	\note	
		*/
        bool GetLinkState();

		/*!
		*	\brief	获得MacsDriver.ini下链路配置参数
		*
		*	\param	strAppName: 驱动名
		*
		*	\retval void
		*
		*	\note	
		*/
		void InitConfigPar( char* strAppName );

		/*!
		*	\brief	置104通信报文标识，准备切换为诊断通道
		*
		*	\retval void
		*
		*	\note	
		*/
		void ForStopDT();

		/*!
		*	\brief	置104通信报文标识，准备切换为数据通道
		*
		*	\retval void
		*
		*	\note	
		*/
		void ForStartDT();

	protected:
		/*!
		*	\brief	组下发命令帧
		*
		*	\param	sndBuf: 帧首地址
		*
		*	\retval OpcUa_Int32 组成帧的长度
		*
		*	\note	
		*/
		OpcUa_Int32 BuildCtlRequest( OpcUa_Byte* sndBuf );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTime& SDI );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24& SDI );

		/*!
		*	\brief	验证单点信息DPI有效性
		*
		*	\param	DDI: 要验证的DPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTime& DDI );

		bool IsValidDDI( structDDI_Time24NoInfoAddr& DDI );

		bool IsValidDDI( structDDI_Time24& DDI );

		/*!
		*	\brief	验证单点信息VTI有效性
		*
		*	\param	VTI: 要验证的VTI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI& VTI );

		bool IsValidVTI( structVTI_Time24NoInfoAddr& VTI );

		bool IsValidVTI( structVTI_Time24& VTI );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	pPoOpcUa_Int: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息DPI有效性
		*
		*	\param	DPI: 要验证的DPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDDI( structDDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDDI( structDDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum  );

				/*!
		*	\brief	验证单点信息DPI有效性
		*
		*	\param	DPI: 要验证的DPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidVTI( structVTINoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的SPI
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum  );



		/*!
		*	\brief	验证子站远程终端状态（32比特串）信息RTS有效性
		*
		*	\param	RTS: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidRTS( structRTS& RTS );

		/*!
		*	\brief	验证遥测量16位RT信息RTS有效性
		*
		*	\param	RT16: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidRT16bit( structRT16bit& RT16 );

		/*!
		*	\brief	验证遥测量32位RT信息RTS有效性
		*
		*	\param	RT32: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidRT32bit( structRT32bit& RT32 );

		/*!
		*	\brief	验证遥测量16位测量值参数有效性
		*
		*	\param	Para16: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidPara16bit( structPara16bit& Para16 );

		/*!
		*	\brief	验证遥测量32位测量值参数有效性
		*
		*	\param	Para32: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidPara32bit( structPara32bit& Para32 );

		/*!
		*	\brief	验证电能累积量有效性
		*
		*	\param	DNLJ: 要验证的电能累积量
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDNLJ( structDNLJ& DNLJ );

		/*!
		*	\brief	验证带时标的继电保护装置事件有效性
		*
		*	\param	JDBH: 要验证的事件
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidJDBH( structJDBHNoTime& JDBH );

		/*!
		*	\brief	验证带时标的继电保护装置成组启动事件有效性
		*
		*	\param	JDBHGroup: 要验证的事件
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidJDBHGroupAct( structJDBHGroupActNoTime& JDBHGroup );

		/*!
		*	\brief	验证带时标的继电保护装置成组输出电路信息有效性
		*
		*	\param	JDBHGroup: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidJDBHGroupOutput( structJDBHGroupOutputNoTime& JDBHGroup );

		/*!
		*	\brief	验证带变位检出的成组单点信息有效性
		*
		*	\param	DIGroup: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDIGroup( structDIGroup& DIGroup );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTimeNoInfoAddr& SDI );

		/*!
		*	\brief	验证单点信息SPI有效性
		*
		*	\param	SDI: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24NoInfoAddr& SDI );

		/*!
		*	\brief	验证双点信息DPI有效性
		*
		*	\param	DDI: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTimeNoInfoAddr& DDI );

		/*!
		*	\brief	验证步位置信息VTI有效性
		*
		*	\param	VTI: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidVTI( structVTINoInfoAddr& VTI );

		/*!
		*	\brief	验证子站远程终端状态（32比特串）信息RTS有效性
		*
		*	\param	RTS: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidRTS( structRTSNoInfoAddr& RTS );

		/*!
		*	\brief	验证遥测量16位RT信息RTS有效性
		*
		*	\param	RT16: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidRT16bit( structRT16bitNoInfoAddr& RT16 );

		/*!
		*	\brief	验证遥测量32位RT信息RTS有效性
		*
		*	\param	RT32: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidRT32bit( structRT32bitNoInfoAddr& RT32 );

		/*!
		*	\brief	验证遥测量16位测量值参数有效性
		*
		*	\param	Para16: 要验证的参数
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidPara16bit( structPara16bitNoInfoAddr& Para16 );

		/*!
		*	\brief	验证遥测量32位测量值参数有效性
		*
		*	\param	Para32: 要验证的参数
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidPara32bit( structPara32bitNoInfoAddr& Para32 );

		/*!
		*	\brief	验证电能累积量有效性
		*
		*	\param	DNLJ: 要验证的累积量
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDNLJ( structDNLJNoInfoAddr& DNLJ );

		/*!
		*	\brief	验证带时标的继电保护装置事件有效性
		*
		*	\param	JDBH: 要验证的事件
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidJDBH( structJDBHNoTimeNoInfoAddr& JDBH );

		/*!
		*	\brief	验证带时标的继电保护装置成组启动事件有效性
		*
		*	\param	JDBHGroup: 要验证的事件
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidJDBHGroupAct( structJDBHGroupActNoTimeNoInfoAddr& JDBHGroup );

		/*!
		*	\brief	验证带时标的继电保护装置成组输出电路信息有效性
		*
		*	\param	JDBHGroup: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidJDBHGroupOutput( structJDBHGroupOutputNoTimeNoInfoAddr& JDBHGroup );

		/*!
		*	\brief	验证带变位检出的成组单点信息有效性
		*
		*	\param	DIGroup: 要验证的信息
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		bool IsValidDIGroup( structDIGroupNoInfoAddr& DIGroup );

		/*!
		*	\brief	解析出NVA的值
		*
		*	\param	NVA: 要解析的NVA
		*
		*	\retval float NVA的值
		*
		*	\note	
		*/
		float GetNVAVal( structNVA& NVA );

		/*!
		*	\brief	验证I格式帧有效性
		*
		*	\param	pBuf: 要验证的帧首地址
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		static bool IsCtrlAreaI( const void* pBuf );

		/*!
		*	\brief	验证S格式帧有效性
		*
		*	\param	pBuf: 要验证的帧首地址
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		static bool IsCtrlAreaS( const void* pBuf );

		/*!
		*	\brief	验证U格式帧有效性
		*
		*	\param	pBuf: 要验证的帧首地址
		*
		*	\retval bool true表示有效，否则无效
		*
		*	\note	
		*/
		static bool IsCtrlAreaU( const void* pBuf );

		//! 声明友元函数以使这些函数能访问本类的私有数据成员
		friend void PeriodProc( void* pParent );
		friend void OnTimeOut( void* p );		
		friend void OnConnectTimeOut(void* p);							//! 连接定时器超时回调函数
	
		/*!
		*	\brief	将时间由ASDUTime格式转换为OpcUa_Int64格式
		*
		*	\param	const ASDUTime& tTime    
		*
		*	\retval OpcUa_Int64 时间
		*
		*	\note	
		*/
		OpcUa_Int64 ASDUTIME2MACSINT64( const ASDUTime& tTime );

		/*!
		*	\brief	将时间由Time24Bit格式转换为OpcUa_Int64格式
		*
		*	\param	const ASDUTime& tTime    
		*
		*	\retval OpcUa_Int64 时间
		*
		*	\note	
		*/
		OpcUa_Int64 Time24Bit2MACSINT64( const Time24bit& tTime );
		
		/*!
		*	\brief	是否闰年
		*
		*	\param	iYear（四位数）    
		*
		*	\retval bool true闰年，false非闰年
		*
		*	\note	
		*/
		bool isLeap( OpcUa_Int32 iYear );

		/*!
		*	\brief	计算天数
		*
		*	\param	OpcUa_Int32 iYear,in iMon，若bool bFrom1970 =true，则计算时间原点至iyear/iMon的天数
		*    否则，计算自本年始，至本月的天数
		*
		*	\retval OpcUa_Int32 天数
		*
		*	\note	
		*/
		OpcUa_Int32 Days( OpcUa_Int32 iYear,OpcUa_Int32 iMon,bool bFrom1970 = false );

		bool IsLogForRxDataProc();
		bool IsLog();

		bool IsLogValueError();	//!是否强制记录以下校验所去掉的无效报文
		bool IsCheckSDI();	//! 是否校验单点值
		bool IsCheckDDI();	//! 是否校验双点值
		bool IsCheckVTI();	//! 是否校验步位置


	public:
		OpcUa_Int32 m_iPeriodCount;		//!周期计数器。初始化为0，以便启动时立即执行一次。
		OpcUa_Int32 m_iForcedReconnectCount;	//! 强制重连计数器
		OpcUa_Int32  m_bNeedConnected;	//! 需要连接（重新连接）子站

		CIOTimer*			m_pPeriodTimer;							//! 周期定时器


	private:
		CIOTimer*			m_pIOTimer;								//! 超时定时器

		OpcUa_Byte m_bycmdType;					//! 命令类型标志
		tIOCtrlRemoteCmd m_tCtrlRemoteCmd;	//! 当前处理的遥控命令

		bool m_bCanWork;				//! 可以开始工作的标志

		bool m_bIncompleteFrame;		//! 是否为不完整帧
		bool m_bExtractError;			//! 解析有误
		OpcUa_Byte* m_pBuf;

		short m_shAdiLogical;			//! 设备逻辑单元号: 0 ~ Channel[0]->nMaxUnit-1

		OpcUa_Int32	  m_lastDevNo;				//! 校时/总召唤/技术量召唤辅助变量

		OpcUa_Int32  m_t0;						//! 建立连接的超时，毫秒
		OpcUa_Int32  m_t1;						//! 发送或测试APDU的超时，毫秒
		OpcUa_Int32  m_t2;						//! 无数据保文t2<t1时确认的超时，毫秒
		OpcUa_Int32  m_t3;						//! 长期空闲t3>t1状态下发送测试帧的超时，毫秒
		OpcUa_Int32	 m_t4;						//! 计数量召唤周期，毫秒
		OpcUa_Int32	 m_TestCycl;				//! 发送TestFrame的周期，毫秒

		bool m_bDebugLog;
		bool m_bLog;

		WORD m_wdValidDataLen;			//! 有效数据长度

		OpcUa_Int32	 m_CheckTimeCycl;			//! 校时周期, 秒.
		OpcUa_Int32  m_TimeOut;
		OpcUa_Int32  m_nMaxOfflineCount;		//! 最大离线次数
		OpcUa_Int32  m_nOfflineCount;			//! 离线计数器
		OpcUa_Int32  m_nIFrameCount;			//! 一段时间内接收到的I帧计数器
		OpcUa_Int32  m_nLastIFrameCount;		//! 最近一次的I帧计数器

		bool m_bIsLogValueError;	//!是否强制记录以下校验所去掉的无效报文
		bool m_bCheckSDI;	//! 校验单点值的整个字节是否仅仅为0或1
		bool m_bCheckDDI;	//! 校验双点值的整个字节是否仅仅为0~3
		bool m_bCheckVTI;	//! 校验步位置值的每个有效位

		bool m_bNeedStartDT;			//! 是否需要发送STARTDT生效帧
		bool m_bNeedStopDT;				//! 是否需要发送STOPDT生效帧
		bool m_bNeedSendTest;			//! 是否需要发送TESTFR生效帧
		bool m_bNeedStartDTcon;			//! 是否需要发送STARTDT确认帧
		bool m_bNeedStopDTcon;			//! 是否需要发送STOPDT确认帧
		bool m_bNeedSendTestcon;		//! 是否需要发送TESTFR确认帧
		bool m_bNeedSendSFrame;			//! 是否需要发送S格式帧。当收到子站S格式帧时置该标志true。

		OpcUa_Int32	m_iCurDevIndex;				//! 当前正在工作的设备索引号

		//! 发送的S帧总数
		OpcUa_Int32 m_nSFrameCount;	
	
	};
}
#endif
