/*!
 *	\file	IOSerialBoard.h
 *
 *	\brief	串口类驱动器实现类头文件（不包含流程操作）
 *
 *	详细的文件说明（可选）
 *
 *	\author Miaoweijie
 *
 *	\date	2014年4月10日	17:29
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef _IOSERIALBOARD_20080927_H
#define _IOSERIALBOARD_20080927_H

#include "RTDBCommon/OS_Ext.h"
#include "RTDBBaseClasses/IOBoardBase.h"


#ifdef IOSERIALBOARD_EXPORTS
#define IOSERIALBOARD_API MACS_DLL_EXPORT
#else
#define IOSERIALBOARD_API MACS_DLL_IMPORT
#endif


namespace MACS_SCADA_SUD
{
	class CIOSrlBoardDealThread;

	/*!
	*	\class	CIOComxBoard
	*
	*	\brief	串口类驱动器实现类
	*
	*	此驱动器不规定驱动流程，只提供了一般接口
	*/
	class IOSERIALBOARD_API CIOSerialBoard : public CIOBoardBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOSerialBoard();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIOSerialBoard();

		/*!
		*	\brief	向串口写数据
		*
		*	\param	pbyData 要写数据的头指针
		*	\param	nDataLen 要写数据的长度
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int Write( unsigned char* pbyData, int nDataLen );

		/*!
		*	\brief	打开驱动器
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int open( );

		/*!
		*	\brief	关闭驱动器
		*
		*	\retval	0为成功，否则失败
		*/
		virtual int close( );

		/*!
		*	\brief	本模块是否能输出调试信息
		*
		*	\retval	true为允许输出，否则不允许
		*/
		static bool CanOutput();

		/*!
		*	\brief	输出调试信息
		*
		*	\param	pchDebugInfo 要输出的信息
		*
		*	\retval	void
		*/
		static void OutputDebug( const char* pchDebugInfo );

		/*!
		*	\brief	获取类名
		*
		*	\retval	std::string 类名
		*/
		std::string GetClassName() ;

	private:
		//!处理线程指针
		CIOSrlBoardDealThread* m_pComDealThd;

	public:
		//! 类名
		static std::string s_ClassName;
	};

#ifndef IOSERIALBOARD_EXPORTS
	extern "C" MACS_DLL_IMPORT int InitDll();
	extern "C" MACS_DLL_IMPORT int UnInitDll();
#endif
}

#endif