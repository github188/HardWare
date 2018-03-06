/*!
*	\file	NodeManagerCompositeImpl.h
*
*	\brief	根节点管理器实现类 
*			实现根节点管理器的各对外接口
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __NODEMANAGERCOMPOSITEIMPL_H__
#define __NODEMANAGERCOMPOSITEIMPL_H__

#include <map>
#include <vector>
#include "RTDBCommon/datavalue.h"
#include "NodeManagerGeneral.h"
#include "RTDBBaseClasses/NodeManagerComposite.h"

using namespace std;

namespace MACS_SCADA_SUD
{
	class CIONodeManager;
	/*!
	*	\class	CNodeManagerCompositeImpl
	*
	*	\brief	根节点管理器实现类
	*/
	class IONODEMANAGER_DECL CNodeManagerCompositeImpl
		:public CNodeManagerComposite		
	{
	public:
		CNodeManagerCompositeImpl();
		virtual ~CNodeManagerCompositeImpl();

		/*!
		*	\brief	 初始化
		*			 显式创建的为NodeManager; 
		*			 xml解析式为Module,不过会创建对应的NodeManager
		*
		*	\retval	 操作成功与否标志
		*			 0成功	其它失败
		*/
		int Init();

		/*!
		*	\brief	 卸载
		*			 地址空间中实时点的本地序列化
		*			 各内存指针的回收
		*	\param	 无
		*	\retval	 操作成功与否标志
		*			 0成功	其它失败
		*/
		int UnInit();

		/*!
		*	\brief	 开始工作 
		*			 启动配置更新线程
		*			 启动各NodeManager和Module创建节点
		*			 对外提供Opc UA服务
		*	\param	 无
		*	\retval	 操作成功与否标志
		*			 0成功	其它失败
		*/
		int StartWork();

		/*!
		*	\brief	 停止工作 
		*			 停止配置更新线程
		*			 对外停止提供Opc UA服务
		*	\param	 无
		*	\retval	 操作成功与否标志
		*			 0成功	其它失败
		*/
		int StopWork();

		//服务是否已经启动
		bool IsStarted();

		/*!
		*	\brief	设定FEP模块数据库单元CIONodeManager指针
		*			RTDB跟FEP合并在一起时该CIONodeManager由FEP创建
		*			两者分开部署时	不会调用该接口
		*	\param	pIONodeManager [in]: FEP模块数据库单元CIONodeManager指针
		*	\retval	无
		*/
		void SetIONodeManager(CIONodeManager* pIONodeManager);

		/*!
		*	\brief	获取FEP模块数据库单元NodeManager指针
		*			RTDB向FEP添加监视项时需要获取该指针
		*			RTDB跟FEP合并在一起时该NodeManager由FEP创建
		*			两者分开部署时 RTDB模块自动创建一个默认的CIONodeManager
		*	\param	无
		*	\retval	CIONodeManager*	CIONodeManager指针
		*/
		CIONodeManager*	GetIONodeManager();
		
		/*!
		*	\brief	判断点是否属于指定的类型
		*							
		*	\param	std::string sPointNodeId[in]：点的NodeId。
		*							
		*	\param	std::string sClassName[in]：类型名。
		*							
		*	\param	bool bFindBase[in]：是否查找基类的类型。
		*							
		*	\retval	设置结果。0：成功；-1：失败；-2：参数错误
		*/
		int IsKindOfPointClass(std::string sPointNodeId,std::string sClassName,bool bFindBase = true);
		
		/*!
		*	\brief	获取本服务下的所有点或指定类型点的个数
		*							
		*	\param	std::string sTypeName[in]：类型名。当sTypeName=""时，获取的是所有点的个数
		*							
		*	\param	bool bChildType = true: 是否包含子类型
		*							
		*	\retval	点个数
		*/
		virtual int GetPointNum(std::string sTypeName,bool bChildType = true);		
		/*!
		*	\brief	 加载本地工程配置
		*/
		virtual int LoadProjectCfg();
	private:

		/*!
		*	\brief	根据ServerConfig.xml中配置的命名空间，
		*			创建Xml解析器对象,并添加到m_pServer中
		*/
		bool createXmlParserModule();

		void DealWithConfigValues(const CKiwiVariant& nodes, const CKiwiVariant& nodevalues);
		void SetConfigLatestValue(const CKiwiVariant& nodes, const CKiwiVariant& nodevalues, bool bTimeCompare = false);
		
	private:
		std::string							m_sRunDir;

		//! 节点管理器
		CIONodeManager*                    m_pIONodeManager;

		std::map<std::string, void *> 		m_mapFunAndName;
	};
}

#endif
