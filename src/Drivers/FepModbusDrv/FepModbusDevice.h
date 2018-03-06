/*!
 *	\file	FILE_BASE$.$FILE_EXT$
 *
 *	\brief	通用Modbus驱动的设备类头文件
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDevice.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_DEVICE_20081030_H_
#define _FEP_MODBUS_DEVICE_20081030_H_

#include "FepModbusDrvH.h"

typedef struct {
	int bEnable;			   //! 0---关闭端口; 1---启动端口。默认1。
	int nSndDelay;			  //! 发送延时，毫秒。默认100（最小5毫秒）
	int nTimeOut;			  //! 超时，毫秒。默认1000
	int nRequestCount;		  //! 端口的请求帧个数
	int nSettingGroupCount;	  //! 触发读定值组帧个数
	int nSettingGroupCycl;	  //!巡检读定值组标志周期
	bool bExceptionAsOnline; //!异常响应视为在线应答。默认1。0表示异常响应不作为设备在线的依据。
	int nCRCOrder;			  //! 0---CRC低字节在前; 1-CRC高字节在前。默认0。
}PORT_CFG;					//! 端口相关配置信息

typedef struct {
	int Enable;			//0---使请求失效; 1---使请求有效。默认1。
	int Function;		//功能码
	int StartingAddress;//起始地址
	int PointCount;		//点数
	int SwapCode;		//0---不翻转数据; 1---翻转数据。默认0。
}REQUEST_CFG;			//单个请求配置信息

typedef struct {
	std::string strDoTagName;		//!触发读取定值组标签名
	int iReadCount;					//!读取定值组次数（递减）
	int iReadCountCfg;				//!读取定值组配置次数
	int Function;		//功能码
	int StartingAddress;//起始地址
	int PointCount;		//点数
	int SwapCode;		//0---不翻转数据; 1---翻转数据。默认0。
}SETTINGGROUP_CFG;			//单个读定值组帧配置信息

namespace MACS_SCADA_SUD
{	
	/*!
	*	\class	CFepModbusDevice
	*
	*	\brief	TEP-I-F直流屏微机监控装置驱动的设备类
	*/
	class FEPMODBUSDRV_API CFepModbusDevice : public CIODeviceBase
	{
	public:
		/*!
		*	\brief	缺省构造函数
		*/
		CFepModbusDevice(void);

		/*!
		*	\brief	缺省析构函数
		*/
		~CFepModbusDevice(void);	

	public:
		/*!
		*	\brief	初始化
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int Init();

		/*!
		*	\brief	构造本地链路在线时的本地下行帧(不含遥控)
		*
		*	\param	pBuf		帧首地址
		*
		*	\retval	int	帧长度
		*/
		virtual int BuildLocalNormalFrame( Byte* pBuf );

		/*!
		*	\brief	构造测试帧
		*
		*	\param	pBuf		帧首地址
		*
		*	\retval	int	帧长度
		*/
		virtual int BuildTestFrame( Byte* pBuf );

		/*!
		*	\brief	组校时帧的逻辑实现
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	组设备相关的特殊轮询帧，如SOE等
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildSpecialReqFrame( Byte* buf );

		/*!
		*	\brief	组轮询帧
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildQueryFrame( Byte* buf );

		/*!
		*	\brief	组DOA遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥控标签地址		
		*	\param	byVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDOAReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	组DOB遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥控标签地址		
		*	\param	byVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDOBReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	组DOC遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥控标签地址		
		*	\param	byVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDOCReqFrame( Byte* buf, int lTagAddr, Byte byVal );
		/*!
		*	\brief	组DOC遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	byVal		值
		*	\param	nLen		数据长度
		*
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDOMReqFrame( Byte* buf, int lTagAddr, Byte byVal, int nLen);

		/*!
		*	\brief	组DOD遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥控标签地址		
		*	\param	byVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDODReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	组DOF遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥控标签地址		
		*	\param	byVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDOFReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	组DOG遥控帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥控标签地址		
		*	\param	byVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDOGReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	组AOA遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	fVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAOAReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	组AOB遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	fVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAOBReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	组AOC、AOF遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	byVal		值
		*	\param	nLen		数据长度
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAOCReqFrame(
			Byte* buf, int lTagAddr, Byte* byVal, int nLen );

		/*!
		*	\brief	组AOD遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	fVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAODReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	组AOE遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	fVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAOEReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	组AOF遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	fVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAOFReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	组AOG遥调帧
		*
		*	\param	buf			帧首地址
		*	\param	lTagAddr	遥调标签地址		
		*	\param	fVal		值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildAOGReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	根据遥信响应帧的数据域，
		*			更新相应遥信点的值,按bit更新（功能码为1或2）
		*
		*	\param	UnitType		此函数只处理MACSMB_DIA或MACSMB_DIB
		*	\param	pbyData			数据首地址
		*	\param	nStartAddr		起始标签地址
		*	\param	nPointCount		标签点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			>0表示成功，否则表示失败;>0时，数值表示此次变化的点的个数
		*/
		virtual int UpdateDI( int UnitType, 
			Byte* pbyData, int nStartAddr, int nPointCount, bool bFromLocal );

		/*!
		*	\brief	根据Holding Register中的数据更新点值（功能码为3）
		*
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	void
		*/

		virtual int UpdateHoldingAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );
		/*!
		*	\brief	组设备特殊的遥控帧
		*
		*	详细的函数说明（可选）
		*
		*	\param	pbyData	帧首地址（一个参数对应一个）
		*	\param	tCtrlRemoteCmd 当前的指令
		*	\param	fCmdVal 命令值
		*
		*	\retval	返回值	帧长度（一个返回值对应一个）
		*
		*	\note		注意事项（可选）
		*/
		virtual int BuildSplCtrlFrame(Byte* pbyData,const tIOCtrlRemoteCmd& tCtrlRemoteCmd,float& fCmdVal);
		/*!
		*	\brief	根据Input Register中的数据更新点值（功能码为4）
		*
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	void
		*/
		virtual int UpdateInputAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	更新其他需要的寄存器的值
		*
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		virtual int UpdateRegister( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	根据Holding Register中的数据更新点值之前需要进行的特殊处理
		*			（功能码为3）
		*	\param	UnitType
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		virtual int BeforeUpdateHoldingAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	根据Input Register中的数据更新点值之前需要进行的特殊处理
		*			（功能码为4）
		*	\param	UnitType
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		virtual int BeforeUpdateInputAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	根据AI响应帧的数据域，
		*			更新其中包含的DI点的值（UnitType为MACSMB_DID或MACSMB_DIE）
		*	\param	UnitType		此函数只处理MACSMB_DID或MACSMB_DIE
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			>0表示成功，否则表示失败;>0时，数值表示此次变化的点的个数
		*/
		template < bool bHighFirst>
		int UpdateDIinReg( int UnitType, 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );		

		/*!
		*	\brief	处理文件读操作返回帧的数据
		*
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		virtual int UpdateFileInfo( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	组多点遥调帧
		*
		*	\param	pbyBuf		帧首地址
		*	\param	wRegAddr	寄存器地址		
		*	\param	wRegNo		寄存器个数
		*	\param	pbyRegVal	待写入寄存器的正常次序的WORD值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildCtrlMultiRegsFrame( Byte* pbyBuf,
			const WORD wRegAddr, const WORD wRegNo, const Byte* pbyRegVal);

		/*!
		*	\brief	组多点遥调帧
		*
		*	\param	pbyBuf		帧首地址
		*	\param	wRegAddr	寄存器地址		
		*	\param	wRegNo		寄存器个数
		*	\param	wRegVal		待写入寄存器的正常次序的WORD值
		*
		*	\retval	int			帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildCtrlMultiRegsFrame( 
			Byte* pbyBuf, const WORD wRegAddr, const WORD wRegNo, const WORD* wRegVal);

		/*!
		 *	\brief	输出OX10功能码日志信息.
		 *	\param	nLen 组出的数据长度
		 *
		 *	\retval	无
		 */
		void WriteLogForMO( int nLen );

		/*!
		*	\brief	记录需要的寄存器（主要是只取寄存器中某位的DI或AI点需要）
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int InitNeedRegister();

		/*!
		*	\brief	记录需要的寄存器（主要是只取寄存器中某位的DI或AI点需要）
		*
		*	\param	iUnitType		UNITTYPE
		*	\param	iDivisor		除数
		*
		*	\retval	0为成功，否则失败
		*/
		int InitNeedRegByUnitType(int iUnitType, int iDivisor);

		/*!
		*	\brief	根据遥控标签地址获取遥控紧随帧
		*
		*	\param	lTagAddr		遥控标签地址
		*
		*	\retval	struCtrlFollowReq*	遥控紧随帧
		*/ 
		struCtrlFollowReq* GetCtrlFwReqByTagAddr( int lTagAddr );	

		/*!
		*	\brief	把时间转换成毫秒数
		*
		*	\param	strTime	要转换的时间	
		*
		*	\retval	MACS_INT64	毫秒数，为-1则表示转换失败
		*/
		//OpcUa_Int64 CaculateMSNum( DAILYTIME strTime );
		long CaculateMSNum( DAILYTIME strTime );
	
		/*!
		*	\brief	m_bFlag变位
		*/
		void SetFlag()
		{
			m_bFlag = !m_bFlag;
		};

		/*!
		*	\brief	获得设备标签
		*/
		int GetDevType()
		{
			return m_nDevType;
		};

		/*!
		*	\brief	设备是否有校时帧
		*/
		bool HasCheckTime()
		{
			return m_bCheckTime;
		};

		/*!
		*	\brief	设备是否有SOE帧
		*/
		bool HasSOE()
		{
			return m_bSOE;
		};

		/*!
		*	\brief	触发读取定值组帧数
		*/
		int GetSETTINGGROUP()
		{
			return m_PortCfg.nSettingGroupCount;
		}

		/*!
		*	\brief	获得设备触发读定值组周期
		*/
		int GetRdSettingGpCycl()
		{
			return m_PortCfg.nSettingGroupCycl;
		}

		/*!
		*	\brief	获得设备校时周期
		*/
		int GetCheckTimeCycl()
		{
			return m_nCheckTimeCycl;
		};

		/*!
		*	\brief	获得设备校时时间点(小时) Add by: Wangyanchao
		*/
		int GetCheckTimeHour()
		{
			return m_nCheckTimeHour;
		};

		/*!
		*	\brief	获得设备校时时间点(分钟) Add by: Wangyanchao
		*/
		int GetCheckTimeMin()
		{
			return m_nCheckTimeMin;
		};

		/*!
		*	\brief	获得设备端口配置信息
		*/
		PORT_CFG GetPortCfg()
		{
			return m_PortCfg;
		};

		/*!
		*	\brief	获得设备设备最大离线次数
		*/
		int GetMaxOffLine()
		{
			return m_nMaxOfflineCount;
		};

		/*!
		*	\brief	当前是否需要校时
		*/
		bool NeedCheckTime()
		{
			return m_bNeedCheckTime;
		};

		/*!
		*	\brief	设置需要校时标志
		*/
		void SetNeedCheckTime( bool bNeedChkTm)
		{
			m_bNeedCheckTime = bNeedChkTm;
		};

		/*!
		*	\brief	当前是否读定值组周期
		*/
		bool ReadSettingGroupPD()
		{
			return m_bNeedReadSettingGroup;
		}

		/*!
		*	\brief	设置需要读定值组标志
		*/
		void SetNeedRdSettingGp( bool bNeedRDSettingGp)
		{
			m_bNeedReadSettingGroup = bNeedRDSettingGp;
		};


		/*!
		*	\brief	获得当前请求帧序号
		*/
		short GetCurrReq( )
		{
			return m_shCurrReq;
		};

		/*!
		*	\brief	获得测试帧的有效信息
		*/
		bool GetTestReqStatus( )
		{
			return m_bTestReq;
		};

		/*!
		*	\brief	上次发送是否是SOE
		*/
		bool IsLastSendSOE( )
		{
			return m_bSOESended;
		};

		/*!
		*	\brief	设置上次发送是否是SOE
		*/
		void SetLastSendSOE(bool bLastSendSOE )
		{
			m_bSOESended = bLastSendSOE;
		};

		/*!
		*	\brief	获得从机该设备的超时次数
		*/
		int GetSlaveTimeOuts( )
		{
			return m_nSlaveTimeOuts;
		};

		/*!
		*	\brief	设置从机该设备的超时次数
		*/
		void SetSlaveTimeOuts(int nSlaveTimeOuts )
		{
			m_nSlaveTimeOuts = nSlaveTimeOuts;
		};

		/*!
		*	\brief	获得该设备是否是广播校时
		*/
		int IsBroadChckTime( )
		{
			return m_bBroadcast;
		};

		/*!
		*	\brief	是否有特殊请求帧
		*/
		int HasSpecialReq( )
		{
			return ( m_nSpecialReqCount > 0 );
		};

		void SetTrySendCount( int nTry )
		{
			m_nTrySendCount = nTry;
		}

		void IncrTrySendCount()
		{
			m_nTrySendCount++;
		}
		
		int GetTrySendCount()
		{
			return m_nTrySendCount;
		}
		
		//!重写父类设置主方法,输出调试信息
		virtual void SetHot( Byte byHot );


		//! 获得类名
		std::string GetClassName();

		//!判断设备是否存在这样请求
		bool IsReqeustExited( Byte Funcode, WORD StartAddr, WORD PointCount );

		//检查设备是否只有一个请求，且该请求从协议解析上还可以被解析为响应。举例：
		//以下请求既可以是请求，也可以是响应：
		//XX 01 03 XX 00 YY XX XX 
		//XX 02 03 XX 00 YY XX XX 
		//其中当YY 范围为17~24时，既可以请求，也可以是响应。
		bool IsFuzzy();

		//设备的异常应答是否作为在线的依据
		bool IsExceptionAsOnline();

	protected:
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
		*	\brief	读取设备配置
		*
		*	\param			
		*
		*	\retval	int	 0表示成功，其他表示失败
		*/
		virtual int InitCfg();

		/*!
		*	\brief	路径是什么类型
		*
		*	\param	strPathName	路径名
		*
		*	\retval	int	值为DIR__TYPE，FILE_TYPE或NONE_TYPE
		*/
		int PathType( std::string strPathName );

		/*!
		*	\brief	根据遥测响应帧的数据域，更新相应AI点的值，
		*			此模板函数用于处理标签值为单字的AI点
		*
		*	\param	bSigned			有符号——true；无符号——false
		*	\param	bHighFirst		高位在前——true；低位在前——false
		*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
		*							此模板函数用于处理标签值为单字的AI点
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		template <bool bSigned, bool bHighFirst>
			int UpdateWordAI(int UnitType, 
				Byte* pbyData,int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	处理遥测的响应帧，更新相应AI点(寄存器中N位)的值
		*
		*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
		*							此模板函数用于处理标签值为单字的AI点
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		template <bool bHighFirst>
			int UpdateAIinReg( int UnitType,
				Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	处理遥测的响应帧，更新相应AI点(字符串类型)的值
		*
		*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
		*							此模板函数用于处理标签值为单字的AI点
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		template <bool bHighFirst>
		int UpdateAIString( int UnitType,
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	根据遥测响应帧的数据域，更新相应AI点的值，
		*			此模板函数用于处理标签值为双字的AI点
		*
		*	\param	bSigned			有符号——true；无符号——false
		*	\param	bHighFirst		高位在前——true；低位在前——false
		*	\param	bHiWdFirst		高字在前——true；低字在前——false
		*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
		*							此模板函数用于处理标签值为双字的AI点
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		template <bool bSigned, bool bHighFirst, bool bHiWdFirst>
			int UpdateDWordAI( int UnitType,
				Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	处理遥测的响应帧，更新相应AI点的值
		*			此函数用于处理无特定规律的AI点（上面两个模板函数处理不了的）
		*
		*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		int UpdateOtherAI( int UnitType, 
				Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );		

		/*!
		*	\brief	获得特殊设备配置信息
		*
		*	\param	file_name		配置文件路径名
		*
		*	\retval	void
		*/
		void GetSpecialCfg(std::string file_name);

		/*!
		*	\brief	获得端口相关配置信息
		*
		*	\param	file_name		配置文件路径名
		*
		*	\retval	void
		*/
		void GetPortCfg(std::string file_name);

		/*!
		*	\brief	获得轮询帧配置信息，必须在调用了GetPortCfg函数后才能调用，
		*			否则没有意义
		*
		*	\param	file_name		配置文件路径名
		*
		*	\retval	void
		*/
		void GetReqCfg(std::string file_name);

		/*!
		*	\brief	获得测试帧信息
		*
		*	\param	file_name		配置文件路径名
		*
		*	\retval	void
		*/
		void GetTestCfg(std::string file_name);

		/*!
		*	\brief	获得遥控伴随帧配置信息
		*
		*	\param	file_name		配置文件路径名
		*
		*	\retval	void
		*/
		void GetFollowReqCfg(std::string file_name);

		/*!
		*	\brief	组校时帧（仅针对深2线TITANS交直流屏）
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		int BuildTITANSCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	组855协议的校时帧 Add by: Wangyanchao
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		int Build855CheckTimeFrame( Byte* buf );
		
		/*!
		*	\brief	组校时帧（仅对NEXUS1272电度表设备有效）
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		int BuildNEXUSCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	组校时帧（仅对50V保护设备DB-MLEs-PL有效）
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		int  BuildDBMLEsPLCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	十进制数转换为相应的BCD码
		*
		*	\param	int x		十进制数	
		*
		*	\retval	int		对应的BCD码数值
		*
		*	\note		
		*/
		int  Dec2Bcd(int x);

		/*!
		*	\brief	组设备校时帧内容
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDevCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	组具体设备相关的特殊轮询帧
		*
		*	\param	buf		帧首地址
		*	\param	nIndex	特殊帧序号（即：第几个特殊查询帧）
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		virtual int BuildDevSplReqFrame( Byte* buf, int nIndex );

		/*!
		*	\brief	写文件记录帧(目前只支持写单个文件请求)
		*
		*	\param	byBuf			帧首地址
		*	\param	strFileReq		文件记录属性
		*	\param	pRegVal			记录数据
		*	\param	bIsBroadcast	是否为广播
		*
		*	\retval	int		帧长
		*/
		virtual int BuildWriteRef( Byte* byBuf, FILEREQ strFileReq,
			const Byte* pbyRegVal, bool bIsBroadcast = false);

		/*!
		*	\brief	读文件记录帧(目前只支持读单个文件请求)
		*
		*	\param	byBuf			帧首地址
		*	\param	strFileReq		文件记录属性
		*
		*	\retval	int		帧长
		*/
		virtual int BuildReadRef( Byte* byBuf, FILEREQ strFileReq);

		/*!
		*	\brief	组TITANS直流屏的读取告警帧
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		int BuildTEPIFReadWarnFrame( Byte* buf );

		/*!
		*	\brief	根据AI响应帧的数据域，
		*			更新深圳2号线TINTANS直流屏的SOE标签值并发送SOE
		*			（UnitType为MACSMB_TEPIFSOE）
		*	\param	UnitType		此函数只处理MACSMB_TEPIFSOE
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		int UpdateTEPIFSOE( int UnitType, 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	根据AI响应帧的数据域， NEXUS1272电度表功率因数
		*			（UnitType为MACSMB_PowFactor）
		*
		*	\param	UnitType		该值对应macsMB.dbf文件中的UnitType项的值，
		*							此函数只处理MACSMB_PowFactor
		*	\param	pbyData			数据首地址,注意不是帧首地址
		*	\param	nStartAddr		起始寄存器地址
		*	\param	nRegCount		寄存器点数
		*	\param	bFromLocal		是否本地发送帧的响应
		*
		*	\retval	int			0表示成功，否则表示失败
		*/
		int Update_PowFactor( int UnitType, 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	获取标签的整型值
		*/
		bool IsDIValueChange( int UnitType, int nTagAddr, Byte& byOldVal, Byte& byNewVal, std::string& strTagName );

		/*!
		*	\brief	组触发读取定值组的请求帧
		*
		*	\param	buf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数buf要预先分配空间
		*/
		int BuildReadSettingGroupFrame( Byte* pbyBuf );

		/*!
		*	\brief	获得读定值组帧信息
		*
		*	\param	file_name		配置文件路径名
		*
		*	\retval	void
		*/
		void GetSettingGroupCfg(std::string file_name);

		/*!
		*	\brief	组校时帧
		*
		*	\param	pbyBuf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数pbyBuf要预先分配空间
		*/
		int BuildF650CheckTimeFrame( Byte* pbyBuf );

		/*!
		*	\brief	是否闰年
		*
		*	\param	iYear（四位数）    
		*
		*	\retval bool true闰年，false非闰年
		*
		*	\note	
		*/
		bool isLeap( int iYear );

		/*!
		*	\brief	计算天数
		*
		*	\param	int iYear,in iMon，若bool bFrom1970 =true，则计算时间原点至iyear/iMon的天数
		*    否则，计算自本年始，至本月的天数
		*
		*	\retval int 天数
		*
		*	\note	
		*/
		int Days( int iYear,int iMon,bool bFrom1970 = false );

		/*!
		*	\brief	组校时帧
		*
		*	\param	pbyBuf		帧首地址
		*
		*	\retval	int		帧长
		*
		*	\note		参数pbyBuf要预先分配空间
		*/		
		int BuildL30CheckTimeFrame( Byte* pbyBuf );

		//BCD码转HEX
		BYTE BCD2HEX(BYTE bcd);

		//HEX转BCD码
		BYTE HEX2BCD(BYTE hex);

		//组校时帧(仅对LD温控仪装置有效)
		int BuildLDCheckTimeFrame( BYTE* buf );

		//组校时帧(仅对SEPCOS设备有效)
		int BuildSEPCOSCheckTimeFrame( BYTE* buf );

		//组校时帧(仅对大全PLC（S7-200）设备有效)
		int BuildDQPLCCheckTimeFrame( BYTE* buf );

		//组校时帧(仅对大连旅顺交直流屏设备有效)
		int BuildDLLSJZLPCheckTimeFrame( BYTE* buf );

		//组校时帧(仅对再生制动设备有效)
		int BuildZSZDCheckTimeFrame( BYTE* buf );

		//组校时帧(仅对西门子1500V开关柜S7-300设备(大连地铁1、2号线)有效)
		int BuildDLSIEMENSS73CheckTimeFrame( BYTE* buf );

		//组校时帧(仅对上海成套1500V开关柜U-MLEs-SC设备(大连地铁1、2号线)有效)
		int BuildDLUMLESSCCheckTimeFrame( BYTE* buf );

		//组校时帧(仅对上海成套1500V开关柜S7-200设备(大连地铁1、2号线)有效)
		int BuildDLSIEMENSS72CheckTimeFrame( BYTE* buf );

		//组校时帧(仅对燕房线四方CSC268设备校时)
		int BuildSFCSC268CheckTimeFrame(BYTE* buf);

	public:
		//! 类名
		static std::string s_ClassName;	
		std::vector<SETTINGGROUP_CFG> vecSettingGroup;	//! 有效请求
		bool m_bRetryPeriod;					//离线设备是否该发送大周期轮询请求
	protected:
		/*
		*	组警告读取帧时的参考标志，为true读130寄存器，为false读150寄存器，
		*	只对深2TITANS直流屏有效
		*/
		bool m_bFlag;

		//////////////////////////////////////////////////////////////////////
		//! 设备特殊配置信息
		int	 m_nDevType;				//! 设备类型：参见宏定义
		bool m_bCheckTime;				//! 是否需要校时帧
		bool m_bSOE;					//! 是否需要SOE帧
		int m_nCheckTimeCycl;			//! 校时周期
		int m_nCheckTimeHour;	//! 校时时间点(小时) Add by: Wangyanchao
		int m_nCheckTimeMin;	//! 校时时间点(分钟) Add by: Wangyanchao

		//! 端口相关配置信息
		PORT_CFG m_PortCfg;				//! 端口相关配置信息		
		int m_nMaxOfflineCount;			//! 设备最大离线次数

		bool m_bNeedCheckTime;			//! 当前是否需要校时
		short m_shCurrReq;				//! 当前请求号，0～m_nValidReqCount－1

		//! 上次发送是否是SOE，现只对深2TITANS直流屏有效
		bool m_bSOESended;	

		int m_nCheckTimeAddr;			//! 校时起始地址
		bool m_bBroadcast;				//! 是否支持广播校时
		int	 m_nValidReqCount;			//! 有效请求帧个数

		std::vector<REQUEST> Request;	//! 有效请求
		REQUEST m_reqTest;				//! 测试帧
		bool m_bTestReq;				//! 测试帧有效标志

		//! 遥控返信请求配置，每次遥控后可以立即请求所配置的返信点。
		std::map<int, struCtrlFollowReq*>  m_mapCtrlFollowReq;	

		//! 记录从机该设备的超时次数，以便判断从机上设备的在线离线
		int m_nSlaveTimeOuts;	

		//! 特殊轮询帧相关
		int m_nSpecialReqCount;			//! 特殊轮询帧个数
		int m_nCurSpecialReq;			//! 当前特殊请求号，0～m_nSpecialReqCount－1

		//! 掩码数组，为了取寄存器数值的n位
		WORD m_wdBitMask[16];

		//! 保存的寄存器（为表示寄存器中N位的AI或DI点使用）
		std::map<int,Byte*> m_mapRegister;

		int m_nTrySendCount;	//! 初始连接成功后的发送次数		

		//! 配置文件名
		std::string m_strCfgFileName;
				
		int m_iCurrSettingGroup;//!当前定值组帧号
		bool m_bNeedReadSettingGroup; //!读定值组周期到
	};
}

#endif
