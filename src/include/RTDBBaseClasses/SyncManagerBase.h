/*!
*	\file	SyncManagerBase.h
*
*	\brief	ͬ���������ͷ�ļ�
*
*	\Author songxiaoyu
*
*	\date	2014-4-23 15:00
*
*	\version	1.0
*
*	\attention	Copyright (c) 2014, MACS-SCADAϵͳ������,HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef SYNMANAGERBASE_H
#define SYNMANAGERBASE_H

// #include "RTDB/Server/DBSyncManager/ConstDef_SyncManager.h"
#include "RTDBCommon/OS_Ext.h"
#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

using namespace std;
namespace MACS_SCADA_SUD
{


	/*!
	*	\class	CSyncManager
	*
	*	\brief	ͬ����������࣬���Ⱪ¶ȫ�ֶ���CSyncManagerBase* g_pSyncManager��
	*           Ϊ�˷�ֹͬ��ģ����ڵ����ģ��֮��Ļ�����������Ҫһ�����������и��롣
	*
	*/
	// class CUnitNodeManager;
	// class CUnitSyncMgr;
	class CIOPort;
	class FieldPointType;
	// class CFepSyncMgr;
	class DBBASECLASSES_API CSyncManagerBase
	{
	public:
		CSyncManagerBase();
		virtual ~CSyncManagerBase();

		// /*!
		// *	\brief	��ʼ��
		// *
		// *	\param	��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int Init() = 0;

		// /*!
		// *	\brief	��ʼ����
		// *
		// *	\param	��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int StartWork() = 0;

		// /*!
		// *	\brief	��������
		// *
		// *	\param	��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int StopWork() = 0;

		// /*!
		// *	\brief	ж��
		// *
		// *	\param	��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int UnInit() = 0;

		// /*!
		// *	\brief	������ݿⵥԪͬ����������ڶ�̬�������ݿⵥԪʱ
		// *
		// *	\param	CUnitNodeManager * pNodeManager���ڵ������ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int AddUnitSync(CUnitNodeManager * pNodeManager) = 0;

		// /*!
		// *	\brief	ͨ�����ݿⵥԪ�Ż�ȡͬ�������ָ��
		// *
		// *	\param	int iIndex�� ���ݿⵥԪ��
		// *
		// *	\retval	CUnitSyncMgr* ���ݿⵥԪͬ����ָ��
		// */
		// virtual CUnitSyncMgr * GetUnitSyncByIndex(int iIndex) = 0;

		// /*!
		// *	\brief	ɾ�����ݿⵥԪͬ����������ڶ�̬ɾ�����ݿⵥԪʱ
		// *
		// *	\param	int iIndex�����ݿⵥԪ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int DelUnitSyncIndex(int iIndex) = 0;

		// /*!
		// *	\brief	�����ͬ���ı���
		// *
		// *	\param	int iIndex�����ݿⵥԪ��
		// *
		// *	\param	std::vector<UaVariable *> & pVariable����ӵı���ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int  AddUnitSyncVariable(int iIndex,std::vector<UaVariable *> & pVariable) = 0;

		// /*!
		// *	\brief	ɾ����ͬ���ı���
		// *
		// *	\param	int iIndex�����ݿⵥԪ��
		// *
		// *	\param	std::vector<UaVariable *> & pVariable��ɾ���ı���ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int DelUnitSyncVariable(int iIndex,std::vector<UaVariable *> & pVariable) = 0;
		// /*!
		// *	\brief	��������֮�䷢������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToCopySource(int iIndex,CCmdContext & cmd) = 0;
		// /*!
		// *	\brief	��������֮�䷢��дֵ����
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CWriteContext &cWriteContext[in]���ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendWriteToCopySource(int iIndex,CWriteContext &cWriteContext) = 0;

		// /*!
		// *	\brief	����дֵ���������˱�վ֮�����в������������վ��ı����ݵ�Ԫ�������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *	\param	CWriteContext &cWriteContext[in]���û�������ɫ�����ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendWriteToOtherUnits(int iIndex, CWriteContext &cWriteContext) = 0;

		// /*!
		// *	\brief	����������������˱�վ֮�����в������������վ��ı����ݵ�Ԫ�������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *	\param	CCmdContext &cmdContext[in]���û�������ɫ�����ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToOtherUnits(int iIndex, const CCmdContext& cmdContext) = 0;

		// /*!
		// *	\brief	����дֵ������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CWriteContext &cWriteContext[in]���û�������ɫ�����ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendWriteToCopy(int iIndex,CWriteContext &cWriteContext) = 0;
		// /*!
		// *	\brief	�������������������������Ч��
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToSameSource(int iIndex,CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	���������������������ӻ���Ч��
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToEquivalent(int iIndex, CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	���������������������ӻ���Ч��
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToEquivalentEx(CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	���������������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToOtherDomain(int iIndex, CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	���������������
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToOtherDomainEx(CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	�����������������������������Ч��
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CCmdContext & cmd[in]��cmd �������ݣ���֧��Call���
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendCmdToCopy(int iIndex, CCmdContext & cmd) = 0;

		// /*!
		// *	\brief	����дֵ����������������������Ч��
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	CWriteContext &cWriteContext[in]���ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendWriteToSameSource(int iIndex,CWriteContext &cWriteContext) = 0;

		// /*!
		// *	\brief	����дֵ�����������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	const CWriteContext& cWriteContext[in]���û�������ɫ�����ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendWriteToEquivalent(int iIndex,CWriteContext& cWriteContext) = 0;



		// /*!
		// *	\brief	����дֵ����������
		// *
		// *	\param	int iIndex[in]�����ݿⵥԪ��
		// *
		// *	\param	const CWriteContext& cWriteContext[in]���û�������ɫ�����ڵ㡢���Ժ�ֵ�б�
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int SendWriteToOtherDomain(int iIndex,CWriteContext& cWriteContext) = 0;

		// /*!
		// *	\brief	�����ͬ����ͨ��
		// *
		// *	\param	CIOPort * pIOPort:��Ҫ��ӵ�ͨ������ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int AddFepSyncIOPort(CIOPort * pIOPort) = 0;

		// /*!
		// *	\brief	ɾ��ͬ����ͨ��
		// *
		// *	\param	CIOPort * pIOPort:��Ҫɾ����ͨ������ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int DelFepSyncIOPort(CIOPort * pIOPort) = 0;

		// /*!
		// *	\brief	�����ͬ����FEP����
		// *
		// *	\param	CIOPort * pIOPort[in]��ͨ������ָ��
		// *
		// *	\param	std::vector<UaVariable *> & pVariableVec[in]����ӵı���ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int  AddFepSyncVariable(CIOPort * pIOPort,std::vector<UaVariable *> & pVariableVec) = 0;

		// /*!
		// *	\brief	ɾ��ͬ����FEP����
		// *
		// *	\param	CIOPort * pIOPort[in]��ͨ������ָ��
		// *
		// *	\param	std::vector<UaVariable *> & pVariableVec[in]��ɾ���ı���ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual int DelFepSyncVariable(CIOPort * pIOPort,std::vector<UaVariable *> & pVariableVec) = 0;

		// /*!
		// *	\brief	�õ�FEPͬ���������ָ��
		// *
		// *	\param	��
		// *
		// *	\param	UaSessionCallback* �ص�ָ��
		// *
		// *	\retval	int	   0��������������ʧ��
		// */
		// virtual CFepSyncMgr* GetFepSyncCallback() = 0;

		// /*!
		// *	\brief	֪ͨPort����״̬������PortȨ��ֵ�и���
		// *
		// *	\param	bIsUpdate:	�Ƿ�������
		// *	\param	pIOPort:	��·Ȩ�ط����仯����·ָ��
		// *
		// *	\retval	��
		// */
		// virtual void NotifyLWUpdate(OpcUa_Boolean bIsUpdate, CIOPort* pIOPort) = 0;
	};

	extern DBBASECLASSES_API CSyncManagerBase * g_pSyncManager;     //!ͬ���������ȫ�ֶ���
}
#endif
