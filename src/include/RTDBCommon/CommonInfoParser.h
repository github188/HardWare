#ifndef COMMONINFOPARSER_H
#define COMMONINFOPARSER_H

#include "RTDBCommon/OS_Ext.h"
#include "tinyxml/tinyxml.h"
#include "RTDBCommon/CommonInfo.h"
#include "Utilities/fepcommondef.h"
#include <map>

namespace MACS_SCADA_SUD
{
	class COMMONBASECODE_API CommonInfoParser
	{
	public:
		CommonInfoParser(const std::string& sFilename);
		virtual ~CommonInfoParser();

		virtual Byte readXmlFile();
	protected:
		virtual Byte parseChannalUnit();
		//解析驱动标签对应的数据库单元列表，用于FEP加载标签配置文件
		virtual Byte parseDBUnit();

		virtual Byte parseTagUnit();

		virtual void ReleaseMemory();

	private:
		std::string						m_sFilename;
		TiXmlDocument* 					m_pXmlDocument;
	};
	extern COMMONBASECODE_API std::map<std::string, tIOPortCfg*> g_mapIOPortCfgs;
	extern COMMONBASECODE_API std::map<std::string, T_DrvInfo*> g_sDrvInfos;
	extern COMMONBASECODE_API std::map<std::string, T_TagItem*> g_sTagItems;
}
 
#endif