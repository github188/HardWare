/*!
 *	\file	IOPort.h
 *
 *	\brief	ͨ����
 *
 *
 *	\author lingling.li
 *
 *	\date	2014��4��11��	11:18
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4ϵͳ������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef __IOPort_H__
#define __IOPort_H__

#include "Utilities/fepcommondef.h"
#include "RTDBPlatformClass/FieldPointType.h"
#include "FepObjectType.h"
#include "RTDBCommon/KiwiVariable.h"
#include "ace/Synch.h"

#ifndef FEPPLATFORMCLASS_API
#ifdef FEPPLATFORMCLASS_EXPORTS
# define FEPPLATFORMCLASS_API __declspec(dllexport)
#else
# define FEPPLATFORMCLASS_API __declspec(dllimport)
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD {

class CIOUnit;


class FEPPLATFORMCLASS_API CIOPort:
    public CFepObjectType
{
protected:
    virtual ~CIOPort();
public:
    // construction / destruction
    CIOPort(long nNodeID, std::string sDriverName);

    static void createTypes();

    static void clearStaticMembers();


	//! ��������ָ�룬����Ŀǰ���õĹ��췽ʽ������Ĺ��캯���д������Զ��������Ҫ
	//! ���Ӹýӿ�����¼���Զ����ָ�롣
	//virtual void SetItemPointer(UaQualifiedName& strItem, UaNode* pNode, NodeManagerConfig* pNodeConfig);
	//! ��ȡ��̬����ָ�룬���㸴�����͵Ļ�������
	//! ���Ӹýӿ�����ȡ��̬����ָ��
	//virtual UaNode* GetStaticItem(UaQualifiedName& strItem);

	//��������
	int sendCmd(
		//const ServiceContext& serviceContext,
		Byte& byCmdVal,
		std::string& bysCmdParam,
		int& result);
	///���á���ȡIO�˿�����
	void setIOPortCfg(const tIOPortCfg& IOPortCfg);
	void getIOPortCfg(tIOPortCfg& IOPortCfg) const;

	///���á���ȡ��������
	void setDrvName(const std::string& strDrvName);

	std::string getDrvName() const;

	/*!
	 *	\brief	ͨ���Ƿ�����
	 *
	 *	\retval	true:����, false:������
	 */
	bool IsOK();

	/*!
	 *	\brief	�豸�Ƿ�ģ��
	 *
	 *	\retval	�Ƿ���ģ��״̬
	 */
	bool IsSimulate();

	/*!
	 *	\brief	�豸�Ƿ񱻽�ֹ
	 *
	 *	\retval	����ֵ	�Ƿ�����
	 */
	bool IsDisable();

	/*!
	 *	\brief	�豸�Ƿ�ǿ��
	 *
	 *	\retval	����ֵ	�Ƿ�ǿ��
	 */
	bool IsForce();

	/*!
	 *	\brief	ͨ���ĵ�ǰ����״̬��1Ϊ����0Ϊ��
	 *
	 *	\param	byState	ͨ��������״̬
	 *
	 *	\retval	0��ʾ�ɹ���-1��ʾ��������
	 */
	int SetRealPrmScnd(Byte byState);


	 /*!
	 *	\brief	����˫������ʵweightֵ��ˢ����·״̬
	 */
	void SetRealPrmScnd();
	/*!
	 *	\brief	ͨ���ĵ�ǰ����״̬��1Ϊ����0Ϊ��
	 *
	 *	\retval	ͨ��������״̬
	 */
	Byte GetRealPrmScnd();


	/*!
	 *	\brief	�õ�ͨ��Ĭ������
	 *
	 *	\retval ͨ��Ĭ������
	 */
	Byte GetDefaultPrmScnd();

	/*!
	 *	\brief	����ͨ��ͨѶ״̬
	 *
	 *	\param	isOK	ͨ��ͨ��״̬ = true;
	 */
	void SetCommState(bool isOK);

	/*!
	 *	\brief	����ͨ��ģ��
	 *
	 *	\param	simMode	ģ�����ͣ����ҡ�����������ȣ������SimulateType
	 */
	void SimulateChannel(Byte simMode);
	/*!
	 *	\brief	ֹͣͨ��ģ��
	 */
	void StopSimulate();

	/*!
	 *	\brief	ͨ����ֹ
	 */
	void DisableChannel();

	/*!
	 *	\brief	���ͨ����ֹ
	 */
	void EnableChannel();

	/*!
	 *	\brief	ǿ��ͨ������
	 *
	 *	\param	PrmOrScnd	ǿ�Ƶ�����״̬��1Ϊǿ������0Ϊǿ��
	 */
	void ForceChannel(Byte PrmOrScnd);

	/*!
	 *	\brief	���ͨ��ǿ��
	 */
	void DeforceChannel();

	/*!
	 *	\brief	�õ���·��ǿ������
	 */
	Byte GetForcePrmScnd();

	/*!
	 *	\brief	��ȡͨ��Ȩֵ
	 *
	 *	\retval	ͨ��Ȩֵ
	 */
	int GetIOChannelWeight();

	/*!
	 *	\brief	����ͨ��Ȩֵ
	 *
	 *	\param	iWeight	ͨ��Ȩֵ
	 */
	void SetIOChannelWeight(int iWeight);

	/*!
	 *	\brief	�����Ƿ��¼ͨ����־
	 *
	 *	\param	bRecord	�Ƿ��¼ͨ����־
	 */
	void SetRecord(bool bRecord);

	/*!
	 *	\brief	�Ƿ��¼ͨ����־
	 *
	 *	\retval	true��ʾ��¼��false��ʾ����¼
	 */
	bool IsRecord();

	/*!
	 *	\brief	�����Ƿ����ͨ�ż���
	 *
	 *	\param	bRecord	�Ƿ����ͨ�ż���
	 */
	void SetMonitor(bool bMonitor);

	/*!
	 *	\brief	�Ƿ����ͨ�ż���
	 *
	 *	\retval	true��ʾ���ӣ�false��ʾ������
	 */
	bool IsMonitor();

	/*!
	 *	\brief	�Ƿ�Ϊ˫������
	 */
	bool IsDouble();

	/*!
	 *	\brief	ͨ���豸����ȡ�豸����ָ��
	 *
	 *	��ϸ�ĺ���˵������ѡ��
	 *
	 *	\param	strUnitName	�豸��
	 *
	 *	\retval		�豸ָ��
	 */
	CIOUnit* GetUnitByName(std::string strUnitName);

	/*!
	 *	\brief	��ȡͨ���µ��豸����
	 *
	 *	��ϸ�ĺ���˵������ѡ��
	 *
	 *	\param	����	�Բ�����˵��
	 *
	 *	\retval	����ֵ	�豸����
	 *
	 *	\note		ע�������ѡ��
	 */
	int GetUnitCount();

	/*!
	 *	\brief	ͨ���豸�ڸ�ͨ���µ������Ż�ȡ�豸��
	 *
	 *	\param	iIndex	������
	 *
	 *	\retval	����ֵ	�豸����ָ��
	 */
	CIOUnit* GetUnitByIndex(int iIndex);

	/*!
	 *	\brief	�������豸�����豸�б�����������
	 *
	 *	\param	strUnitNmae	�豸��
	 *	\param	pUnit  �豸�ڵ�ָ��
	 *
	 *	\retval	����ֵ	�Ƿ�ɹ�
	 */
	int AddUnit(std::string strUnitName, CIOUnit* pUnit);

	 	/*!
	 *	\brief	ɾ���豸
	 *
	 *	\param	nodeId	�豸�ڵ�Id
	 *
	 *	\retval	����ֵ	�Ƿ�ɹ�
	 */
	int DelUnit(std::string szNode);
	/*!
	 *	\brief	��ȡͨ���µ����б���
	 *
	 *	\param	vecTags	��������
	 *
	 *	\retval	����ֵ	0���������������쳣
	 */
	void GetTag(std::vector<CKiwiVariable*> &vecTags);

	/*!
	 *	\brief	�Ƿ��ڱ������򽵴�״̬
	 *
	 *	\retval	bool
	 */
	bool  isInPassiveState();

	/*!
	 *	\brief	�����Ƿ��ڱ������򽵴�״̬
	 *
	 *	\retval	bool ״̬
	 */
	void SetPassiveState(bool bState);

	//!���������򽵴�
	void PassiveSPrimScnd(Byte state);

	//!�ӱ��������򽵴��лָ�
	void RecorvePassiveState();


	/*!
	 *	\brief	�ָ�Port��Ĭ������״̬
	 *
	 *	\retval	OpcUa::BaseDataVariableType*
	 */
	void RecoverDefaultPrmScnd();

	//!�������Ȩ����Ϣ
	void PutPeerPrmScndWeight(int iWeight);

	//!��ȡ���Ȩ����Ϣ
	int GetPeerPrmScndWeight();

	//!������·�л�״̬
	void SetIOPortSwitchStatus(IOPortSwitchStatus flag);

	//!��ȡ����·���Լ����豸����Ҫ���������ݵ�Variable
	int GetNeedBackupVar( std::vector<CKiwiVariable*>& vecHotVariable );

	//!��ȡ��·��ģ�ⷽʽ
	Byte GetSimMode();

	void SetPeerDevComm(bool isOnLine );

	bool GetConnect2Peer();

	void SetConnect2Peer(bool flag);
protected:
	std::map<std::string , CIOUnit*> m_mapIOUnits;
	ACE_Thread_Mutex m_mapIOUnitsLock;

   // Variable nodes
	// Variable RealPrmScnd	ͨ������״̬
	Byte m_byRealPrmScndBak; //����ǿ������֮ǰ��ʵ������״̬

	ACE_Thread_Mutex m_RealPrmScndLock;

	// Variable IsOK	ͨ���û�
	// OpcUa::BaseDataVariableType*  m_pIsOK;
	bool				m_bIsOK;
	// // Variable IsSimulate	ͨ���Ƿ���ģ��״̬
	// OpcUa::BaseDataVariableType*  m_pIsSimulate;
	bool				m_bIsSimulate;
	// // Variable SimMode ģ���㷨����:���ҡ������������
	// OpcUa::BaseDataVariableType*  m_pSimMode;
	unsigned char		m_bySimMode;
	// // Variable IsForce	ͨ���Ƿ�ǿ��
	// OpcUa::BaseDataVariableType*  m_pIsForce;

	// // Variable Weight	ͨ��Ȩֵ
	// OpcUa::BaseDataVariableType*  m_pWeight;

	// // Variable PeerWeight	���ͨ��Ȩֵ
	// OpcUa::BaseDataVariableType*  m_pPeerWeight;

	// // Variable IsMonitor �Ƿ�ͨ�ż���
	// OpcUa::BaseDataVariableType*  m_pIsMonitor;
	bool			m_bIsMonitor;
	// // Variable IsRecord �Ƿ��¼ͨ����־���¼�
	// OpcUa::BaseDataVariableType*  m_pIsRecord;

	// // Variable DrvName ����������Э������
	// OpcUa::PropertyType*  m_pDrvName;
	std::string		m_szDrvName;
	// // Variable Type ��·����
	// OpcUa::PropertyType*  m_pType;
	std::string			m_szType;
	// // Variable Port �˿ں�
	// OpcUa::PropertyType*  m_pPort;
	unsigned int			m_unPort;
	// // Variable BaudRate ������
	// OpcUa::PropertyType*  m_pBaudRate;
	unsigned int			m_unBoadRate;
	// // Variable DataBits ����λ
	// OpcUa::PropertyType*  m_pDataBits;
	unsigned int			m_unDataBits;
	// // Variable StopBits ֹͣλ
	// OpcUa::PropertyType*  m_pStopBits;
	unsigned int			m_unStopBits;
	// // Variable Parity ��żУ�� 0��У�飬1żУ�飬2��У��
	// OpcUa::PropertyType*  m_pParity;
	unsigned int			m_unParity;
	// // Variable DefaultPrmScnd Ĭ������
	// OpcUa::PropertyType*  m_pDefaultPrmScnd;

	// // Variable PortAccessMode ͨ������ģʽ��1��ͻ��0����ͻ
	// OpcUa::PropertyType*  m_pPortAccessMode;

	// // Variable SwitchAlgName �����л��㷨����
	// OpcUa::PropertyType*  m_pSwitchAlgName;

	// // Variable Option ��չ����
	// OpcUa::PropertyType*  m_pOption;
	Byte*				m_pOption;

	// // Variable IPA_A A�豸��A��IP��ַ
	// OpcUa::PropertyType*  m_pIP;
	std::string			m_szIP;
protected:

private:
	/*!
	 *	\brief	��ʼ����·���ӽڵ�
	 *
	 *	\param	bCreateProp	�Ƿ���Ҫ��������ʼ������
	 *
	 */
    void initialize(bool bCreateProp = true);

	/*!
	 *	\brief ������·�µ������豸�Ľ�ֹ״̬
	 *
	 *	\param	bDisable	�Ƿ��ֹ
	 *
	 */
    void SetUnitDisableState(bool bDisable);

private:
    static bool s_typeNodesCreated;

	//����Port״̬�Ƿ��ڱ������򽵴�״̬
	bool m_bIsInPassvie;

	int m_PeerPrmScndWeight;

	bool m_Connect2PeerStatus;
};

} // End namespace MACS_SCADA_SUD

#endif // #ifndef __IOPort_H__

