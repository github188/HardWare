/*!
 *	\file	IODriverBase.h
 *
 *	\brief	驱动对象基类
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月3日	19:52
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERBASE_H_
#define _IODRIVERBASE_H_

#include <string>
#include <map>
#include "Utilities/fepcommondef.h"
#include "RTDBDriverFrame/IODriverCommonDef.h"
// #include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"


namespace MACS_SCADA_SUD
{
	class CIOCfgDriverDbfParser;
	class CIOUnit;
	class CIOPort;
	class FieldPointType;
	class CIODeviceBase;

	typedef std::map<std::string, CIODeviceBase*> T_MapDevices;

	/*!
	 *	\class	CIODriverBase
	 *
	 *	\brief	驱动基类定义
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIODriverBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIODriverBase();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODriverBase();

		/*!
		 *	\brief	驱动开始工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StartWork() = 0;

		/*!
		 *	\brief	驱动停止工作
		 *
		 *	\param	无
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StopWork()= 0;

		/*!
		 *	\brief	打开channel处理
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 */
		virtual int OpenChannel() = 0;

		/*!
		 *	\brief	关闭channel处理
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 */
		virtual int CloseChannel() = 0;

		/*!
		 *	\brief	初始化设备
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 */
		virtual int InitUnit(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	关闭设备
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	strDrvName		设备名称
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StopUnit(std::string strUnitName) = 0;

		/*!
		 *	\brief	将遥控命令加入驱动遥控队列
		 *
		 *	\param	tRemote			遥控命令内容
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int WriteDCB(tIORemoteCtrl& tRemote) = 0;

		/*!
		 *	\brief	下发通道诊断命令
		 *
		 *	\param	tPacket			通道诊断包
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int WritePacket(tIOCtrlPacketCmd& tPacket) = 0;

		/*!
		 *	\brief	下发运行管理命令
		 *
		 *	\param	tPacket			通道诊断包
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int WriteMgr(tIORunMgrCmd& tPacket) = 0;

		/*!
		 *	\brief	通道状态诊断
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0代表成功,其他代表失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StatusUnit() = 0;

		/*!
		 *	\brief	获取类名
		 *
		 *	\retval	std::string		驱动类名称
		 *
		 *	\note		注意事项（可选）
		 */
		std::string getClassName();

		/*!
		 *	\brief	得到用户配置的Port名称
		 *
		 *	\param	无
		 *
		 *	\retval	Port名称
		 */
		virtual std::string GetDriverName() = 0;

		/*!
		 *	\brief	得到驱动配置的协议名称
		 *
		 *	\param	无
		 *
		 *	\retval	协议名称
		 */
		virtual std::string GetCfgPrtclName() = 0;

		/*!
		 *	\brief	获取驱动标签解析对象
		 *
		 *	\param	void
		 *
		 *	\retval	CIOCfgDriverDbfParser*	标签解析对象指针
		 *
		 */
		virtual CIOCfgDriverDbfParser* GetDbfParser() = 0;

		/*!
		 *	\brief	设置驱动标签解析对象
		 *
		 *	\param	pDbfParser	驱动标签解析对象
		 *
		 *	\retval	void
		 *
		 */
		virtual void SetDbfParser(CIOCfgDriverDbfParser* pDbfParser) = 0;

		/*!
		 *	\brief	得到驱动下的所有设备对象
		 *
		 *	\param	无
		 *
		 *	\retval	T_MapDevices& 设备对象MAP
		 *
		 */
		virtual T_MapDevices&  GetAllDevice() = 0;

		/*!
		 *	\brief	获取IOPort对象指针
		 *
		 *	\param	无
		 *
		 *	\retval	CIOPort*	IOPort对象指针
		 */
		virtual CIOPort* GetIOPort() = 0;

		/*!
		 *	\brief	设置IOPort对象指针
		 *
		 *	\param	pIOPort	IOPort对象指针
		 *
		 *	\retval	无
		 */
		virtual void SetIOPort(CIOPort* pIOPort) = 0;

		/*!
		 *	\brief	获取Status检测时间
		 *
		 *	\param	无
		 *
		 *	\retval	int	检测时间
		 */
		virtual int GetStatusWatchTime() = 0;

		/*!
		 *	\brief	配置更新:添加数据变量和处理标签地址更新
		 *
		 *	\param	pFieldPoint 标签对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int UpdateDataVariable( FieldPointType* pFieldPoint ) = 0;

		//!得到正弦模拟Y值
		virtual int GetSineY() = 0;

		//!配置更新:删除驱动
		virtual int BlockDriver() = 0;

		//!更新驱动配置信息
		virtual int UpdateDrvCfg() = 0;

		//!配置更新:暂停设备的工作
		virtual int BlockDevice(CIOUnit* pIOUnit) = 0;

		//!配置更新:更新设备配置信息
		virtual int UpdateDeviceCfg( CIOUnit* pIOUnit ) = 0;

		//!UaDriver主动从FEP再读一次数据
		virtual void ReadOneTimeData();

		//!处理驱动的热数据状态当前处于模拟状态//
		virtual int ProcessHotSim();

		//short OpcUaDateTimeCompare(const OpcUa_DateTime& A, const OpcUa_DateTime& B);
	public:
		//!类名
		static std::string s_ClassName;

	};

	//!声明全局变量驱动类工厂//
	extern IODRIVERFRAME_VAR GenericFactory<CIODriverBase> g_IODriverFactory;

}
#endif
