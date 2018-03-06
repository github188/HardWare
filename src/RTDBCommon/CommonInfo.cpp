#include "RTDBCommon/Commoninfo.h"
#include <string.h>

EmConnType GetConType(const char* szData)
{
    if(szData == NULL)
        return emIO;
    if(strcmp(szData, "IO") == 0)
        return emIO;
    else if(strcmp(szData, "Com") == 0)
        return emCom;
    else if(strcmp(szData, "TcpClient") == 0)
        return emTcpClient;
    else if(strcmp(szData, "TcpSvr") == 0)
        return emTcpSvr;
    else if(strcmp(szData, "UdpClient") == 0)
        return emUdpClient;
    else if(strcmp(szData, "UdpSvr") == 0)
        return emUdpSvr;
    else 
        return emIO;
}
std::string GetConType(EmConnType emType)
{
    switch(emType)
    {
    case emIO:
        return "IO";
        break;
    case emCom:
        return "Com";
        break;
    case emTcpClient:
        return "TcpClient";
        break;
    case emTcpSvr:
        return "TcpSvr";
        break;
    case emUdpClient:
        return "UdpClient";
        break;
    case emUdpSvr:
        return "UdpSvr";
        break;
    default:
        return "IO";
        break;
    }
}