/*!
 *	\file	IODriver.h
 *
 *	\brief	驱动类头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月22日	19:48
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVER_H_
#define _IODRIVER_H_

#include <map>
#include "utilities/fepcommondef.h"
#include "RTDBDriverFrame/IODriverH.h"
#include "RTDBDriverFrame/IORemoteCtrl.h"

namespace MACS_SCADA_SUD
{
	class CIODeviceBase;
	class CIOLog;
	class CIOPort;
	class CIODrvBoard;
	class CIOCfgDriverDbfParser;
	class FieldPointType;
	class CIOPrtclParserBase;
	

	/*!
	 *	\class	CIODriver
	 *
	 *	\brief	驱动类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIODriver : public CIODriverBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIODriver();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODriver();

		/*!
		 *	\brief	得到驱动类类名
		 */
		std::string GetClassName();

		/*!
		 *	\brief	打开通道
		 *
		 *	创建协议解析类对象,设置通道状态,创建日志对象,设置端口配置信息等
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int OpenChannel();

		/*!
		 *	\brief	关闭通道通道
		 *
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int CloseChannel();

		/*!
		 *	\brief	将遥控指令放入遥控队列
		 *
		 *	\param	tRemote 遥控命令
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int WriteDCB(tIORemoteCtrl& tRemote);

		/*!
		 *	\brief	根据命令修改通道或设备相应状态(模拟状态,在线离线状态)
		 *
		 *	\param	tRunMgr 状态管理命令
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int WriteMgr(tIORunMgrCmd& tRunMgr);

		/*!
		 *	\brief	发送通道诊断命令包
		 *
		 *	\param	tPacket 通道诊断命令
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int WritePacket(tIOCtrlPacketCmd& tPacket);

		/*!
		 *	\brief	处理对标签运行管理命令
		 *	如果FEP与实时服务在同一进程该方法只进行FeildPoint状态设置和SetVal
		 *	如果FEP与实时服务不在同一进程该方法应该被UADriver重写按照UA机制实现标签强制
		 *
		 *	\param	tPacket 标签强制命令
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int WriteMgrForTag( tIORunMgrCmd& tRunMgr );

		/*!
		 *	\brief	为设备设置通道诊断标志,表示要对另机进行通道诊断(只对实际为主设备设置标志)
		 *	
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int StatusUnit(void);

		/*!
		 *	\brief	设置链路下的设备通道诊断状态
		 *	
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		void SetDevCHNCheck(bool bCheck);

		/*!
		 *	\brief	创建驱动器对象、初始化并打开驱动器
		 *	
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int StartWork(void);

		/*!
		 *	\brief	关闭驱动器并删除驱动器对象
		 *	
		 *	\param	无
		 *
		 *	\retval	0为成功其他失败
		 */
		virtual int StopWork(void);

		/*!
		 *	\brief	得到协议解析类类名
		 *	
		 *	\param	无
		 *
		 *	\retval	std::string 协议解析类类名
		 */
		virtual std::string GetPrtclClassName();

		/*!
		 *	\brief	得到设备类类名
		 *	
		 *	\param	无
		 *
		 *	\retval	std::string 设备类类名
		 */
		virtual std::string GetDeviceClassName();

		/*!
		 *	\brief	得到设备模拟类类名
		 *	
		 *	\param	无
		 *
		 *	\retval	std::string 设备模拟类类名
		 */
		virtual std::string GetDeviceSimClassName();

		/*!
		 *	\brief	得到下一个设备对象指针
		 *	
		 *	\param	无
		 *
		 *	\retval	CIODeviceBase* 设备对象指针
		 */
		virtual CIODeviceBase* GetNextDevice();

		/*!
		 *	\brief	得到当前轮训到的设备对象指针
		 *	
		 *	\param	无
		 *
		 *	\retval	CIODeviceBase* 设备对象指针
		 */
		virtual CIODeviceBase* GetCurDevice();

		/*!
		 *	\brief	通过设备名得到设备对象指针
		 *	
		 *	\param	szName	设备名
		 *
		 *	\retval	CIODeviceBase* 设备对象指针
		 */
		CIODeviceBase* GetDeviceByName(std::string szName);

		/*!
		 *	\brief	通过设备地址得到设备类对象指针
		 *	
		 *	\param	nUnitAddr	设备地址(int)
		 *
		 *	\retval	CIODeviceBase* 设备对象指针
		 */
		CIODeviceBase* GetDeviceByAddr( int nUnitAddr );

		/*!
		 *	\brief	通过设备地址得到设备类对象指针
		 *	
		 *	\param	nUnitAddr	设备地址(string)
		 *
		 *	\retval	CIODeviceBase* 设备对象指针
		 */
		CIODeviceBase* GetDeviceByAddr( std::string sUnitAddr );

		/*!
		 *	\brief	得到所有设备
		 *	
		 *	\param	无
		 *
		 *	\retval	T_MapDevices& 设备MAP表
		 */
		T_MapDevices&   GetAllDevice();

		/*!
		 *	\brief	得到设备总数
		 *
		 *	\param	无
		 *
		 *	\retval	int 设备数量
		 */
		int GetDeviceCount();

		/*!
		 *	\brief	按序号得到设备对象指针
		 *	
		 *	\param	iIndex	序号
		 *
		 *	\retval	CIODeviceBase* 设备对象指针
		 */
		CIODeviceBase* GetDeviceByIndex( int iIndex );

		/*!
		 *	\brief	得到用户配置的Port名称
		 *
		 *	\param	无
		 *
		 *	\retval	Port名称
		 */
		std::string GetDriverName();

		/*!
		 *	\brief	得到驱动配置的协议名称
		 *
		 *	\param	无
		 *
		 *	\retval	协议名称
		 */
		std::string GetCfgPrtclName();

		/*!
		 *	\brief	设置遥控队列的大小
		 *	
		 *	\param	size_t 
		 *
		 *	\retval	无
		 */
		void SetCtrlQueueLimit( size_t nLimit );

		/*!
		 *	\brief	写驱动运行日志
		 *	
		 *	\param	pchLog	写入内容 
		 *	\param	bTime	是否带时标
		 *
		 *	\retval	0为成功,其他失败
		 */
		int WriteLog( const char* pchLog, bool bTime=true );

		/*!
		 *	\brief	将驱动通信信息写入调试工具同时写入日志
		 *	
		 *	\param	pbyBuf		数据指针 
		 *	\param	nDataLen	数据长度
		 *	\param	byRcvOrSnd	发送,接收标识
		 *
		 *	\retval	0为成功,其他失败
		 */
		int WriteCommData( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd=0 );

		/*!
		 *	\brief	写驱动通信信息写入日志日志
		 *	
		 *	\param	pbyBuf		数据指针 
		 *	\param	nDataLen	数据长度
		 *	\param	byRcvOrSnd	接收发送标识
		 *
		 *	\retval	0为成功,其他失败
		 */
		int WriteCommDataToLog( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd=0 );

		/*!
		 *	\brief	链路访问是否冲突
		 *	
		 *	\param	无
		 *
		 *	\retval	true冲突,false不冲突
		 */
		bool IsPortAccessClash();

		/*!
		 *	\brief	发送遥控之后的SOE
		 *	
		 *	\param	szUnitName	设备名
		 *	\param  nUnitType	标签类型
		 *	\param	nTagAddr	标签地址
		 *	\param	nVal		值
		 *	\param  nTime		时标信息
		 *
		 *	\retval	0为成功,其他失败
		 */
		int PostYKSOE( std::string szUnitName, int nUnitType, int nTagAddr, int nVal, long nTime = 0 );

		/*!
		 *	\brief	获取超时时间
		 *	
		 *	\param	无
		 *
		 *	\retval	int 超时时间
		 */
		int GetTimeOut();

		/*!
		 *	\brief	获取协议处理类指针
		 *	
		 *	\param	无
		 *
		 *	\retval	CIOPrtclParserBase* 协议类指针
		 */
		CIOPrtclParserBase* GetIOPrtclParse();

		/*!
		 *	\brief	通道是否被禁止
		 *	
		 *	\param	无
		 *
		 *	\retval	true被禁止,false未被禁止
		 */
		bool IsDisable();

		/*!
		 *	\brief	是否处于模拟状态
		 *	
		 *	\param	无
		 *
		 *	\retval	true模拟状态,false非模拟状态
		 */
		bool IsSimulate();

		/*!
		 *	\brief	设置通道状态
		 *	
		 *	\param	pChannelState 通道状态指针
		 *
		 *	\retval	无
		 */
		void SetChannelState(tIOScheduleChannelState* pChannelState);

		/*!
		 *	\brief	设置通道的模拟方式
		 *	
		 *	\param	bySimMode 通道模拟方式
		 *
		 *	\retval	无
		 */
		virtual void SetSimulate(Byte bySimMode);

		/*!
		 *	\brief	停止模拟
		 *	
		 *	\param	无
		 *
		 *	\retval	无
		 */
		virtual void StopSimulate(void);

		/*!
		 *	\brief	设置通道禁止
		 *	
		 *	\param	byDisable 1为禁止，0为不禁止
		 *
		 *	\retval	无
		 */
		virtual void SetDisable(Byte byDisable);

		/*!
		 *	\brief	是否记录日志
		 *	
		 *	\param	无
		 *
		 *	\retval	Byte 1为记录，0为不记录
		 */
		virtual bool IsLog();

		/*!
		 *	\brief	设置是否记录日志标志
		 *	
		 *	\param	byLog 1为记录，0为不记录
		 *
		 *	\retval	无
		 */
		virtual void SetLog(bool byLog);

		/*!
		 *	\brief	是否处于通信监视状态
		 *	
		 *	\param	无
		 *
		 *	\retval	Byte 1为记录，0为不记录
		 */
		virtual bool IsMonitor();

		/*!
		 *	\brief	设置是否进行通信监视
		 *	
		 *	\param	byMonitor 是否进行通信监视
		 *
		 *	\retval	无
		 */
		virtual void SetMonitor(bool byMonitor);

		/*!
		 *	\brief	得到通道冷热状态
		 *	
		 *	\param	void
		 *
		 *	\retval	Byte 1为Hot，0为Cool
		 */
		virtual Byte GetHot();

		/*!
		 *	\brief	设置通道冷热状态
		 *	
		 *	\param	Byte 1为Hot，0为Cool
		 *
		 *	\retval	void
		 */
		virtual void SetHot(Byte byHot);

		/*!
		 *	\brief	通道是否处于强制状态
		 *	
		 *	\param	
		 *
		 *	\retval	bool 通道是否处于强制状态
		 */
		bool IsForce();

		/*!
		 *	\brief	强制通道状态
		 *	
		 *	\param	byPrmScnd 1为Hot，0为Cool
		 *
		 *	\retval	void
		 */
		void ForcePort(Byte byPrmScnd);

		/*!
		 *	\brief	取消通道强制状态
		 *	
		 *	\param	byPrmScnd 1为Hot，0为Cool
		 *
		 *	\retval	void
		 */
		void DeforcePort();

		/*!
		 *	\brief	获取驱动标签解析对象
		 *
		 *	\retval	CIOCfgDriverDbfParser*	标签解析对象指针
		 *
		 *	\note		注意事项（可选）
		 */
		CIOCfgDriverDbfParser* GetDbfParser();

		/*!
		 *	\brief	设置驱动标签解析对象
		 *
		 *	\param	pDbfParser	驱动标签解析对象
		 *
		 *	\retval	CIOCfgDriverDbfParser*	标签解析对象指针
		 *
		 */
		void SetDbfParser(CIOCfgDriverDbfParser* pDbfParser);

		/*!
		 *	\brief	初始化驱动下的所有设备对象
		 *
		 *	\param	无
		 *
		 *	\retval	int 0为成功,其他失败
		 *
		 */
		virtual int InitUnit(CIOUnit* pIOUnit);

		/*!
		 *	\brief	删除指定设备对象
		 *
		 *	\param	strUnitName 设备名
		 *
		 *	\retval	int 0为成功,其他失败
		 *
		 */
		virtual int StopUnit(std::string strUnitName);

		/*!
		 *	\brief	得到驱动的IOPort对象指针
		 *
		 *	\param	无
		 *
		 *	\retval	CIOPort* IOPort对象指针
		 *
		 */
		 virtual CIOPort* GetIOPort();

		/*!
		 *	\brief	设置驱动的IOPort对象指针
		 *
		 *	\param	pIOPort IOPort对象指针
		 *
		 *	\retval	无
		 *
		 */
		 virtual void SetIOPort(CIOPort* pIOPort);


		/*!
		 *	\brief	配置更新:添加数据变量和处理标签地址更新
		 *
		 *	\param	pFieldPoint	标签对象指针
		 *
		 *	\retval	int 0为成功，其他失败
		 */
		virtual int UpdateDataVariable( FieldPointType* pFieldPoint );

		/*!
		 *	\brief	配置更新:删除驱动
		 *	只是让驱动停止工作同时置驱动被删除标识
		 *
		 *	\param	pFieldPoint 标签对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int BlockDriver();

		/*!
		 *	\brief	更新驱动配置
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int UpdateDrvCfg();

		/*!
		 *	\brief	配置更新:暂停指定设备的工作
		 *
		 *	\param	pIOUnit		IOUnit对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int BlockDevice( CIOUnit* pIOUnit );

		/*!
		 *	\brief	配置更新:更新设备的配置信息
		 *
		 *	\param	pIOUnit		IOUnit对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int UpdateDeviceCfg( CIOUnit* pIOUnit );

		
		//!得到正弦模拟Y值
		int GetSineY();
		/*!
		 *	\brief	获取Status检测时间
		 *
		 *	\param	无
		 *
		 *	\retval	int	检测时间
		 */
		int GetStatusWatchTime();

		/*!
		 *	\brief	输出调试信息
		 *
		 *	\param	pchInfo 调试信息
		 *
		 *	\retval	无
		 */
		void OutputDebug(const char* pchInfo);

		/*!
		 *	\brief	获取驱动所在站点名称
		 */
		static std::string GetFEPRunDir();

		 /*!
		 *	\brief	获取驱动日志公共信息配置文件路径
		    接口保留，供其他第三方使用
		 */
		static std::string GetRunCommFepDir();

		/*!
		 *	\brief	驱动是否停止工作
		 */
		bool IsStopWork();

		/*!
		 *	\brief	写驱动运行日志(无论是否开启写日志开关)
		 *	
		 *	\param	pchLog	写入内容 
		 *	\param	bTime	是否带时标
		 *
		 *	\retval	0为成功,其他失败
		 */
		int WriteLogAnyWay( const char* pchLog, bool bTime=true );

		//!链路是否为冗余配置
		bool IsDouble();

		//!通过设备地址得到设备索引号
		bool GetDevIndexByAddr(int devAddr, int& nIndex);

		/*!
		 *	\brief	将遥控命令转发到链路为从的另机
		 *	
		 *	\param	tRemote	遥控命令
		 *
		 *	\retval	int 0为成功,其他失败
		 */
		virtual int SyncWriteCmd2SP( tIORemoteCtrl& tRemote );

		int getQueueSize();
	private:

		/*!
		 *	\brief	向设备发送运行管理命令
		 *	
		 *	\param	tRunMgr	运行管理命令
		 *
		 *	\retval	int 0为成功,其他失败
		 */
		virtual int WriteMgrForUnit(tIORunMgrCmd& tRunMgr);

		/*!
		 *	\brief	向通道发送运行管理命令
		 *	
		 *	\param	tRunMgr	运行管理命令
		 *
		 *	\retval	int 0为成功,其他失败
		 */
		virtual int WriteMgrForPort(tIORunMgrCmd& tRunMgr);

		/*!
		 *	\brief	从MacsDriver.ini文件中获取时间信息配置
		 *			超时时间,轮训时间,数据显示格式
		 *	
		 *	\param	无
		 *
		 *	\retval	无
		 */
		void InitDriverCfg();

		void InitDriverCommCfg();

		void InitOpcUaDriverCfg();
		//create folder as config parameter
		void CreateDriverLogFolder();

		//!处理驱动的热数据状态当前处于模拟状态
		int ProcessHotSim();

		//!获取驱动的当前的模拟方式
		Byte GetSimMode();

	public:
		//!遥控命令队列
		CIORemoteCtrl m_IORemoteCtrl;

		 //!驱动器对象
		 CIODrvBoard* m_pIODrvBoard;

		 //!日志对象指针
		 CIOLog* m_pLog;

		//!类名
		static std::string s_ClassName;

	protected:
		//!双机标识
		bool m_bDoubleComputer;

		//!协议处理类
		 CIOPrtclParserBase* m_pIOPrtclParser;

		//!端口配置信息结构体
		tIOPortCfg m_pPortCfg;

		//!IOPort信息
		CIOPort* m_pIOPort;

		//!设备索引表<设备名,设备对象指针>
		T_MapDevices m_mapDevices;

		//!驱动通信监视日志对象指针
		CIOLog* m_pCommLog;

		//!轮训设备的索引号
		int m_iCurDevIndex;

		//!StatusUnit调度时间
		int	m_nWatchTime;

		//!CPU调度时间
		int m_nPollTime;

		//!超时时间
		int m_nTimeOut;

		//!向数据通信日志中写入数据时数据格式(默认16进制)
		int m_nCommDataFormat;

		//!正选模拟Y值大小
		int m_nSineY;

		//!通道状态指针
		tIOScheduleChannelState*	m_pChannelState;

		//!驱动标签解析对象
		CIOCfgDriverDbfParser* m_pDbfParser;

		//!是否记录日志标识
		Byte m_byIsLog;

		//!该驱动是否已被删除(停止工作)
		bool m_bIsBlocked;

		//!驱动遥控队列大小
		size_t m_nRemoteQueueLimit;

		//!驱动日志文件生成路径
		std::string m_strFilePath;

		//!驱动日志文件大小
		int m_nLogFileSize;

		//!驱动日志文件个数
		int m_nLogFileStoreDays;

		//!UADriver驱动采用队列长度
		int m_QueueSize;
	};
}

#endif