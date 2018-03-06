/*!
 *	\file	IODriver.h
 *
 *	\brief	������ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��22��	19:48
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVER_H_
#define _IODRIVER_H_

#include <map>
#include "utilities/fepcommondef.h"
#include "RTDBDriverFrame/IODriverH.h"
#include "RTDBDriverFrame/IORemoteCtrl.h"

namespace MACS_SCADA_SUD
{
	class CIODeviceBase;
	class CIOLog;
	class CIOPort;
	class CIODrvBoard;
	class CIOCfgDriverDbfParser;
	class FieldPointType;
	class CIOPrtclParserBase;
	

	/*!
	 *	\class	CIODriver
	 *
	 *	\brief	������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIODriver : public CIODriverBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODriver();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODriver();

		/*!
		 *	\brief	�õ�����������
		 */
		std::string GetClassName();

		/*!
		 *	\brief	��ͨ��
		 *
		 *	����Э����������,����ͨ��״̬,������־����,���ö˿�������Ϣ��
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int OpenChannel();

		/*!
		 *	\brief	�ر�ͨ��ͨ��
		 *
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int CloseChannel();

		/*!
		 *	\brief	��ң��ָ�����ң�ض���
		 *
		 *	\param	tRemote ң������
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int WriteDCB(tIORemoteCtrl& tRemote);

		/*!
		 *	\brief	���������޸�ͨ�����豸��Ӧ״̬(ģ��״̬,��������״̬)
		 *
		 *	\param	tRunMgr ״̬��������
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int WriteMgr(tIORunMgrCmd& tRunMgr);

		/*!
		 *	\brief	����ͨ����������
		 *
		 *	\param	tPacket ͨ���������
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int WritePacket(tIOCtrlPacketCmd& tPacket);

		/*!
		 *	\brief	����Ա�ǩ���й�������
		 *	���FEP��ʵʱ������ͬһ���̸÷���ֻ����FeildPoint״̬���ú�SetVal
		 *	���FEP��ʵʱ������ͬһ���̸÷���Ӧ�ñ�UADriver��д����UA����ʵ�ֱ�ǩǿ��
		 *
		 *	\param	tPacket ��ǩǿ������
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int WriteMgrForTag( tIORunMgrCmd& tRunMgr );

		/*!
		 *	\brief	Ϊ�豸����ͨ����ϱ�־,��ʾҪ���������ͨ�����(ֻ��ʵ��Ϊ���豸���ñ�־)
		 *	
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int StatusUnit(void);

		/*!
		 *	\brief	������·�µ��豸ͨ�����״̬
		 *	
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		void SetDevCHNCheck(bool bCheck);

		/*!
		 *	\brief	�������������󡢳�ʼ������������
		 *	
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int StartWork(void);

		/*!
		 *	\brief	�ر���������ɾ������������
		 *	
		 *	\param	��
		 *
		 *	\retval	0Ϊ�ɹ�����ʧ��
		 */
		virtual int StopWork(void);

		/*!
		 *	\brief	�õ�Э�����������
		 *	
		 *	\param	��
		 *
		 *	\retval	std::string Э�����������
		 */
		virtual std::string GetPrtclClassName();

		/*!
		 *	\brief	�õ��豸������
		 *	
		 *	\param	��
		 *
		 *	\retval	std::string �豸������
		 */
		virtual std::string GetDeviceClassName();

		/*!
		 *	\brief	�õ��豸ģ��������
		 *	
		 *	\param	��
		 *
		 *	\retval	std::string �豸ģ��������
		 */
		virtual std::string GetDeviceSimClassName();

		/*!
		 *	\brief	�õ���һ���豸����ָ��
		 *	
		 *	\param	��
		 *
		 *	\retval	CIODeviceBase* �豸����ָ��
		 */
		virtual CIODeviceBase* GetNextDevice();

		/*!
		 *	\brief	�õ���ǰ��ѵ�����豸����ָ��
		 *	
		 *	\param	��
		 *
		 *	\retval	CIODeviceBase* �豸����ָ��
		 */
		virtual CIODeviceBase* GetCurDevice();

		/*!
		 *	\brief	ͨ���豸���õ��豸����ָ��
		 *	
		 *	\param	szName	�豸��
		 *
		 *	\retval	CIODeviceBase* �豸����ָ��
		 */
		CIODeviceBase* GetDeviceByName(std::string szName);

		/*!
		 *	\brief	ͨ���豸��ַ�õ��豸�����ָ��
		 *	
		 *	\param	nUnitAddr	�豸��ַ(int)
		 *
		 *	\retval	CIODeviceBase* �豸����ָ��
		 */
		CIODeviceBase* GetDeviceByAddr( int nUnitAddr );

		/*!
		 *	\brief	ͨ���豸��ַ�õ��豸�����ָ��
		 *	
		 *	\param	nUnitAddr	�豸��ַ(string)
		 *
		 *	\retval	CIODeviceBase* �豸����ָ��
		 */
		CIODeviceBase* GetDeviceByAddr( std::string sUnitAddr );

		/*!
		 *	\brief	�õ������豸
		 *	
		 *	\param	��
		 *
		 *	\retval	T_MapDevices& �豸MAP��
		 */
		T_MapDevices&   GetAllDevice();

		/*!
		 *	\brief	�õ��豸����
		 *
		 *	\param	��
		 *
		 *	\retval	int �豸����
		 */
		int GetDeviceCount();

		/*!
		 *	\brief	����ŵõ��豸����ָ��
		 *	
		 *	\param	iIndex	���
		 *
		 *	\retval	CIODeviceBase* �豸����ָ��
		 */
		CIODeviceBase* GetDeviceByIndex( int iIndex );

		/*!
		 *	\brief	�õ��û����õ�Port����
		 *
		 *	\param	��
		 *
		 *	\retval	Port����
		 */
		std::string GetDriverName();

		/*!
		 *	\brief	�õ��������õ�Э������
		 *
		 *	\param	��
		 *
		 *	\retval	Э������
		 */
		std::string GetCfgPrtclName();

		/*!
		 *	\brief	����ң�ض��еĴ�С
		 *	
		 *	\param	size_t 
		 *
		 *	\retval	��
		 */
		void SetCtrlQueueLimit( size_t nLimit );

		/*!
		 *	\brief	д����������־
		 *	
		 *	\param	pchLog	д������ 
		 *	\param	bTime	�Ƿ��ʱ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int WriteLog( const char* pchLog, bool bTime=true );

		/*!
		 *	\brief	������ͨ����Ϣд����Թ���ͬʱд����־
		 *	
		 *	\param	pbyBuf		����ָ�� 
		 *	\param	nDataLen	���ݳ���
		 *	\param	byRcvOrSnd	����,���ձ�ʶ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int WriteCommData( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd=0 );

		/*!
		 *	\brief	д����ͨ����Ϣд����־��־
		 *	
		 *	\param	pbyBuf		����ָ�� 
		 *	\param	nDataLen	���ݳ���
		 *	\param	byRcvOrSnd	���շ��ͱ�ʶ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int WriteCommDataToLog( Byte* pbyBuf, int nDataLen, Byte byRcvOrSnd=0 );

		/*!
		 *	\brief	��·�����Ƿ��ͻ
		 *	
		 *	\param	��
		 *
		 *	\retval	true��ͻ,false����ͻ
		 */
		bool IsPortAccessClash();

		/*!
		 *	\brief	����ң��֮���SOE
		 *	
		 *	\param	szUnitName	�豸��
		 *	\param  nUnitType	��ǩ����
		 *	\param	nTagAddr	��ǩ��ַ
		 *	\param	nVal		ֵ
		 *	\param  nTime		ʱ����Ϣ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int PostYKSOE( std::string szUnitName, int nUnitType, int nTagAddr, int nVal, long nTime = 0 );

		/*!
		 *	\brief	��ȡ��ʱʱ��
		 *	
		 *	\param	��
		 *
		 *	\retval	int ��ʱʱ��
		 */
		int GetTimeOut();

		/*!
		 *	\brief	��ȡЭ�鴦����ָ��
		 *	
		 *	\param	��
		 *
		 *	\retval	CIOPrtclParserBase* Э����ָ��
		 */
		CIOPrtclParserBase* GetIOPrtclParse();

		/*!
		 *	\brief	ͨ���Ƿ񱻽�ֹ
		 *	
		 *	\param	��
		 *
		 *	\retval	true����ֹ,falseδ����ֹ
		 */
		bool IsDisable();

		/*!
		 *	\brief	�Ƿ���ģ��״̬
		 *	
		 *	\param	��
		 *
		 *	\retval	trueģ��״̬,false��ģ��״̬
		 */
		bool IsSimulate();

		/*!
		 *	\brief	����ͨ��״̬
		 *	
		 *	\param	pChannelState ͨ��״ָ̬��
		 *
		 *	\retval	��
		 */
		void SetChannelState(tIOScheduleChannelState* pChannelState);

		/*!
		 *	\brief	����ͨ����ģ�ⷽʽ
		 *	
		 *	\param	bySimMode ͨ��ģ�ⷽʽ
		 *
		 *	\retval	��
		 */
		virtual void SetSimulate(Byte bySimMode);

		/*!
		 *	\brief	ֹͣģ��
		 *	
		 *	\param	��
		 *
		 *	\retval	��
		 */
		virtual void StopSimulate(void);

		/*!
		 *	\brief	����ͨ����ֹ
		 *	
		 *	\param	byDisable 1Ϊ��ֹ��0Ϊ����ֹ
		 *
		 *	\retval	��
		 */
		virtual void SetDisable(Byte byDisable);

		/*!
		 *	\brief	�Ƿ��¼��־
		 *	
		 *	\param	��
		 *
		 *	\retval	Byte 1Ϊ��¼��0Ϊ����¼
		 */
		virtual bool IsLog();

		/*!
		 *	\brief	�����Ƿ��¼��־��־
		 *	
		 *	\param	byLog 1Ϊ��¼��0Ϊ����¼
		 *
		 *	\retval	��
		 */
		virtual void SetLog(bool byLog);

		/*!
		 *	\brief	�Ƿ���ͨ�ż���״̬
		 *	
		 *	\param	��
		 *
		 *	\retval	Byte 1Ϊ��¼��0Ϊ����¼
		 */
		virtual bool IsMonitor();

		/*!
		 *	\brief	�����Ƿ����ͨ�ż���
		 *	
		 *	\param	byMonitor �Ƿ����ͨ�ż���
		 *
		 *	\retval	��
		 */
		virtual void SetMonitor(bool byMonitor);

		/*!
		 *	\brief	�õ�ͨ������״̬
		 *	
		 *	\param	void
		 *
		 *	\retval	Byte 1ΪHot��0ΪCool
		 */
		virtual Byte GetHot();

		/*!
		 *	\brief	����ͨ������״̬
		 *	
		 *	\param	Byte 1ΪHot��0ΪCool
		 *
		 *	\retval	void
		 */
		virtual void SetHot(Byte byHot);

		/*!
		 *	\brief	ͨ���Ƿ���ǿ��״̬
		 *	
		 *	\param	
		 *
		 *	\retval	bool ͨ���Ƿ���ǿ��״̬
		 */
		bool IsForce();

		/*!
		 *	\brief	ǿ��ͨ��״̬
		 *	
		 *	\param	byPrmScnd 1ΪHot��0ΪCool
		 *
		 *	\retval	void
		 */
		void ForcePort(Byte byPrmScnd);

		/*!
		 *	\brief	ȡ��ͨ��ǿ��״̬
		 *	
		 *	\param	byPrmScnd 1ΪHot��0ΪCool
		 *
		 *	\retval	void
		 */
		void DeforcePort();

		/*!
		 *	\brief	��ȡ������ǩ��������
		 *
		 *	\retval	CIOCfgDriverDbfParser*	��ǩ��������ָ��
		 *
		 *	\note		ע�������ѡ��
		 */
		CIOCfgDriverDbfParser* GetDbfParser();

		/*!
		 *	\brief	����������ǩ��������
		 *
		 *	\param	pDbfParser	������ǩ��������
		 *
		 *	\retval	CIOCfgDriverDbfParser*	��ǩ��������ָ��
		 *
		 */
		void SetDbfParser(CIOCfgDriverDbfParser* pDbfParser);

		/*!
		 *	\brief	��ʼ�������µ������豸����
		 *
		 *	\param	��
		 *
		 *	\retval	int 0Ϊ�ɹ�,����ʧ��
		 *
		 */
		virtual int InitUnit(CIOUnit* pIOUnit);

		/*!
		 *	\brief	ɾ��ָ���豸����
		 *
		 *	\param	strUnitName �豸��
		 *
		 *	\retval	int 0Ϊ�ɹ�,����ʧ��
		 *
		 */
		virtual int StopUnit(std::string strUnitName);

		/*!
		 *	\brief	�õ�������IOPort����ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CIOPort* IOPort����ָ��
		 *
		 */
		 virtual CIOPort* GetIOPort();

		/*!
		 *	\brief	����������IOPort����ָ��
		 *
		 *	\param	pIOPort IOPort����ָ��
		 *
		 *	\retval	��
		 *
		 */
		 virtual void SetIOPort(CIOPort* pIOPort);


		/*!
		 *	\brief	���ø���:������ݱ����ʹ����ǩ��ַ����
		 *
		 *	\param	pFieldPoint	��ǩ����ָ��
		 *
		 *	\retval	int 0Ϊ�ɹ�������ʧ��
		 */
		virtual int UpdateDataVariable( FieldPointType* pFieldPoint );

		/*!
		 *	\brief	���ø���:ɾ������
		 *	ֻ��������ֹͣ����ͬʱ��������ɾ����ʶ
		 *
		 *	\param	pFieldPoint ��ǩ����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int BlockDriver();

		/*!
		 *	\brief	������������
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int UpdateDrvCfg();

		/*!
		 *	\brief	���ø���:��ָͣ���豸�Ĺ���
		 *
		 *	\param	pIOUnit		IOUnit����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int BlockDevice( CIOUnit* pIOUnit );

		/*!
		 *	\brief	���ø���:�����豸��������Ϣ
		 *
		 *	\param	pIOUnit		IOUnit����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int UpdateDeviceCfg( CIOUnit* pIOUnit );

		
		//!�õ�����ģ��Yֵ
		int GetSineY();
		/*!
		 *	\brief	��ȡStatus���ʱ��
		 *
		 *	\param	��
		 *
		 *	\retval	int	���ʱ��
		 */
		int GetStatusWatchTime();

		/*!
		 *	\brief	���������Ϣ
		 *
		 *	\param	pchInfo ������Ϣ
		 *
		 *	\retval	��
		 */
		void OutputDebug(const char* pchInfo);

		/*!
		 *	\brief	��ȡ��������վ������
		 */
		static std::string GetFEPRunDir();

		 /*!
		 *	\brief	��ȡ������־������Ϣ�����ļ�·��
		    �ӿڱ�����������������ʹ��
		 */
		static std::string GetRunCommFepDir();

		/*!
		 *	\brief	�����Ƿ�ֹͣ����
		 */
		bool IsStopWork();

		/*!
		 *	\brief	д����������־(�����Ƿ���д��־����)
		 *	
		 *	\param	pchLog	д������ 
		 *	\param	bTime	�Ƿ��ʱ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int WriteLogAnyWay( const char* pchLog, bool bTime=true );

		//!��·�Ƿ�Ϊ��������
		bool IsDouble();

		//!ͨ���豸��ַ�õ��豸������
		bool GetDevIndexByAddr(int devAddr, int& nIndex);

		/*!
		 *	\brief	��ң������ת������·Ϊ�ӵ����
		 *	
		 *	\param	tRemote	ң������
		 *
		 *	\retval	int 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int SyncWriteCmd2SP( tIORemoteCtrl& tRemote );

		int getQueueSize();
	private:

		/*!
		 *	\brief	���豸�������й�������
		 *	
		 *	\param	tRunMgr	���й�������
		 *
		 *	\retval	int 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int WriteMgrForUnit(tIORunMgrCmd& tRunMgr);

		/*!
		 *	\brief	��ͨ���������й�������
		 *	
		 *	\param	tRunMgr	���й�������
		 *
		 *	\retval	int 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int WriteMgrForPort(tIORunMgrCmd& tRunMgr);

		/*!
		 *	\brief	��MacsDriver.ini�ļ��л�ȡʱ����Ϣ����
		 *			��ʱʱ��,��ѵʱ��,������ʾ��ʽ
		 *	
		 *	\param	��
		 *
		 *	\retval	��
		 */
		void InitDriverCfg();

		void InitDriverCommCfg();

		void InitOpcUaDriverCfg();
		//create folder as config parameter
		void CreateDriverLogFolder();

		//!����������������״̬��ǰ����ģ��״̬
		int ProcessHotSim();

		//!��ȡ�����ĵ�ǰ��ģ�ⷽʽ
		Byte GetSimMode();

	public:
		//!ң���������
		CIORemoteCtrl m_IORemoteCtrl;

		 //!����������
		 CIODrvBoard* m_pIODrvBoard;

		 //!��־����ָ��
		 CIOLog* m_pLog;

		//!����
		static std::string s_ClassName;

	protected:
		//!˫����ʶ
		bool m_bDoubleComputer;

		//!Э�鴦����
		 CIOPrtclParserBase* m_pIOPrtclParser;

		//!�˿�������Ϣ�ṹ��
		tIOPortCfg m_pPortCfg;

		//!IOPort��Ϣ
		CIOPort* m_pIOPort;

		//!�豸������<�豸��,�豸����ָ��>
		T_MapDevices m_mapDevices;

		//!����ͨ�ż�����־����ָ��
		CIOLog* m_pCommLog;

		//!��ѵ�豸��������
		int m_iCurDevIndex;

		//!StatusUnit����ʱ��
		int	m_nWatchTime;

		//!CPU����ʱ��
		int m_nPollTime;

		//!��ʱʱ��
		int m_nTimeOut;

		//!������ͨ����־��д������ʱ���ݸ�ʽ(Ĭ��16����)
		int m_nCommDataFormat;

		//!��ѡģ��Yֵ��С
		int m_nSineY;

		//!ͨ��״ָ̬��
		tIOScheduleChannelState*	m_pChannelState;

		//!������ǩ��������
		CIOCfgDriverDbfParser* m_pDbfParser;

		//!�Ƿ��¼��־��ʶ
		Byte m_byIsLog;

		//!�������Ƿ��ѱ�ɾ��(ֹͣ����)
		bool m_bIsBlocked;

		//!����ң�ض��д�С
		size_t m_nRemoteQueueLimit;

		//!������־�ļ�����·��
		std::string m_strFilePath;

		//!������־�ļ���С
		int m_nLogFileSize;

		//!������־�ļ�����
		int m_nLogFileStoreDays;

		//!UADriver�������ö��г���
		int m_QueueSize;
	};
}

#endif