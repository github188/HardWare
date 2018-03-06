/*!
 *	\file	IOCmdManagerBase.h
 *
 *	\brief	
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014年4月22日	16:38
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */


#ifndef __IOCMDMANAGERBASE_H__
#define __IOCMDMANAGERBASE_H__

#include <string>
#include "utilities/fepcommondef.h"
#include "RTDBCommon/OS_Ext.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD {

class DBBASECLASSES_API CIOCmdManagerBase
{
public:
	CIOCmdManagerBase(void);
	virtual ~CIOCmdManagerBase(void);

public:
	/*!
	*	\brief	初始化
	*		暂无内容需要加载
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int Init() = 0;

	/*!
	*	\brief	开始工作
	*		启动主动对象，因本模块没有主动对象，所以直接返回。
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int StartWork() = 0;

	/*!
	*	\brief	是否可以开始工作
	*
	*	\retval	true为可以开始工作；false为不可以。
	*/
	virtual bool CanWork() = 0;

	/*!
	*	\brief	停止工作
	*		停止主动对象，因本模块没有主动对象，所以直接返回。
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int StopWork() = 0;

	/*!
	*	\brief	卸载
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int UnInit() = 0;

	/*!
	*	\brief	写标签值,即发送遥控/遥调命令
	*
	*	\param	tRemoteCmd:遥控命令结构
	*
	*	\retval	0为成功，其他失败
	*/
	virtual Byte WriteDataVariable(tIORemoteCtrl& tRemoteCmd, tIOCmdCallBackParam* pCallBack = NULL) = 0;

	/*!
	*	\brief	发送诊断数据包命令
	*
	*	\param	tPacketCmd:命令结构
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int SendIOCtrlPacketCmd(tIOCtrlPacketCmd& tPacketCmd, tIOCmdCallBackParam* pCallBack = NULL) = 0;

	/*!
	*	\brief	发送运行管理命令,控制实体的状态
	*
	*	\param	tRunMgrCmd:命令结构
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int SendIORunMgrCmd(tIORunMgrCmd& tRunMgrCmd, tIOCmdCallBackParam* pCallBack = NULL) = 0;

	/*!
	*	\brief	向另机发送通道诊断包
	*
	*	\param	tPacketCmd:命令结构
	*
	*	\retval	0为成功，其他失败
	*/
	virtual int SendIOChannelDiag(tIOCtrlPacketCmd& tDiagPacket, tIOCmdCallBackParam* pCallBack = NULL) = 0;
};

extern  DBBASECLASSES_API CIOCmdManagerBase* g_pIOCmdManager;     //!命令管理器全局对象

}// End namespace
#endif // __IOCMDMANAGERBASE_H__
