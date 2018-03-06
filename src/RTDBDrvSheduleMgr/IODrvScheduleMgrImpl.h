/*!
 *	\file	IODrvScheduleMgrImpl.h
 *
 *	\brief	驱动调度实现头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年3月31日	15:20
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODRVSCHEDULEMRGIMPL_H_
#define _IODRVSCHEDULEMRGIMPL_H_


#ifdef IODRVSHEDULEMGR_EXPORTS
#define IODRVSCHEDULEMGR_API MACS_DLL_EXPORT
#else
#define IODRVSCHEDULEMGR_API MACS_DLL_IMPORT
#endif

#include <string>
#include <map>
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/IOTimer.h"
#include "RTDBBaseClasses/IODrvScheduleMgrBase.h"

namespace MACS_SCADA_SUD
{
	class CIODriverMgr;
	class CIOCfgDriverDbfParser;
	class FieldPointType;

	/*!
	 *	\class	CIODrvScheduleMgr
	 *
	 *	\brief	驱动调度实现类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRVSCHEDULEMGR_API CIODrvScheduleMgr : public CIODrvScheduleMgrBase
	{
		//!驱动管理对象MAP
		typedef std::map<long, CIODriverMgr*> T_MapDriverMgr;

		//!驱动标签解析对象MAP
		typedef std::map<std::string, CIOCfgDriverDbfParser*> T_MapDrvDbfParser;

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIODrvScheduleMgr();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODrvScheduleMgr();

	public:
		/*!
		 *	\brief	驱动调度初始化
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int	0为成功,其他失败
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
		 *	\retval	int	0为成功,其他失败
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
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StopWork();

		/*!
		 *	\brief	驱动调度卸载工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	无
		 *
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int UnInit();

		/*!
		 *	\brief	遥控、遥调命令处理
		 *
		 *	接受遥控、遥调命令，并将命令加入驱动命令队列
		 *
		 *	\param	remoteCmd	遥控命令
		 *
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddWriteCmd(tIORemoteCtrl& remoteCmd);

		/*!
		 *	\brief	通道诊断命令处理
		 *
		 *	接受通道诊断命令，并将命令加入驱动命令队列
		 *
		 *	\param	packetCmd	通道诊断包命令
		 *
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddCtrlPacketCmd(tIOCtrlPacketCmd& packetCmd);

		/*!
		 *	\brief	运行管理命令处理
		 *
		 *	接受管理命令处理，下发给驱动框架
		 *
		 *	\param	ioRunMgrCmd		运行管理命令
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddRunMgrCmd( tIORunMgrCmd& ioRunMgrCmd );

		/*!
		 *	\brief	添加数据变量，更新数据变量信息
		 *
		 *	\param	pFieldPoint	
		 *
		 *	\retval	返回值	0为成功,其他失败
		 */
		virtual int UpdateDataVariable( FieldPointType* pFieldPoint );
		
		/*!
		 *	\brief	配置更新:新增驱动并使其开始工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	CIODriverMgr*	驱动管理对象指针
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:驱动开始工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 */
		virtual int StartDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:驱动停止工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pIOPort			IOPort对象指针
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int StopDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:删除驱动
		 *
		 *	\param	pchDriverName	驱动名称
		 *
		 *	\retval	int		0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int DeleteDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	更新驱动配置信息
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pchDriverName	驱动名称
		 *
		 *	\retval	int		
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int UpdateDrvCfg(CIOPort* pIOPort);

		/*!
		 *	\brief	配置更新:添加IOUnit
		 *
		 *	\param	pIOUnit		IOUnit对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int AddUnit( CIOUnit* pIOUnit );

		/*!
		 *	\brief	配置更新:删除IOUnit
		 *
		 *	\param	pIOUnit		IOUnit对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int DeleteUnit(CIOUnit* pIOUnit);

		/*!
		 *	\brief	配置更新:更新设备的配置信息
		 *
		 *	\param	pIOUnit		IOUnit对象指针
		 *
		 *	\retval	int	0为成功,其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int UpdateUnitCfg(CIOUnit* pIOUnit);

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
		 *	\brief	根据驱动名得到驱动标签解析对象
		 *
		 *	\param	pchDriverName 驱动名称
		 *
		 *	\retval	返回值	CIOCfgDriverDbfParser* 驱动标签解析对象指针
		 */
		CIOCfgDriverDbfParser* GetDbfParserByFileName( const char* pchDriverName );

		/*!
		 *	\brief	驱动调度初始化是否完成
		 *
		 *	\retval	返回值	true 为完成,false为未完成
		 */
		virtual bool IsInitFinished();

		/*!
		 *	\brief	根据IOPort信息创建驱动管理对象
		 *
		 *	\param	pIOPort IOPort对象指针
		 *	
		 *	\retval	返回值	0为成功,其他失败
		 */
		int ProcessIOPort(CIOPort* pIOPort);

		/*!
		 *	\brief	通过NodeId获得驱动管理对象
		 *
		 *	\param	nodeId	Port的NodeId
		 *
		 *	\retval	返回值	CIODriverMgr* 驱动管理对象指针
		 */
		virtual CIODriverMgr* GetDriverMgrByNodeId(long nodeId);

		virtual CIODriverBase* GetDriverBaseByNodeId( long nodeId );
	private:
		/*!
		 *	\brief	根据驱动名解析驱动.dbf文件,初始化相应驱动解析对象信息
		 *
		 *	\param	pchDbfFileName 驱动标签配置文件名称
		 *	\param	pParser		   驱动标签解析对象指针
		 *
		 *	\retval	返回值	0为成功其他失败
		 */
		int InitDriverDbfInfo(const char* pchDbfFileName, CIOCfgDriverDbfParser* pParser);
		/*!
		 *	\brief	通过FieldPoint指针获取驱动管理对象指针
		 *
		 *	\param	FieldPointType*	
		 *
		 *	\retval	CIODriverMgr*	驱动管理对象指针
		 */
		CIODriverMgr* GetDrvMgrByFieldPoint(FieldPointType* pFieldPoint);

	public:
		//!主动定时器队列
		ActiveTimer m_ActiveTimer;

	private:
		//! 通道节点与ID驱动管理对象MAP表
		T_MapDriverMgr m_mapDriverMgr;
		
		//!是否可以开始工作
		bool m_bCanWork;

		//!驱动标签解析对象MAP表
		T_MapDrvDbfParser m_mapDrvDbfParser;
	};
}

#endif