/*!
 *	\file	IONodeManager.h
 *
 *	\brief	��̵��ļ�˵������ѡ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author ���峬
 *
 *	\date	2014��3��31��	15:20
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADAϵͳ������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IONODEMANAGER_H_2014331_
#define _IONODEMANAGER_H_2014331_
#pragma once
#include "utilities/fepcommondef.h"
#include "RTDBPlatformClass/IOPort.h"
#include "RTDBPlatformClass/IOUnit.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "ace/Thread.h"
#include "ace/Synch.h"

namespace MACS_SCADA_SUD 
{

	/*!
	*	\class	CIONodeManager
	*
	*	\brief	��̵���˵������ѡ��
	*
	*	��ϸ����˵������ѡ��
	*/
	class IONODEMANAGER_API CIONodeManager 
		:public ACE_Thread
	{
	public:
		CIONodeManager(const std::string& sNamespaceUri);
		virtual ~CIONodeManager();

		// NodeManager / NodeManagerUaNode overwrite
		//����startUp�����ڵ���������õ�CNodeManagerCompositeImpl
		// virtual Byte startUp(ServerManager*);
		//���󴴽���,������Ͷ���ΪFepId_ChannelType,�����m_mapPorts
		// virtual void objectCreated(UaObject* pNewNode, UaBase::Object *pObject);

		virtual Byte   afterStartUp();
		virtual Byte   beforeShutDown();

		//- Interface IOManagerUaNode ----------------------------------------------------------
		// Byte beginWrite(OpcUa_Handle hIOManagerContext, unsigned int callbackHandle, VariableHandle* pVariableHandle, OpcUa_WriteValue* pWriteValue);
		// Byte writeValues(const CKiwiVariableArray& arrCKiwiVariables, const PDataValueArray& arrpDataValues, ByteCodeArray& arrStatusCodes);
		//- Interface IOManagerUaNode ----------------------------------------------------------

		void run();

		/*!
		*	\brief	���ظ���ӿڣ�����IO���ݣ�ͬʱ֪ͨ��������Ҫ���޸�
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	����	�Բ�����˵��
		*
		*	\retval	����ֵ	�Ը÷���ֵ��˵��
		*
		*	\note		ע�������ѡ��
		*/
		int UpdateCfg(std::string& sData);

		/*!
		*	\brief	����ͨ���������������ȷ�֪ͨ
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	����	�Բ�����˵��
		*
		*	\retval	����ֵ	�Ը÷���ֵ��˵��
		*
		*	\note		ע�������ѡ��
		*/
		long CreatePort(long sNodeId, tIOPortCfg tPortCfg, std::string sDrvName, tIODrvCfg tDrvCfg);

		/*!
		*	\brief	����UA��·���󣬲���ӵ���ַ�ռ�
		*
		*	�����ݿⵥԪ������Դ���ڱ���ʱ���ô˽ӿ���Ϊÿ������Դ����һ���������·
		*
		*	\param	sDriverName ��·���ƣ���������Դվ�����ƣ���Ӧһ��UADriver������
		*
		*	\retval	Byte ����ִ��״̬��
		*/
		Byte CreatePort(std::string sDriverName);

		/*!
		*	\brief	����UA�豸���󣬲���ӵ���ַ�ռ�
		*
		*	�����ݿⵥԪ������Դ���ڱ���ʱ���ô˽ӿ���Ϊÿ�����ݿⵥԪ����һ��������豸��
		*  ���뵽��Ӧ������Դ������·��  
		*
		*	\param	sDriverName ��·���ƣ���������Դվ�����ƣ���Ӧһ��UADriver������
		*	\param	sDeviceName �豸���ƣ��������ݿⵥԪ������
		*  \param  iDevAddr    �豸��ַ�����Դ������ݿⵥԪId��Ϊ�豸��ַ
		*
		*	\retval	Byte ����ִ��״̬��
		*/
		Byte CreateUnit(std::string sDriverName, std::string sDeviceName, int iDevAddr);

		/*!
		*	\brief	��һ��ͨ��������豸�������������ȷ�֪ͨ
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	����	�Բ�����˵��
		*
		*	\retval	����ֵ	�Ը÷���ֵ��˵��
		*
		*	\note		ע�������ѡ��
		*/
		long AddUnit(long sNodeId, std::string sUnitName, tIOUnitCfg tCfg, long sPort);

		/*!
		*	\brief	��UaDriver������豸�������������ȷ�֪ͨ
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	����	�Բ�����˵��
		*
		*	\retval	����ֵ	�Ը÷���ֵ��˵��
		*
		*	\note		ע�������ѡ��
		*/
		//long AddUnit(ServerInfo tCfg, long sPort);

		/*!
		*	\brief	��һ���豸��������ݱ����������������ȷ�֪ͨ
		*
		*	\param	pTag ���ݱ���ָ��
		*
		*	\retval	trueΪ�ɹ���falseΪʧ��
		*
		*	\note		ע�������ѡ��
		*/
		bool AddTag(CKiwiVariable* pTag);

		/*!
		*	\brief	��һ���豸��ɾ�����ݱ����������������ȷ�֪ͨ
		*
		*	\param	pTag ���ݱ���ָ��
		*
		*	\retval	trueΪ�ɹ���falseΪʧ��
		*/
		bool DeleteTag(CKiwiVariable* pTag);

		/*!
		*	\brief	��Ua�豸��������ݱ����������������ȷ�֪ͨ
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	sDriverName	��������
		*	\param	sDeviceName �豸����
		*	\param	pTag	    ���ݱ���ָ��
		*
		*	\retval	�ɹ���true��ʧ�ܣ�false
		*/
		bool AddTag(std::string sDriverName, std::string sDeviceName, CKiwiVariable* pTag);

		/*!
		*	\brief	ɾ��UA�豸�еı�ǩ�����������ȷ�֪ͨ
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	sDriverName	��������
		*	\param	sDeviceName �豸����
		*	\param	pTag	    ���ݱ���ָ��
		*
		*	\retval	�ɹ���true��ʧ�ܣ�false
		*/
		bool DeleteTag(std::string sDriverName, std::string sDeviceName, CKiwiVariable* pTag);

		/*!
		*	\brief	��ȡͨ���б�
		*
		*	\param	mapPorts  ���Σ��������ͨ��map����	
		*/
		void GetAllIOPorts(MAP_PORT& mapPorts);

		//������������ռ�
		static short getTypeNamespace();

		/*!
		*	\brief	���ͨ��
		*
		*	\param	pIOPort	ͨ��ָ��
		*	\param	bOnline �Ƿ�������ӣ�true��������ӣ�false����ͨ���
		*
		*	\retval	�ɹ���true��ʧ�ܣ�false
		*/	
		bool AddPort(CIOPort* pIOPort, bool bOnline = false);
		/*!
		*	\brief	ɾ��ͨ��
		*
		*	\param	PortId	ͨ���Ľڵ�Id
		*	\param	bOnline �Ƿ�����ɾ����true������ɾ����false����ͨɾ��
		*
		*	\retval	�ɹ���true��ʧ�ܣ�false
		*/
		bool DelPort(const long& PortId, bool bOnline = false);

		//������·���ƣ�������·ָ�룬�����ڣ��򷵻ؿ�
		CIOPort* GetPort(std::string sName);

		//!��ȡIOPort��IOUnit����Ҫ���н��������ݵı�����Ϣ
		int GetNeedBackupVar(std::vector<CKiwiVariable*>& vecHotVariable);

	private:

	protected:
		static unsigned short s_typenamespaceIndex; //fep���ͽڵ�������������ݶ�������

	private:
		MAP_PORTID	m_mapPortIDs;
		MAP_PORT    m_mapPorts;	//!	ͨ��map��

		ACE_Thread_Mutex m_mapPortslock;
		//UaMutex  m_mapPortslock;//! ͨ��map���������
	};

}// End namespace 
#endif


