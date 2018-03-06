/*!
 *	\file	$FILE_BASE$.$FILE_EXT$
 *
 *	\brief	104��վ�豸��ͷ�ļ�
 *
 *	$Header: /MACS-SCADA_SUD/FEP_Design/Drivers/FepIEC104Drv/FepIEC104Device.h 5     09-09-28 16:41 Miaoweijie $
 *	$Author: Miaoweijie $
 *	$Date: 09-09-28 16:41 $
 *	$Revision: 5 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOMACSIEC104DEVICE_20080812_H_
#define _IOMACSIEC104DEVICE_20080812_H_

#include "FepIEC104DrvH.h"
#include "RTDB/Server/DBDriverFrame/IODeviceBase.h"

namespace MACS_SCADA_SUD
{	
	class IOMACSIEC104_API CFepIEC104Device :
		public CIODeviceBase
	{
	public:
		CFepIEC104Device();

		/*!
		*	\brief	�������
		*
		*	\retval	std::string ����
		*/
		std::string GetClassName();


		//////////////////////////////////////////////////////////////////////////////////
		/*!
		*	\brief	�����в��԰�
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		static OpcUa_Int32 BuildTestFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	��STOPDT֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		static OpcUa_Int32 BuildStopFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	��STARTDT֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		static OpcUa_Int32 BuildStartFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	�����в���ȷ�ϰ�
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		static OpcUa_Int32 BuildTestFrameCON( OpcUa_Byte* buf );

		/*!
		*	\brief	��STOPDTȷ��֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		static OpcUa_Int32 BuildStopFrameCON( OpcUa_Byte* buf );

		/*!
		*	\brief	��STARTDTȷ��֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		static OpcUa_Int32 BuildStartFrameCON( OpcUa_Byte* buf );

		/*!
		*	\brief	�ѱ���ʱ��ת��ΪASDUTime
		*
		*	\param	sysTime	����ʱ��
		*
		*	\retval	ASDUTime
		*/
		static ASDUTime SystimeToASDUTime( ACE_Time_Value sysTime );

		/*!
		*	\brief	��S��ʽ֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	�����ٻ�֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildWholeQueryFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	��������ٻ�֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildCountQueryFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	��Уʱ֡
		*
		*	\param	buf	֡�׵�ַ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildCheckTimeFrame( OpcUa_Byte* buf );

		/*!
		*	\brief	��֡��ң��. ��ң��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��ң��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��ң������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ˫ң��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildDDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ˫ң��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildDDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ˫ң������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildDDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ������ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildUPDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ������ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildUPDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ����������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildUPDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��ң����ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTSDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��ң����ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTSDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��ң����ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTSDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ˫ң����ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTDDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ˫ң����ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTDDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ˫ң����ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTDDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��������ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTUPDOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��������ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTUPDObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң��. ��������ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTUPDOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������һ��ֵ��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildNVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������һ��ֵ��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildNVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������һ��ֵ������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildNVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������Ȼ�ֵ��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������Ȼ�ֵ��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������Ȼ�ֵ������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildSVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң�����̸�������ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildFAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң�����̸�������ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildFAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң�����̸�����������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildFAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң����32λ�ִ���ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildDWAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң����32λ�ִ���ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildDWAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң����32λ�ִ�������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildDWAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������һ��ֵ����ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTNVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������һ��ֵ����ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTNVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������һ��ֵ����ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTNVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������Ȼ�ֵ����ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTSVAAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������Ȼ�ֵ����ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTSVAAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң������Ȼ�ֵ����ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTSVAAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң�����̸���������ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTFAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң�����̸���������ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTFAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң�����̸���������ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTFAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң����32λ�ִ�����ʱ�꣩��ѡ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTDWAOaFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң����32λ�ִ�����ʱ�꣩��ִ��
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTDWAObFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	��֡��ң����32λ�ִ�����ʱ�꣩������
		*
		*	\param	buf		֡�׵�ַ
		*	\param	Addr	��Ϣ��ַ
		*	\param	pVal	��Ϣֵ
		*
		*	\retval	OpcUa_Int32 ���֡�ĳ���
		*/
		OpcUa_Int32 BuildTDWAOcFrame( OpcUa_Byte* buf, WORD Addr, OpcUa_Byte* pVal );

		/*!
		*	\brief	������һ���������
		*
		*	\retval	WORD �����ķ������
		*/
		WORD GenNextNs();

		/*!
		*	\brief	������һ���������
		*
		*	\retval	WORD �����Ľ������
		*/
		WORD GenNextNr();

		/*!
		*	\brief	�޸ķ������. ���յ���վ��������S��ʽ֡���֡�е�Nr���µ�m_wdNs.
		*
		*	\param	Ns	�������
		*
		*	\retval	void
		*/
		void SetNs( WORD Ns );

		/*!
		*	\brief	�޸Ľ������
		*
		*	ԭ��: ���յ������Nr��m_wdNr�Ƚ�,�����������������
		*	(1). Nr����m_wdNrʱ.
		*	(2). m_wdNr = 32768(������)
		*
		*	\param	Nr	�������
		*
		*	\retval	void
		*/
		void SetNr( WORD Nr );

		/*!
		*	\brief	�Ƿ���ҪУʱ��
		*
		*	\retval	bool ture��ʾ��ҪУʱ��false��ʾ����Ҫ
		*/
		bool IsTimeForCheckTime();

		/*!
		*	\brief	�Ƿ����ٻ�ʱ�䵽��
		*
		*	\retval	bool ture��ʾ�ǣ�false��ʾ��
		*/
		bool IsTimeForWholeQuery();

		/*!
		*	\brief	�Ƿ�������ٻ�ʱ�䵽��
		*
		*	\retval	bool ture��ʾ�ǣ�false��ʾ��
		*/
		bool IsTimeForCountQuery();

		/*!
		 *	\brief	�Ƿ���Ҫ����S֡
		 *
		 *	\retval bool true��, false��
		 *
		 *	\note	�ֳ���104Э���豸�϶���һ��һ�ģ�����һ���˿�ֻ��һ���豸��
		 *			���Ǵ��߼��Ͻ��Ļ�����ͬ�Ķ˿ڿ�����ͬһ��Э�飨��Ӧ��ͬ���豸����
		 *			������Щ����֡�ĺ������ŵ����豸�����档���������߼��Ļ���
		 *			Ӧ���ǰ�m_bNeedSendSFrame֮���һЩ�������ŵ��豸�����档
		 */
		bool IsNeedSendSFrame();


		/*!
		*	\brief	��ȡ��ǩ������ֵ
		*/
		bool IsDIValueChange( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr, OpcUa_Byte& byOldVal, OpcUa_Byte& byNewVal, std::string& strTagName );

		/*!
		*	\brief	��ȡFLOAT��ǩ��ֵ��ת����OpcUa_Byteֵ�Ƚ�
		*/
		bool IsAIValueChange( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr, OpcUa_Byte& byOldVal, OpcUa_Byte& byNewVal, std::string& strTagName );

		bool IsExitTag( OpcUa_Int32 UnitType, OpcUa_Int32 nTagAddr );

		//! �����豸������
		virtual void SetHot(OpcUa_Byte byHot);

	public:
		inline bool GetState()
		{
			return m_bState;
		}
		inline void SetState( bool bState)
		{
			m_bState = bState;
		}
		inline bool NeedWholeQuery()
		{
			return m_bNeedWholeQuery;
		}
		inline void NeedWholeQuery(bool bNeedWholeQuery)
		{
			m_bNeedWholeQuery = bNeedWholeQuery;
		}

		inline bool NeedCountQuery()
		{
			return m_bNeedCountQuery;
		}
		inline void NeedCountQuery(bool bNeedCountQuery)
		{
			m_bNeedCountQuery = bNeedCountQuery;
		}

		inline bool NeedCheckTime()
		{
			return m_bNeedCheckTime;
		}
		inline void NeedCheckTime(bool bNeedCheckTime)
		{
			m_bNeedCheckTime = bNeedCheckTime;
		}

		inline bool IsCheckTiming()
		{
			return m_bIsCheckTiming;
		}
		inline void IsCheckTiming(bool bIsCheckTiming)
		{
			m_bIsCheckTiming = bIsCheckTiming;
		}

		inline bool WholeQuerying()
		{
			return m_bWholeQuerying;
		}
		inline void WholeQuerying(bool bWholeQuerying)
		{
			m_bWholeQuerying = bWholeQuerying;
		}

		inline bool CountQuerying()
		{
			return m_bCountQuerying;
		}
		inline void CountQuerying(bool bCountQuerying)
		{
			m_bCountQuerying = bCountQuerying;
		}

		//! ����������¹���
		inline void ResetNr()
		{
			m_wdNr = 0;
		}

		//! ����������¹���
		inline void ResetNs()
		{
			m_wdNs = 0;
		}

		//! ��λ��δȷ�ϵ��յ���w��I��ʽ��APDU����
		inline void ResetCtrlAreaINum()
		{
			m_wCtrlAreaINum = 0;
		}

	public:
		//! ����
		static std::string s_ClassName;	

	private:
		bool m_bState;	//�豸ΪHotʱ��true��ʾ���߲���N�����յ�����վ��I֡��ΪStandbyʱ����m_bOnline����һ�¡���ʼ��Ϊfalse��

		//! �������: 0~32768
		WORD	m_wdNs;					
		
		//! ����յ������������: 0~32768
		WORD	m_wdNr;					
		
		//! ��δȷ�ϵ��յ���w��I��ʽ��APDU����
		WORD	m_wCtrlAreaINum;		
		
		//! ����յ�w��I��ʽ��APDU�����ȷ�ϣ�S��ʽ֡��
		OpcUa_Int32		m_nCtrlAreaIMax;		

		/*! 
		 *	�Ƿ���Ҫ�����������ٻ�. ����������øñ�־��Ч:
		 *		(1)���ڵ���(2)�ϴ����ٻ��ѳɹ������!
		 */
		bool	m_bNeedWholeQuery;		
		
		/*! 
		 *	�Ƿ���Ҫ�������м������ٻ�. ����������øñ�־��Ч:
		 *		(1)���ڵ���(2)�ϴμ������ٻ��ѳɹ������
		 */
		bool	m_bNeedCountQuery;		
		
		//! �Ƿ���Ҫ�������ж�ʱ
		bool	m_bNeedCheckTime;		

		/*! 
		 *	�Ƿ�����Уʱ���̡�����Уʱ������øñ�־��
		 *	ֱ����������֮һʱ�ͷ�(1)TimeOut(2)�ɹ��յ���ɵ�Ӧ��֡
		 */
		bool	m_bIsCheckTiming;		
		
		/*! 
		 *	�Ƿ��������ٻ����̡��������ٻ�������øñ�־��
		 *	ֱ����������֮һʱ�ͷ�(1)TimeOut(2)�ɹ��յ���ɵ�Ӧ��֡
		 */
		bool	m_bWholeQuerying;		
		
		/*! 
		 *	�Ƿ����ڼ������ٻ����̡����ͼ������ٻ�������øñ�־��
		 *	ֱ����������֮һʱ�ͷ�(1)TimeOut(2)�ɹ��յ���ɵ�Ӧ��֡
		 */
		bool	m_bCountQuerying;		
	};
}

#endif
