#ifndef _CONSTDEFINATION_H
#define _CONSTDEFINATION_H

//! rtdb的类型定义空间
const int RTDB_TYPE_NS_INDEX = 10;
//! fep的类型定义空间
const int FEP_TYPE_NS_INDEX = 9;
//! IONodeManager的地址空间
const int IONODEMANAGER_NS_INDEX = 11;
//! 事件通知View的地址空间
const int VIEW_EVENT_NS_INDEX = 12;

//! HashTable的size
const int MAX_UNITNM_HASHTABLE_SIZE = 1000000;
//! HashTable的size调整
const int Opt_HashTable_Size = 100000;

//! RTDB类型命名空间uri
#define  RTDB_TYPE_NAMESPACEURI  "urn:Hollysys:SCADA V4:RTDBType"
//! RTDB所需数据库单元配置文件Url标志
#define  RTDB_DBUnit_Tag	"RTDB"

//! fep类型命名空间uri
#define  FEP_TYPE_NAMESPACEURI  "urn:Hollysys:SCADA V4:FEPType"
//! fep节点命名空间uri
#define  FEP_DATA_NAMESPACEURI  "urn:Hollysys:SCADA V4:FEPData"

//! View命名空间uri
#define  VIEW_NAMESPACEURI  "urn:Hollysys:SCADA V4:View"

//! RTDB自定义类型文件目录文件夹名
#define RTDB_Type_File		"SvrCommonRes"
//! RTDB自定义类型文件名
#define RTDB_Type_FileName	"RTDBType"
//! RTDB配置文件目录文件夹名
#define RTDB_Config_File	"RTDB"
//! FEP配置文件目录文件夹名
#define FEP_Config_File		"FEP"
//! fep地址空间配置文件名
#define  FEP_NODESET_FILENAME	"FEPData"
//! rtdb地址空间配置文件名模式串 
#define  RTDB_NODESET_FILENAME_PATTERN   "RTDB%"

//! 独立fep标签地址空间配置文件名
#define  FEP_NODESET_TAG_FILENAME	"FEPTag_"


#endif //_CONSTDEFINATION_H
