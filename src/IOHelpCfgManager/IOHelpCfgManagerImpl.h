/*!
 *	\file	IOHelpManagerImpl.h
 *
 *	\brief	Help.dbf�ļ���Ϣ����
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��3��	9:07
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOHELPMANAGERIMPL_H_
#define _IOHELPMANAGERIMPL_H_

#include <string>
#include <map>
#include "utilities/fepcommondef.h"

namespace MACS_SCADA_SUD
{
	//! map<���ƣ���չ��Ϣ�ṹ>
	typedef std::map<std::string,tExtInfo> tMapStrExtInfo;

	/*!
	 *	\class	CIOExtCfgImpl
	 *
	 *	\brief	��չ���õ�ʵ����
	 *
	 */
	class CIOExtCfgImpl
	{
	public:
		/*!
		 *	\brief	ȱʡ���캯��
		 */
		CIOExtCfgImpl(void);

		/*!
		 *	\brief	ȱʡ��������
		 */
		~CIOExtCfgImpl(void);

	public:
		/*!
		 *	\brief	��ʼ��
		 *		���ļ�help.dbf�м���������Ϣ
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 *
		 */
		int Init();

		/*!
		 *	\brief	��ʼ����
		 *		��������������ģ��û��������������ֱ�ӷ��ء�
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 *
		 */
		int StartWork();

		/*!
		 *	\brief	�Ƿ���Կ�ʼ����
		 *
		 *	\retval	trueΪ���Կ�ʼ������falseΪ�����ԡ�
		 *
		 */
		bool CanWork();

		/*!
		 *	\brief	ֹͣ����
		 *		ֹͣ��������
		 *
		 *	\retval	0Ϊ�ɹ�������ʧ��
		 *
		 */
		int StopWork();

		/*!
		*	\brief	ж��
		*
		*	\retval	0Ϊ�ɹ�������ʧ��
		*
		*/
		int UnInit();

		/*!
		 *	\brief	��ȡЭ�����չ��Ϣ
		 *
		 *	\param	szPrtclName: Э�����ƣ���MacsModbus,���豸��̬�е�Э����
		 *
		 *	\retval	��ȡ��Э����չ��Ϣ�Ľṹָ�룬ʧ��ΪNULL��
		 *
		 */
		tExtInfo* GetPrtclInfo( std::string szPrtclName );

		/*!
		 *	\brief	��ȡ�������չ��Ϣ������IO����������չ��Ϣ
		 *
		 *	\param	szBoardName: �����������ƣ���COMX��TCPIP, ��������̬�е���������
		 *
		 *	\retval	��ȡ��������չ��Ϣ�Ľṹָ�룬ʧ��ΪNULL��
		 *
		 */
		tExtInfo* GetBoardInfo( std::string szBoardName );

		/*!
		 *	\brief	��ȡ�����л��㷨����չ��Ϣ
		 *
		 *	\param	szAlgName: �л��㷨����
		 *
		 *	\retval	��ȡ�������л��㷨��չ��Ϣ�Ľṹָ�룬ʧ��ΪNULL��
		 *
		 */
		tExtInfo* GetSwitchAlgInfo( std::string szAlgName );

		/*!
		 *	\brief	��ȡ����������չ��Ϣ
		 *
		 *	\param	szServiceName: �����������ƣ������ǣ�RTDB��CITECT��CONSOLE������
		 *
		 *	\retval	��ȡ�Ķ��������չ��Ϣ�Ľṹָ�룬ʧ��ΪNULL��
		 *
		 */
		tExtInfo* GetServiceInfo( std::string szServiceName );

		//! ��ȡ��װĿ¼��Ϣ�������װĿ¼/Bin/FEPConfig/
		std::string GetFepInstallPath();

		//! ��ȡ����Ŀ¼��Ϣ������Ŀ¼/FEP/Start/
		std::string GetFepStartPath();

		//! ��ȡ�ȱ���Ŀ¼��Ϣ������Ŀ¼/FEP/hot/
		std::string GetFepHotPath();

		//! ��ȡhelp.dbf�����ļ����� 
		// return 0 ��ʾ�ɹ���������ȡʧ��
		int readHelpCfg(const std::string &szHelpFile);
	private:

		//! �Ƿ���Կ�ʼ�����ı�־
		bool m_bCanWork;

		//! Э����չ��Ϣ������
		tMapStrExtInfo m_mapPrtclIndex;

		//! ����ʵ����չ��Ϣ������
		tMapStrExtInfo m_mapCardIndex;

		//! ������չ��Ϣ������
		tMapStrExtInfo m_mapBoardIndex;

		//! �л��㷨��չ��Ϣ������
		tMapStrExtInfo m_mapAlgIndex;

		//! ���������չ��Ϣ������
		tMapStrExtInfo m_mapServiceIndex;
		
	};
}

#endif