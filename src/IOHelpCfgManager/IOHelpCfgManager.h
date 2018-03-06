/*!
 *	\file	IOHelpCfgManager.h
 *
 *	\brief	Help.dbf文件管理模块
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年4月3日	8:46
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
	*	\brief	获取扩展信息的接口类
	*
	*			提供获取：协议、主从切换算法、IO驱动器的扩展信息的接口，即对应的文件名和类名。
	*/
    class IOHELPCFGMANAGER_API CIOExtCfgManager
    {
    public:
        CIOExtCfgManager();
        ~CIOExtCfgManager();

    public:
        /*!
        *	\brief	初始化,从文件help.dbf中加载配置信息
        *		
        *
        *	\retval	0为成功，其他失败
        *
        */
        int Init();

        /*!
        *	\brief	开始工作
        *		启动主动对象，因本模块没有主动对象，所以直接返回。
        *
        *	\retval	0为成功，其他失败
        *
        */
        int StartWork();

        /*!
        *	\brief	是否可以开始工作
        *
        *	\retval	true为可以开始工作；false为不可以。
        *
        */
        bool CanWork();

        /*!
        *	\brief	停止工作,停止主动对象。
        *		
        *	\retval	0为成功，其他失败
        *
        */
        int StopWork();

        /*!
        *	\brief	卸载
        *
        *	\retval	0为成功，其他失败
        *
        */
        int UnInit();

        /*!
        *	\brief	获取协议的扩展信息
        *
        *	\param	szPrtclName: 协议名称，如MacsModbus,即设备组态中的协议名
        *
        *	\retval	获取的协议扩展信息的结构指针，失败为NULL。
        *
        */
        tExtInfo* GetPrtclInfo( std::string szPrtclName );

        /*!
        *	\brief	获取主板的扩展信息，即对IO驱动器的扩展信息
        *
        *	\param	szBoardName: 主板类型名称，如COMX或TCPIP, 即主板组态中的主板类型
        *
        *	\retval	获取的主板扩展信息的结构指针，失败为NULL。
        *
        */
        tExtInfo* GetBoardInfo( std::string szBoardName );

        /*!
        *	\brief	获取主从切换算法的扩展信息
        *
        *	\param	szAlgName: 切换算法名称
        *
        *	\retval	获取的主从切换算法扩展信息的结构指针，失败为NULL。
        *
        */
        tExtInfo* GetSwitchAlgInfo( std::string szAlgName );

        /*!
        *	\brief	获取对外服务的扩展信息
        *
        *	\param	szServiceName: 对外服务的名称，可以是：RTDB，CITECT，CONSOLE或其他
        *
        *	\retval	获取的对外服务扩展信息的结构指针，失败为NULL。
        *
        */
        tExtInfo* GetServiceInfo( std::string szServiceName );

        //! 获取安装目录信息：软件安装目录/Bin/FEPConfig/
        std::string GetFepInstallPath();

        //! 获取运行目录信息：运行目录/FEP/Start/
        std::string GetFepStartPath();

        //! 获取热备份目录信息：运行目录/FEP/hot/
        std::string GetFepHotPath();

		//! 获取CommDriver.ini配置文件数据 
		tCommDriverCfg* GetFepDecCommDrvCfg();
		
		//! 设置CommDriver.ini配置文件数据 
		int SetFepDecCommDrvCfg();

		std::string GetFepCommDrvDir();

		void TransforSingleChar(std::string &srcString, char oldChar, char newChar);

		//! 获取MACS-SCADA-RTDB.ini配置文件数据 
		//tUADriverCfg* GetUADrvCfg();
		
		//! 设置MACS-SCADA-RTDB.ini配置文件数据 
		int SetUADrvCfg();

		//!FEP是否向实时服务发送事件
		 bool GetIsSendEvent2RT();

		//!UADriver是否发送和FEP链接中断的事件
		bool GetIsUADrvSendEvent();

    private:
        //! 接口的具体实现类
        CIOExtCfgImpl* m_pImpl;

		//!CommDriver.ini配置文件对应结构
		tCommDriverCfg *m_fepCommCfg;
        
        //!MACS-SCADA-RTDB.ini配置文件UDDriver对应结构
        //tUADriverCfg *m_uaDriverCfg;
    };

    //! 输出全局变量
    extern IOHELPCFGMANAGER_API CIOExtCfgManager* g_pIOExtCfgManager;

}
#endif

