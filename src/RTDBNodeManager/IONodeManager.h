/*!
 *	\file	IONodeManager.h
 *
 *	\brief	简短的文件说明（可选）
 *
 *	详细的文件说明（可选）
 *
 *	\author 王彦超
 *
 *	\date	2014年3月31日	15:20
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IONODEMANAGER_H_2014331_
#define _IONODEMANAGER_H_2014331_
#pragma once
#include "utilities/fepcommondef.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "ace/Thread.h"
#include "ace/Synch.h"

namespace MACS_SCADA_SUD 
{

	/*!
	*	\class	CIONodeManager
	*
	*	\brief	简短的类说明（可选）
	*
	*	详细的类说明（可选）
	*/
	class IONODEMANAGER_API CIONodeManager 
		:public ACE_Thread
	{
	public:
		CIONodeManager(const std::string& sNamespaceUri);
		virtual ~CIONodeManager();

		// NodeManager / NodeManagerUaNode overwrite
		//重载startUp，将节点管理器设置到CNodeManagerCompositeImpl
		// virtual Byte startUp(ServerManager*);
		//对象创建后,如果类型定义为FepId_ChannelType,则加入m_mapPorts
		// virtual void objectCreated(UaObject* pNewNode, UaBase::Object *pObject);

		virtual Byte   afterStartUp();
		virtual Byte   beforeShutDown();

		//- Interface IOManagerUaNode ----------------------------------------------------------
		// Byte beginWrite(OpcUa_Handle hIOManagerContext, unsigned int callbackHandle, VariableHandle* pVariableHandle, OpcUa_WriteValue* pWriteValue);
		// Byte writeValues(const CKiwiVariableArray& arrCKiwiVariables, const PDataValueArray& arrpDataValues, ByteCodeArray& arrStatusCodes);
		//- Interface IOManagerUaNode ----------------------------------------------------------

		void run();

		/*!
		*	\brief	重载父类接口，更新IO数据，同时通知驱动做必要的修改
		*
		*	详细的函数说明（可选）
		*
		*	\param	参数	对参数的说明
		*
		*	\retval	返回值	对该返回值的说明
		*
		*	\note		注意事项（可选）
		*/
		int UpdateCfg(std::string& sData);

		/*!
		*	\brief	创建通道，并向驱动调度发通知
		*
		*	详细的函数说明（可选）
		*
		*	\param	参数	对参数的说明
		*
		*	\retval	返回值	对该返回值的说明
		*
		*	\note		注意事项（可选）
		*/
		long CreatePort(long sNodeId, tIOPortCfg tPortCfg, std::string sDrvName, tIODrvCfg tDrvCfg);

		/*!
		*	\brief	创建UA链路对象，并添加到地址空间
		*
		*	当数据库单元的数据源不在本地时，用此接口来为每个数据源创建一个虚拟的链路
		*
		*	\param	sDriverName 链路名称，采用数据源站点名称，对应一个UADriver的驱动
		*
		*	\retval	Byte 返回执行状态码
		*/
		Byte CreatePort(std::string sDriverName);

		/*!
		*	\brief	创建UA设备对象，并添加到地址空间
		*
		*	当数据库单元的数据源不在本地时，用此接口来为每个数据库单元创建一个虚拟的设备，
		*  加入到相应的数据源虚拟链路下  
		*
		*	\param	sDriverName 链路名称，采用数据源站点名称，对应一个UADriver的驱动
		*	\param	sDeviceName 设备名称，采用数据库单元的名字
		*  \param  iDevAddr    设备地址，可以传入数据库单元Id作为设备地址
		*
		*	\retval	Byte 返回执行状态码
		*/
		Byte CreateUnit(std::string sDriverName, std::string sDeviceName, int iDevAddr);

		/*!
		*	\brief	在一个通道上添加设备，并向驱动调度发通知
		*
		*	详细的函数说明（可选）
		*
		*	\param	参数	对参数的说明
		*
		*	\retval	返回值	对该返回值的说明
		*
		*	\note		注意事项（可选）
		*/
		long AddUnit(long sNodeId, std::string sUnitName, tIOUnitCfg tCfg, long sPort);

		/*!
		*	\brief	在UaDriver上添加设备，并向驱动调度发通知
		*
		*	详细的函数说明（可选）
		*
		*	\param	参数	对参数的说明
		*
		*	\retval	返回值	对该返回值的说明
		*
		*	\note		注意事项（可选）
		*/
		//long AddUnit(ServerInfo tCfg, long sPort);

		/*!
		*	\brief	在一个设备上添加数据变量，并向驱动调度发通知
		*
		*	\param	pTag 数据变量指针
		*
		*	\retval	true为成功，false为失败
		*
		*	\note		注意事项（可选）
		*/
		bool AddTag(CKiwiVariable* pTag);

		/*!
		*	\brief	在一个设备上删除数据变量，并向驱动调度发通知
		*
		*	\param	pTag 数据变量指针
		*
		*	\retval	true为成功，false为失败
		*/
		bool DeleteTag(CKiwiVariable* pTag);

		/*!
		*	\brief	向Ua设备中添加数据变量，并向驱动调度发通知
		*
		*	详细的函数说明（可选）
		*
		*	\param	sDriverName	驱动名称
		*	\param	sDeviceName 设备名称
		*	\param	pTag	    数据变量指针
		*
		*	\retval	成功，true；失败，false
		*/
		bool AddTag(std::string sDriverName, std::string sDeviceName, CKiwiVariable* pTag);

		/*!
		*	\brief	删除UA设备中的标签并向驱动调度发通知
		*
		*	详细的函数说明（可选）
		*
		*	\param	sDriverName	驱动名称
		*	\param	sDeviceName 设备名称
		*	\param	pTag	    数据变量指针
		*
		*	\retval	成功，true；失败，false
		*/
		bool DeleteTag(std::string sDriverName, std::string sDeviceName, CKiwiVariable* pTag);

		/*!
		*	\brief	获取通道列表
		*
		*	\param	mapPorts  出参，将本类的通道map表传出	
		*/
		void GetAllIOPorts(MAP_PORT& mapPorts);

		//获得类型命名空间
		static short getTypeNamespace();

		/*!
		*	\brief	添加通道
		*
		*	\param	pIOPort	通道指针
		*	\param	bOnline 是否在线添加，true：在线添加，false：普通添加
		*
		*	\retval	成功，true；失败，false
		*/	
		bool AddPort(CIOPort* pIOPort, bool bOnline = false);
		/*!
		*	\brief	删除通道
		*
		*	\param	PortId	通道的节点Id
		*	\param	bOnline 是否在线删除，true：在线删除，false：普通删除
		*
		*	\retval	成功，true；失败，false
		*/
		bool DelPort(const long& PortId, bool bOnline = false);

		//根据链路名称，返回链路指针，不存在，则返回空
		CIOPort* GetPort(std::string sName);

		//!获取IOPort和IOUnit中需要进行进入热数据的变量信息
		int GetNeedBackupVar(std::vector<CKiwiVariable*>& vecHotVariable);

	private:

	protected:
		static unsigned short s_typenamespaceIndex; //fep类型节点管理器索引（暂定方案）

	private:
		MAP_PORTID	m_mapPortIDs;
		MAP_PORT    m_mapPorts;	//!	通道map表

		ACE_Thread_Mutex m_mapPortslock;
		//UaMutex  m_mapPortslock;//! 通道map表互斥访问锁
	};

}// End namespace 
#endif


