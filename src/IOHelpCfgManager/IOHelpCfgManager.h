/*!
 *	\file	IOHelpCfgManager.h
 *
 *	\brief	Help.dbf�ļ�����ģ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��3��	8:46
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOHELPCFGMANAGER_H_
#define	_IOHELPCFGMANAGER_H_

#include "RTDBCommon/OS_Ext.h"
#include <string>
#include "utilities/fepcommondef.h"

#ifdef IOHELPCFGMANAGER_EXPORTS  
#define IOHELPCFGMANAGER_API MACS_DLL_EXPORT
#else
#define IOHELPCFGMANAGER_API MACS_DLL_IMPORT
#endif

namespace MACS_SCADA_SUD
{
	class CIOExtCfgImpl;

	/*!
	*	\class	CIOExtCfgManager
	*
	*	\brief	��ȡ��չ��Ϣ�Ľӿ���
	*
	*			�ṩ��ȡ��Э�顢�����л��㷨��IO����������չ��Ϣ�Ľӿڣ�����Ӧ���ļ�����������
	*/
    class IOHELPCFGMANAGER_API CIOExtCfgManager
    {
    public:
        CIOExtCfgManager();
        ~CIOExtCfgManager();

    public:
        /*!
        *	\brief	��ʼ��,���ļ�help.dbf�м���������Ϣ
        *		
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
        *	\brief	ֹͣ����,ֹͣ��������
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

		//! ��ȡCommDriver.ini�����ļ����� 
		tCommDriverCfg* GetFepDecCommDrvCfg();
		
		//! ����CommDriver.ini�����ļ����� 
		int SetFepDecCommDrvCfg();

		std::string GetFepCommDrvDir();

		void TransforSingleChar(std::string &srcString, char oldChar, char newChar);

		//! ��ȡMACS-SCADA-RTDB.ini�����ļ����� 
		//tUADriverCfg* GetUADrvCfg();
		
		//! ����MACS-SCADA-RTDB.ini�����ļ����� 
		int SetUADrvCfg();

		//!FEP�Ƿ���ʵʱ�������¼�
		 bool GetIsSendEvent2RT();

		//!UADriver�Ƿ��ͺ�FEP�����жϵ��¼�
		bool GetIsUADrvSendEvent();

    private:
        //! �ӿڵľ���ʵ����
        CIOExtCfgImpl* m_pImpl;

		//!CommDriver.ini�����ļ���Ӧ�ṹ
		tCommDriverCfg *m_fepCommCfg;
        
        //!MACS-SCADA-RTDB.ini�����ļ�UDDriver��Ӧ�ṹ
        //tUADriverCfg *m_uaDriverCfg;
    };

    //! ���ȫ�ֱ���
    extern IOHELPCFGMANAGER_API CIOExtCfgManager* g_pIOExtCfgManager;

}
#endif

