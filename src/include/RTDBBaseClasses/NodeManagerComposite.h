/*!
*	\file	NodeManagerComposite.h
*
*	\brief	根节点管理器接口类
*			定义节点管理器的对外接口，功能由实现类CNodeManagerCompositeImpl完成
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __NODEMANAGERCOMPOSITE_H__
#define __NODEMANAGERCOMPOSITE_H__

#include "RTDBCommon/OS_Ext.h"
#include <map>
// #include "RTDBPlatformClass/ComponentType.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIONodeManager;
	/*!
	*	\class	CNodeManagerComposite
	*
	*	\brief	根节点管理器接口类
	*			定义节点管理器的对外接口，在接口内部调用实现类相应接口完成接口功能
	*/
	class DBBASECLASSES_API CNodeManagerComposite
	{
	public:
		CNodeManagerComposite();
		virtual ~CNodeManagerComposite();

		/*!
		*	\brief	初始化
		*			 读取配置文件,外部调用首入口,之后调用StartWork开始提供服务
		*
		*	\param	OpcServer*	[in]	Server制针
		*	\retval	操作成功与否标志
		*			0成功	其它失败
		*/
		virtual int Init() = 0;

		/*!
		*	\brief	卸载
		*			内存对象的销毁,调用前先调用StopWork停止服务
		*	\param	无
		*	\retval	操作成功与否标志
		*			0成功	其它失败
		*/
		virtual int UnInit() = 0;

		/*!
		*	\brief	开始工作
		*			地址空间中节点的创建和实例化
		*			受内存和地址空间中节点数量级等限制,启动时可能会耗费较长时间才会进入正常服务状态
		*	\param	无
		*	\retval	操作成功与否标志
		*			0成功	其它失败
		*/
		virtual int StartWork() = 0;

		/*!
		*	\brief	停止工作
		*			停线程和服务,不再对外提供 OPC UA 相关服务
		*	\param	无
		*	\retval	操作成功与否标志
		*			0成功	其它失败
		*/
		virtual int StopWork() = 0;

		//服务是否已经启动
		virtual bool IsStarted() = 0;

		/*!
		*	\brief	设定FEP模块数据库单元CIONodeManager指针
		*			RTDB跟FEP合并在一起时该CIONodeManager由FEP创建
		*			两者分开部署时	不会调用该接口
		*	\param	pIONodeManager [in]: FEP模块数据库单元CIONodeManager指针
		*	\retval	无
		*/
		virtual void SetIONodeManager(CIONodeManager* pIONodeManager) = 0;

		/*!
		*	\brief	获取FEP模块数据库单元NodeManager指针
		*			RTDB向FEP添加监视项时需要获取该指针
		*			RTDB跟FEP合并在一起时该NodeManager由FEP创建
		*			两者分开部署时 RTDB模块自动创建一个默认的CIONodeManager
		*	\param	无
		*	\retval	CIONodeManager*	CIONodeManager指针
		*/
		virtual CIONodeManager*	GetIONodeManager() = 0;
	
		/*!
		*	\brief	获取本服务下的所有点或指定类型点的个数
		*							
		*	\param	std::string sTypeName[in]：类型名。当sTypeName=""时，获取的是所有点的个数
		*							
		*	\param	bool bChildType = true: 是否包含子类型
		*							
		*	\retval	点个数
		*/
		virtual int GetPointNum(std::string sTypeName,bool bChildType = true) = 0;	
		
		/*!
		*	\brief	加载工程文件
		*							
		*	\param	std::string sTypeName[in]：类型名。当sTypeName=""时，获取的是所有点的个数
		*							
		*	\param	bool bChildType = true: 是否包含子类型
		*							
		*	\retval	点个数
		*/
		virtual int LoadProjectCfg(void) = 0;			
	};

	extern DBBASECLASSES_API CNodeManagerComposite* g_pNodeManagerCompsite;
}

#endif
