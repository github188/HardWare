/*!
 *	\file	IODrvScheduleMgrBase.cpp
 *
 *	\brief	�������ȹ������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author miaoweijie
 *
 *	\date	2014��4��8��	14:54
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "RTDBBaseClasses/IODrvScheduleMgrBase.h"

namespace MACS_SCADA_SUD
{
	DBBASECLASSES_API CIODrvScheduleMgrBase* g_pIOScheduleManager = NULL;

	/*!
		*	\brief	ȱʡ���캯��
		*/

	CIODrvScheduleMgrBase::CIODrvScheduleMgrBase()
	{

	}
	/*!
		*	\brief	ȱʡ��������
		*/
	CIODrvScheduleMgrBase::~CIODrvScheduleMgrBase()
	{

	}

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
	int CIODrvScheduleMgrBase::Init()
	{
		return 0;
	}

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
	int CIODrvScheduleMgrBase::StartWork()
	{
		return 0;
	}

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
	int CIODrvScheduleMgrBase::StopWork()
	{
		return 0;
	}

	/*!
	*	\brief	��������ж�ع���
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\retval	int		0Ϊ�ɹ�,����ʧ��
	*
	*	\note		ע�������ѡ��
	*/
	int CIODrvScheduleMgrBase::UnInit()
	{
		return 0;
	}

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
	int CIODrvScheduleMgrBase::DeleteDataVariable(
		const char* pchDriverName, const char* pchUnitName, long nodeId)
	{
		return 0;
	}

	/*!
	*	\brief	��������
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\param	pchDriverName	��������
	*
	*	\retval	CIODriverMgr*	�����������ָ��
	*
	*	\note		ע�������ѡ��
	*/
	int CIODrvScheduleMgrBase::AddDriver(CIOPort* pIOPort)
	{
		return -1;
	}

	/*!
	*	\brief	����������ʼ����
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\param	pIOPort			IOPort����ָ��
	*
	*	\retval	int		0Ϊ�ɹ�,����ʧ��
	*
	*	\note		ע�������ѡ��
	*/
	int CIODrvScheduleMgrBase::StartDriver(CIOPort* pIOPort)
	{
		return 0;
	}

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
	int CIODrvScheduleMgrBase::StopDriver(CIOPort* pIOPort)
	{
		return 0;
	}

	/*!
	*	\brief	���ø���:ɾ������
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\param	pchDriverName	��������
	*
	*	\retval	int		0Ϊ�ɹ�,����ʧ��
	*
	*	\note		ע�������ѡ��
	*/
	int CIODrvScheduleMgrBase::DeleteDriver(CIOPort* pIOPort)
	{
		return 0;
	}

	/*!
	*	\brief	��������������Ϣ
	*
	*	��ϸ�ĺ���˵������ѡ��
	*
	*	\param	pchDriverName	��������
	*
	*	\retval	int		0Ϊ�ɹ�,����ʧ��
	*
	*	\note		ע�������ѡ��
	*/
	int CIODrvScheduleMgrBase::UpdateDrvCfg(CIOPort* pIOPort)
	{
		return 0;
	}

	CIODriverBase* CIODrvScheduleMgrBase::GetDriverBaseByNodeId( long nodeId )
	{
		return NULL;
	}
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
	int CIODrvScheduleMgrBase::SendEvent(const char* pchDriverName, const char* pchUnitName)
	{
		return 0;
	}
}