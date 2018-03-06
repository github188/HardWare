/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104��վЭ����ͷ�ļ�
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Prtcl.h 6     09-09-28 16:41 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 09-09-28 16:41 $
 *	$Revision: 6 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOMACSIEC104PRTCL_20080812_H_
#define _IOMACSIEC104PRTCL_20080812_H_

#include "RTDB/Server/DBDriverFrame/IOPrtclParser.h"
#include "FepIEC104DrvH.h"

namespace MACS_SCADA_SUD
{	
	class CIODeviceBase;

	/*!
	 *	\class	CFepIEC104Prtcl
	 *
	 *	\brief	104��վЭ����
	 */
	class IOMACSIEC104_API CFepIEC104Prtcl:
		public CIOPrtclParser
	{
	public:
		CFepIEC104Prtcl(void);
		~CFepIEC104Prtcl(void);

	public:

		/*!
		*	\brief	����һ�����յ����ݰ�
		*
		*	\param	pBuf: ֡�׵�ַ
		*	\param	iLen: ֡����
		*
		*	\retval bool true��ʾ�ɹ�����һ����Ч֡��false��ʾʧ��
		*
		*	\note	
		*/
		virtual OpcUa_Int32 RxDataProc( OpcUa_Byte* pDataBuf, OpcUa_Int32& nLen );

		/*!
		*	\brief	������֡
		*	���ӿ����������ڷ�������ǰ����,ͨ������bTrigTimer�������Ƿ�������ʱ��ʱ��
		*
		*	\param	pbyData: Ҫ���͵�����
		*	\param	nDataLen: Ҫ���͵����ݵĳ���
		*	\param	bTrigTimer: ���ӿڷ��أ����������Ƿ�������ʱ��ʱ���ı�־��
		*						true��������ʱ��ʱ����false������
		*
		*	\retval��0Ϊ�ɹ�������ʧ��
		*/
		virtual OpcUa_Int32 BuildRequest( OpcUa_Byte* pbyData, OpcUa_Int32& nDataLen );		

		/*!
		*	\brief	�������
		*
		*	\retval string	����
		*
		*	\note	
		*/
		std::string GetClassName();

		/*!
		*	\brief	�õ���С�İ�����
		*
		*	\retval��OpcUa_Int32 ��С����
		*
		*	\note	
		*/
		virtual OpcUa_Int32 GetMinPacketLength();

		/*!
		*	\brief	���ӳɹ������������ص�����������������Э��
		*
		*	\param	pPeerIP: �Զ� IP
		*
		*	\retval��0Ϊ�ɹ�������ʧ��
		*
		*	\note	
		*/
		virtual OpcUa_Int32 OnConnSuccess(std::string pPeerIP );

		//! ���ݱ������ݻ������ַ�����byRcvOrSnd��1ΪRcv��0ΪSnd��
		std::string GetCommData( OpcUa_Byte* pbyBuf, OpcUa_Int32 nDataLen, OpcUa_Byte byRcvOrSnd );


		//! ����
		static std::string s_ClassName;

		//! ��¼���η������ݵĳ���
		OpcUa_Int32 m_nSndLen;

	private:
		/*!
		*	\brief	�ӽ��ջ���������ȡһ��֡
		*
		*	\retval��bool	true��ʾ�ɹ���ȡһ����Ч֡��false��ʾʧ��
		*/
		bool ExtractPacket();

		/*!
		*	\brief	������ȡ�����Ľ�������֡����д����������Ҫʱ���ͱ�λ
		*
		*	\param	bFromLocal: �Ƿ�Ϊ���ط���֡����Ӧ֡
		*
		*	\retval��void
		*
		*	\note	
		*/
		void ProcessRcvdPacket( bool bFromLocal );

	///////////////////////////////////////////////////////////////
	public:


		/*!
		*	\brief	���챾����·����ʱ�ı�������֡(����ң��)
		*
		*	\param	pBuf: ֡�׵�ַ
		*
		*	\retval OpcUa_Int32 ���֡�ĳ���
		*
		*	\note	
		*/
		OpcUa_Int32 BuildLocalNormalFrame( OpcUa_Byte* pBuf );


		/*!
		*	\brief	���챾����·����ʱ�ı��شӻ�����֡(����ң��)
		*
		*	\param	pBuf: ֡�׵�ַ
		*
		*	\retval OpcUa_Int32 ���֡�ĳ���
		*
		*	\note	
		*/
		OpcUa_Int32 BuildLocalStandbyFrame( OpcUa_Byte* pBuf );

		/*!
		*	\brief	��֤�Ƿ�Ϊ��ЧIP
		*
		*	\param	ip: Ҫ��֤��IP�ַ�����ָ��
		*
		*	\retval bool true��ʾipΪһ����Ч��IP��ַ��false��ʾip��Ч
		*
		*	\note	
		*/
		bool IsValidIP( const char* ip );

		/*!
		*	\brief	����������¹���
		*
		*	\retval void
		*
		*	\note	
		*/
		void ResetAllDeviceNr(); 

		/*!
		*	\brief	����������¹���
		*
		*	\retval void
		*
		*	\note	
		*/
		void ResetAllDeviceNs();

		/*!
		*	\brief	���������豸������״̬
		*
		*	\retval void
		*
		*	\note	
		*/
		void UpdateDevState();

		/*!
		*	\brief	�����豸��ַ��ȡ�豸������
		*
		*	\param	Addr: �豸��ַ
		*	\param	DevNo: �豸�����ţ����ã�
		*
		*	\retval bool true��ʾ��ȡ�ɹ���false��ʾ��ȡʧ��
		*
		*	\note	
		*/
		bool GetDeviceNoByAddr( OpcUa_Int32 Addr, OpcUa_Int32& DevNo );

		/*!
		*	\brief	�����·״̬
		*
		*	\retval bool true--������һ���豸���ߣ�false--�����豸������
		*
		*	\note	
		*/
        bool GetLinkState();

		/*!
		*	\brief	���MacsDriver.ini����·���ò���
		*
		*	\param	strAppName: ������
		*
		*	\retval void
		*
		*	\note	
		*/
		void InitConfigPar( char* strAppName );

		/*!
		*	\brief	��104ͨ�ű��ı�ʶ��׼���л�Ϊ���ͨ��
		*
		*	\retval void
		*
		*	\note	
		*/
		void ForStopDT();

		/*!
		*	\brief	��104ͨ�ű��ı�ʶ��׼���л�Ϊ����ͨ��
		*
		*	\retval void
		*
		*	\note	
		*/
		void ForStartDT();

	protected:
		/*!
		*	\brief	���·�����֡
		*
		*	\param	sndBuf: ֡�׵�ַ
		*
		*	\retval OpcUa_Int32 ���֡�ĳ���
		*
		*	\note	
		*/
		OpcUa_Int32 BuildCtlRequest( OpcUa_Byte* sndBuf );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTime& SDI );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24& SDI );

		/*!
		*	\brief	��֤������ϢDPI��Ч��
		*
		*	\param	DDI: Ҫ��֤��DPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTime& DDI );

		bool IsValidDDI( structDDI_Time24NoInfoAddr& DDI );

		bool IsValidDDI( structDDI_Time24& DDI );

		/*!
		*	\brief	��֤������ϢVTI��Ч��
		*
		*	\param	VTI: Ҫ��֤��VTI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI& VTI );

		bool IsValidVTI( structVTI_Time24NoInfoAddr& VTI );

		bool IsValidVTI( structVTI_Time24& VTI );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	pPoOpcUa_Int: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢDPI��Ч��
		*
		*	\param	DPI: Ҫ��֤��DPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTimeNoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTime* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDDI( structDDI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDDI( structDDI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum  );

				/*!
		*	\brief	��֤������ϢDPI��Ч��
		*
		*	\param	DPI: Ҫ��֤��DPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidVTI( structVTINoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI_Time24NoInfoAddr* pPoOpcUa_Int, OpcUa_Int32 nNum  );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤��SPI
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidVTI( structVTI_Time24* pPoOpcUa_Int, OpcUa_Int32 nNum  );



		/*!
		*	\brief	��֤��վԶ���ն�״̬��32���ش�����ϢRTS��Ч��
		*
		*	\param	RTS: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidRTS( structRTS& RTS );

		/*!
		*	\brief	��֤ң����16λRT��ϢRTS��Ч��
		*
		*	\param	RT16: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidRT16bit( structRT16bit& RT16 );

		/*!
		*	\brief	��֤ң����32λRT��ϢRTS��Ч��
		*
		*	\param	RT32: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidRT32bit( structRT32bit& RT32 );

		/*!
		*	\brief	��֤ң����16λ����ֵ������Ч��
		*
		*	\param	Para16: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidPara16bit( structPara16bit& Para16 );

		/*!
		*	\brief	��֤ң����32λ����ֵ������Ч��
		*
		*	\param	Para32: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidPara32bit( structPara32bit& Para32 );

		/*!
		*	\brief	��֤�����ۻ�����Ч��
		*
		*	\param	DNLJ: Ҫ��֤�ĵ����ۻ���
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDNLJ( structDNLJ& DNLJ );

		/*!
		*	\brief	��֤��ʱ��ļ̵籣��װ���¼���Ч��
		*
		*	\param	JDBH: Ҫ��֤���¼�
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidJDBH( structJDBHNoTime& JDBH );

		/*!
		*	\brief	��֤��ʱ��ļ̵籣��װ�ó��������¼���Ч��
		*
		*	\param	JDBHGroup: Ҫ��֤���¼�
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidJDBHGroupAct( structJDBHGroupActNoTime& JDBHGroup );

		/*!
		*	\brief	��֤��ʱ��ļ̵籣��װ�ó��������·��Ϣ��Ч��
		*
		*	\param	JDBHGroup: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidJDBHGroupOutput( structJDBHGroupOutputNoTime& JDBHGroup );

		/*!
		*	\brief	��֤����λ����ĳ��鵥����Ϣ��Ч��
		*
		*	\param	DIGroup: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDIGroup( structDIGroup& DIGroup );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDINoTimeNoInfoAddr& SDI );

		/*!
		*	\brief	��֤������ϢSPI��Ч��
		*
		*	\param	SDI: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidSDI( structSDI_Time24NoInfoAddr& SDI );

		/*!
		*	\brief	��֤˫����ϢDPI��Ч��
		*
		*	\param	DDI: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDDI( structDDINoTimeNoInfoAddr& DDI );

		/*!
		*	\brief	��֤��λ����ϢVTI��Ч��
		*
		*	\param	VTI: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidVTI( structVTINoInfoAddr& VTI );

		/*!
		*	\brief	��֤��վԶ���ն�״̬��32���ش�����ϢRTS��Ч��
		*
		*	\param	RTS: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidRTS( structRTSNoInfoAddr& RTS );

		/*!
		*	\brief	��֤ң����16λRT��ϢRTS��Ч��
		*
		*	\param	RT16: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidRT16bit( structRT16bitNoInfoAddr& RT16 );

		/*!
		*	\brief	��֤ң����32λRT��ϢRTS��Ч��
		*
		*	\param	RT32: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidRT32bit( structRT32bitNoInfoAddr& RT32 );

		/*!
		*	\brief	��֤ң����16λ����ֵ������Ч��
		*
		*	\param	Para16: Ҫ��֤�Ĳ���
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidPara16bit( structPara16bitNoInfoAddr& Para16 );

		/*!
		*	\brief	��֤ң����32λ����ֵ������Ч��
		*
		*	\param	Para32: Ҫ��֤�Ĳ���
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidPara32bit( structPara32bitNoInfoAddr& Para32 );

		/*!
		*	\brief	��֤�����ۻ�����Ч��
		*
		*	\param	DNLJ: Ҫ��֤���ۻ���
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDNLJ( structDNLJNoInfoAddr& DNLJ );

		/*!
		*	\brief	��֤��ʱ��ļ̵籣��װ���¼���Ч��
		*
		*	\param	JDBH: Ҫ��֤���¼�
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidJDBH( structJDBHNoTimeNoInfoAddr& JDBH );

		/*!
		*	\brief	��֤��ʱ��ļ̵籣��װ�ó��������¼���Ч��
		*
		*	\param	JDBHGroup: Ҫ��֤���¼�
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidJDBHGroupAct( structJDBHGroupActNoTimeNoInfoAddr& JDBHGroup );

		/*!
		*	\brief	��֤��ʱ��ļ̵籣��װ�ó��������·��Ϣ��Ч��
		*
		*	\param	JDBHGroup: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidJDBHGroupOutput( structJDBHGroupOutputNoTimeNoInfoAddr& JDBHGroup );

		/*!
		*	\brief	��֤����λ����ĳ��鵥����Ϣ��Ч��
		*
		*	\param	DIGroup: Ҫ��֤����Ϣ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		bool IsValidDIGroup( structDIGroupNoInfoAddr& DIGroup );

		/*!
		*	\brief	������NVA��ֵ
		*
		*	\param	NVA: Ҫ������NVA
		*
		*	\retval float NVA��ֵ
		*
		*	\note	
		*/
		float GetNVAVal( structNVA& NVA );

		/*!
		*	\brief	��֤I��ʽ֡��Ч��
		*
		*	\param	pBuf: Ҫ��֤��֡�׵�ַ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		static bool IsCtrlAreaI( const void* pBuf );

		/*!
		*	\brief	��֤S��ʽ֡��Ч��
		*
		*	\param	pBuf: Ҫ��֤��֡�׵�ַ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		static bool IsCtrlAreaS( const void* pBuf );

		/*!
		*	\brief	��֤U��ʽ֡��Ч��
		*
		*	\param	pBuf: Ҫ��֤��֡�׵�ַ
		*
		*	\retval bool true��ʾ��Ч��������Ч
		*
		*	\note	
		*/
		static bool IsCtrlAreaU( const void* pBuf );

		//! ������Ԫ������ʹ��Щ�����ܷ��ʱ����˽�����ݳ�Ա
		friend void PeriodProc( void* pParent );
		friend void OnTimeOut( void* p );		
		friend void OnConnectTimeOut(void* p);							//! ���Ӷ�ʱ����ʱ�ص�����
	
		/*!
		*	\brief	��ʱ����ASDUTime��ʽת��ΪOpcUa_Int64��ʽ
		*
		*	\param	const ASDUTime& tTime    
		*
		*	\retval OpcUa_Int64 ʱ��
		*
		*	\note	
		*/
		OpcUa_Int64 ASDUTIME2MACSINT64( const ASDUTime& tTime );

		/*!
		*	\brief	��ʱ����Time24Bit��ʽת��ΪOpcUa_Int64��ʽ
		*
		*	\param	const ASDUTime& tTime    
		*
		*	\retval OpcUa_Int64 ʱ��
		*
		*	\note	
		*/
		OpcUa_Int64 Time24Bit2MACSINT64( const Time24bit& tTime );
		
		/*!
		*	\brief	�Ƿ�����
		*
		*	\param	iYear����λ����    
		*
		*	\retval bool true���꣬false������
		*
		*	\note	
		*/
		bool isLeap( OpcUa_Int32 iYear );

		/*!
		*	\brief	��������
		*
		*	\param	OpcUa_Int32 iYear,in iMon����bool bFrom1970 =true�������ʱ��ԭ����iyear/iMon������
		*    ���򣬼����Ա���ʼ�������µ�����
		*
		*	\retval OpcUa_Int32 ����
		*
		*	\note	
		*/
		OpcUa_Int32 Days( OpcUa_Int32 iYear,OpcUa_Int32 iMon,bool bFrom1970 = false );

		bool IsLogForRxDataProc();
		bool IsLog();

		bool IsLogValueError();	//!�Ƿ�ǿ�Ƽ�¼����У����ȥ������Ч����
		bool IsCheckSDI();	//! �Ƿ�У�鵥��ֵ
		bool IsCheckDDI();	//! �Ƿ�У��˫��ֵ
		bool IsCheckVTI();	//! �Ƿ�У�鲽λ��


	public:
		OpcUa_Int32 m_iPeriodCount;		//!���ڼ���������ʼ��Ϊ0���Ա�����ʱ����ִ��һ�Ρ�
		OpcUa_Int32 m_iForcedReconnectCount;	//! ǿ������������
		OpcUa_Int32  m_bNeedConnected;	//! ��Ҫ���ӣ��������ӣ���վ

		CIOTimer*			m_pPeriodTimer;							//! ���ڶ�ʱ��


	private:
		CIOTimer*			m_pIOTimer;								//! ��ʱ��ʱ��

		OpcUa_Byte m_bycmdType;					//! �������ͱ�־
		tIOCtrlRemoteCmd m_tCtrlRemoteCmd;	//! ��ǰ�����ң������

		bool m_bCanWork;				//! ���Կ�ʼ�����ı�־

		bool m_bIncompleteFrame;		//! �Ƿ�Ϊ������֡
		bool m_bExtractError;			//! ��������
		OpcUa_Byte* m_pBuf;

		short m_shAdiLogical;			//! �豸�߼���Ԫ��: 0 ~ Channel[0]->nMaxUnit-1

		OpcUa_Int32	  m_lastDevNo;				//! Уʱ/���ٻ�/�������ٻ���������

		OpcUa_Int32  m_t0;						//! �������ӵĳ�ʱ������
		OpcUa_Int32  m_t1;						//! ���ͻ����APDU�ĳ�ʱ������
		OpcUa_Int32  m_t2;						//! �����ݱ���t2<t1ʱȷ�ϵĳ�ʱ������
		OpcUa_Int32  m_t3;						//! ���ڿ���t3>t1״̬�·��Ͳ���֡�ĳ�ʱ������
		OpcUa_Int32	 m_t4;						//! �������ٻ����ڣ�����
		OpcUa_Int32	 m_TestCycl;				//! ����TestFrame�����ڣ�����

		bool m_bDebugLog;
		bool m_bLog;

		WORD m_wdValidDataLen;			//! ��Ч���ݳ���

		OpcUa_Int32	 m_CheckTimeCycl;			//! Уʱ����, ��.
		OpcUa_Int32  m_TimeOut;
		OpcUa_Int32  m_nMaxOfflineCount;		//! ������ߴ���
		OpcUa_Int32  m_nOfflineCount;			//! ���߼�����
		OpcUa_Int32  m_nIFrameCount;			//! һ��ʱ���ڽ��յ���I֡������
		OpcUa_Int32  m_nLastIFrameCount;		//! ���һ�ε�I֡������

		bool m_bIsLogValueError;	//!�Ƿ�ǿ�Ƽ�¼����У����ȥ������Ч����
		bool m_bCheckSDI;	//! У�鵥��ֵ�������ֽ��Ƿ����Ϊ0��1
		bool m_bCheckDDI;	//! У��˫��ֵ�������ֽ��Ƿ����Ϊ0~3
		bool m_bCheckVTI;	//! У�鲽λ��ֵ��ÿ����Чλ

		bool m_bNeedStartDT;			//! �Ƿ���Ҫ����STARTDT��Ч֡
		bool m_bNeedStopDT;				//! �Ƿ���Ҫ����STOPDT��Ч֡
		bool m_bNeedSendTest;			//! �Ƿ���Ҫ����TESTFR��Ч֡
		bool m_bNeedStartDTcon;			//! �Ƿ���Ҫ����STARTDTȷ��֡
		bool m_bNeedStopDTcon;			//! �Ƿ���Ҫ����STOPDTȷ��֡
		bool m_bNeedSendTestcon;		//! �Ƿ���Ҫ����TESTFRȷ��֡
		bool m_bNeedSendSFrame;			//! �Ƿ���Ҫ����S��ʽ֡�����յ���վS��ʽ֡ʱ�øñ�־true��

		OpcUa_Int32	m_iCurDevIndex;				//! ��ǰ���ڹ������豸������

		//! ���͵�S֡����
		OpcUa_Int32 m_nSFrameCount;	
	
	};
}
#endif
