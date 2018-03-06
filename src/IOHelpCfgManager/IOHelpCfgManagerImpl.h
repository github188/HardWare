/*!
 *	\file	IOHelpManagerImpl.h
 *
 *	\brief	Help.dbf文件信息管理
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年4月3日	9:07
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
	//! map<名称，扩展信息结构>
	typedef std::map<std::string,tExtInfo> tMapStrExtInfo;

	/*!
	 *	\class	CIOExtCfgImpl
	 *
	 *	\brief	扩展配置的实现类
	 *
	 */
	class CIOExtCfgImpl
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOExtCfgImpl(void);

		/*!
		 *	\brief	缺省析构函数
		 */
		~CIOExtCfgImpl(void);

	public:
		/*!
		 *	\brief	初始化
		 *		从文件help.dbf中加载配置信息
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
		 *	\brief	停止工作
		 *		停止主动对象。
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

		//! 获取help.dbf配置文件内容 
		// return 0 表示成功，其他读取失败
		int readHelpCfg(const std::string &szHelpFile);
	private:

		//! 是否可以开始工作的标志
		bool m_bCanWork;

		//! 协议扩展信息索引表
		tMapStrExtInfo m_mapPrtclIndex;

		//! 主板实现扩展信息索引表
		tMapStrExtInfo m_mapCardIndex;

		//! 主板扩展信息索引表
		tMapStrExtInfo m_mapBoardIndex;

		//! 切换算法扩展信息索引表
		tMapStrExtInfo m_mapAlgIndex;

		//! 对外服务扩展信息索引表
		tMapStrExtInfo m_mapServiceIndex;
		
	};
}

#endif