/*!
 *	\file	FILE_BASE$.$FILE_EXT$
 *
 *	\brief	ͨ��Modbus�������豸��ͷ�ļ�
 *
 *	$Header: ?/MACS-SCADA 3.2 �������/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusDevice.h, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _FEP_MODBUS_DEVICE_20081030_H_
#define _FEP_MODBUS_DEVICE_20081030_H_

#include "FepModbusDrvH.h"

typedef struct {
	int bEnable;			   //! 0---�رն˿�; 1---�����˿ڡ�Ĭ��1��
	int nSndDelay;			  //! ������ʱ�����롣Ĭ��100����С5���룩
	int nTimeOut;			  //! ��ʱ�����롣Ĭ��1000
	int nRequestCount;		  //! �˿ڵ�����֡����
	int nSettingGroupCount;	  //! ��������ֵ��֡����
	int nSettingGroupCycl;	  //!Ѳ�����ֵ���־����
	bool bExceptionAsOnline; //!�쳣��Ӧ��Ϊ����Ӧ��Ĭ��1��0��ʾ�쳣��Ӧ����Ϊ�豸���ߵ����ݡ�
	int nCRCOrder;			  //! 0---CRC���ֽ���ǰ; 1-CRC���ֽ���ǰ��Ĭ��0��
}PORT_CFG;					//! �˿����������Ϣ

typedef struct {
	int Enable;			//0---ʹ����ʧЧ; 1---ʹ������Ч��Ĭ��1��
	int Function;		//������
	int StartingAddress;//��ʼ��ַ
	int PointCount;		//����
	int SwapCode;		//0---����ת����; 1---��ת���ݡ�Ĭ��0��
}REQUEST_CFG;			//��������������Ϣ

typedef struct {
	std::string strDoTagName;		//!������ȡ��ֵ���ǩ��
	int iReadCount;					//!��ȡ��ֵ��������ݼ���
	int iReadCountCfg;				//!��ȡ��ֵ�����ô���
	int Function;		//������
	int StartingAddress;//��ʼ��ַ
	int PointCount;		//����
	int SwapCode;		//0---����ת����; 1---��ת���ݡ�Ĭ��0��
}SETTINGGROUP_CFG;			//��������ֵ��֡������Ϣ

namespace MACS_SCADA_SUD
{	
	/*!
	*	\class	CFepModbusDevice
	*
	*	\brief	TEP-I-Fֱ����΢�����װ���������豸��
	*/
	class FEPMODBUSDRV_API CFepModbusDevice : public CIODeviceBase
	{
	public:
		/*!
		*	\brief	ȱʡ���캯��
		*/
		CFepModbusDevice(void);

		/*!
		*	\brief	ȱʡ��������
		*/
		~CFepModbusDevice(void);	

	public:
		/*!
		*	\brief	��ʼ��
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int Init();

		/*!
		*	\brief	���챾����·����ʱ�ı�������֡(����ң��)
		*
		*	\param	pBuf		֡�׵�ַ
		*
		*	\retval	int	֡����
		*/
		virtual int BuildLocalNormalFrame( Byte* pBuf );

		/*!
		*	\brief	�������֡
		*
		*	\param	pBuf		֡�׵�ַ
		*
		*	\retval	int	֡����
		*/
		virtual int BuildTestFrame( Byte* pBuf );

		/*!
		*	\brief	��Уʱ֡���߼�ʵ��
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	���豸��ص�������ѯ֡����SOE��
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildSpecialReqFrame( Byte* buf );

		/*!
		*	\brief	����ѯ֡
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildQueryFrame( Byte* buf );

		/*!
		*	\brief	��DOAң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң�ر�ǩ��ַ		
		*	\param	byVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDOAReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	��DOBң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң�ر�ǩ��ַ		
		*	\param	byVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDOBReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	��DOCң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң�ر�ǩ��ַ		
		*	\param	byVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDOCReqFrame( Byte* buf, int lTagAddr, Byte byVal );
		/*!
		*	\brief	��DOCң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	byVal		ֵ
		*	\param	nLen		���ݳ���
		*
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDOMReqFrame( Byte* buf, int lTagAddr, Byte byVal, int nLen);

		/*!
		*	\brief	��DODң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң�ر�ǩ��ַ		
		*	\param	byVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDODReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	��DOFң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң�ر�ǩ��ַ		
		*	\param	byVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDOFReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	��DOGң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң�ر�ǩ��ַ		
		*	\param	byVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDOGReqFrame( Byte* buf, int lTagAddr, Byte byVal );

		/*!
		*	\brief	��AOAң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	fVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAOAReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	��AOBң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	fVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAOBReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	��AOC��AOFң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	byVal		ֵ
		*	\param	nLen		���ݳ���
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAOCReqFrame(
			Byte* buf, int lTagAddr, Byte* byVal, int nLen );

		/*!
		*	\brief	��AODң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	fVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAODReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	��AOEң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	fVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAOEReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	��AOFң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	fVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAOFReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	��AOGң��֡
		*
		*	\param	buf			֡�׵�ַ
		*	\param	lTagAddr	ң����ǩ��ַ		
		*	\param	fVal		ֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildAOGReqFrame( Byte* buf, int lTagAddr, float fVal );

		/*!
		*	\brief	����ң����Ӧ֡��������
		*			������Ӧң�ŵ��ֵ,��bit���£�������Ϊ1��2��
		*
		*	\param	UnitType		�˺���ֻ����MACSMB_DIA��MACSMB_DIB
		*	\param	pbyData			�����׵�ַ
		*	\param	nStartAddr		��ʼ��ǩ��ַ
		*	\param	nPointCount		��ǩ����
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			>0��ʾ�ɹ��������ʾʧ��;>0ʱ����ֵ��ʾ�˴α仯�ĵ�ĸ���
		*/
		virtual int UpdateDI( int UnitType, 
			Byte* pbyData, int nStartAddr, int nPointCount, bool bFromLocal );

		/*!
		*	\brief	����Holding Register�е����ݸ��µ�ֵ��������Ϊ3��
		*
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	void
		*/

		virtual int UpdateHoldingAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );
		/*!
		*	\brief	���豸�����ң��֡
		*
		*	��ϸ�ĺ���˵������ѡ��
		*
		*	\param	pbyData	֡�׵�ַ��һ��������Ӧһ����
		*	\param	tCtrlRemoteCmd ��ǰ��ָ��
		*	\param	fCmdVal ����ֵ
		*
		*	\retval	����ֵ	֡���ȣ�һ������ֵ��Ӧһ����
		*
		*	\note		ע�������ѡ��
		*/
		virtual int BuildSplCtrlFrame(Byte* pbyData,const tIOCtrlRemoteCmd& tCtrlRemoteCmd,float& fCmdVal);
		/*!
		*	\brief	����Input Register�е����ݸ��µ�ֵ��������Ϊ4��
		*
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	void
		*/
		virtual int UpdateInputAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	����������Ҫ�ļĴ�����ֵ
		*
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int UpdateRegister( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	����Holding Register�е����ݸ��µ�ֵ֮ǰ��Ҫ���е����⴦��
		*			��������Ϊ3��
		*	\param	UnitType
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int BeforeUpdateHoldingAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	����Input Register�е����ݸ��µ�ֵ֮ǰ��Ҫ���е����⴦��
		*			��������Ϊ4��
		*	\param	UnitType
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int BeforeUpdateInputAI( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	����AI��Ӧ֡��������
		*			�������а�����DI���ֵ��UnitTypeΪMACSMB_DID��MACSMB_DIE��
		*	\param	UnitType		�˺���ֻ����MACSMB_DID��MACSMB_DIE
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			>0��ʾ�ɹ��������ʾʧ��;>0ʱ����ֵ��ʾ�˴α仯�ĵ�ĸ���
		*/
		template < bool bHighFirst>
		int UpdateDIinReg( int UnitType, 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );		

		/*!
		*	\brief	�����ļ�����������֡������
		*
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		virtual int UpdateFileInfo( 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	����ң��֡
		*
		*	\param	pbyBuf		֡�׵�ַ
		*	\param	wRegAddr	�Ĵ�����ַ		
		*	\param	wRegNo		�Ĵ�������
		*	\param	pbyRegVal	��д��Ĵ��������������WORDֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildCtrlMultiRegsFrame( Byte* pbyBuf,
			const WORD wRegAddr, const WORD wRegNo, const Byte* pbyRegVal);

		/*!
		*	\brief	����ң��֡
		*
		*	\param	pbyBuf		֡�׵�ַ
		*	\param	wRegAddr	�Ĵ�����ַ		
		*	\param	wRegNo		�Ĵ�������
		*	\param	wRegVal		��д��Ĵ��������������WORDֵ
		*
		*	\retval	int			֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildCtrlMultiRegsFrame( 
			Byte* pbyBuf, const WORD wRegAddr, const WORD wRegNo, const WORD* wRegVal);

		/*!
		 *	\brief	���OX10��������־��Ϣ.
		 *	\param	nLen ��������ݳ���
		 *
		 *	\retval	��
		 */
		void WriteLogForMO( int nLen );

		/*!
		*	\brief	��¼��Ҫ�ļĴ�������Ҫ��ֻȡ�Ĵ�����ĳλ��DI��AI����Ҫ��
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		virtual int InitNeedRegister();

		/*!
		*	\brief	��¼��Ҫ�ļĴ�������Ҫ��ֻȡ�Ĵ�����ĳλ��DI��AI����Ҫ��
		*
		*	\param	iUnitType		UNITTYPE
		*	\param	iDivisor		����
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*/
		int InitNeedRegByUnitType(int iUnitType, int iDivisor);

		/*!
		*	\brief	����ң�ر�ǩ��ַ��ȡң�ؽ���֡
		*
		*	\param	lTagAddr		ң�ر�ǩ��ַ
		*
		*	\retval	struCtrlFollowReq*	ң�ؽ���֡
		*/ 
		struCtrlFollowReq* GetCtrlFwReqByTagAddr( int lTagAddr );	

		/*!
		*	\brief	��ʱ��ת���ɺ�����
		*
		*	\param	strTime	Ҫת����ʱ��	
		*
		*	\retval	MACS_INT64	��������Ϊ-1���ʾת��ʧ��
		*/
		//OpcUa_Int64 CaculateMSNum( DAILYTIME strTime );
		long CaculateMSNum( DAILYTIME strTime );
	
		/*!
		*	\brief	m_bFlag��λ
		*/
		void SetFlag()
		{
			m_bFlag = !m_bFlag;
		};

		/*!
		*	\brief	����豸��ǩ
		*/
		int GetDevType()
		{
			return m_nDevType;
		};

		/*!
		*	\brief	�豸�Ƿ���Уʱ֡
		*/
		bool HasCheckTime()
		{
			return m_bCheckTime;
		};

		/*!
		*	\brief	�豸�Ƿ���SOE֡
		*/
		bool HasSOE()
		{
			return m_bSOE;
		};

		/*!
		*	\brief	������ȡ��ֵ��֡��
		*/
		int GetSETTINGGROUP()
		{
			return m_PortCfg.nSettingGroupCount;
		}

		/*!
		*	\brief	����豸��������ֵ������
		*/
		int GetRdSettingGpCycl()
		{
			return m_PortCfg.nSettingGroupCycl;
		}

		/*!
		*	\brief	����豸Уʱ����
		*/
		int GetCheckTimeCycl()
		{
			return m_nCheckTimeCycl;
		};

		/*!
		*	\brief	����豸Уʱʱ���(Сʱ) Add by: Wangyanchao
		*/
		int GetCheckTimeHour()
		{
			return m_nCheckTimeHour;
		};

		/*!
		*	\brief	����豸Уʱʱ���(����) Add by: Wangyanchao
		*/
		int GetCheckTimeMin()
		{
			return m_nCheckTimeMin;
		};

		/*!
		*	\brief	����豸�˿�������Ϣ
		*/
		PORT_CFG GetPortCfg()
		{
			return m_PortCfg;
		};

		/*!
		*	\brief	����豸�豸������ߴ���
		*/
		int GetMaxOffLine()
		{
			return m_nMaxOfflineCount;
		};

		/*!
		*	\brief	��ǰ�Ƿ���ҪУʱ
		*/
		bool NeedCheckTime()
		{
			return m_bNeedCheckTime;
		};

		/*!
		*	\brief	������ҪУʱ��־
		*/
		void SetNeedCheckTime( bool bNeedChkTm)
		{
			m_bNeedCheckTime = bNeedChkTm;
		};

		/*!
		*	\brief	��ǰ�Ƿ����ֵ������
		*/
		bool ReadSettingGroupPD()
		{
			return m_bNeedReadSettingGroup;
		}

		/*!
		*	\brief	������Ҫ����ֵ���־
		*/
		void SetNeedRdSettingGp( bool bNeedRDSettingGp)
		{
			m_bNeedReadSettingGroup = bNeedRDSettingGp;
		};


		/*!
		*	\brief	��õ�ǰ����֡���
		*/
		short GetCurrReq( )
		{
			return m_shCurrReq;
		};

		/*!
		*	\brief	��ò���֡����Ч��Ϣ
		*/
		bool GetTestReqStatus( )
		{
			return m_bTestReq;
		};

		/*!
		*	\brief	�ϴη����Ƿ���SOE
		*/
		bool IsLastSendSOE( )
		{
			return m_bSOESended;
		};

		/*!
		*	\brief	�����ϴη����Ƿ���SOE
		*/
		void SetLastSendSOE(bool bLastSendSOE )
		{
			m_bSOESended = bLastSendSOE;
		};

		/*!
		*	\brief	��ôӻ����豸�ĳ�ʱ����
		*/
		int GetSlaveTimeOuts( )
		{
			return m_nSlaveTimeOuts;
		};

		/*!
		*	\brief	���ôӻ����豸�ĳ�ʱ����
		*/
		void SetSlaveTimeOuts(int nSlaveTimeOuts )
		{
			m_nSlaveTimeOuts = nSlaveTimeOuts;
		};

		/*!
		*	\brief	��ø��豸�Ƿ��ǹ㲥Уʱ
		*/
		int IsBroadChckTime( )
		{
			return m_bBroadcast;
		};

		/*!
		*	\brief	�Ƿ�����������֡
		*/
		int HasSpecialReq( )
		{
			return ( m_nSpecialReqCount > 0 );
		};

		void SetTrySendCount( int nTry )
		{
			m_nTrySendCount = nTry;
		}

		void IncrTrySendCount()
		{
			m_nTrySendCount++;
		}
		
		int GetTrySendCount()
		{
			return m_nTrySendCount;
		}
		
		//!��д��������������,���������Ϣ
		virtual void SetHot( Byte byHot );


		//! �������
		std::string GetClassName();

		//!�ж��豸�Ƿ������������
		bool IsReqeustExited( Byte Funcode, WORD StartAddr, WORD PointCount );

		//����豸�Ƿ�ֻ��һ�������Ҹ������Э������ϻ����Ա�����Ϊ��Ӧ��������
		//��������ȿ���������Ҳ��������Ӧ��
		//XX 01 03 XX 00 YY XX XX 
		//XX 02 03 XX 00 YY XX XX 
		//���е�YY ��ΧΪ17~24ʱ���ȿ�������Ҳ��������Ӧ��
		bool IsFuzzy();

		//�豸���쳣Ӧ���Ƿ���Ϊ���ߵ�����
		bool IsExceptionAsOnline();

	protected:
		/*!
		*	\brief	����16λ����У���--CRC
		*
		*	1��	��һ16λ�Ĵ���λȫ1��
		*	2��	���������ݵĸ�λ�ֽ����Ĵ����ĵͰ�λ������Ĵ�����
		*	3��	���ƼĴ��������λ��0���Ƴ��ĵͰ�λ�����־λ��
		*	4��	���־λ��1������A001���Ĵ��������־λ��0����������3��
		*	5��	�ظ�����3��4��ֱ����λ��λ��
		*	6��	�����һλ�ֽ���Ĵ�����
		*	7��	�ظ�����3��5��ֱ�����б������ݾ���Ĵ��������λ8�Σ�
		*	8��	��ʱ�Ĵ����м�ΪCRCУ���룬���λ�ȷ��ͣ�
		*
		*	\param	datapt		������ʼλ��
		*	\param	bytecount	�ֽڸ���
		*
		*	\retval	WORD	16λ������У���
		*/
		WORD CRC(const Byte * datapt,int bytecount);

		/*!
		*	\brief	��ȡ�豸����
		*
		*	\param			
		*
		*	\retval	int	 0��ʾ�ɹ���������ʾʧ��
		*/
		virtual int InitCfg();

		/*!
		*	\brief	·����ʲô����
		*
		*	\param	strPathName	·����
		*
		*	\retval	int	ֵΪDIR__TYPE��FILE_TYPE��NONE_TYPE
		*/
		int PathType( std::string strPathName );

		/*!
		*	\brief	����ң����Ӧ֡�������򣬸�����ӦAI���ֵ��
		*			��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
		*
		*	\param	bSigned			�з��š���true���޷��š���false
		*	\param	bHighFirst		��λ��ǰ����true����λ��ǰ����false
		*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
		*							��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		template <bool bSigned, bool bHighFirst>
			int UpdateWordAI(int UnitType, 
				Byte* pbyData,int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	����ң�����Ӧ֡��������ӦAI��(�Ĵ�����Nλ)��ֵ
		*
		*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
		*							��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		template <bool bHighFirst>
			int UpdateAIinReg( int UnitType,
				Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	����ң�����Ӧ֡��������ӦAI��(�ַ�������)��ֵ
		*
		*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
		*							��ģ�庯�����ڴ����ǩֵΪ���ֵ�AI��
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		template <bool bHighFirst>
		int UpdateAIString( int UnitType,
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	����ң����Ӧ֡�������򣬸�����ӦAI���ֵ��
		*			��ģ�庯�����ڴ����ǩֵΪ˫�ֵ�AI��
		*
		*	\param	bSigned			�з��š���true���޷��š���false
		*	\param	bHighFirst		��λ��ǰ����true����λ��ǰ����false
		*	\param	bHiWdFirst		������ǰ����true��������ǰ����false
		*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
		*							��ģ�庯�����ڴ����ǩֵΪ˫�ֵ�AI��
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		template <bool bSigned, bool bHighFirst, bool bHiWdFirst>
			int UpdateDWordAI( int UnitType,
				Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal);

		/*!
		*	\brief	����ң�����Ӧ֡��������ӦAI���ֵ
		*			�˺������ڴ������ض����ɵ�AI�㣨��������ģ�庯�������˵ģ�
		*
		*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		int UpdateOtherAI( int UnitType, 
				Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );		

		/*!
		*	\brief	��������豸������Ϣ
		*
		*	\param	file_name		�����ļ�·����
		*
		*	\retval	void
		*/
		void GetSpecialCfg(std::string file_name);

		/*!
		*	\brief	��ö˿����������Ϣ
		*
		*	\param	file_name		�����ļ�·����
		*
		*	\retval	void
		*/
		void GetPortCfg(std::string file_name);

		/*!
		*	\brief	�����ѯ֡������Ϣ�������ڵ�����GetPortCfg��������ܵ��ã�
		*			����û������
		*
		*	\param	file_name		�����ļ�·����
		*
		*	\retval	void
		*/
		void GetReqCfg(std::string file_name);

		/*!
		*	\brief	��ò���֡��Ϣ
		*
		*	\param	file_name		�����ļ�·����
		*
		*	\retval	void
		*/
		void GetTestCfg(std::string file_name);

		/*!
		*	\brief	���ң�ذ���֡������Ϣ
		*
		*	\param	file_name		�����ļ�·����
		*
		*	\retval	void
		*/
		void GetFollowReqCfg(std::string file_name);

		/*!
		*	\brief	��Уʱ֡���������2��TITANS��ֱ������
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		int BuildTITANSCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	��855Э���Уʱ֡ Add by: Wangyanchao
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		int Build855CheckTimeFrame( Byte* buf );
		
		/*!
		*	\brief	��Уʱ֡������NEXUS1272��ȱ��豸��Ч��
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		int BuildNEXUSCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	��Уʱ֡������50V�����豸DB-MLEs-PL��Ч��
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		int  BuildDBMLEsPLCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	ʮ������ת��Ϊ��Ӧ��BCD��
		*
		*	\param	int x		ʮ������	
		*
		*	\retval	int		��Ӧ��BCD����ֵ
		*
		*	\note		
		*/
		int  Dec2Bcd(int x);

		/*!
		*	\brief	���豸Уʱ֡����
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDevCheckTimeFrame( Byte* buf );

		/*!
		*	\brief	������豸��ص�������ѯ֡
		*
		*	\param	buf		֡�׵�ַ
		*	\param	nIndex	����֡��ţ������ڼ��������ѯ֡��
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		virtual int BuildDevSplReqFrame( Byte* buf, int nIndex );

		/*!
		*	\brief	д�ļ���¼֡(Ŀǰֻ֧��д�����ļ�����)
		*
		*	\param	byBuf			֡�׵�ַ
		*	\param	strFileReq		�ļ���¼����
		*	\param	pRegVal			��¼����
		*	\param	bIsBroadcast	�Ƿ�Ϊ�㲥
		*
		*	\retval	int		֡��
		*/
		virtual int BuildWriteRef( Byte* byBuf, FILEREQ strFileReq,
			const Byte* pbyRegVal, bool bIsBroadcast = false);

		/*!
		*	\brief	���ļ���¼֡(Ŀǰֻ֧�ֶ������ļ�����)
		*
		*	\param	byBuf			֡�׵�ַ
		*	\param	strFileReq		�ļ���¼����
		*
		*	\retval	int		֡��
		*/
		virtual int BuildReadRef( Byte* byBuf, FILEREQ strFileReq);

		/*!
		*	\brief	��TITANSֱ�����Ķ�ȡ�澯֡
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		int BuildTEPIFReadWarnFrame( Byte* buf );

		/*!
		*	\brief	����AI��Ӧ֡��������
		*			��������2����TINTANSֱ������SOE��ǩֵ������SOE
		*			��UnitTypeΪMACSMB_TEPIFSOE��
		*	\param	UnitType		�˺���ֻ����MACSMB_TEPIFSOE
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		int UpdateTEPIFSOE( int UnitType, 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	����AI��Ӧ֡�������� NEXUS1272��ȱ�������
		*			��UnitTypeΪMACSMB_PowFactor��
		*
		*	\param	UnitType		��ֵ��ӦmacsMB.dbf�ļ��е�UnitType���ֵ��
		*							�˺���ֻ����MACSMB_PowFactor
		*	\param	pbyData			�����׵�ַ,ע�ⲻ��֡�׵�ַ
		*	\param	nStartAddr		��ʼ�Ĵ�����ַ
		*	\param	nRegCount		�Ĵ�������
		*	\param	bFromLocal		�Ƿ񱾵ط���֡����Ӧ
		*
		*	\retval	int			0��ʾ�ɹ��������ʾʧ��
		*/
		int Update_PowFactor( int UnitType, 
			Byte* pbyData, int nStartAddr, int nRegCount, bool bFromLocal );

		/*!
		*	\brief	��ȡ��ǩ������ֵ
		*/
		bool IsDIValueChange( int UnitType, int nTagAddr, Byte& byOldVal, Byte& byNewVal, std::string& strTagName );

		/*!
		*	\brief	�鴥����ȡ��ֵ�������֡
		*
		*	\param	buf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����bufҪԤ�ȷ���ռ�
		*/
		int BuildReadSettingGroupFrame( Byte* pbyBuf );

		/*!
		*	\brief	��ö���ֵ��֡��Ϣ
		*
		*	\param	file_name		�����ļ�·����
		*
		*	\retval	void
		*/
		void GetSettingGroupCfg(std::string file_name);

		/*!
		*	\brief	��Уʱ֡
		*
		*	\param	pbyBuf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����pbyBufҪԤ�ȷ���ռ�
		*/
		int BuildF650CheckTimeFrame( Byte* pbyBuf );

		/*!
		*	\brief	�Ƿ�����
		*
		*	\param	iYear����λ����    
		*
		*	\retval bool true���꣬false������
		*
		*	\note	
		*/
		bool isLeap( int iYear );

		/*!
		*	\brief	��������
		*
		*	\param	int iYear,in iMon����bool bFrom1970 =true�������ʱ��ԭ����iyear/iMon������
		*    ���򣬼����Ա���ʼ�������µ�����
		*
		*	\retval int ����
		*
		*	\note	
		*/
		int Days( int iYear,int iMon,bool bFrom1970 = false );

		/*!
		*	\brief	��Уʱ֡
		*
		*	\param	pbyBuf		֡�׵�ַ
		*
		*	\retval	int		֡��
		*
		*	\note		����pbyBufҪԤ�ȷ���ռ�
		*/		
		int BuildL30CheckTimeFrame( Byte* pbyBuf );

		//BCD��תHEX
		BYTE BCD2HEX(BYTE bcd);

		//HEXתBCD��
		BYTE HEX2BCD(BYTE hex);

		//��Уʱ֡(����LD�¿���װ����Ч)
		int BuildLDCheckTimeFrame( BYTE* buf );

		//��Уʱ֡(����SEPCOS�豸��Ч)
		int BuildSEPCOSCheckTimeFrame( BYTE* buf );

		//��Уʱ֡(���Դ�ȫPLC��S7-200���豸��Ч)
		int BuildDQPLCCheckTimeFrame( BYTE* buf );

		//��Уʱ֡(���Դ�����˳��ֱ�����豸��Ч)
		int BuildDLLSJZLPCheckTimeFrame( BYTE* buf );

		//��Уʱ֡(���������ƶ��豸��Ч)
		int BuildZSZDCheckTimeFrame( BYTE* buf );

		//��Уʱ֡(����������1500V���ع�S7-300�豸(��������1��2����)��Ч)
		int BuildDLSIEMENSS73CheckTimeFrame( BYTE* buf );

		//��Уʱ֡(�����Ϻ�����1500V���ع�U-MLEs-SC�豸(��������1��2����)��Ч)
		int BuildDLUMLESSCCheckTimeFrame( BYTE* buf );

		//��Уʱ֡(�����Ϻ�����1500V���ع�S7-200�豸(��������1��2����)��Ч)
		int BuildDLSIEMENSS72CheckTimeFrame( BYTE* buf );

		//��Уʱ֡(�����෿���ķ�CSC268�豸Уʱ)
		int BuildSFCSC268CheckTimeFrame(BYTE* buf);

	public:
		//! ����
		static std::string s_ClassName;	
		std::vector<SETTINGGROUP_CFG> vecSettingGroup;	//! ��Ч����
		bool m_bRetryPeriod;					//�����豸�Ƿ�÷��ʹ�������ѯ����
	protected:
		/*
		*	�龯���ȡ֡ʱ�Ĳο���־��Ϊtrue��130�Ĵ�����Ϊfalse��150�Ĵ�����
		*	ֻ����2TITANSֱ������Ч
		*/
		bool m_bFlag;

		//////////////////////////////////////////////////////////////////////
		//! �豸����������Ϣ
		int	 m_nDevType;				//! �豸���ͣ��μ��궨��
		bool m_bCheckTime;				//! �Ƿ���ҪУʱ֡
		bool m_bSOE;					//! �Ƿ���ҪSOE֡
		int m_nCheckTimeCycl;			//! Уʱ����
		int m_nCheckTimeHour;	//! Уʱʱ���(Сʱ) Add by: Wangyanchao
		int m_nCheckTimeMin;	//! Уʱʱ���(����) Add by: Wangyanchao

		//! �˿����������Ϣ
		PORT_CFG m_PortCfg;				//! �˿����������Ϣ		
		int m_nMaxOfflineCount;			//! �豸������ߴ���

		bool m_bNeedCheckTime;			//! ��ǰ�Ƿ���ҪУʱ
		short m_shCurrReq;				//! ��ǰ����ţ�0��m_nValidReqCount��1

		//! �ϴη����Ƿ���SOE����ֻ����2TITANSֱ������Ч
		bool m_bSOESended;	

		int m_nCheckTimeAddr;			//! Уʱ��ʼ��ַ
		bool m_bBroadcast;				//! �Ƿ�֧�ֹ㲥Уʱ
		int	 m_nValidReqCount;			//! ��Ч����֡����

		std::vector<REQUEST> Request;	//! ��Ч����
		REQUEST m_reqTest;				//! ����֡
		bool m_bTestReq;				//! ����֡��Ч��־

		//! ң�ط����������ã�ÿ��ң�غ�����������������õķ��ŵ㡣
		std::map<int, struCtrlFollowReq*>  m_mapCtrlFollowReq;	

		//! ��¼�ӻ����豸�ĳ�ʱ�������Ա��жϴӻ����豸����������
		int m_nSlaveTimeOuts;	

		//! ������ѯ֡���
		int m_nSpecialReqCount;			//! ������ѯ֡����
		int m_nCurSpecialReq;			//! ��ǰ��������ţ�0��m_nSpecialReqCount��1

		//! �������飬Ϊ��ȡ�Ĵ�����ֵ��nλ
		WORD m_wdBitMask[16];

		//! ����ļĴ�����Ϊ��ʾ�Ĵ�����Nλ��AI��DI��ʹ�ã�
		std::map<int,Byte*> m_mapRegister;

		int m_nTrySendCount;	//! ��ʼ���ӳɹ���ķ��ʹ���		

		//! �����ļ���
		std::string m_strCfgFileName;
				
		int m_iCurrSettingGroup;//!��ǰ��ֵ��֡��
		bool m_bNeedReadSettingGroup; //!����ֵ�����ڵ�
	};
}

#endif
