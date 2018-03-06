#ifndef COMMONINFO_H
#define COMMONINFO_H
#include <string>
#include <map>
#include "RTDBCommon/OS_Ext.h"

typedef enum _em_connect_type_
{
    emIO = 1,
    emCom,
    emTcpClient,
    emTcpSvr,
    emUdpClient,
    emUdpSvr
}EmConnType;

typedef struct _drvinfo_
{
    std::string szModule;
    std::string szName;
    std::string szChnl;
}T_DrvInfo, *LPTDrvInfo;

typedef struct _tagItem_
{
    std::string szTagName;
    std::string szDrvName;
	std::string szDeviceName;
    std::string szDes;
    std::string szAddr;
    Byte        byType;
    float       fDownLimit;
    float       fUpperLimit;
    float       fRatio;
    float       fOffset;
    long        lPeriod;
}T_TagItem, *LPTTagItem;


extern EmConnType GetConType(const char* szData);
extern std::string GetConType(EmConnType emType);
extern std::map<std::string, T_DrvInfo*> g_sDrvInfos;
extern std::map<std::string, T_TagItem*> g_sTagItems;
#endif //#ifndef COMMONINFO_H

