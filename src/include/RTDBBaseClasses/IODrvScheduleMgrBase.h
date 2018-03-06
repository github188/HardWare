/*!
 *	\file	IODrvScheduleMgrBase.h
 *
 *	\brief	驱动调度接口定义头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年3月31日	13:32
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODRVSCHEDULEMRGBASE_H_
#define _IODRVSCHEDULEMRGBASE_H_


#include "RTDBCommon/OS_Ext.h"
#include "RTDBBaseClasses/ScheduleBase.h"
#include "utilities/fepcommondef.h"
#include "RTDBDriverFrame/IODriverBase.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIODriverMgr;
	class FieldPointType;
	class CIOUnit;
	class CSiteProxy;

	/*!
	 *	\class	CIODrvScheduleMgrBase
	 *
	 *	\brief	驱动调度基类
	 *			定义管理驱动的接口,定义管理数据变量的接口,定义管理事件和命令的接口
	 *
	 *	\note	***该类继承自实时库模块的CScheduleBase
	 */
	class DBBASECLASSES_API CIODrvScheduleMgrBase
		: public CScheduleBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */

		 CIODrvScheduleMgrBase();
		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODrvScheduleMgrBase();

	public:
		/*!
		 *	\brief	驱动调度初始化
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int Init(); 

		/*!
		 *	\brief	驱动调度开始工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StartWork();

		/*!
		 *	\brief	驱动调度停止工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StopWork();

		/*!
		 *	\brief	驱动调度卸载工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int UnInit();

		/*!
		 *	\brief	遥控、遥调命令处理
		 *
		 *	接受遥控、遥调命令，并将命令加入驱动命令队列
		 *
		 *	\param	portNodeId		Port的NodeId
		 *	\param	remoteCmd		遥控命令
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddWriteCmd(tIORemoteCtrl& remoteCmd) =0;

		/*!
		 *	\brief	通道诊断命令处理
		 *
		 *	接受通道诊断命令，并将命令加入驱动命令队列
		 *
		 *	\param	packetCmd	通道诊断包命令
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddCtrlPacketCmd(tIOCtrlPacketCmd& packetCmd) = 0;

		/*!
		 *	\brief	运行管理命令处理
		 *
		 *	接受管理命令处理，下发给驱动
		 *
		 *	\param	ioRunMgrCmd		运行管理命令
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddRunMgrCmd(tIORunMgrCmd& ioRunMgrCmd) = 0;

		/*!
		 *	\brief	添加数据变量，更新数据变量信息
		 *
		 *	接受管理命令处理，下发给驱动
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int UpdateDataVariable(FieldPointType* pFieldPoint) = 0;

		/*!
		 *	\brief	删除数据变量
		 *
		 *	如果不存在则直接添加。只要NodeId相同则认为是一个数据变量
		 *
		 *	\param	pchDriverName	驱动名
		 *	\param	pchUnitName		设备名
		 *	\param	nodeId			dataVariable的NodeId
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int DeleteDataVariable(const char* pchDriverName, const char* pchUnitName, long nodeId);

		/*!
		 *	\brief	配置更新:添加驱动
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功，其他失败
		 */
		virtual int AddDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	启动驱动开始工作
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int StartDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:驱动停止工作
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int StopDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:删除驱动
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int DeleteDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	更新驱动配置信息
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int UpdateDrvCfg(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:添加IOUnit
		 *
		 *	\param	pIOUnit			IOUnit对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int AddUnit(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	配置更新:删除IOUnit
		 *
		 *	\param	pIOUnit			IOUnit对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int DeleteUnit(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	配置更新:更新设备配置信息
		 *
		 *	\param	pIOUnit			IOUnit对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int UpdateUnitCfg(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	下发事件信息
		 *
		 *	将系统内部产生的事件下发给某个外部设备
		 *
		 *	\param	pchDriverName	驱动名称
		 *	\param	pchUnitName		设备名称
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int SendEvent(const char* pchDriverName, const char* pchUnitName);

		/*!
		 *	\brief	驱动调度初始化是否完成
		 *
		 *	\retval OpcUa_True 初始化结束,初始化未结束
		 */
		virtual bool IsInitFinished() = 0;

		/*!
		 *	\brief	驱动调度初始化是否完成
		 *
		 *	\retval OpcUa_True 初始化结束,初始化未结束
		 */

		virtual CIODriverMgr* GetDriverMgrByNodeId(long nodeId) = 0;

		virtual CIODriverBase* GetDriverBaseByNodeId( long nodeId );
		
	};

	//!输出驱动调度全局对象
	extern DBBASECLASSES_API CIODrvScheduleMgrBase* g_pIOScheduleManager;
}

#endif