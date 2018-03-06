/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104主站设备类头文件
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Device.h 5     09-09-28 16:41 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 09-09-28 16:41 $
 *	$Revision: 5 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOMACSIEC104DEVICE_20080812_H_
#define _IOMACSIEC104DEVICE_20080812_H_

#include "FepIEC104DrvH.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{	
	class IOMACSIEC104_API CFepIEC104Device :
		public CIODeviceBase
	{
	public:
		CFepIEC104Device();

		/*!
		*	\brief	获得类名
		*
		*	\retval	std::string 类名
		*/
		std::string GetClassName();


		//////////////////////////////////////////////////////////////////////////////////
		/*!
		*	\brief	组下行测试包
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		static OpcUa_Int32 BuildTestFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组STOPDT帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		static OpcUa_Int32 BuildStopFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组STARTDT帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		static OpcUa_Int32 BuildStartFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组下行测试确认包
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		static OpcUa_Int32 BuildTestFrameCON( OpcUa_Byte* buf );

		/*!
		*	\brief	组STOPDT确认帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		static OpcUa_Int32 BuildStopFrameCON( OpcUa_Byte* buf );

		/*!
		*	\brief	组STARTDT确认帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		static OpcUa_Int32 BuildStartFrameCON( OpcUa_Byte* buf );

		/*!
		*	\brief	把本地时间转化为ASDUTime
		*
		*	\param	sysTime	本地时间
		*
		*	\retval	ASDUTime
		*/
		static ASDUTime SystimeToASDUTime( ACE_Time_Value sysTime );

		/*!
		*	\brief	组S格式帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组总召唤帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildWholeQueryFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组计数量召唤帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildCountQueryFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组校时帧
		*
		*	\param	buf	帧首地址
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildCheckTimeFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	组帧：遥控. 单遥，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 单遥，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 单遥，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 双遥，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildDDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 双遥，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildDDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 双遥，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildDDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 升降，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildUPDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 升降，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildUPDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 升降，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildUPDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 单遥（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTSDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 单遥（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTSDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 单遥（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTSDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 双遥（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTDDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 双遥（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTDDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 双遥（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTDDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 升降（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTUPDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 升降（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTUPDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥控. 升降（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTUPDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，归一化值，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildNVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，归一化值，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildNVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，归一化值，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildNVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，标度化值，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，标度化值，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，标度化值，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildSVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，短浮点数，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildFAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，短浮点数，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildFAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，短浮点数，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildFAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，32位字串，选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildDWAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，32位字串，执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildDWAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，32位字串，撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildDWAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，归一化值（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTNVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，归一化值（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTNVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，归一化值（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTNVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，标度化值（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTSVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，标度化值（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTSVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，标度化值（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTSVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，短浮点数（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTFAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，短浮点数（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTFAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，短浮点数（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTFAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，32位字串（带时标），选择
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTDWAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，32位字串（带时标），执行
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTDWAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	组帧：遥调，32位字串（带时标），撤销
		*
		*	\param	buf		帧首地址
		*	\param	Addr	信息地址
		*	\param	pVal	信息值
		*
		*	\retval	OpcUa_Int32 组成帧的长度
		*/
		OpcUa_Int32 BuildTDWAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	产生下一个发送序号
		*
		*	\retval	WORD 产生的发送序号
		*/
		WORD GenNextNs();

		/*!
		*	\brief	产生下一个接收序号
		*
		*	\retval	WORD 产生的接收序号
		*/
		WORD GenNextNr();

		/*!
		*	\brief	修改发送序号. 当收到子站发送来的S格式帧后把帧中的Nr更新到m_wdNs.
		*
		*	\param	Ns	发送序号
		*
		*	\retval	void
		*/
		void SetNs( WORD Ns );

		/*!
		*	\brief	修改接收序号
		*
		*	原则: 新收到的序号Nr与m_wdNr比较,以下两种情况均更新
		*	(1). Nr大于m_wdNr时.
		*	(2). m_wdNr = 32768(最大序号)
		*
		*	\param	Nr	接收序号
		*
		*	\retval	void
		*/
		void SetNr( WORD Nr );

		/*!
		*	\brief	是否需要校时了
		*
		*	\retval	bool ture表示需要校时，false表示不需要
		*/
		bool IsTimeForCheckTime();

		/*!
		*	\brief	是否总召唤时间到了
		*
		*	\retval	bool ture表示是，false表示否
		*/
		bool IsTimeForWholeQuery();

		/*!
		*	\brief	是否计数量召唤时间到了
		*
		*	\retval	bool ture表示是，false表示否
		*/
		bool IsTimeForCountQuery();

		/*!
		 *	\brief	是否需要发送S帧
		 *
		 *	\retval bool true是, false否
		 *
		 *	\note	现场中104协议设备肯定是一对一的，就是一个端口只有一个设备。
		 *			但是从逻辑上讲的话，不同的端口可以用同一个协议（对应不同的设备），
		 *			所以那些发送帧的函数都放到了设备类里面。按这样的逻辑的话，
		 *			应该是把m_bNeedSendSFrame之类的一些变量都放到设备类里面。
		 */
		bool IsNeedSendSFrame();


		/*!
		*	\brief	获取标签的整型值
		*/
		bool IsDIValueChange( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr, OpcUa_Byte& byOldVal, OpcUa_Byte& byNewVal, std::string& strTagName );

		/*!
		*	\brief	获取FLOAT标签的值并转化成OpcUa_Byte值比较
		*/
		bool IsAIValueChange( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr, OpcUa_Byte& byOldVal, OpcUa_Byte& byNewVal, std::string& strTagName );

		bool IsExitTag( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr );

		//! 设置设备的主从
		virtual void SetHot(OpcUa_Byte byHot);

	public:
		inline bool GetState()
		{
			return m_bState;
		}
		inline void SetState( bool bState)
		{
			m_bState = bState;
		}
		inline bool NeedWholeQuery()
		{
			return m_bNeedWholeQuery;
		}
		inline void NeedWholeQuery(bool bNeedWholeQuery)
		{
			m_bNeedWholeQuery = bNeedWholeQuery;
		}

		inline bool NeedCountQuery()
		{
			return m_bNeedCountQuery;
		}
		inline void NeedCountQuery(bool bNeedCountQuery)
		{
			m_bNeedCountQuery = bNeedCountQuery;
		}

		inline bool NeedCheckTime()
		{
			return m_bNeedCheckTime;
		}
		inline void NeedCheckTime(bool bNeedCheckTime)
		{
			m_bNeedCheckTime = bNeedCheckTime;
		}

		inline bool IsCheckTiming()
		{
			return m_bIsCheckTiming;
		}
		inline void IsCheckTiming(bool bIsCheckTiming)
		{
			m_bIsCheckTiming = bIsCheckTiming;
		}

		inline bool WholeQuerying()
		{
			return m_bWholeQuerying;
		}
		inline void WholeQuerying(bool bWholeQuerying)
		{
			m_bWholeQuerying = bWholeQuerying;
		}

		inline bool CountQuerying()
		{
			return m_bCountQuerying;
		}
		inline void CountQuerying(bool bCountQuerying)
		{
			m_bCountQuerying = bCountQuerying;
		}

		//! 接收序号重新归零
		inline void ResetNr()
		{
			m_wdNr = 0;
		}

		//! 发送序号重新归零
		inline void ResetNs()
		{
			m_wdNs = 0;
		}

		//! 复位尚未确认的收到的w个I格式的APDU个数
		inline void ResetCtrlAreaINum()
		{
			m_wCtrlAreaINum = 0;
		}

	public:
		//! 类名
		static std::string s_ClassName;	

	private:
		bool m_bState;	//设备为Hot时，true表示在线并且N秒内收到过子站的I帧；为Standby时，与m_bOnline含义一致。初始化为false。

		//! 发送序号: 0~32768
		WORD	m_wdNs;					
		
		//! 最近收到的最大接收序号: 0~32768
		WORD	m_wdNr;					
		
		//! 尚未确认的收到的w个I格式的APDU个数
		WORD	m_wCtrlAreaINum;		
		
		//! 最迟收到w个I格式的APDU后给出确认（S格式帧）
		OpcUa_Int32		m_nCtrlAreaIMax;		

		/*! 
		 *	是否需要立即进行总召唤. 两种情况下置该标志有效:
		 *		(1)周期到了(2)上次总召唤已成功完成了!
		 */
		bool	m_bNeedWholeQuery;		
		
		/*! 
		 *	是否需要立即进行计数量召唤. 两种情况下置该标志有效:
		 *		(1)周期到了(2)上次计数量召唤已成功完成了
		 */
		bool	m_bNeedCountQuery;		
		
		//! 是否需要立即进行对时
		bool	m_bNeedCheckTime;		

		/*! 
		 *	是否正在校时过程。发送校时命令后置该标志，
		 *	直到以下情形之一时释放(1)TimeOut(2)成功收到完成的应答帧
		 */
		bool	m_bIsCheckTiming;		
		
		/*! 
		 *	是否正在总召唤过程。发送总召唤命令后置该标志，
		 *	直到以下情形之一时释放(1)TimeOut(2)成功收到完成的应答帧
		 */
		bool	m_bWholeQuerying;		
		
		/*! 
		 *	是否正在计数量召唤过程。发送计数量召唤命令后置该标志，
		 *	直到以下情形之一时释放(1)TimeOut(2)成功收到完成的应答帧
		 */
		bool	m_bCountQuerying;		
	};
}

#endif
