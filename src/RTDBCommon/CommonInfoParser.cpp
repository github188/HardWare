
#include "RTDBCommon\OS_Ext.h"
#include "RTDBCommon\CommonInfoParser.h"
#include "RTDBCommon\RuntimeCatlog.h"

namespace MACS_SCADA_SUD
{
	using namespace std;

	COMMONBASECODE_API std::map<std::string, tIOPortCfg*> g_mapIOPortCfgs;
	COMMONBASECODE_API std::map<std::string, T_DrvInfo*> g_sDrvInfos;
	COMMONBASECODE_API std::map<std::string, T_TagItem*> g_sTagItems;
	
	CommonInfoParser::CommonInfoParser(const std::string& sFilename)
		: m_sFilename(sFilename)
		, m_pXmlDocument(NULL)
	{
	}

	CommonInfoParser::~CommonInfoParser()
	{

	}

	void CommonInfoParser::ReleaseMemory()
	{
		if (m_pXmlDocument)
		{
			delete m_pXmlDocument;
			m_pXmlDocument = NULL;
		}
	}
	
	Byte CommonInfoParser::readXmlFile()
	{
		Byte byRet = -1;

		std::string szPath = SingletonRuntimeCatlog::instance()->GetRunDir();

		TiXmlDocument* pXmlDocument = NULL;
		/*xmldocument set content*/
		pXmlDocument = new TiXmlDocument;
		if ( true != pXmlDocument->LoadFile( (char*)m_sFilename.c_str() ) )
		{
			delete pXmlDocument;
			return byRet;
		}

		TiXmlElement* root = pXmlDocument->RootElement();  
		if(root)
		{
			m_pXmlDocument = pXmlDocument;

			parseChannalUnit();

			parseDBUnit();

			parseTagUnit();

			byRet = 0;
		}

		ReleaseMemory();
		
		return byRet;
	}

	Byte CommonInfoParser::parseChannalUnit()
	{
		Byte byRet = -1;
		if(!m_pXmlDocument)
			return byRet;

		TiXmlElement *root = m_pXmlDocument->RootElement(); 
		TiXmlNode *pNode = root->FirstChild( "channals" );
		if(!pNode)
			return byRet;

		for(TiXmlNode *item = pNode->FirstChild("channal" );
			item;  
			item = item->NextSibling()) 
		{
			tIOPortCfg* pIOPortCfg = new tIOPortCfg;
			if(!pIOPortCfg)
				continue;
			pIOPortCfg->strType  = item->ToElement()->Attribute("type");		//!	��·���ͣ�TCPIP�����ڵ�
			
			std::string szData = item->ToElement()->Attribute("Port");			
			pIOPortCfg->nPort  = atoi(szData.c_str());			//!	�˿ں�

			szData = item->ToElement()->Attribute("BaudRate");	
			pIOPortCfg->iBaudRate  = atoi(szData.c_str());	//!	������

			szData = item->ToElement()->Attribute("DataBit");	
			pIOPortCfg->iDataBits  = atoi(szData.c_str());		//!	����λ

			szData = item->ToElement()->Attribute("StopBit");	
			pIOPortCfg->iStopBits  = atoi(szData.c_str());		//!	ֹͣλ

			szData = item->ToElement()->Attribute("Parity");	
			pIOPortCfg->byParity  = atoi(szData.c_str());		//!	��żУ�� 0��У�飬1żУ�飬2��У��

			//szData = item->ToElement()->Attribute("PortAccessMode");	
			//pIOPortCfg->byPortAccessMode  = atoi(szData.c_str());	//!	ͨ������ģʽ��1��ͻ��0����ͻ

			//szData = item->ToElement()->Attribute("Port");	
			//pIOPortCfg->iPower  = atoi(szData.c_str());		//!	Ȩֵ
			
			//pIOPortCfg->strOption  = item->ToElement()->Attribute("module");		//!	��չ����
			
			pIOPortCfg->strIPA_A  = item->ToElement()->Attribute("IPA");		//!	A�豸��A��IP��ַ
			
			pIOPortCfg->strSrlBrdPrefix  = item->ToElement()->Attribute("SrlBrdPrefix"); //!	�����豸��linux�����ϵ�����ǰ׺
			
			szData = item->ToElement()->Attribute("name");
			g_mapIOPortCfgs.insert(make_pair(szData, pIOPortCfg));
		}

		byRet = 0;
		return byRet;
	}

	Byte CommonInfoParser::parseDBUnit()
	{
		Byte byRet = -1;
		if(!m_pXmlDocument)
			return byRet;

		TiXmlElement *root = m_pXmlDocument->RootElement(); 
		TiXmlNode *DrvItem = root->FirstChild( "items" );
		if(!DrvItem)
			return byRet;

		for(TiXmlNode *item = DrvItem->FirstChild("item" );
         item;  
		 item = item->NextSibling()) 
		{
			T_DrvInfo* pDrvInfo = new T_DrvInfo;
			if(!pDrvInfo)
				continue;
			pDrvInfo->szModule = item->ToElement()->Attribute("module");
			pDrvInfo->szName = item->ToElement()->Attribute("name");
			//pDrvInfo->emConnType = item->Attribute("ConnectType");
			pDrvInfo->szChnl = item->ToElement()->Attribute("chnl");

			g_sDrvInfos.insert(make_pair(pDrvInfo->szName, pDrvInfo));
		}

		byRet = 0;
		return byRet;
	}


	Byte CommonInfoParser::parseTagUnit()
	{
		Byte byRet = -1;
		if(!m_pXmlDocument)
			return byRet;

		TiXmlElement *root = m_pXmlDocument->RootElement(); 
		TiXmlNode *pDrvItem = root->FirstChild( "tags" );
		if(!pDrvItem)
			return byRet;

		for(TiXmlNode *item = pDrvItem->FirstChild( "tag" );
         item;  
		 item = item->NextSibling() ) 
		{
			T_TagItem* pTagItem = new T_TagItem;
			if(!pTagItem)
				continue;
			pTagItem->szTagName = item->ToElement()->Attribute("name");
			pTagItem->szDrvName = item->ToElement()->Attribute("driver");
			pTagItem->szDeviceName = item->ToElement()->Attribute("device");
			pTagItem->szDes = item->ToElement()->Attribute("des");
			pTagItem->szAddr = item->ToElement()->Attribute("addr");
			pTagItem->byType;
			pTagItem->fDownLimit;
			pTagItem->fUpperLimit;
			pTagItem->fRatio;
			pTagItem->fOffset;
			pTagItem->lPeriod;

			g_sTagItems.insert(make_pair(pTagItem->szTagName, pTagItem));
		}
		return byRet;
	}
}


