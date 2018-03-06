/*!
 *	\file	IOCmdManagerBase.h
 *
 *	\brief	
 *	
 *
 *	\author lingling.li
 *
 *	\date	2014��4��22��	16:38
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4ϵͳ������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */


#ifndef __IOCMDMANAGERBASE_H__
#define __IOCMDMANAGERBASE_H__

#include <string>
#include "utilities/fepcommondef.h"
#include "RTDBCommon/OS_Ext.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD {

class DBBASECLASSES_API CIOCmdManagerBase
{
public:
	CIOCmdManagerBase(void);
	virtual ~CIOCmdManagerBase(void);

public:
	/*!
	*	\brief	��ʼ��
	*		����������Ҫ����
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int Init() = 0;

	/*!
	*	\brief	��ʼ����
	*		��������������ģ��û��������������ֱ�ӷ��ء�
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int StartWork() = 0;

	/*!
	*	\brief	�Ƿ���Կ�ʼ����
	*
	*	\retval	trueΪ���Կ�ʼ������falseΪ�����ԡ�
	*/
	virtual bool CanWork() = 0;

	/*!
	*	\brief	ֹͣ����
	*		ֹͣ����������ģ��û��������������ֱ�ӷ��ء�
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int StopWork() = 0;

	/*!
	*	\brief	ж��
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int UnInit() = 0;

	/*!
	*	\brief	д��ǩֵ,������ң��/ң������
	*
	*	\param	tRemoteCmd:ң������ṹ
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual Byte WriteDataVariable(tIORemoteCtrl& tRemoteCmd, tIOCmdCallBackParam* pCallBack = NULL) = 0;

	/*!
	*	\brief	����������ݰ�����
	*
	*	\param	tPacketCmd:����ṹ
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int SendIOCtrlPacketCmd(tIOCtrlPacketCmd& tPacketCmd, tIOCmdCallBackParam* pCallBack = NULL) = 0;

	/*!
	*	\brief	�������й�������,����ʵ���״̬
	*
	*	\param	tRunMgrCmd:����ṹ
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int SendIORunMgrCmd(tIORunMgrCmd& tRunMgrCmd, tIOCmdCallBackParam* pCallBack = NULL) = 0;

	/*!
	*	\brief	���������ͨ����ϰ�
	*
	*	\param	tPacketCmd:����ṹ
	*
	*	\retval	0Ϊ�ɹ�������ʧ��
	*/
	virtual int SendIOChannelDiag(tIOCtrlPacketCmd& tDiagPacket, tIOCmdCallBackParam* pCallBack = NULL) = 0;
};

extern  DBBASECLASSES_API CIOCmdManagerBase* g_pIOCmdManager;     //!���������ȫ�ֶ���

}// End namespace
#endif // __IOCMDMANAGERBASE_H__
