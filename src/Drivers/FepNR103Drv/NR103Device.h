/*!
*	\file	NR103Driver.h
*
*	\brief	NR103驱动类
*
*	$Date: 15-03-30$
*
*/
#ifndef _NR103DEVICE_H_
#define _NR103DEVICE_H_

#include "NR103CommDef.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{
	class CNR103Driver;

	class FEPNR103Drv_API CNR103Device : public CIODeviceBase
	{
	public:
		CNR103Device();

		~CNR103Device();

		std::string GetClassName();

		//!进行设备的初始化
		virtual OpcUa_Int32 Init();

		//!初始化设备配置信息
		OpcUa_Int32 InitDevConfig();

		//!处理ASDU21请求组信息
		void ProcessGroupNo( std::string &strFilePath );
		
		//!设置设备校时标识
		void SetCheckTFlag( OpcUa_Boolean bIsNeedCT );
		
		//!设置设备心跳标识
		void SetSendHBTag( OpcUa_Boolean bSendHB );

		//!设置发送ASDU21总查询的标识
		void SetASDU21WQ(  OpcUa_Boolean bFlag  );

		//!设置总查询标识
		void SetASDU7( OpcUa_Boolean bFlag );

		//!初始化数据编号
		void InitDataNum(OpcUa_Int32 nDataNum);

		//!设备是否需要校时
		OpcUa_Boolean IsNeedCheckT();

		//!设备是否需要发送心跳
		OpcUa_Boolean IsNeedHeartB();

		//!是否需要发送ASDU7总召唤
		OpcUa_Boolean IsNeedASUD7();

		//!是否需要发送ASDU21总召唤
		OpcUa_Boolean IsNeedASUD21();

		//!组校时帧
		OpcUa_Int32 BuildCTFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!组APCI
		OpcUa_Int32 BuildAPCIFrame( FrameAPCI* pAPCI, DWORD dwFrameLen );

		//!组心跳帧
		OpcUa_Int32 BuildHBFrame( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!组ASDU7总召唤帧
		OpcUa_Int32 BuildASDU7( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );

		//!组ASDU21总召唤帧
		OpcUa_Int32 BuildASDU21( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );
		
		/*!
		 *	\brief	处理设备上ASDU10通用分类数据
		 *
		 *	\param	pASDUFrame		: 待处理ASDU数据区指针
		 *	\param	nDataLen		: ASDU数据长度
		 *
		 *	\retval	返回值	-1:失败，0:成功
		 */
		OpcUa_Int32 ProcessASDU10Data( const OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nDataLen );

		//!处理ASDU10无数据响应
		OpcUa_Byte ProASDU10NoDataSet( OpcUa_Byte byCOT, OpcUa_Int32 nTagAddr );

		//!得到标签地址
		OpcUa_Int32 GetTagAddr( OpcUa_Byte byGroup, OpcUa_Byte byEntry );
		
		//!处理GID数据
		OpcUa_Int32 ProcessGID( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver);
		
		//!处理UI类型GID
		OpcUa_Int32 ProcessDTUI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver);
		
		//!处理Float类型GID
		OpcUa_Int32 ProcessDTFloat( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!处理double类型GID
		OpcUa_Int32 ProcessDTDouble( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );

		//!处理双点信息
		OpcUa_Int32 ProcessDTDPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!处理单点信息
		OpcUa_Int32 ProcessDTSPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!带瞬变和差错的双点信息
		OpcUa_Int32 ProcessDT11DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!处理带品质描述被测值
		OpcUa_Int32 ProcessDTAIWT( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!带时标报文
		OpcUa_Int32 ProcessDT18DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!带相对时间的时标报文
		OpcUa_Int32 ProcessDT19DPI( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );

		//!检查更新后的值
		void CheckUpdateVal( OpcUa_Int32 nTagType, OpcUa_Int32 nTagAddr, CNR103Driver* pDriver );

		//!带7字节时标报文
		OpcUa_Int32 ProcessDT203( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );
		
		//!带相对时间7字节时标报文
		OpcUa_Int32 Processdt204( OpcUa_Byte* pCurData, GDATAPointHead* pGDHead, CNR103Driver* pDriver );

		//!输出调试信息
		void WriteProInfo( GDATAPointHead* pGDHead, OpcUa_Float fValue, CNR103Driver* pDriver, OpcUa_Boolean isAnyWay=OpcUa_False );
		
		//!处理ASDU5数据
		OpcUa_Int32 ProcessASDU5Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );
		
		//!处理ASDU6数据
		OpcUa_Int32 ProcessASDU6Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );
		
		//!解析NR103远方复归的ASDU1返信命令
		OpcUa_Int32 ProcessASDU1FRest( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );

		//!处理设备上传扰动数据
		OpcUa_Int32 ProcessASDU23Data( OpcUa_Byte* pASDUFrame, OpcUa_UInt32 nRestByteCount );

		//!组遥控命令
		OpcUa_Int32 BuildASDU10DOFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress, OpcUa_Byte byType, OpcUa_Byte byData );
		
		//!组读定值组命令
		OpcUa_Int32 BuildReadSFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress );
		
		//!组信号复归命令byASDU10
		OpcUa_Int32 BuildResetFrame( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress, OpcUa_Boolean bExe = true );
		
		//!南瑞103远方复归命令ASDU2O
		OpcUa_Int32 BuildResetASDU20( OpcUa_Byte* pbyData, OpcUa_Int64 lTagAddress );
		
		//通过当前日期计算毫秒数
		OpcUa_Int64 CaculateMSNum( DAILYTIME sTime );
		
		//DI是否发生变位并产生日志
		OpcUa_Boolean IsDIValueChange(OpcUa_Byte byType, OpcUa_Int32 nTagAdrr, OpcUa_Byte byNewVal, CNR103Driver* pDriver);
		
		//输出SOE的时间信息
		void WriteSOETime( std::string strDevName, OpcUa_Int32 nTagAddr, DAILYTIME sTime, CNR103Driver* pDriver );

	public:
		static std::string s_ClassName;
	private:
		WORD m_wSFacAddr;					//!源厂站地址
		WORD m_wDFacAddr;					//!目标厂站地址
		WORD m_wSDevAddr;					//!源设备地址
		WORD m_wDDevAddr;					//!目标设备地址
		WORD m_wFRoutAddr;					//!首级路由地址
		WORD m_wLRoutAddr;					//!末级路由地址
		WORD m_nDataNum;					//!数据编号
		WORD m_wDevType;					//!设备类型
		WORD m_wDNetState;					//!设备网络状态
		OpcUa_Boolean m_bIsNeedCheckT;		//!是否需要校时
		OpcUa_Boolean m_bIsNeedSendHB;		//!是否需要发送心跳
		OpcUa_Boolean m_bNeedASDU21GI;		//!是否需要ASDU21总召唤
		OpcUa_Boolean m_bNeedASDU7;			//!是否需要ASDU7总召唤
		OpcUa_Int32 m_nASDU21GCount;		//!需要ASDU21定值组召唤数目
		vector<OpcUa_Int32> m_vec21GroupNo;	//!定值召唤组
		OpcUa_Int32 m_bySCN;				//!扫描序列号
	};
}

#endif