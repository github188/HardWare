/*!
 *	\file	IODeviceSimBase.h
 *
 *	\brief	�豸ģ�������ͷ�ļ�
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��22��	8:16
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IODEVICESIMBASE_H_
#define _IODEVICESIMBASE_H_

#include "RTDBDriverFrame/IODriverH.h"
#include "RTDBCommon/IOTimer.h"

#define  PI 3.1415926

namespace MACS_SCADA_SUD
{
	class CIODeviceBase;
	class CIOCfgUnit;
	/*!
	 *	\class	CIODeviceSimBase
	 *
	 *	\brief	�豸ģ����
	 *
	 *	��ϸ����˵������ѡ��
	 */
	class IODRIVERFRAME_API CIODeviceSimBase
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIODeviceSimBase();

		/*!
		 *	\brief	ȱʡ��������
		 */
		virtual ~CIODeviceSimBase();

		/*!
		 *	\brief	��ȡ����
		 */
		std::string GetClassName();

		/*!
		 *	\brief	��ȡ����
		 *
		 *	\param	bySimMode ģ��ģʽ
		 *
		 *	\retval ��
		 */
		virtual void SetSimMode(Byte bySimMode);

		/*!
		 *	\brief	���ģ��
		 *
		 *	\param	p 
		 *
		 *	\retval 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int Random(void* p);

		/*!
		 *	\brief	����ģ��
		 *
		 *	\param	p 
		 *
		 *	\retval 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int Increase(void* p);

		/*!
		 *	\brief	����ģ��
		 *
		 *	\param	p 
		 *
		 *	\retval 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int Sine(void* p);

		/*!
		 *	\brief	����ģ��
		 *
		 *	\param	p 
		 *
		 *	\retval 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int Start(void);

		/*!
		 *	\brief	����ģ��
		 *
		 *	\param	p 
		 *
		 *	\retval 0Ϊ�ɹ�,����ʧ��
		 */
		virtual int Stop(void);

		/*!
		 *	\brief	���ñ�ǩ���������ָ�� 
		 *
		 *	\param	pIOUnitCfg ��ǩ���������ָ�� 
		 *
		 *	\retval 0Ϊ�ɹ�,����ʧ��
		 */
		void SetIOCfgUnit(CIOCfgUnit* pIOUnitCfg);

		/*!
		 *	\brief	��ȡ��ǩ���������ָ��
		 *
		 *	\param	��
		 *
		 *	\retval CIOCfgUnit* ��ǩ���������ָ�� 
		 */
		CIOCfgUnit* GetIOCfgUnit();

		/*!
		 *	\brief	���ö�Ӧ�豸����ָ��
		 *
		 *	\param	pParent	�豸����ָ��
		 *
		 *	\retval �� 
		 */
		void SetParent(CIODeviceBase* pParent);

		/*!
		 *	\brief	��ȡ��Ӧ�豸����ָ��
		 *
		 *	\param	��
		 *
		 *	\retval CIODeviceBase* �豸����ָ�� 
		 */
		CIODeviceBase* GetParent();

		/*!
		 *	\brief	��ȡģ������
		 *
		 *	\param	��
		 *
		 *	\retval Byte ģ������
		 */
		Byte GetSimMode();

		/*!
		 *	\brief	�õ�����ַ�
		 *
		 *	\param	chStr	�õ����ַ�
		 *
		 *	\retval ��
		 */
		void GetRandString( char& chStr );

		//����ģ�������
		void SetSimPeriod( int nInternal );

	public:
		const static int FULLDEG = 360;
		int nDegree;
		//!����
		static std::string s_ClassName;

		//!ģ��ģʽ
		Byte m_bySimMode;

		// //!��ǩ���������ָ��
		CIOCfgUnit* m_pIOCfgUnit;

		// //!�豸����ָ��
		CIODeviceBase* m_pDevice;

		// //!��ʱ��
		// CIOTimer* m_pIOTimer;

		//!ģ������
		int m_nSimPeriod;
	};

	//!���ȫ�ֱ���
	extern IODRIVERFRAME_VAR GenericFactory<CIODeviceSimBase> g_DrvSimFactory;
}

#endif