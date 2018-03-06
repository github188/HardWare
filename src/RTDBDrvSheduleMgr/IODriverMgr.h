/*!
 *	\file	IODriverMgr.h
 *
 *	\brief	驱动管理类定义头文件
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年3月31日	18:19
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERMRG_H_
#define _IODRIVERMRG_H_

#include <string>
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIODriverBase;
	class CIOPort;
	class CIODrvStatusHandle;
	class CIODrvScheduleMgr;
	class CIOCfgDriverDbfParser;
	class FieldPointType;
	class CIOUnit;

	/*!
	 *	\class	CIODriverMgr
	 *
	 *	\brief	驱动管理类
	 *
	 *	详细的类说明（可选）
	 */
	class CIODriverMgr
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIODriverMgr();

		/*!
		 *	\brief	构造函数
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pDrvScheduleImpl 驱动调度对象指针
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		CIODriverMgr(CIODrvScheduleMgr* pDrvScheduleImpl);
		
		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODriverMgr();

		/*!
		 *	\brief	初始化驱动
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	strDrvName	驱动名称
		 *	\param	pIOPort		IOPort对象指针
		 *
		 *	\retval	int	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		int InitDriver(std::string strDrvName, CIOPort* pIOPort);

		/*!
		 *	\brief	驱动开始工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\retval	int	0为成功，其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		int StartWork();

		/*!
		 *	\brief	驱动停止工作
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\retval	int	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		int StopWork();

		/*!
		 *	\brief	卸载驱动
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\retval	int	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		int UnInitDriver();

		/*!
		 *	\brief	处理遥控、遥调命令
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	tRemoteCmd	遥控遥调命令结构体
		 *
		 *	\retval	int	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		int AddWriteCmd(tIORemoteCtrl& tRemoteCmd);

		/*!
		 *	\brief	处理通道诊断命令
		 *
		 *	\param	tPacketCmd	通道诊断命令结构体
		 *
		 *	\retval	int	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		int AddCtrlPacketCmd(tIOCtrlPacketCmd& tPacketCmd);

		/*!
		 *	\brief	处理运行管理命令
		 *
		 *	\param	tRunCmd		运行管理命令
		 *
		 *	\retval	int	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		int AddRunMgrCmd(tIORunMgrCmd& tRunCmd);

		/*!
		 *	\brief	设置通道对象指针,将通道对象指针赋给具体的驱动对象
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pIOPort		IOPort对象指针
		 *
		 *	\retval	void
		 *
		 *	\note		注意事项（可选）
		 */
		void SetChannelPrt(CIOPort* pIOPort);

		/*!
		 *	\brief	设置具体驱动对象指针
		 *
		 *	\param	pDriverBase	驱动对象指针
		 *
		 *	\retval	void
		 */
		void SetDrvPrt(CIODriverBase* pDriverBase);

		/*!
		 *	\brief	得到驱动的DBF解析对象
		 *
		 *	\param	pchDriverName	驱动Dbf文件名
		 *
		 *	\retval	0为成功,其他失败
		 */
		int InitDriverDbfParser(const char* pchDriverName);

		/*!
		 *	\brief	配置更新:添加数据变量和处理FieldPoint标签地址更新
		 *
		 *	\param	pFieldPoint	标签对象指针
		 *
		 *	\retval	0为成功,其他失败
		 */
		int UpdateDataVariable( FieldPointType* pFieldPoint );
		
		/*!
		 *	\brief	配置更新:删除驱动
		 *
		 *	\param	pFieldPoint	标签对象指针
		 *
		 *	\retval	0为成功,其他失败
		 */
		int DeleteDriver();

		/*!
		 *	\brief	更新驱动配置信息
		 *
		 *	\retval	0为成功,其他失败
		 */
		int UpdateDrvCfg();

		/*!
		 *	\brief	配置更新:添加设备
		 *
		 *	\param	pIOUnit IOUnit对象指针
		 *
		 *	\retval	0为成功,其他失败
		 */
		int AddUnit( CIOUnit* pIOUnit );

		/*!
		 *	\brief	配置更新:删除设备
		 *
		 *	\param	pIOUnit IOUnit对象指针
		 *
		 *	\retval	0为成功,其他失败
		 */
		int DeleteUnit( CIOUnit* pIOUnit );

		/*!
		 *	\brief	配置更新:更新设备配置信息
		 *
		 *	\param	pIOUnit IOUnit对象指针
		 *
		 *	\retval	0为成功,其他失败
		 */
		int UpdateDeviceCfg( CIOUnit* pIOUnit );


		//!获取驱动对象指针
		CIODriverBase* GetDriverBase();

		//!处理驱动的热数据状态当前处于模拟状态
		int ProcessHotSim();

	private:
		//! 驱动对象指针
		CIODriverBase* m_pDriverBase;

		//! StatusUnit函数的定时器回调对象
		CIODrvStatusHandle* m_pStatusCb;

		//! 调度StatusUnit函数的定时器ID号
		int m_lStatusId;

		//! 调度StatusUnit函数的时间间隔
		int m_nWatchInterval;

		//!驱动调度管理对象
		CIODrvScheduleMgr* m_pIODrvScheduleMgr;
	};
}
#endif