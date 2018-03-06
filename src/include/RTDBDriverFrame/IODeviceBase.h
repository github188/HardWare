/*!
 *	\file	IODeviceBase.h
 *
 *	\brief	设备基类
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月14日	19:06
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODEVICEBASE_H_
#define _IODEVICEBASE_H_

#include <map>
#include "RTDBDriverFrame/IODriverH.h"
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIOCfgTag;
	class CIOUnit;
	class CIOCfgUnit;
	class CIODeviceSimBase;
	class FieldPointType;
	class CIODriverBase;
	class CIOPrtclParserBase;
	

	
	//!定义设备下的标签MAP
	typedef std::map<long long, CIOCfgTag*> MAP_OBJTAG;

	//!定义设备下的字符窜通道的标签MAP
	typedef std::map<std::string, CIOCfgTag*> STRMAP_OBJTAG;

	/*!
	 *	\class	CIODeviceBase
	 *
	 *	\brief	设备基类
	 *
	 *	详细的类说明（可选）
	 */
	class IODRIVERFRAME_API CIODeviceBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIODeviceBase(void);

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIODeviceBase(void);

		//!获取类名
		std::string GetClassName();
		
		/*!
		 *	\brief	设备标签初始化,建立设备对象中包含FieldPoint索引表
		 *
		 *	\retval	0为成功，否则失败
		 *
		 *	\note	注意事项（可选）
		 */
		virtual int Init();

		/*!
		 *	\brief	设置设备为模拟状态
		 *
		 *	\param	Byte bySimMode	模拟模式:Random,Increase,Sine
		 *
		 *	\retval	0为成功，否则失败
		 *
		 *	\note	注意事项（可选）
		 */
		virtual void StartSimulate(Byte bySimulate);

		/*!
		 *	\brief	停止模拟
		 */
		virtual void StopSimulate(void);

		/*!
		 *	\brief	设备是否处于模拟状态
		 *
		 *	\retval	true = 模拟,false = 正常
		 */
		virtual bool IsSimulate(void);

		/*!
		 *	\brief	设置标签值
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	Byte byUNIT_TYPE,	标签UNITYTYPE
		 *	\param	int nTagAddr,		标签地址
		 *	\param	Byte* pbyData,		标签值
		 *	\param	int nDataLen		标签值长度
		 *	\param	int nTime			时标
		 *	\param	int bFromLocal		是否本机产生的，即本机发送的请求而得到的响应
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		virtual void SetVal(Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, 
			int nDataLen, long nTime, bool bFromLocal);

		/*!
		 *	\brief	设置标签值(标签地址为字符串)
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	Byte byUNIT_TYPE,		标签UNITYTYPE
		 *	\param	std::string sTagAddr,	标签地址
		 *	\param	Byte* pbyData,			标签值
		 *	\param	int nDataLen			标签值长度
		 *	\param	int nTime				时标
		 *	\param	int bFromLocal			是否本机产生的，即本机发送的请求而得到的响应
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		virtual void SetVal(Byte byUNIT_TYPE, std::string sTagAddr, Byte* pbyData, 
			int nDataLen, long nTime, bool bFromLocal);

		//!得到设备地址(以字符串形式表示)
		std::string GetStringAddr();

		//!得到设备地址
		int GetAddr();

		//!得到设备名称(设备浏览名)
		std::string GetName();

		//!得到设备描述信息
		std::string GetDescription();

		//!得到设备配置文件名
		std::string GetDevCfgName();

		//!设备是否为主
		bool IsHot();

		//!设置设备的主从(1为主,0为从)
		virtual void SetHot(Byte byHot);

		//!设备是否在线
		bool IsOnLine();

		//!设置设备在线离线状态
		virtual void SetOnLine(Byte byOnLine);

		//!设备是否离线
		bool IsOffLine();

		//!设备是否处于初始状态
		bool IsInit();

		//!设备是否被禁止
		bool IsDisable();

		//!设置设备禁止状态
		void SetDisable(Byte byDisable);

		//!设备是否超量程
		bool IsExtraRange();

		//!设置设备超量程状态
		void SetExtraRange(Byte byExtraRange);

		//!设置设备状态指针
		void SetUnitState(tIOScheduleUnitState* pUnitState);

		//!获取设备状态指针
		tIOScheduleUnitState* GetUnitState();

		/*!
		 *	\brief	设备是否处于强制状态
		 *
		 *	\param	无
		 *
		 *	\retval	bool 设备是否处于强制状态
		 */
		bool IsForce();

		/*!
		 *	\brief	强制设备的主从状态
		 *
		 *	\param	byPrmScnd 强制状态
		 *
		 *	\retval	bool 设备是否处于强制状态
		 */
		void Force(Byte byPrmScnd);

		/*!
		 *	\brief	取消设备强制
		 *
		 *	\param	
		 *
		 *	\retval	
		 */
		void DeforceUnit();

		/*!
		 *	\brief	设置设备节点指针
		 *
		 *	\param	CIOUnit* pUnit
		 *
		 *	\retval	无
		 */
		void SetIOUnitNode(CIOUnit* pUnit);

		/*!
		 *	\brief	获取设备节点指针
		 *
		 *	\param	无
		 *
		 *	\retval	CIOUnit* 设备节点指针
		 */
		CIOUnit* GetIOUnitNode();

		/*!
		 *	\brief	获取设备标签信息管理对象
		 *
		 *	\param	无
		 *
		 *	\retval	CIOCfgUnit	设备标签信息管理对象
		 */
		CIOCfgUnit* GetIOCfgUnit();

		/*!
		 *	\brief	设置设备对应的驱动配置信息即CIOPort信息
		 *
		 *	\param	pIOPort		驱动配置信息
		 *
		 *	\retval	无
		 */
		void SetIOCfgDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	获取设备对应的驱动配置信息即IOPort信息
		 *
		 *	\param	无
		 *
		 *	\retval	CIOPort*	设备标签信息管理对象
		 */
		CIOPort* GetIOCfgDriver();

		/*!
		 *	\brief	设置模拟设备的对象指针
		 *
		 *	\param	pDeviceSim	模拟设备对象指针
		 *
		 *	\retval	无
		 */
		void SetIODeviceSim(CIODeviceSimBase* pDeviceSim);

		/*!
		 *	\brief	获取模拟设备的对象指针
		 *
		 *	\param	无
		 *
		 *	\retval	CIODeviceSimBase*	模拟设备对象指针
		 */
		CIODeviceSimBase* GetIODeviceSim();

		/*!
		 *	\brief	核查UnitType、TagAddr是否有效
		 *
		 *	\param	byUnitType	标签类型
		 *	\param	lTagAddress	标签地址
		 *
		 *	\retval	true--有效，false--无效
		 */
		bool CheckUnitTypeAndTagAddr(Byte byUnitType, int lTagAddr);

		/*!
		 *	\brief	得到FeildPoint MAP表
		 *
		 *	\param	无
		 *
		 *	\retval	MAP_OBJTAG&	
		 */
		MAP_OBJTAG& GetMapobjTag();

		/*!
		 *	\brief	设置驱动对象指针
		 *
		 *	\param	CIODriverBase* 驱动对象指针
		 *
		 *	\retval	MAP_OBJTAG&	
		 */
		void SetDriver( CIODriverBase* pDriver );

		/*!
		 *	\brief	获取驱动对象指针
		 *
		 *	\param	无
		 *
		 *	\retval	CIODriverBase*	驱动对象指针
		 */
		CIODriverBase* GetDriver();

		/*!
		 *	\brief	设置协议对象指针
		 *
		 *	\param	pPrtcl 协议对象指针
		 *
		 *	\retval	无
		 */
		void SetPrtcl( CIOPrtclParserBase* pPrtcl );

		/*!
		 *	\brief	获取协议对象指针
		 *
		 *	\param	无
		 *
		 *	\retval	CIOPrtclParserBase* 协议对象指针
		 */
		CIOPrtclParserBase* GetPrtcl();

		/*!
		 *	\brief	设置初始标识
		 *
		 *	\param	bSet	
		 *
		 *	\retval	无
		 */
		void SetQualityFirstFlag( bool bSet );

		/*!
		 *	\brief	是否进行通道诊断
		 *
		 *	\param	无	
		 *
		 *	\retval	true 诊断,false 不诊断
		 */
		bool IsStatusUnit();

		/*!
		 *	\brief	设置通道诊断标识
		 *
		 *	\param	bStatusUnit 通道诊断标识	
		 *
		 *	\retval	无
		 */
		void SetStatusUnit( bool bStatusUnit );

		/*!
		 *	\brief	通过索引获取标签对象指针
		 *
		 *	\param	nUnitType	标签类型
		 *	\param	lTagAddr	标签地址
		 *
		 *	\retval	T_lstFieldPoint	FieldPoint对象列表
		 */
		CIOCfgTag* GetTagByIndex( int nUnitType, int lTagAddr);

		/*!
		 *	\brief	初始化IOUnit下的数据变量(FieldPoint)信息
		 *
		 *	\param	pIOUnit		设备节点
		 *
		 *	\retval	FieldPointType	标签节点对象
		 */
		virtual int InitVariableInfo(CIOUnit* pIOUnit);

		/*!
		 *	\brief	配置更新:添加数据变量和FeildPoint标签地址更新
		 *
		 *	\param	FieldPointType* pFieldPoint
		 *
		 *	\retval	int 0为成功，其他失败
		 */
		int UpdateDataVariable( FieldPointType* pFieldPoint );

		/*!
		 *	\brief	配置更新:设置暂停工作标志
		 *
		 *	\param	block	 OpcUa_True:暂停，OpcUa_False:不暂停
		 *
		 *	\retval	无
		 */
		void SetBlock(bool block);

		/*!
		 *	\brief	返回设备当前是否暂停工作
		 *
		 *	\retval	bool 设备是否暂停工作
		 */
		bool IsBlock();

		//!设置设备模拟的周期
		int SetDevSimPeriod(int mInternal);

	 	/*!
	 	 *	\brief	在链路为从设备上更新标签值
	 	 *			该接口用来在链路为从设备上进行更新值的操作，如果当前链路为从则会将写值操作转发链路为主设备进行更新
	 	 *			值操作，如果当前链路为主则直接更新值
	 	 *
	 	 *	\param	byUNIT_TYPE,	标签UNITYTYPE
	 	 *	\param	nTagAddr,		标签地址
	 	 *	\param	pbyData,		标签值
	 	 *	\param	nDataLen		标签值长度
	 	 *	\param	nTime			时标
	 	 *	\param	bFromLocal		是否本机产生的，即本机发送的请求而得到的响应
	 	 *
	 	 *	\retval	int 0:成功, 其他:失败
	 	 */
	 	int SetVal2Hot(Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, 
	 		int nDataLen, long nTime, bool bFromLocal);
         //将链路从机设备通信状态标签写给主机
         int SetVal2Hot(CIOCfgTag* pTag, Byte byOnLine);
		//!处理设备的热数据状态当前处于模拟状态
		int ProcesHotSim();
	private:
	 	/*!
	 	 *	\brief	处理FieldPoint将其放入合适UnitType计算标签地址
	 	 *
	 	 *	\param	FieldPointType*			标签对象
	 	 *	\param	CIOCfgDriverDbfParser*	标签解析对象
	 	 *
	 	 *	\retval	int 0为成功，其他失败
	 	 */
	 	int ProcessFieldPoint(FieldPointType* pFieldPoint, CIOCfgDriverDbfParser* pDrvDbfParser);

	 	//读取设备配置信息
	 	void InitDevConfig( std::string strDevCfgFile );

		
	 	//!发送通道上的SOE事件
	 	int FireSOEEvent( FieldPointType* pFieldPoint, CKiwiVariant& OldVarint, long nTime );

	// 	//!得到与另机的代理对象
	// 	HySession* GetEquivallentProxy();

	 	/*!
	 	 *	\brief	将链路为从的设备上更新值操作转发链路为主设备上执行
	 	 *
	 	 *	\param	pPeerSvrSession		另机代理指针
	 	 *	\param	uNodeId				标签的NodeId
	 	 *	\param	cmdVal				命令值
	 	 *	\param	uCmdParam			值参数
	 	 *
	 	 *	\retval	int 0为成功，其他失败
	 	 */
	 	//int SyncVal2HotDev( HySession* pPeerSvrSession, UaNodeId uNodeId, Byte cmdVal, UaByteString uCmdParam );

		//!得到设备模拟方式
		Byte GetSimMode();
		
	private:

		//初始化普通数据采集节点到m_strMapObjTag
		int  InitMapObjTag();

		//初始化字符串地址通道采集节点到m_mapObjTag
		int  InitStrAddMapObjTag();

		//处理主从机设备通讯状态点
		bool ProcessDevCommValue(Byte byOnLine);	

		//通过设备名称获取IOCfg标签结点
		CIOCfgTag* GetDevCommIoCfgTag(std::string DecCommName);

		bool SetDevCommFp4IoCfgTag(CIOCfgTag* pIoCfgTag, Byte byOnLine);

		bool AddDevComCfgTag2UnitMap(CIOCfgTag* pIoCfgTag, std::string DecCommName);
	public:
		//!类名
		static std::string s_ClassName;

		//!发送数据包次数
		int		m_byTxTimes;

		//!接收包次数
		int		m_byRxTimes;

	protected:
		//!以UnitType(标签类型)+TagAddr(标签地址)为索引的FieldPoints MAP表
		MAP_OBJTAG m_mapObjTag;

		//!以字符串(通道地址)+TagAddr(标签地址)为索引的FieldPoints MAP表
		STRMAP_OBJTAG m_strAddMapObjTag;
		//!设备状态结构
		tIOScheduleUnitState* m_pUnitState;

		//!模式设备对象指针
		CIODeviceSimBase* m_pDeviceSim;

		// !驱动信息IOPort对象指针
		CIOPort* m_pIOPort;

		//!设备对应的IOUnit对象指针
		CIOUnit* m_pIOUnit;

		//!标签类别信息管理对象
		CIOCfgUnit* m_pIOUnitCfg;

		//!模拟定时器ID号
		int m_nTimerID;

		//!驱动对象指针
		CIODriverBase* m_pDriver;

		//!协议对象指针
		CIOPrtclParserBase* m_pPrtcl;

		//!进行通道诊断的标志,即接收到STATUSUNIT指令
		bool	m_bStatusUnit;

		//!首次设置品质的标志，如果本标志为true，则在SetOnline接口中设置本设备的所有标签的品质；否则，只有为主时才设置标签的品质
		bool	m_bSetQualityFirstFlag;

		//!标志相应的IOUnit对象是否被删除
		bool m_bIsBlocked;

	};
	
	//!输出全局变量
	extern IODRIVERFRAME_VAR  GenericFactory<CIODeviceBase> g_DrvDeviceFactory;

}

#endif
