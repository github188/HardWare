/*!
 *	\file	IODrvScheduleMgrImpl.h
 *
 *	\brief	��������ʵ��ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��3��31��	15:20
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODRVSCHEDULEMRGIMPL_H_
#define _IODRVSCHEDULEMRGIMPL_H_


#ifdef IODRVSHEDULEMGR_EXPORTS
#define IODRVSCHEDULEMGR_API MACS_DLL_EXPORT
#else
#define IODRVSCHEDULEMGR_API MACS_DLL_IMPORT
#endif

#include <string>
#include <map>
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/IOTimer.h"
#include "RTDBBaseClasses/IODrvScheduleMgrBase.h"

namespace MACS_SCADA_SUD
{
	class CIODriverMgr;
	class CIOCfgDriverDbfParser;
	class FieldPointType;

	/*!
	 *	\class	CIODrvScheduleMgr
	 *
	 *	\brief	��������ʵ����
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRVSCHEDULEMGR_API CIODrvScheduleMgr : public CIODrvScheduleMgrBase
	{
		//!�����������MAP
		typedef std::map<long, CIODriverMgr*> T_MapDriverMgr;

		//!������ǩ��������MAP
		typedef std::map<std::string, CIOCfgDriverDbfParser*> T_MapDrvDbfParser;

	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODrvScheduleMgr();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODrvScheduleMgr();

	public:
		/*!
		 *	\brief	�������ȳ�ʼ��
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
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
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
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
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StopWork();

		/*!
		 *	\brief	��������ж�ع���
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int UnInit();

		/*!
		 *	\brief	ң�ء�ң�������
		 *
		 *	����ң�ء�ң���������������������������
		 *
		 *	\param	remoteCmd	ң������
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddWriteCmd(tIORemoteCtrl& remoteCmd);

		/*!
		 *	\brief	ͨ����������
		 *
		 *	����ͨ������������������������������
		 *
		 *	\param	packetCmd	ͨ����ϰ�����
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddCtrlPacketCmd(tIOCtrlPacketCmd& packetCmd);

		/*!
		 *	\brief	���й��������
		 *
		 *	���ܹ���������·����������
		 *
		 *	\param	ioRunMgrCmd		���й�������
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddRunMgrCmd( tIORunMgrCmd& ioRunMgrCmd );

		/*!
		 *	\brief	������ݱ������������ݱ�����Ϣ
		 *
		 *	\param	pFieldPoint	
		 *
		 *	\retval	����ֵ	0Ϊ�ɹ�,����ʧ��
		 */
		virtual int UpdateDataVariable( FieldPointType* pFieldPoint );
		
		/*!
		 *	\brief	���ø���:����������ʹ�俪ʼ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	CIODriverMgr*	�����������ָ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:������ʼ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 */
		virtual int StartDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:����ֹͣ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pIOPort			IOPort����ָ��
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int StopDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:ɾ������
		 *
		 *	\param	pchDriverName	��������
		 *
		 *	\retval	int		0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int DeleteDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	��������������Ϣ
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pchDriverName	��������
		 *
		 *	\retval	int		
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int UpdateDrvCfg(CIOPort* pIOPort);

		/*!
		 *	\brief	���ø���:���IOUnit
		 *
		 *	\param	pIOUnit		IOUnit����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int AddUnit( CIOUnit* pIOUnit );

		/*!
		 *	\brief	���ø���:ɾ��IOUnit
		 *
		 *	\param	pIOUnit		IOUnit����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int DeleteUnit(CIOUnit* pIOUnit);

		/*!
		 *	\brief	���ø���:�����豸��������Ϣ
		 *
		 *	\param	pIOUnit		IOUnit����ָ��
		 *
		 *	\retval	int	0Ϊ�ɹ�,����ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual int UpdateUnitCfg(CIOUnit* pIOUnit);

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
		 *	\brief	�����������õ�������ǩ��������
		 *
		 *	\param	pchDriverName ��������
		 *
		 *	\retval	����ֵ	CIOCfgDriverDbfParser* ������ǩ��������ָ��
		 */
		CIOCfgDriverDbfParser* GetDbfParserByFileName( const char* pchDriverName );

		/*!
		 *	\brief	�������ȳ�ʼ���Ƿ����
		 *
		 *	\retval	����ֵ	true Ϊ���,falseΪδ���
		 */
		virtual bool IsInitFinished();

		/*!
		 *	\brief	����IOPort��Ϣ���������������
		 *
		 *	\param	pIOPort IOPort����ָ��
		 *	
		 *	\retval	����ֵ	0Ϊ�ɹ�,����ʧ��
		 */
		int ProcessIOPort(CIOPort* pIOPort);

		/*!
		 *	\brief	ͨ��NodeId��������������
		 *
		 *	\param	nodeId	Port��NodeId
		 *
		 *	\retval	����ֵ	CIODriverMgr* �����������ָ��
		 */
		virtual CIODriverMgr* GetDriverMgrByNodeId(long nodeId);

		virtual CIODriverBase* GetDriverBaseByNodeId( long nodeId );
	private:
		/*!
		 *	\brief	������������������.dbf�ļ�,��ʼ����Ӧ��������������Ϣ
		 *
		 *	\param	pchDbfFileName ������ǩ�����ļ�����
		 *	\param	pParser		   ������ǩ��������ָ��
		 *
		 *	\retval	����ֵ	0Ϊ�ɹ�����ʧ��
		 */
		int InitDriverDbfInfo(const char* pchDbfFileName, CIOCfgDriverDbfParser* pParser);
		/*!
		 *	\brief	ͨ��FieldPointָ���ȡ�����������ָ��
		 *
		 *	\param	FieldPointType*	
		 *
		 *	\retval	CIODriverMgr*	�����������ָ��
		 */
		CIODriverMgr* GetDrvMgrByFieldPoint(FieldPointType* pFieldPoint);

	public:
		//!������ʱ������
		ActiveTimer m_ActiveTimer;

	private:
		//! ͨ���ڵ���ID�����������MAP��
		T_MapDriverMgr m_mapDriverMgr;
		
		//!�Ƿ���Կ�ʼ����
		bool m_bCanWork;

		//!������ǩ��������MAP��
		T_MapDrvDbfParser m_mapDrvDbfParser;
	};
}

#endif