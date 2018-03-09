/*!
 *	\file	IOUnit.h
 *
 *	\brief	设备类
 *
 *
 *	\author lingling.li
 *
 *	\date	2014年4月14日	16:27
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */


#ifndef __IOUnit_H__
#define __IOUnit_H__

#include "RTDBPlatformClass/fep_identifiers.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "RTDBPlatformClass/FepObjectType.h"
#include "RTDBCommon/KiwiVariable.h"

#ifndef FEPPLATFORMCLASS_API
#ifdef FEPPLATFORMCLASS_EXPORTS
# define FEPPLATFORMCLASS_API __declspec(dllexport)
#else
# define FEPPLATFORMCLASS_API __declspec(dllimport)
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD 
{
	class CIODeviceBase;
	class CIODeviceSimBase;

	class FEPPLATFORMCLASS_API CIOUnit:
		public CFepObjectType
	{
	protected:
		virtual ~CIOUnit();
	public:
		// construction / destruction
		CIOUnit(const long& nId);
		CIOUnit(const long& nodeId, const std::string& name);
		CIOUnit(const long& nodeId, const std::string& szUnit, const std::string& szDevice);

		static void createTypes();
		static void clearStaticMembers();

		virtual long  typeDefinitionId() const;
		//设置设备所属的通道
		void setIOPort(CIOPort* pIOPort);
		//添加数据变量，可能为IO变量、PV变量或者内部变量，由实时库调用
		bool addTag(CKiwiVariable* pTag);
		//删除数据变量，可能为IO变量、PV变量或者内部变量，由实时库调用
		bool DeleteTag(CKiwiVariable* pTag);
		//获取数据变量，可能为IO变量、PV变量或者内部变量
		void GetAllTags(std::vector<CKiwiVariable*> &vecTags);

		std::string GetUnitName(void);
		/*!
		*	\brief	获取设备地址
		*
		*	\retval	设备地址
		*/
		int GetDevAddr();

		/*!
		*	\brief	设置设备地址
		*
		*	\param	iDeviceAddr 设备地址
		*/
		void SetDevAddr(int iDeviceAddr);

		/*!
		*	\brief	获取设备配置文件名
		*
		*	\retval	设备配置文件名
		*/
		std::string GetDevCfgFileName();

		/*!
		*	\brief	设备是否在线
		*
		*	\retval	在线状态，true表示在线，false表示不在线
		*/
		bool IsOnline();

		/*!
		*	\brief	设备是否离线
		*
		*	\retval	在线状态，true表示在线，false表示不在线
		*/
		bool IsOffLine();

		/*!
		*	\brief	获取设备运行状态
		*
		*	\retval	运行状态，初始:0，正常1，离线:2
		*/
		Byte GetRunState();

		/*!
		*	\brief	设备是否被模拟
		*
		*	\retval	是否处于模拟状态
		*/
		bool IsSimulate();

		//!设置链路的切换状态
		void SetIOPortSwitchStatus(IOPortSwitchStatus flag);

		//!得到链路切换状态
		IOPortSwitchStatus GetIOPortSwitchStatus();

		/*!
		*	\brief	设备是否被模拟
		*
		*	\retval	是否处于模拟状态
		*/
		void SetIODeviceSim(CIODeviceSimBase* pDeviceSim);
		
		/*!
		*	\brief	设备是否被模拟
		*
		*	\retval	是否处于模拟状态
		*/
		CIODeviceSimBase* GetIODeviceSim();

		//!获取IOUnit下需要进入热数据的Variable
		int GetNeedBackUpVar( std::vector<CKiwiVariable*>& vecHotVariable );
		/*!
		*	\brief	开启设备模拟
		*
		*	\param	simMode	模拟类型：正弦、递增、随机等，具体见SimulateType
		*/
		void StartSimulate(Byte simMode);
		/*!
		*	\brief	停止设备模拟
		*/
		void StopSimulate();

		/*!
		*	\brief	设备是否被禁止
		*
		*	\retval	返回值	是否是能
		*/
		bool IsDisable();

		/*!
		*	\brief	设备是否被强制
		*
		*	\retval	返回值	是否被强制
		*/
		bool IsForce();

		/*!
		*	\brief	设备主从状态
		*
		*	\retval	返回值	主从状态，1为主，0为从
		*/
		Byte PrmOrScnd();

		/*!
		*	\brief	设备的当前主从状态。1为主，0为从
		*
		*	\param	byState	设备的主从状态
		*
		*	\retval	0表示成功，-1表示参数错误
		*/
		int SetRealPrmScnd(Byte byState);

		/*!
		*	\brief	设置设备离线
		*	\param	bChangeW 是否改变链路权重
		*/
		void SetOnline(bool bChangeW = true);

		/*!
		*	\brief	设置设备离线
		*	\param	bChangeW 是否改变链路权重
		*/
		void SetOffline(bool bChangeW = true);

		/*!
		*	\brief	禁止设备
		*/
		void DisableUnit();

		/*!
		*	\brief	解除设备禁止
		*/
		void EnableUnit();

		/*!
		*	\brief	强制设备的主从状态
		*
		*	\param	PrmScnd	强制主从状态，1为强制主，0为强制
		*/
		void ForceUnit(Byte PrmScnd);

		/*!
		*	\brief	解除强制
		*/
		void DeforceUnit();

		/*!
		*	\brief	设置超量程
		*
		*	\param	bExtraRange	是否超量程
		*/
		void SetExtraRange(bool bExtraRange);

		/*!
		*	\brief	是否超量程
		*
		*	\retval	是否超量程
		*/
		bool GetExtraRange();

		/*!
		*	\brief	获取设备所属通道的指针
		*
		*	\retval	设备所属的通道指针
		*/
		CIOPort* GetIOPort();

		/*!
		*	\brief	通过节点号获取数据变量对象指针
		*
		*	\param	strNodeId	数据变量节点Id
		*
		*	\retval	返回值	数据变量
		*/
		CKiwiVariable* GetDataVariableByNodeId(long strNodeId);

		/*!
		*	\brief	获取设备下数据变量个数
		*
		*	\retval	数据变量个数
		*/
		unsigned int GetVariableCount();

		/*!
		*	\brief	通过数据变量在map表中的索引访问变量
		*
		*	\param	iIndex	数据变量索引号
		*
		*	\retval	 数据变量指针
		*/
		CKiwiVariable* GetVariableByIndex(unsigned int iIndex);

			//!获取本机的设备通讯状态点通道地址
		std::string GetLocalDevCommType();

		//!获取对机的设备通讯状态点通道地址
		std::string GetPeerDevCommType();

		//!通过标签地址判断是不是DevComm结点
		bool IsDevCommFieldPoint(const std::string &offSet);

		//!更新通信状态点
		void SetPeerDevComm(bool isOnLine);

		//!获取设备地址(string)
		std::string GetStrDevAddr();

		/*!
		*	\brief 设置链路下的所有标签的在线状态
		*
		*	\param	isOnLine	是否在线
		*
		*/
		void SetTagOnLineState(bool isOnLine, long dateTime);

	protected:
		//发送命令
		int sendCmd(
			//const ServiceContext& serviceContext,
			Byte& byCmdVal,
			std::string& bysCmdParam,
			int& result);

	private:
		/*!
		*	\brief	初始化设备的子节点
		*
		*	\param	bCreateProp	是否需要创建并初始化特性
		*
		*/
		void initialize(bool bCreateProp = true);

		/*!
		*	\brief 设置链路下的所有标签的禁止状态
		*
		*	\param	bDisable	是否禁止
		*
		*/
		void SetTagDisableState(bool bDisable);

	protected:
		Byte	m_bSimulate;
		Byte	m_bOnline;
		int		m_iDeviceAddr;
		///所属通道指针
		CIOPort* m_pIOPort;
		//数据变量列表，可能为IO变量、PV变量或者内部变量
		std::map<long, CKiwiVariable*> m_mapDataVariables;

		std::string szUnitName;

	public:
		std::map<std::string, FieldPointType*> m_mapStrAddTag;

	private:
		//!设备所属的链路主备转化标记
		IOPortSwitchStatus m_SwitchStatus;
		//!模式设备对象指针
		CIODeviceSimBase* m_pDeviceSim;
	};
}

#endif

