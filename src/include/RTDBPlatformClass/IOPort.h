/*!
 *	\file	IOPort.h
 *
 *	\brief	通道类
 *
 *
 *	\author lingling.li
 *
 *	\date	2014年4月11日	11:18
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef __IOPort_H__
#define __IOPort_H__

#include "Utilities/fepcommondef.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "FepObjectType.h"
#include "RTDBCommon/KiwiVariable.h"
#include "ace/Synch.h"

#ifndef FEPPLATFORMCLASS_API
#ifdef FEPPLATFORMCLASS_EXPORTS
# define FEPPLATFORMCLASS_API __declspec(dllexport)
#else
# define FEPPLATFORMCLASS_API __declspec(dllimport)
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD {

class CIOUnit;


class FEPPLATFORMCLASS_API CIOPort:
    public CFepObjectType
{
protected:
    virtual ~CIOPort();
public:
    // construction / destruction
    CIOPort(long nNodeID, std::string sDriverName);

    static void createTypes();

    static void clearStaticMembers();


	//! 设置属性指针，由于目前采用的构造方式不在类的构造函数中创建属性对象，因此需要
	//! 增加该接口来记录属性对象的指针。
	//virtual void SetItemPointer(UaQualifiedName& strItem, UaNode* pNode, NodeManagerConfig* pNodeConfig);
	//! 获取静态属性指针，方便复用类型的基本属性
	//! 增加该接口来获取静态属性指针
	//virtual UaNode* GetStaticItem(UaQualifiedName& strItem);

	//发送命令
	int sendCmd(
		//const ServiceContext& serviceContext,
		Byte& byCmdVal,
		std::string& bysCmdParam,
		int& result);
	///设置、获取IO端口配置
	void setIOPortCfg(const tIOPortCfg& IOPortCfg);
	void getIOPortCfg(tIOPortCfg& IOPortCfg) const;

	///设置、获取驱动名称
	void setDrvName(const std::string& strDrvName);

	std::string getDrvName() const;

	/*!
	 *	\brief	通道是否正常
	 *
	 *	\retval	true:正常, false:不正常
	 */
	bool IsOK();

	/*!
	 *	\brief	设备是否被模拟
	 *
	 *	\retval	是否处于模拟状态
	 */
	bool IsSimulate();

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
	 *	\brief	通道的当前主从状态。1为主，0为从
	 *
	 *	\param	byState	通道的主从状态
	 *
	 *	\retval	0表示成功，-1表示参数错误
	 */
	int SetRealPrmScnd(Byte byState);


	 /*!
	 *	\brief	根据双机的真实weight值，刷新链路状态
	 */
	void SetRealPrmScnd();
	/*!
	 *	\brief	通道的当前主从状态。1为主，0为从
	 *
	 *	\retval	通道的主从状态
	 */
	Byte GetRealPrmScnd();


	/*!
	 *	\brief	得到通道默认主从
	 *
	 *	\retval 通道默认主从
	 */
	Byte GetDefaultPrmScnd();

	/*!
	 *	\brief	设置通道通讯状态
	 *
	 *	\param	isOK	通道通信状态 = true;
	 */
	void SetCommState(bool isOK);

	/*!
	 *	\brief	开启通道模拟
	 *
	 *	\param	simMode	模拟类型：正弦、递增、随机等，具体见SimulateType
	 */
	void SimulateChannel(Byte simMode);
	/*!
	 *	\brief	停止通道模拟
	 */
	void StopSimulate();

	/*!
	 *	\brief	通道禁止
	 */
	void DisableChannel();

	/*!
	 *	\brief	解除通道禁止
	 */
	void EnableChannel();

	/*!
	 *	\brief	强制通道主从
	 *
	 *	\param	PrmOrScnd	强制的主从状态，1为强制主，0为强制
	 */
	void ForceChannel(Byte PrmOrScnd);

	/*!
	 *	\brief	解除通道强制
	 */
	void DeforceChannel();

	/*!
	 *	\brief	得到链路的强制主从
	 */
	Byte GetForcePrmScnd();

	/*!
	 *	\brief	获取通道权值
	 *
	 *	\retval	通道权值
	 */
	int GetIOChannelWeight();

	/*!
	 *	\brief	设置通道权值
	 *
	 *	\param	iWeight	通道权值
	 */
	void SetIOChannelWeight(int iWeight);

	/*!
	 *	\brief	设置是否记录通信日志
	 *
	 *	\param	bRecord	是否记录通信日志
	 */
	void SetRecord(bool bRecord);

	/*!
	 *	\brief	是否记录通信日志
	 *
	 *	\retval	true表示记录，false表示不记录
	 */
	bool IsRecord();

	/*!
	 *	\brief	设置是否进行通信监视
	 *
	 *	\param	bRecord	是否进行通信监视
	 */
	void SetMonitor(bool bMonitor);

	/*!
	 *	\brief	是否进行通信监视
	 *
	 *	\retval	true表示监视，false表示不监视
	 */
	bool IsMonitor();

	/*!
	 *	\brief	是否为双机配置
	 */
	bool IsDouble();

	/*!
	 *	\brief	通过设备名获取设备对象指针
	 *
	 *	详细的函数说明（可选）
	 *
	 *	\param	strUnitName	设备名
	 *
	 *	\retval		设备指针
	 */
	CIOUnit* GetUnitByName(std::string strUnitName);

	/*!
	 *	\brief	获取通道下的设备个数
	 *
	 *	详细的函数说明（可选）
	 *
	 *	\param	参数	对参数的说明
	 *
	 *	\retval	返回值	设备个数
	 *
	 *	\note		注意事项（可选）
	 */
	int GetUnitCount();

	/*!
	 *	\brief	通过设备在该通道下的索引号获取设备。
	 *
	 *	\param	iIndex	索引号
	 *
	 *	\retval	返回值	设备对象指针
	 */
	CIOUnit* GetUnitByIndex(int iIndex);

	/*!
	 *	\brief	将新增设备加入设备列表，并建立引用
	 *
	 *	\param	strUnitNmae	设备名
	 *	\param	pUnit  设备节点指针
	 *
	 *	\retval	返回值	是否成功
	 */
	int AddUnit(std::string strUnitName, CIOUnit* pUnit);

	 	/*!
	 *	\brief	删除设备
	 *
	 *	\param	nodeId	设备节点Id
	 *
	 *	\retval	返回值	是否成功
	 */
	int DelUnit(std::string szNode);
	/*!
	 *	\brief	获取通道下的所有变量
	 *
	 *	\param	vecTags	变量数组
	 *
	 *	\retval	返回值	0：正常；其他：异常
	 */
	void GetTag(std::vector<CKiwiVariable*> &vecTags);

	/*!
	 *	\brief	是否处于被升主或降从状态
	 *
	 *	\retval	bool
	 */
	bool  isInPassiveState();

	/*!
	 *	\brief	设置是否处于被升主或降从状态
	 *
	 *	\retval	bool 状态
	 */
	void SetPassiveState(bool bState);

	//!被动升主或降从
	void PassiveSPrimScnd(Byte state);

	//!从被动升主或降从中恢复
	void RecorvePassiveState();


	/*!
	 *	\brief	恢复Port的默认主从状态
	 *
	 *	\retval	OpcUa::BaseDataVariableType*
	 */
	void RecoverDefaultPrmScnd();

	//!设置另机权重信息
	void PutPeerPrmScndWeight(int iWeight);

	//!获取另机权重信息
	int GetPeerPrmScndWeight();

	//!设置链路切换状态
	void SetIOPortSwitchStatus(IOPortSwitchStatus flag);

	//!获取该链路下以及该设备下需要进行热数据的Variable
	int GetNeedBackupVar( std::vector<CKiwiVariable*>& vecHotVariable );

	//!获取链路的模拟方式
	Byte GetSimMode();

	void SetPeerDevComm(bool isOnLine );

	bool GetConnect2Peer();

	void SetConnect2Peer(bool flag);
protected:
	std::map<std::string , CIOUnit*> m_mapIOUnits;
	ACE_Thread_Mutex m_mapIOUnitsLock;

   // Variable nodes
	// Variable RealPrmScnd	通道主从状态
	Byte m_byRealPrmScndBak; //设置强制主从之前的实际主从状态

	ACE_Thread_Mutex m_RealPrmScndLock;

	// Variable IsOK	通道好坏
	// OpcUa::BaseDataVariableType*  m_pIsOK;
	bool				m_bIsOK;
	// // Variable IsSimulate	通道是否处于模拟状态
	// OpcUa::BaseDataVariableType*  m_pIsSimulate;
	bool				m_bIsSimulate;
	// // Variable SimMode 模拟算法类型:正弦、递增、随机等
	// OpcUa::BaseDataVariableType*  m_pSimMode;
	unsigned char		m_bySimMode;
	// // Variable IsForce	通道是否被强制
	// OpcUa::BaseDataVariableType*  m_pIsForce;

	// // Variable Weight	通道权值
	// OpcUa::BaseDataVariableType*  m_pWeight;

	// // Variable PeerWeight	另机通道权值
	// OpcUa::BaseDataVariableType*  m_pPeerWeight;

	// // Variable IsMonitor 是否通信监视
	// OpcUa::BaseDataVariableType*  m_pIsMonitor;
	bool			m_bIsMonitor;
	// // Variable IsRecord 是否记录通道日志和事件
	// OpcUa::BaseDataVariableType*  m_pIsRecord;

	// // Variable DrvName 驱动名，即协议类型
	// OpcUa::PropertyType*  m_pDrvName;
	std::string		m_szDrvName;
	// // Variable Type 链路类型
	// OpcUa::PropertyType*  m_pType;
	std::string			m_szType;
	// // Variable Port 端口号
	// OpcUa::PropertyType*  m_pPort;
	unsigned int			m_unPort;
	// // Variable BaudRate 波特率
	// OpcUa::PropertyType*  m_pBaudRate;
	unsigned int			m_unBoadRate;
	// // Variable DataBits 数据位
	// OpcUa::PropertyType*  m_pDataBits;
	unsigned int			m_unDataBits;
	// // Variable StopBits 停止位
	// OpcUa::PropertyType*  m_pStopBits;
	unsigned int			m_unStopBits;
	// // Variable Parity 奇偶校验 0无校验，1偶校验，2奇校验
	// OpcUa::PropertyType*  m_pParity;
	unsigned int			m_unParity;
	// // Variable DefaultPrmScnd 默认主从
	// OpcUa::PropertyType*  m_pDefaultPrmScnd;

	// // Variable PortAccessMode 通道访问模式：1冲突；0不冲突
	// OpcUa::PropertyType*  m_pPortAccessMode;

	// // Variable SwitchAlgName 主从切换算法名称
	// OpcUa::PropertyType*  m_pSwitchAlgName;

	// // Variable Option 扩展配置
	// OpcUa::PropertyType*  m_pOption;
	Byte*				m_pOption;

	// // Variable IPA_A A设备的A网IP地址
	// OpcUa::PropertyType*  m_pIP;
	std::string			m_szIP;
protected:

private:
	/*!
	 *	\brief	初始化链路的子节点
	 *
	 *	\param	bCreateProp	是否需要创建并初始化特性
	 *
	 */
    void initialize(bool bCreateProp = true);

	/*!
	 *	\brief 设置链路下的所有设备的禁止状态
	 *
	 *	\param	bDisable	是否禁止
	 *
	 */
    void SetUnitDisableState(bool bDisable);

private:
    static bool s_typeNodesCreated;

	//本机Port状态是否处于被升主或降从状态
	bool m_bIsInPassvie;

	int m_PeerPrmScndWeight;

	bool m_Connect2PeerStatus;
};

} // End namespace MACS_SCADA_SUD

#endif // #ifndef __IOPort_H__

