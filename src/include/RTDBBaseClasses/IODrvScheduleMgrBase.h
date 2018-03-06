/*!
 *	\file	IODrvScheduleMgrBase.h
 *
 *	\brief	�������Ƚӿڶ���ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��3��31��	13:32
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODRVSCHEDULEMRGBASE_H_
#define _IODRVSCHEDULEMRGBASE_H_


#include "RTDBCommon/OS_Ext.h"
#include "RTDBBaseClasses/ScheduleBase.h"
#include "utilities/fepcommondef.h"
#include "RTDBDriverFrame/IODriverBase.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	class CIODriverMgr;
	class FieldPointType;
	class CIOUnit;
	class CSiteProxy;

	/*!
	 *	\class	CIODrvScheduleMgrBase
	 *
	 *	\brief	�������Ȼ���
	 *			������������Ľӿ�,����������ݱ����Ľӿ�,��������¼�������Ľӿ�
	 *
	 *	\note	***����̳���ʵʱ��ģ���CScheduleBase
	 */
	class DBBASECLASSES_API CIODrvScheduleMgrBase
		: public CScheduleBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */

		 CIODrvScheduleMgrBase();
		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODrvScheduleMgrBase();

	public:
		/*!
		 *	\brief	�������ȳ�ʼ��
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int Init(); 

		/*!
		 *	\brief	�������ȿ�ʼ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StartWork();

		/*!
		 *	\brief	��������ֹͣ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StopWork();

		/*!
		 *	\brief	��������ж�ع���
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int UnInit();

		/*!
		 *	\brief	ң�ء�ң�������
		 *
		 *	����ң�ء�ң���������������������������
		 *
		 *	\param	portNodeId		Port��NodeId
		 *	\param	remoteCmd		ң������
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddWriteCmd(tIORemoteCtrl& remoteCmd) =0;

		/*!
		 *	\brief	ͨ����������
		 *
		 *	����ͨ������������������������������
		 *
		 *	\param	packetCmd	ͨ����ϰ�����
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddCtrlPacketCmd(tIOCtrlPacketCmd& packetCmd) = 0;

		/*!
		 *	\brief	���й��������
		 *
		 *	���ܹ���������·�������
		 *
		 *	\param	ioRunMgrCmd		���й�������
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddRunMgrCmd(tIORunMgrCmd& ioRunMgrCmd) = 0;

		/*!
		 *	\brief	������ݱ������������ݱ�����Ϣ
		 *
		 *	���ܹ���������·�������
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int UpdateDataVariable(FieldPointType* pFieldPoint) = 0;

		/*!
		 *	\brief	ɾ�����ݱ���
		 *
		 *	�����������ֱ����ӡ�ֻҪNodeId��ͬ����Ϊ��һ�����ݱ���
		 *
		 *	\param	pchDriverName	������
		 *	\param	pchUnitName		�豸��
		 *	\param	nodeId			dataVariable��NodeId
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int DeleteDataVariable(const char* pchDriverName, const char* pchUnitName, long nodeId);

		/*!
		 *	\brief	���ø���:�������
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�������ʧ��
		 */
		virtual int AddDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	����������ʼ����
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int StartDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:����ֹͣ����
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int StopDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:ɾ������
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int DeleteDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	��������������Ϣ
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int UpdateDrvCfg(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:���IOUnit
		 *
		 *	\param	pIOUnit			IOUnit����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int AddUnit(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	���ø���:ɾ��IOUnit
		 *
		 *	\param	pIOUnit			IOUnit����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int DeleteUnit(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	���ø���:�����豸������Ϣ
		 *
		 *	\param	pIOUnit			IOUnit����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int UpdateUnitCfg(CIOUnit* pIOUnit) = 0;

		/*!
		 *	\brief	�·��¼���Ϣ
		 *
		 *	��ϵͳ�ڲ��������¼��·���ĳ���ⲿ�豸
		 *
		 *	\param	pchDriverName	��������
		 *	\param	pchUnitName		�豸����
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int SendEvent(const char* pchDriverName, const char* pchUnitName);

		/*!
		 *	\brief	�������ȳ�ʼ���Ƿ����
		 *
		 *	\retval OpcUa_True ��ʼ������,��ʼ��δ����
		 */
		virtual bool IsInitFinished() = 0;

		/*!
		 *	\brief	�������ȳ�ʼ���Ƿ����
		 *
		 *	\retval OpcUa_True ��ʼ������,��ʼ��δ����
		 */

		virtual CIODriverMgr* GetDriverMgrByNodeId(long nodeId) = 0;

		virtual CIODriverBase* GetDriverBaseByNodeId( long nodeId );
		
	};

	//!�����������ȫ�ֶ���
	extern DBBASECLASSES_API CIODrvScheduleMgrBase* g_pIOScheduleManager;
}

#endif