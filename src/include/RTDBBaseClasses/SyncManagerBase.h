/*!
*	\file	SyncManagerBase.h
*
*	\brief	同步管理基类头文件
*
*	\Author songxiaoyu
*
*	\date	2014-4-23 15:00
*
*	\version	1.0
*
*	\attention	Copyright (c) 2014, MACS-SCADA系统开发组,HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef SYNMANAGERBASE_H
#define SYNMANAGERBASE_H

// #include "RTDB/Server/DBSyncManager/ConstDef_SyncManager.h"
#include "RTDBCommon/OS_Ext.h"
#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

using namespace std;
namespace MACS_SCADA_SUD
{


	/*!
	*	\class	CSyncManager
	*
	*	\brief	同步管理类基类，对外暴露全局对象CSyncManagerBase* g_pSyncManager。
	*           为了防止同步模块与节点管理模块之间的互相依赖，需要一个基类来进行隔离。
	*
	*/
	// class CUnitNodeManager;
	// class CUnitSyncMgr;
	class CIOPort;
	class FieldPointType;
	// class CFepSyncMgr;
	class DBBASECLASSES_API CSyncManagerBase
	{
	public:
		CSyncManagerBase();
		virtual ~CSyncManagerBase();

		// /*!
		// *	\brief	初始化
		// *
		// *	\param	无
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int Init() = 0;

		// /*!
		// *	\brief	开始工作
		// *
		// *	\param	无
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int StartWork() = 0;

		// /*!
		// *	\brief	结束工作
		// *
		// *	\param	无
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int StopWork() = 0;

		// /*!
		// *	\brief	卸载
		// *
		// *	\param	无
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int UnInit() = 0;

		// /*!
		// *	\brief	添加数据库单元同步类对象，用于动态增加数据库单元时
		// *
		// *	\param	CUnitNodeManager * pNodeManager：节点管理器指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int AddUnitSync(CUnitNodeManager * pNodeManager) = 0;

		// /*!
		// *	\brief	通过数据库单元号获取同步类对象指针
		// *
		// *	\param	int iIndex： 数据库单元号
		// *
		// *	\retval	CUnitSyncMgr* 数据库单元同步类指针
		// */
		// virtual CUnitSyncMgr * GetUnitSyncByIndex(int iIndex) = 0;

		// /*!
		// *	\brief	删除数据库单元同步类对象，用于动态删除数据库单元时
		// *
		// *	\param	int iIndex：数据库单元号
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int DelUnitSyncIndex(int iIndex) = 0;

		// /*!
		// *	\brief	添加需同步的变量
		// *
		// *	\param	int iIndex：数据库单元号
		// *
		// *	\param	std::vector<UaVariable *> & pVariable：添加的变量指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int  AddUnitSyncVariable(int iIndex,std::vector<UaVariable *> & pVariable) = 0;

		// /*!
		// *	\brief	删除需同步的变量
		// *
		// *	\param	int iIndex：数据库单元号
		// *
		// *	\param	std::vector<UaVariable *> & pVariable：删除的变量指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int DelUnitSyncVariable(int iIndex,std::vector<UaVariable *> & pVariable) = 0;
		// /*!
		// *	\brief	在正副本之间发送命令
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToCopySource(int iIndex,CCmdContext & cmd) = 0;
		// /*!
		// *	\brief	在正副本之间发送写值操作
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CWriteContext &cWriteContext[in]：节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendWriteToCopySource(int iIndex,CWriteContext &cWriteContext) = 0;

		// /*!
		// *	\brief	发送写值操作到除了本站之外所有部署在其它域和站点的本数据单元服务代理
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *	\param	CWriteContext &cWriteContext[in]：用户名、角色名、节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendWriteToOtherUnits(int iIndex, CWriteContext &cWriteContext) = 0;

		// /*!
		// *	\brief	发送命令操作到除了本站之外所有部署在其它域和站点的本数据单元服务代理
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *	\param	CCmdContext &cmdContext[in]：用户名、角色名、节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToOtherUnits(int iIndex, const CCmdContext& cmdContext) = 0;

		// /*!
		// *	\brief	发送写值给副本
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CWriteContext &cWriteContext[in]：用户名、角色名、节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendWriteToCopy(int iIndex,CWriteContext &cWriteContext) = 0;
		// /*!
		// *	\brief	发送命令给正本（仅对正本有效）
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToSameSource(int iIndex,CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	发送命令给另机（仅对主从机有效）
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToEquivalent(int iIndex, CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	发送命令给另机（仅对主从机有效）
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToEquivalentEx(CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	发送命令给备份域
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToOtherDomain(int iIndex, CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	发送命令给备份域
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToOtherDomainEx(CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	发送命令给副本（仅对正副本机有效）
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CCmdContext & cmd[in]：cmd 命令内容（仅支持Call命令）
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendCmdToCopy(int iIndex, CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	发送写值操作给正本（仅对正本有效）
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	CWriteContext &cWriteContext[in]：节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendWriteToSameSource(int iIndex,CWriteContext &cWriteContext) = 0;

		// /*!
		// *	\brief	发送写值操作给其另机
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	const CWriteContext& cWriteContext[in]：用户名、角色名、节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendWriteToEquivalent(int iIndex,CWriteContext& cWriteContext) = 0;



		// /*!
		// *	\brief	发送写值操作给领域
		// *
		// *	\param	int iIndex[in]：数据库单元号
		// *
		// *	\param	const CWriteContext& cWriteContext[in]：用户名、角色名、节点、属性和值列表
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int SendWriteToOtherDomain(int iIndex,CWriteContext& cWriteContext) = 0;

		// /*!
		// *	\brief	添加需同步的通道
		// *
		// *	\param	CIOPort * pIOPort:需要添加的通道对象指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int AddFepSyncIOPort(CIOPort * pIOPort) = 0;

		// /*!
		// *	\brief	删除同步的通道
		// *
		// *	\param	CIOPort * pIOPort:需要删除的通道对象指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int DelFepSyncIOPort(CIOPort * pIOPort) = 0;

		// /*!
		// *	\brief	添加需同步的FEP变量
		// *
		// *	\param	CIOPort * pIOPort[in]：通道对象指针
		// *
		// *	\param	std::vector<UaVariable *> & pVariableVec[in]：添加的变量指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int  AddFepSyncVariable(CIOPort * pIOPort,std::vector<UaVariable *> & pVariableVec) = 0;

		// /*!
		// *	\brief	删除同步的FEP变量
		// *
		// *	\param	CIOPort * pIOPort[in]：通道对象指针
		// *
		// *	\param	std::vector<UaVariable *> & pVariableVec[in]：删除的变量指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual int DelFepSyncVariable(CIOPort * pIOPort,std::vector<UaVariable *> & pVariableVec) = 0;

		// /*!
		// *	\brief	得到FEP同步管理对象指针
		// *
		// *	\param	无
		// *
		// *	\param	UaSessionCallback* 回调指针
		// *
		// *	\retval	int	   0：正常；其他：失败
		// */
		// virtual CFepSyncMgr* GetFepSyncCallback() = 0;

		// /*!
		// *	\brief	通知Port主从状态管理本地Port权重值有更新
		// *
		// *	\param	bIsUpdate:	是否发生更新
		// *	\param	pIOPort:	链路权重发生变化的链路指针
		// *
		// *	\retval	无
		// */
		// virtual void NotifyLWUpdate(OpcUa_Boolean bIsUpdate, CIOPort* pIOPort) = 0;
	};

	extern DBBASECLASSES_API CSyncManagerBase * g_pSyncManager;     //!同步管理基类全局对象
}
#endif
