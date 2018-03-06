/*!
 *	\file	IODriverMgr.h
 *
 *	\brief	���������ඨ��ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��3��31��	18:19
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODRIVERMRG_H_
#define _IODRIVERMRG_H_

#include <string>
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIODriverBase;
	class CIOPort;
	class CIODrvStatusHandle;
	class CIODrvScheduleMgr;
	class CIOCfgDriverDbfParser;
	class FieldPointType;
	class CIOUnit;

	/*!
	 *	\class	CIODriverMgr
	 *
	 *	\brief	����������
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class CIODriverMgr
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODriverMgr();

		/*!
		 *	\brief	���캯��
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pDrvScheduleImpl �������ȶ���ָ��
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		CIODriverMgr(CIODrvScheduleMgr* pDrvScheduleImpl);
		
		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODriverMgr();

		/*!
		 *	\brief	��ʼ������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	strDrvName	��������
		 *	\param	pIOPort		IOPort����ָ��
		 *
		 *	\retval	int	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		int InitDriver(std::string strDrvName, CIOPort* pIOPort);

		/*!
		 *	\brief	������ʼ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\retval	int	0Ϊ�ɹ�������ʧ��
		 *
		 *	\note		ע�������ѡ��
		 */
		int StartWork();

		/*!
		 *	\brief	����ֹͣ����
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\retval	int	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		int StopWork();

		/*!
		 *	\brief	ж������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\retval	int	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		int UnInitDriver();

		/*!
		 *	\brief	����ң�ء�ң������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	tRemoteCmd	ң��ң������ṹ��
		 *
		 *	\retval	int	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		int AddWriteCmd(tIORemoteCtrl& tRemoteCmd);

		/*!
		 *	\brief	����ͨ���������
		 *
		 *	\param	tPacketCmd	ͨ���������ṹ��
		 *
		 *	\retval	int	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		int AddCtrlPacketCmd(tIOCtrlPacketCmd& tPacketCmd);

		/*!
		 *	\brief	�������й�������
		 *
		 *	\param	tRunCmd		���й�������
		 *
		 *	\retval	int	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		int AddRunMgrCmd(tIORunMgrCmd& tRunCmd);

		/*!
		 *	\brief	����ͨ������ָ��,��ͨ������ָ�븳���������������
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	pIOPort		IOPort����ָ��
		 *
		 *	\retval	void
		 *
		 *	\note		ע�������ѡ��
		 */
		void SetChannelPrt(CIOPort* pIOPort);

		/*!
		 *	\brief	���þ�����������ָ��
		 *
		 *	\param	pDriverBase	��������ָ��
		 *
		 *	\retval	void
		 */
		void SetDrvPrt(CIODriverBase* pDriverBase);

		/*!
		 *	\brief	�õ�������DBF��������
		 *
		 *	\param	pchDriverName	����Dbf�ļ���
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int InitDriverDbfParser(const char* pchDriverName);

		/*!
		 *	\brief	���ø���:������ݱ����ʹ���FieldPoint��ǩ��ַ����
		 *
		 *	\param	pFieldPoint	��ǩ����ָ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int UpdateDataVariable( FieldPointType* pFieldPoint );
		
		/*!
		 *	\brief	���ø���:ɾ������
		 *
		 *	\param	pFieldPoint	��ǩ����ָ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int DeleteDriver();

		/*!
		 *	\brief	��������������Ϣ
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int UpdateDrvCfg();

		/*!
		 *	\brief	���ø���:����豸
		 *
		 *	\param	pIOUnit IOUnit����ָ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int AddUnit( CIOUnit* pIOUnit );

		/*!
		 *	\brief	���ø���:ɾ���豸
		 *
		 *	\param	pIOUnit IOUnit����ָ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int DeleteUnit( CIOUnit* pIOUnit );

		/*!
		 *	\brief	���ø���:�����豸������Ϣ
		 *
		 *	\param	pIOUnit IOUnit����ָ��
		 *
		 *	\retval	0Ϊ�ɹ�,����ʧ��
		 */
		int UpdateDeviceCfg( CIOUnit* pIOUnit );


		//!��ȡ��������ָ��
		CIODriverBase* GetDriverBase();

		//!����������������״̬��ǰ����ģ��״̬
		int ProcessHotSim();

	private:
		//! ��������ָ��
		CIODriverBase* m_pDriverBase;

		//! StatusUnit�����Ķ�ʱ���ص�����
		CIODrvStatusHandle* m_pStatusCb;

		//! ����StatusUnit�����Ķ�ʱ��ID��
		int m_lStatusId;

		//! ����StatusUnit������ʱ����
		int m_nWatchInterval;

		//!�������ȹ������
		CIODrvScheduleMgr* m_pIODrvScheduleMgr;
	};
}
#endif