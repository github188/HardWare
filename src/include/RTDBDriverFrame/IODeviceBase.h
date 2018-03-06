/*!
 *	\file	IODeviceBase.h
 *
 *	\brief	�豸����
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author JYF
 *
 *	\date	2014��4��14��	19:06
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IODEVICEBASE_H_
#define _IODEVICEBASE_H_

#include <map>
#include "RTDBDriverFrame/IODriverH.h"
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	class CIOCfgTag;
	class CIOUnit;
	class CIOCfgUnit;
	class CIODeviceSimBase;
	class FieldPointType;
	class CIODriverBase;
	class CIOPrtclParserBase;
	

	
	//!�����豸�µı�ǩMAP
	typedef std::map<long long, CIOCfgTag*> MAP_OBJTAG;

	//!�����豸�µ��ַ���ͨ���ı�ǩMAP
	typedef std::map<std::string, CIOCfgTag*> STRMAP_OBJTAG;

	/*!
	 *	\class	CIODeviceBase
	 *
	 *	\brief	�豸����
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIODeviceBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODeviceBase(void);

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODeviceBase(void);

		//!��ȡ����
		std::string GetClassName();
		
		/*!
		 *	\brief	�豸��ǩ��ʼ��,�����豸�����а���FieldPoint������
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 *
		 *	\note	ע�������ѡ��
		 */
		virtual int Init();

		/*!
		 *	\brief	�����豸Ϊģ��״̬
		 *
		 *	\param	Byte bySimMode	ģ��ģʽ:Random,Increase,Sine
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 *
		 *	\note	ע�������ѡ��
		 */
		virtual void StartSimulate(Byte bySimulate);

		/*!
		 *	\brief	ֹͣģ��
		 */
		virtual void StopSimulate(void);

		/*!
		 *	\brief	�豸�Ƿ���ģ��״̬
		 *
		 *	\retval	true = ģ��,false = ����
		 */
		virtual bool IsSimulate(void);

		/*!
		 *	\brief	���ñ�ǩֵ
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	Byte byUNIT_TYPE,	��ǩUNITYTYPE
		 *	\param	int nTagAddr,		��ǩ��ַ
		 *	\param	Byte* pbyData,		��ǩֵ
		 *	\param	int nDataLen		��ǩֵ����
		 *	\param	int nTime			ʱ��
		 *	\param	int bFromLocal		�Ƿ񱾻������ģ����������͵�������õ�����Ӧ
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual void SetVal(Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, 
			int nDataLen, long nTime, bool bFromLocal);

		/*!
		 *	\brief	���ñ�ǩֵ(��ǩ��ַΪ�ַ���)
		 *
		 *	��ϸ�ĺ���˵������ѡ��
		 *
		 *	\param	Byte byUNIT_TYPE,		��ǩUNITYTYPE
		 *	\param	std::string sTagAddr,	��ǩ��ַ
		 *	\param	Byte* pbyData,			��ǩֵ
		 *	\param	int nDataLen			��ǩֵ����
		 *	\param	int nTime				ʱ��
		 *	\param	int bFromLocal			�Ƿ񱾻������ģ����������͵�������õ�����Ӧ
		 *
		 *	\retval	����ֵ	�Ը÷���ֵ��˵����һ������ֵ��Ӧһ����
		 *
		 *	\note		ע�������ѡ��
		 */
		virtual void SetVal(Byte byUNIT_TYPE, std::string sTagAddr, Byte* pbyData, 
			int nDataLen, long nTime, bool bFromLocal);

		//!�õ��豸��ַ(���ַ�����ʽ��ʾ)
		std::string GetStringAddr();

		//!�õ��豸��ַ
		int GetAddr();

		//!�õ��豸����(�豸�����)
		std::string GetName();

		//!�õ��豸������Ϣ
		std::string GetDescription();

		//!�õ��豸�����ļ���
		std::string GetDevCfgName();

		//!�豸�Ƿ�Ϊ��
		bool IsHot();

		//!�����豸������(1Ϊ��,0Ϊ��)
		virtual void SetHot(Byte byHot);

		//!�豸�Ƿ�����
		bool IsOnLine();

		//!�����豸��������״̬
		virtual void SetOnLine(Byte byOnLine);

		//!�豸�Ƿ�����
		bool IsOffLine();

		//!�豸�Ƿ��ڳ�ʼ״̬
		bool IsInit();

		//!�豸�Ƿ񱻽�ֹ
		bool IsDisable();

		//!�����豸��ֹ״̬
		void SetDisable(Byte byDisable);

		//!�豸�Ƿ�����
		bool IsExtraRange();

		//!�����豸������״̬
		void SetExtraRange(Byte byExtraRange);

		//!�����豸״ָ̬��
		void SetUnitState(tIOScheduleUnitState* pUnitState);

		//!��ȡ�豸״ָ̬��
		tIOScheduleUnitState* GetUnitState();

		/*!
		 *	\brief	�豸�Ƿ���ǿ��״̬
		 *
		 *	\param	��
		 *
		 *	\retval	bool �豸�Ƿ���ǿ��״̬
		 */
		bool IsForce();

		/*!
		 *	\brief	ǿ���豸������״̬
		 *
		 *	\param	byPrmScnd ǿ��״̬
		 *
		 *	\retval	bool �豸�Ƿ���ǿ��״̬
		 */
		void Force(Byte byPrmScnd);

		/*!
		 *	\brief	ȡ���豸ǿ��
		 *
		 *	\param	
		 *
		 *	\retval	
		 */
		void DeforceUnit();

		/*!
		 *	\brief	�����豸�ڵ�ָ��
		 *
		 *	\param	CIOUnit* pUnit
		 *
		 *	\retval	��
		 */
		void SetIOUnitNode(CIOUnit* pUnit);

		/*!
		 *	\brief	��ȡ�豸�ڵ�ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CIOUnit* �豸�ڵ�ָ��
		 */
		CIOUnit* GetIOUnitNode();

		/*!
		 *	\brief	��ȡ�豸��ǩ��Ϣ�������
		 *
		 *	\param	��
		 *
		 *	\retval	CIOCfgUnit	�豸��ǩ��Ϣ�������
		 */
		CIOCfgUnit* GetIOCfgUnit();

		/*!
		 *	\brief	�����豸��Ӧ������������Ϣ��CIOPort��Ϣ
		 *
		 *	\param	pIOPort		����������Ϣ
		 *
		 *	\retval	��
		 */
		void SetIOCfgDriver(CIOPort* pIOPort);

		/*!
		 *	\brief	��ȡ�豸��Ӧ������������Ϣ��IOPort��Ϣ
		 *
		 *	\param	��
		 *
		 *	\retval	CIOPort*	�豸��ǩ��Ϣ�������
		 */
		CIOPort* GetIOCfgDriver();

		/*!
		 *	\brief	����ģ���豸�Ķ���ָ��
		 *
		 *	\param	pDeviceSim	ģ���豸����ָ��
		 *
		 *	\retval	��
		 */
		void SetIODeviceSim(CIODeviceSimBase* pDeviceSim);

		/*!
		 *	\brief	��ȡģ���豸�Ķ���ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CIODeviceSimBase*	ģ���豸����ָ��
		 */
		CIODeviceSimBase* GetIODeviceSim();

		/*!
		 *	\brief	�˲�UnitType��TagAddr�Ƿ���Ч
		 *
		 *	\param	byUnitType	��ǩ����
		 *	\param	lTagAddress	��ǩ��ַ
		 *
		 *	\retval	true--��Ч��false--��Ч
		 */
		bool CheckUnitTypeAndTagAddr(Byte byUnitType, int lTagAddr);

		/*!
		 *	\brief	�õ�FeildPoint MAP��
		 *
		 *	\param	��
		 *
		 *	\retval	MAP_OBJTAG&	
		 */
		MAP_OBJTAG& GetMapobjTag();

		/*!
		 *	\brief	������������ָ��
		 *
		 *	\param	CIODriverBase* ��������ָ��
		 *
		 *	\retval	MAP_OBJTAG&	
		 */
		void SetDriver( CIODriverBase* pDriver );

		/*!
		 *	\brief	��ȡ��������ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CIODriverBase*	��������ָ��
		 */
		CIODriverBase* GetDriver();

		/*!
		 *	\brief	����Э�����ָ��
		 *
		 *	\param	pPrtcl Э�����ָ��
		 *
		 *	\retval	��
		 */
		void SetPrtcl( CIOPrtclParserBase* pPrtcl );

		/*!
		 *	\brief	��ȡЭ�����ָ��
		 *
		 *	\param	��
		 *
		 *	\retval	CIOPrtclParserBase* Э�����ָ��
		 */
		CIOPrtclParserBase* GetPrtcl();

		/*!
		 *	\brief	���ó�ʼ��ʶ
		 *
		 *	\param	bSet	
		 *
		 *	\retval	��
		 */
		void SetQualityFirstFlag( bool bSet );

		/*!
		 *	\brief	�Ƿ����ͨ�����
		 *
		 *	\param	��	
		 *
		 *	\retval	true ���,false �����
		 */
		bool IsStatusUnit();

		/*!
		 *	\brief	����ͨ����ϱ�ʶ
		 *
		 *	\param	bStatusUnit ͨ����ϱ�ʶ	
		 *
		 *	\retval	��
		 */
		void SetStatusUnit( bool bStatusUnit );

		/*!
		 *	\brief	ͨ��������ȡ��ǩ����ָ��
		 *
		 *	\param	nUnitType	��ǩ����
		 *	\param	lTagAddr	��ǩ��ַ
		 *
		 *	\retval	T_lstFieldPoint	FieldPoint�����б�
		 */
		CIOCfgTag* GetTagByIndex( int nUnitType, int lTagAddr);

		/*!
		 *	\brief	��ʼ��IOUnit�µ����ݱ���(FieldPoint)��Ϣ
		 *
		 *	\param	pIOUnit		�豸�ڵ�
		 *
		 *	\retval	FieldPointType	��ǩ�ڵ����
		 */
		virtual int InitVariableInfo(CIOUnit* pIOUnit);

		/*!
		 *	\brief	���ø���:������ݱ�����FeildPoint��ǩ��ַ����
		 *
		 *	\param	FieldPointType* pFieldPoint
		 *
		 *	\retval	int 0Ϊ�ɹ�������ʧ��
		 */
		int UpdateDataVariable( FieldPointType* pFieldPoint );

		/*!
		 *	\brief	���ø���:������ͣ������־
		 *
		 *	\param	block	 OpcUa_True:��ͣ��OpcUa_False:����ͣ
		 *
		 *	\retval	��
		 */
		void SetBlock(bool block);

		/*!
		 *	\brief	�����豸��ǰ�Ƿ���ͣ����
		 *
		 *	\retval	bool �豸�Ƿ���ͣ����
		 */
		bool IsBlock();

		//!�����豸ģ�������
		int SetDevSimPeriod(int mInternal);

	 	/*!
	 	 *	\brief	����·Ϊ���豸�ϸ��±�ǩֵ
	 	 *			�ýӿ���������·Ϊ���豸�Ͻ��и���ֵ�Ĳ����������ǰ��·Ϊ����Ὣдֵ����ת����·Ϊ���豸���и���
	 	 *			ֵ�����������ǰ��·Ϊ����ֱ�Ӹ���ֵ
	 	 *
	 	 *	\param	byUNIT_TYPE,	��ǩUNITYTYPE
	 	 *	\param	nTagAddr,		��ǩ��ַ
	 	 *	\param	pbyData,		��ǩֵ
	 	 *	\param	nDataLen		��ǩֵ����
	 	 *	\param	nTime			ʱ��
	 	 *	\param	bFromLocal		�Ƿ񱾻������ģ����������͵�������õ�����Ӧ
	 	 *
	 	 *	\retval	int 0:�ɹ�, ����:ʧ��
	 	 */
	 	int SetVal2Hot(Byte byUNIT_TYPE, int lTagAddr, Byte* pbyData, 
	 		int nDataLen, long nTime, bool bFromLocal);
         //����·�ӻ��豸ͨ��״̬��ǩд������
         int SetVal2Hot(CIOCfgTag* pTag, Byte byOnLine);
		//!�����豸��������״̬��ǰ����ģ��״̬
		int ProcesHotSim();
	private:
	 	/*!
	 	 *	\brief	����FieldPoint����������UnitType�����ǩ��ַ
	 	 *
	 	 *	\param	FieldPointType*			��ǩ����
	 	 *	\param	CIOCfgDriverDbfParser*	��ǩ��������
	 	 *
	 	 *	\retval	int 0Ϊ�ɹ�������ʧ��
	 	 */
	 	int ProcessFieldPoint(FieldPointType* pFieldPoint, CIOCfgDriverDbfParser* pDrvDbfParser);

	 	//��ȡ�豸������Ϣ
	 	void InitDevConfig( std::string strDevCfgFile );

		
	 	//!����ͨ���ϵ�SOE�¼�
	 	int FireSOEEvent( FieldPointType* pFieldPoint, CKiwiVariant& OldVarint, long nTime );

	// 	//!�õ�������Ĵ������
	// 	HySession* GetEquivallentProxy();

	 	/*!
	 	 *	\brief	����·Ϊ�ӵ��豸�ϸ���ֵ����ת����·Ϊ���豸��ִ��
	 	 *
	 	 *	\param	pPeerSvrSession		�������ָ��
	 	 *	\param	uNodeId				��ǩ��NodeId
	 	 *	\param	cmdVal				����ֵ
	 	 *	\param	uCmdParam			ֵ����
	 	 *
	 	 *	\retval	int 0Ϊ�ɹ�������ʧ��
	 	 */
	 	//int SyncVal2HotDev( HySession* pPeerSvrSession, UaNodeId uNodeId, Byte cmdVal, UaByteString uCmdParam );

		//!�õ��豸ģ�ⷽʽ
		Byte GetSimMode();
		
	private:

		//��ʼ����ͨ���ݲɼ��ڵ㵽m_strMapObjTag
		int  InitMapObjTag();

		//��ʼ���ַ�����ַͨ���ɼ��ڵ㵽m_mapObjTag
		int  InitStrAddMapObjTag();

		//�������ӻ��豸ͨѶ״̬��
		bool ProcessDevCommValue(Byte byOnLine);	

		//ͨ���豸���ƻ�ȡIOCfg��ǩ���
		CIOCfgTag* GetDevCommIoCfgTag(std::string DecCommName);

		bool SetDevCommFp4IoCfgTag(CIOCfgTag* pIoCfgTag, Byte byOnLine);

		bool AddDevComCfgTag2UnitMap(CIOCfgTag* pIoCfgTag, std::string DecCommName);
	public:
		//!����
		static std::string s_ClassName;

		//!�������ݰ�����
		int		m_byTxTimes;

		//!���հ�����
		int		m_byRxTimes;

	protected:
		//!��UnitType(��ǩ����)+TagAddr(��ǩ��ַ)Ϊ������FieldPoints MAP��
		MAP_OBJTAG m_mapObjTag;

		//!���ַ���(ͨ����ַ)+TagAddr(��ǩ��ַ)Ϊ������FieldPoints MAP��
		STRMAP_OBJTAG m_strAddMapObjTag;
		//!�豸״̬�ṹ
		tIOScheduleUnitState* m_pUnitState;

		//!ģʽ�豸����ָ��
		CIODeviceSimBase* m_pDeviceSim;

		// !������ϢIOPort����ָ��
		CIOPort* m_pIOPort;

		//!�豸��Ӧ��IOUnit����ָ��
		CIOUnit* m_pIOUnit;

		//!��ǩ�����Ϣ�������
		CIOCfgUnit* m_pIOUnitCfg;

		//!ģ�ⶨʱ��ID��
		int m_nTimerID;

		//!��������ָ��
		CIODriverBase* m_pDriver;

		//!Э�����ָ��
		CIOPrtclParserBase* m_pPrtcl;

		//!����ͨ����ϵı�־,�����յ�STATUSUNITָ��
		bool	m_bStatusUnit;

		//!�״�����Ʒ�ʵı�־���������־Ϊtrue������SetOnline�ӿ������ñ��豸�����б�ǩ��Ʒ�ʣ�����ֻ��Ϊ��ʱ�����ñ�ǩ��Ʒ��
		bool	m_bSetQualityFirstFlag;

		//!��־��Ӧ��IOUnit�����Ƿ�ɾ��
		bool m_bIsBlocked;

	};
	
	//!���ȫ�ֱ���
	extern IODRIVERFRAME_VAR  GenericFactory<CIODeviceBase> g_DrvDeviceFactory;

}

#endif
