/*!
 *	\file	FepModbusFun.cpp
 *
 *	\brief	公共函数实现源文件
 *
 *	详细的文件说明（可选）
 *
 *	$Header: ?/MACS-SCADA 3.2 软件开发/MACS-SCADA_SUD/FEP_Design/Drivers/FepModbusDrv/FepModbusFun.cpp, 470+1 2011/04/22 01:56:47 xuhongtao $
 *	$Author: xuhongtao $
 *	$Date: 2011/04/22 01:56:47 $
 *	$Revision: 470+1 $
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "FepModbusDrvH.h"

namespace MACS_SCADA_SUD
{

	/*!
	*	\brief	获取ini配置文件中某项配置信息
	*
	*	\param	pchAppName	节点名，即"[]"中内容
	*	\param	pchKeyName	关键字名
	*	\param	nDefault	默认值，读取失败则返回该值
	*	\param	pchFileName	文件名
	*
	*	\retval	int	ini配置文件中对应项的值，读取失败则返回默认值
	*/
	FEPMODBUSDRV_API int GetPrivateInifileInt(const char* pchAppName,
				const char* pchKeyName, int nDefault, const char* pchFileName)
	{
		int status;
		ACE_Configuration_Heap cf;
		status = cf.open();
		if( status != 0 )
		{
			return nDefault;
		}

		ACE_Ini_ImpExp import(cf);
		status = import.import_config( pchFileName );
		if (status!=0)
		{
			return nDefault;
		}

		const ACE_Configuration_Section_Key &root = cf.root_section ();
		ACE_Configuration_Section_Key Section;
		status = cf.open_section( root, pchAppName, 0, Section );
		if( status != 0) 
		{
			return nDefault;
		}

		unsigned int nValue;
		ACE_TString strV;
		status = cf.get_string_value( Section, pchKeyName, strV );
		sscanf( strV.c_str(), "%d", &nValue );
		if ( status==0 )
		{
			return nValue;
		}

		return nDefault;
	}

	FEPMODBUSDRV_API std::string GetPrivateInifileString(const char* pchAppName,
		const char* pchKeyName, std::string  strDefault, const char* pchFileName)
	{
		int status;
		ACE_Configuration_Heap cf;
		status = cf.open();
		if( status != 0 )
		{
			return strDefault;
		}

		ACE_Ini_ImpExp import(cf);
		    
		status = import.import_config( pchFileName );
		if (status!=0)
		{
			return strDefault ;
		}

		const ACE_Configuration_Section_Key &root = cf.root_section ();
		ACE_Configuration_Section_Key DrvSection;
		status = cf.open_section( root, pchAppName, 0, DrvSection );
		if( status != 0) 
		{
			return strDefault;
		}		
		ACE_TString strV;		
		status = cf.get_string_value( DrvSection, pchKeyName, strV );		
		if ( status==0 )
		{
			std::string temp = strV.c_str() ;
			return  temp;			
		}

		return strDefault;		
	}
}
