/*!
*	\file	AreaDeviceParse.h
*
*	\brief	节点间所属区域和设备解析类
*			区域报警注册
*			脚本解析
*			分为两个子类处理，Ref类正向解析，Reverse类反向解析
*
*			区域分三级，一级包含二级，二级包含三级
*			区域下有设备，但是设备下不可以有区域
*			设备下可以有子设备
*			层级关系可以如下：
*
*			数据库单元文件夹
*					——Area1
*						——Area2
*							——Area3
*								——EQ1
*									——EQ2
*										——EQ3
*											——AI001
*			
*			那么AI001所属区域就为Area123，所属设备为EQ3
*
*			$AREA可以嵌套，即对过程系统分级。
*			$AREA以Organizes引用类型组织各种组件对象、$EQ或子区域
*
*			$EQ可以聚合其它的$EQ类装置作为自己的部件
*			$EQ以HasComponent引用组合各种组件对象或$EQ的子类型对象
*
*
*			另外针对Area还需要做特殊处理，即报警统计功能，需要向Area下挂载的所有对象注册。
*			当Area对象创建时，遍历其ReferenceList找到每个挂载对象。
*			此时需要分情况考虑：
*				A）对象之前创建过了，直接注册
*				B）对象尚未创建，先将该对象NodeId记录到map表中，待到objectCreated调用时，
*					先判定该对象是否在map表中，如果存在，从ReferenceList表中找到Area父对象执行注册
*
*			实时库节点管理器类辅助类
*			
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/

#ifndef __AREADEVICEPARSE_H__
#define __AREADEVICEPARSE_H__

#include "basenodes.h"
#include "uabasenodes.h"
#include "RTDB/Server/DBPlatformClass/BaUserData.h"
#include "RTDB/Server/DBPlatformClass/ComponentType.h"
#include "RTDB/Server/DBPlatformClass/CallBackVariable.h"
#include "IOTypeNodeManager.h"

#include <map>
using namespace std;

namespace MACS_SCADA_SUD
{
	/*!
	*	\brief	区域节点的层次关系
	*			最多有3级区域
	*			level标注当前节点是第几级区域
	*			parentNode标注其上层区域节点的NodeId
	*			bSetOver标注该节点Info信息是否标注完成
	*/
	typedef struct AreaLevelInfo
	{
		OpcUa_UInt16	level;
		UaNodeId		parentNode;
		OpcUa_Boolean	bSetOver;
	};

	/*!
	*	\brief	普通变量节点的区域和设备节点信息
	*			AreaNodeId标注当前节点所挂载的区域节点NodeId
	*			DevNodeId标注当前节点所挂载的设备节点NodeId
	*			如果想进一步确定当前节点的上级区域信息,
	*			根据AreaNodeId在区域级别map表中定位即可
	*/
	typedef struct AreaDevInfo
	{
		UaNodeId		AreaNodeId;
		UaNodeId		DevNodeId;
	};

	/*!
	*	\brief	普通变量节点的三级区域和设备节点信息
	*/
	typedef struct ADInfo
	{
		UaNodeId		Area1;
		UaNodeId		Area2;
		UaNodeId		Area3;
		//UaNodeId		Dev;
	};

	class CAreaDeviceParse
	{
	public:
		CAreaDeviceParse();
		virtual ~CAreaDeviceParse();

		void SetIndex(OpcUa_UInt16 nsIndex);

		/*!
		*	\brief	给定节点类型NodeId,判定是否为AreaType类型
		*			
		*	\param	typeId	[in]	节点类型NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsAreaType(UaNodeId typeId);

		/*!
		*	\brief	给定节点类型NodeId,判定是否为FolderType类型
		*			
		*	\param	typeId	[in]	节点类型NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsFolderType(UaNodeId typeId);

		/*!
		*	\brief	给定节点类型NodeId,判定是否为EQType类型
		*			
		*	\param	typeId	[in]	节点类型NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsEQType(UaNodeId typeId);

		/*!
		*	\brief	给定节点类型NodeId,判定是否为基础类库基本变量类型
		*			
		*	\param	typeId	[in]	节点类型NodeId
		*	\retval	OpcUa_Boolean
		*/
		virtual OpcUa_Boolean IsPointType(UaNodeId NodeId);

		/*!
		*	\brief	object对象创建时, 调用此接口解析其区域和设备信息关系
		*
		*	\param	pNode	[in]			节点变量
		*	\param	pObject	[in]			节点
		*	\retval	无
		*/
		virtual void parseNodeAreaDeviceInfo(UaObject* pNode, UaBase::Object* pObject);

		
		/*!
		*	\brief	当所有节点解析完毕后 统一统计基础类库点的三级区域和设备信息
		*
		*	\retval	无
		*/
		virtual void SetNodeAreaAndDevInfo();

		/*!
		*	\brief	获取EQ节点所挂载的Area节点NodeId
		*
		*	\param	EQNodeId	[in]		EQ节点NodeId
		*	\param	nodeId		[in,out]	Area节点NodeId
		*	\retval	OpcUa_Boolean					查找是否成功标志
		*/
		virtual OpcUa_Boolean getEQNodeInArea(UaNodeId EQNodeId, UaNodeId& nodeId);

		/*!
		*	\brief	给定一个变量节点的直接关联的Area区域节点NodeId,获取其3级Area节点NodeId信息
		*
		*	\param	nodeId	[in]			变量节点所在的最底层区域节点NodeId
		*	\param	pInfo	[in,out]		变量节点的3级区域和设备节点NodeId信息集合
		*	\retval	无
		*/
		virtual void setAreaInfo(UaNodeId nodeId, ADInfo* pInfo);
		virtual ADInfo* getAreaInfo(UaNodeId nodeId);
		virtual void setAllAreaInfo();

		////////////////////////////////////以下部分为Area报警统计注册功能服务//////////////////////////////////////
		UaNode* FindTargetNode(UaNodeId nodeId);


		////////////////////////////////////以下部分为Method方法脚本功能服务//////////////////////////////////////
		virtual void DealMethodCreated(UaMethod* pNewNode, UaBase::Method *pMethod);
		virtual void DealAllMethodScript();
		virtual void ChangeAllScript();
		virtual void ChangeScript(UaMethod* pMethod, ScriptRegion* pRegion);
		virtual void DealAllUserScript();
		virtual void CreateValueChange();
		virtual void GetTypeChangeSources(UaNodeId sourceId, UaString sType, UaString sSuffix, OpcUa_Boolean bInclude, vector<UaNodeId> &vecSources);
		virtual void DealWithTypeChangeSources(UaString sSource, ScriptRegion* pRegion);
		virtual void AddChangeVariable(UaNodeId sourceNode, ScriptRegion* pScriptRegion);
		virtual void AddUnitChangeVariables();

		//! 获取节点的父对象节点NodeId
		OpcUa_Boolean FindParentNode(UaBase::BaseNode* pNode, UaNodeId &nodeId);

		//! 处理扩展模板类方法中的值变化源通配符实例类
		virtual void DealWithExtendObjectMethod(ComponentType* pPoint);

		////////////////////////////////////以下部分为区域设备基础类库点创建事件通知树功能服务//////////////////////////////////////
		virtual void MonitorEventTree(UaNodeId source, UaNodeId target);
		virtual void MonitorDBUnitFolder(UaBase::Object* pObject);
		virtual void GetAllEventTree(map<UaNodeId, vector<UaNodeId> >& treeMap);
		virtual void ClearResources();

	protected:

		OpcUa_UInt16							m_iNamespaceIndex;

		/*!
		*	\brief	以下为3级区域map列表 
		*			其中key为AreaType节点或者FolderType节点的NodeId, 
		*			value为当前节点的区域级别和其上级区域节点的NodeId
		*/
		map<UaNodeId, AreaLevelInfo*>			m_mapArea;


		/*!
		*	\brief	遍历m_mapArea区域节点map表，给每个Area节点的3级区域做统计
		*			方便给每个基础类库点做设置
		*/
		map<UaNodeId, ADInfo*>					m_mapAreaWhole;

		/*
		*	\brief	设备节点的层级关系
		*			key为设备节点的NodeId
		*			value为设备节点上层节点的NodeId
		*			上层节点可以是EQ\Area\Folder
		*/
		map<UaNodeId, UaNodeId>					m_mapEQ;

		map<UaNodeId, UaNodeId>					m_mapNodeInArea;
		map<UaNodeId, UaNodeId>					m_mapNodeInEQ;

		vector<ScriptRegion*>					m_vecEndScripts;

		map<UaNodeId, vector<UaNodeId> >		m_mapEventReferences;
		map<UaNodeId, CCallBackVariable*>		m_mapChangeSources;

		vector<UaMethod*>						m_vecMethodWithScript;
		static IOTypeNodeManager*				s_pIOTypeNodeManager;
	};

}
#endif