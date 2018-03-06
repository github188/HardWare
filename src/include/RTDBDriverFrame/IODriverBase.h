/*!
 *	\file	IODriverBase.h
 *
 *	\brief	�����������
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��3��	19:52
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERBASE_H_
#define _IODRIVERBASE_H_

#include <string>
#include <map>
#include "Utilities/fepcommondef.h"
#include "RTDBDriverFrame/IODriverCommonDef.h"
// #include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/GenericFactory.h"


namespace MACS_SCADA_SUD
{
	class CIOCfgDriverDbfParser;
	class CIOUnit;
	class CIOPort;
	class FieldPointType;
	class CIODeviceBase;

	typedef std::map<std::string, CIODeviceBase*> T_MapDevices;

	/*!
	 *	\class	CIODriverBase
	 *
	 *	\brief	�������ඨ��
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIODriverBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODriverBase();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODriverBase();

		/*!
		 *	\brief	������ʼ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StartWork() = 0;

		/*!
		 *	\brief	����ֹͣ����
		 *
		 *	\param	��
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StopWork()= 0;

		/*!
		 *	\brief	��channel����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 */
		virtual int OpenChannel() = 0;

		/*!
		 *	\brief	�ر�channel����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 */
		virtual int CloseChannel() = 0;

		/*!
		 *	\brief	��ʼ���豸
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 */
		virtual int InitUnit(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	�ر��豸
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	strDrvName		�豸����
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StopUnit(std::string strUnitName) = 0;

		/*!
		 *	\brief	��ң�������������ң�ض���
		 *
		 *	\param	tRemote			ң����������
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int WriteDCB(tIORemoteCtrl& tRemote) = 0;

		/*!
		 *	\brief	�·�ͨ���������
		 *
		 *	\param	tPacket			ͨ����ϰ�
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int WritePacket(tIOCtrlPacketCmd& tPacket) = 0;

		/*!
		 *	\brief	�·����й�������
		 *
		 *	\param	tPacket			ͨ����ϰ�
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int WriteMgr(tIORunMgrCmd& tPacket) = 0;

		/*!
		 *	\brief	ͨ��״̬���
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0����ɹ�,��������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StatusUnit() = 0;

		/*!
		 *	\brief	��ȡ����
		 *
		 *	\retval	std::string		����������
		 *
		 *	\note		ע�������ѡ��
		 */
		std::string getClassName();

		/*!
		 *	\brief	�õ��û����õ�Port����
		 *
		 *	\param	��
		 *
		 *	\retval	Port����
		 */
		virtual std::string GetDriverName() = 0;

		/*!
		 *	\brief	�õ��������õ�Э������
		 *
		 *	\param	��
		 *
		 *	\retval	Э������
		 */
		virtual std::string GetCfgPrtclName() = 0;

		/*!
		 *	\brief	��ȡ������ǩ��������
		 *
		 *	\param	void
		 *
		 *	\retval	CIOCfgDriverDbfParser*	��ǩ��������ָ��
		 *
		 */
		virtual CIOCfgDriverDbfParser* GetDbfParser() = 0;

		/*!
		 *	\brief	����������ǩ��������
		 *
		 *	\param	pDbfParser	������ǩ��������
		 *
		 *	\retval	void
		 *
		 */
		virtual void SetDbfParser(CIOCfgDriverDbfParser* pDbfParser) = 0;

		/*!
		 *	\brief	�õ������µ������豸����
		 *
		 *	\param	��
		 *
		 *	\retval	T_MapDevices& �豸����MAP
		 *
		 */
		virtual T_MapDevices&  GetAllDevice() = 0;

		/*!
		 *	\brief	��ȡIOPort����ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CIOPort*	IOPort����ָ��
		 */
		virtual CIOPort* GetIOPort() = 0;

		/*!
		 *	\brief	����IOPort����ָ��
		 *
		 *	\param	pIOPort	IOPort����ָ��
		 *
		 *	\retval	��
		 */
		virtual void SetIOPort(CIOPort* pIOPort) = 0;

		/*!
		 *	\brief	��ȡStatus���ʱ��
		 *
		 *	\param	��
		 *
		 *	\retval	int	���ʱ��
		 */
		virtual int GetStatusWatchTime() = 0;

		/*!
		 *	\brief	���ø���:������ݱ����ʹ����ǩ��ַ����
		 *
		 *	\param	pFieldPoint ��ǩ����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int UpdateDataVariable( FieldPointType* pFieldPoint ) = 0;

		//!�õ�����ģ��Yֵ
		virtual int GetSineY() = 0;

		//!���ø���:ɾ������
		virtual int BlockDriver() = 0;

		//!��������������Ϣ
		virtual int UpdateDrvCfg() = 0;

		//!���ø���:��ͣ�豸�Ĺ���
		virtual int BlockDevice(CIOUnit* pIOUnit) = 0;

		//!���ø���:�����豸������Ϣ
		virtual int UpdateDeviceCfg( CIOUnit* pIOUnit ) = 0;

		//!UaDriver������FEP�ٶ�һ������
		virtual void ReadOneTimeData();

		//!����������������״̬��ǰ����ģ��״̬//
		virtual int ProcessHotSim();

		//short OpcUaDateTimeCompare(const OpcUa_DateTime& A, const OpcUa_DateTime& B);
	public:
		//!����
		static std::string s_ClassName;

	};

	//!����ȫ�ֱ��������๤��//
	extern IODRIVERFRAME_VAR GenericFactory<CIODriverBase> g_IODriverFactory;

}
#endif
