#ifndef _CONSTDEFINATION_H
#define _CONSTDEFINATION_H

//! rtdb�����Ͷ���ռ�
const int RTDB_TYPE_NS_INDEX = 10;
//! fep�����Ͷ���ռ�
const int FEP_TYPE_NS_INDEX = 9;
//! IONodeManager�ĵ�ַ�ռ�
const int IONODEMANAGER_NS_INDEX = 11;
//! �¼�֪ͨView�ĵ�ַ�ռ�
const int VIEW_EVENT_NS_INDEX = 12;

//! HashTable��size
const int MAX_UNITNM_HASHTABLE_SIZE = 1000000;
//! HashTable��size����
const int Opt_HashTable_Size = 100000;

//! RTDB���������ռ�uri
#define  RTDB_TYPE_NAMESPACEURI  "urn:Hollysys:SCADA V4:RTDBType"
//! RTDB�������ݿⵥԪ�����ļ�Url��־
#define  RTDB_DBUnit_Tag	"RTDB"

//! fep���������ռ�uri
#define  FEP_TYPE_NAMESPACEURI  "urn:Hollysys:SCADA V4:FEPType"
//! fep�ڵ������ռ�uri
#define  FEP_DATA_NAMESPACEURI  "urn:Hollysys:SCADA V4:FEPData"

//! View�����ռ�uri
#define  VIEW_NAMESPACEURI  "urn:Hollysys:SCADA V4:View"

//! RTDB�Զ��������ļ�Ŀ¼�ļ�����
#define RTDB_Type_File		"SvrCommonRes"
//! RTDB�Զ��������ļ���
#define RTDB_Type_FileName	"RTDBType"
//! RTDB�����ļ�Ŀ¼�ļ�����
#define RTDB_Config_File	"RTDB"
//! FEP�����ļ�Ŀ¼�ļ�����
#define FEP_Config_File		"FEP"
//! fep��ַ�ռ������ļ���
#define  FEP_NODESET_FILENAME	"FEPData"
//! rtdb��ַ�ռ������ļ���ģʽ�� 
#define  RTDB_NODESET_FILENAME_PATTERN   "RTDB%"

//! ����fep��ǩ��ַ�ռ������ļ���
#define  FEP_NODESET_TAG_FILENAME	"FEPTag_"


#endif //_CONSTDEFINATION_H
