/*!
 *	\file	fepcommondef.h
 *
 *	\brief	FEP�������ݶ���
 *
 *
 *	\author lingling.li
 *
 *	\date	2014��4��15��	9:14
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4ϵͳ������,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef fepcommondef_h__
#define fepcommondef_h__

#include <map>
#include <string>
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/nodebase.h"

#ifdef IONODEMANAGER_EXPORTS
# define IONODEMANAGER_API MACS_DLL_EXPORT
#else
# define IONODEMANAGER_API MACS_DLL_IMPORT
#endif

//! ���Ƴ���
const int NAME_LENGTH = 33;
//! ң�����������ݵ���󳤶ȣ�Ϊ�����ַ������ͺ�BTYE[]���ͣ��ʶ����1024
const int MAX_REMOTECMD_LEN = 2048;
//! ͨ����������ݵ���󳤶�
const int MAX_PACKETCMD_LEN = 1024;

//! ��ʽ���ݵ���󳤶�
const int MAX_STREAMDATA_LEN = 2048*256;
//! ������������������
const int MAX_SCHEDULECMD_LEN = 2048;
//!������Ϣ����
const int DEBUGINFOLEN = 1024;

//!�ɹ���ʶ
const static int Success = 0;

//!OPCUADriver������
static const std::string OPCUADRIVER = "FepOPCUADriver";
static const std::string RTDBDRIVER = "RtdbDriver";

// Namespace
namespace MACS_SCADA_SUD {

class CIOPort;

typedef std::map<long, CIOPort*> MAP_PORT;	//!	ͨ���ڵ����ͨ��ָ��MAP��
typedef std::map<std::string, long> MAP_PORTID;	//!	ͨ���ڵ����ͨ��ָ��MAP��

//��ģ���㷨���ͣ�����ģ��������Ч��1-���ģ�⣻2-����ģ�⣻3-����ģ��
typedef enum SimulateType
{
	SimulateType_Unknown = 0,	//δ֪����
	SimulateType_Random = 1,	//���ģ��
	SimulateType_Increase = 2,	//����ģ��
	SimulateType_Sin = 3,		//����ģ��
}emSimulateType;

//��������ö�ٶ���
typedef enum ObjectType
{
	ObjectType_Unknown = 0,		//δ֪����
	ObjectType_Channel = 1,		//ͨ������
	ObjectType_Device = 2,		//�豸����
	ObjectType_Tag = 3,			//��ǩ����
	ObjectType_Site = 4,		//վ������
}emObjectType;

//����ֵö�ٶ���
typedef enum CmdValue
{
	CmdValue_Unknown = 0,					//δ֪
	CmdValue_Force = 1,						//ǿ�ƣ�nodeIdӦΪ��ǩ
	CmdValue_UnForce = 2,					//ȡ��ǿ�ƣ�nodeIdӦΪ��ǩ
	CmdValue_Disable = 3,					//��ֹ��nodeIdӦΪ��ǩ
	CmdValue_Enable = 4,					//ȡ����ֹ��nodeIdӦΪ��ǩ

	CmdValue_Simulate = 5,					//ģ�⣬nodeIdӦΪ�豸��ͨ��
	CmdValue_StopSimulate = 6,				//ȡ��ģ�⣬nodeIdӦΪ�豸��ͨ��
	CmdValue_Monitor = 7,					//ͨѶ���ӣ�nodeIdӦΪͨ��
	CmdValue_StopMonitor = 8,				//ȡ��ͨѶ���ӣ�nodeIdӦΪͨ��
	CmdValue_StartLog = 9,					//��¼��־��nodeIdӦΪͨ��
	CmdValue_StopLog= 10,					//ȡ����¼��־��nodeIdӦΪͨ��
	CmdValue_Start = 11,					//������nodeIdӦΪ�豸��ͨ��
	CmdValue_Stop = 12,						//ֹͣ��nodeIdӦΪ�豸��ͨ��
	CmdValue_RedunForce = 13,				//����ǿ�ƣ�nodeIdӦΪ�豸��ͨ��
	CmdValue_UnRedunForce = 14,				//ȡ������ǿ�ƣ�nodeIdӦΪ�豸��ͨ��

	CmdValue_ExtraRangeEvent_1 = 15,		//!�򿪳������¼�����
	CmdValue_ExtraRangeEvent_0 = 16,		//!�رճ������¼�����

	CmdValue_SyncVal2Hot = 17,				//!����·Ϊ���豸�ϸ���ֵ����ת������·Ϊ���豸
	IOPACKET_TYPE_SENDPACKET = 18,			//!���������ͨ����ϰ�
	IOPACKET_TYPE_RECVPACKET = 19,			//!�յ�������������ݰ�

	IOPACKET_TYPE_SYNIOCMD2SN = 20,			//!��ң������ת������·Ϊ�����
} CMDVALUE;

//!����ǿ�������
typedef enum RedunStatus
{
	Redun_Second = 0, //ǿ�ƴ�
	Redun_Prim = 1,   //ǿ����
	Redun_NotSet = 2, //δ��������ǿ��

}RedunStatus;


typedef enum IOPortSwitchStatus
{
	IOPORT_NO_SWITCH = 0,
	IOPORT_SlAVE_TO_MASTER ,
	IOPORT_MASTER_TO_SlAVE,
}IOPortSwitchStatus;

//!	�˿����ýṹ��
typedef struct tIOPortCfg
{
	std::string     strType;			//!	��·���ͣ�TCPIP�����ڵ�
	int  nPort;				//!	�˿ں�
	int	 iBaudRate;			//!	������
	int  iDataBits;			//!	����λ
	int	 iStopBits;			//!	ֹͣλ
	Byte	 byParity;			//!	��żУ�� 0��У�飬1żУ�飬2��У��
	Byte	 byPortAccessMode;	//!	ͨ������ģʽ��1��ͻ��0����ͻ
//	Byte	 byDefaultPrmScnd;	//!	Ĭ������
//	std::string	 strSwitchAlgName;	//!	�����л��㷨����
	int     iPower;			//!	Ȩֵ
	std::string	 strOption;			//!	��չ����
	std::string	 strIPA_A;			//!	A�豸��A��IP��ַ
//	std::string	 strIPA_B;			//!	A�豸��B��IP��ַ
//	std::string	 strIPB_A;			//!	B�豸��A��IP��ַ
//	std::string	 strIPB_B;			//!	B�豸��B��IP��ַ
	std::string     strSrlBrdPrefix;   //!	�����豸��linux�����ϵ�����ǰ׺
}tIOPortCfg;

//!	�������ýṹ��
typedef struct tIODrvCfg
{
	int  iWatchTime;		//!	���ʱ��
	int	 iPollTime;			//!	��ѵʱ��
	int	 iTimeOut;			//!	��ʱʱ��
	Byte   byCommDataFormat;	//!	ͨѶ���ӵ����ݸ�ʽ��ȱʡΪ16����
	std::string     strDrvName;		//!	������
}tIODrvCfg;

//!	�豸���ýṹ��
typedef struct tIOUnitCfg
{
	int  iAddr;				//!	�豸��ַ
	std::string	 strCfgName;		//!	�����ļ���
}tIOUnitCfg;

typedef struct tIODataVariableCfg
{
	std::string m_strCfgAddress;   //��ǩ���õ�ַ
	int     m_lFinalAddress;   //�����ַ
	int     m_iTagType;        //��ǩ���ͣ������õ�ַ�����ó�
	int     m_iDataType;       //��������
	float  m_fLowRange;        //ԭʼ�̶����̷�Χ
	float   m_fHeightRange;
	float  m_fEngineerLowRange;        //���̶̿����̷�Χ
	float   m_fEngineerHeightRange;
	float  m_fDeadZone;       //����
}tIODataVariableCfg;


//!��ͨ���������
typedef struct tIOCtrlPacketCmd
{
	long	    objNodeId;					//����ڵ�Id
	Byte    byType;						//������
	int  nLen;						//���ݳ���
	Byte byData[MAX_PACKETCMD_LEN];	//����
}tIOCtrlPacketCmd;

//!ң��ָ��ʵʱ����ʹ��
typedef struct tIORemoteCtrl
{
	CNode*	pNode;						//FieldPoint�ڵ�ָ��
	std::string bysValue;				//ֵ
}tIORemoteCtrl;


//!���й�������
typedef struct tIORunMgrCmd
{
	CNode*			pNode;				//����ڵ�
	Byte		byObjectType;		//�����������
	Byte		byCmdValue;			//����ֵ�����������ֵ�ĺ궨��CmdValue
	std::string	byCmdParam;			//��ģ�������ʾģ���㷨����: 1-���ģ�⣻2-����ģ�⣻3-����ģ��
										//�Ա�ǩǿ�������ʾǿ��ֵ
										//������ǿ�������ʾ����ӣ�ΪRedunStatus����
}tIORunMgrCmd;

//!ͨ��״̬�ṹ
typedef struct tIOScheduleChannelState
{
	char sDriverName[NAME_LENGTH];		//������
	char sChannelName[NAME_LENGTH];		//ͨ����
	Byte byGood;							//ͨ���û�: 1--��, 0---��
	Byte bySimulate;						//�Ƿ�ģ������: 1--ģ��, 0---ûģ��
	Byte bySimMode;						//ģ���㷨����
	Byte byDisable;						//ͨ���Ƿ񱻽�ֹ: 1--����ֹ, 0--û����ֹ
	Byte byLog;							//ͨ���Ƿ��¼��־: 1--��¼��־, 0---����¼��־
	Byte byMonitor;						//ͨ���Ƿ�ͨ�ż���: 1--ͨ�ż���, 0---��ͨ�ż���
	Byte byHot;							//ͨ���Ƿ�Ϊ�Ȼ�: 1--���Ȼ�, 0--�����
}tIOScheduleChannelState;

//!�豸״̬
typedef struct tIOScheduleUnitState
{
	char sUnitName[NAME_LENGTH];			//�豸��
	Byte byOnline;						//�Ƿ�����: 1--����, 0---����
	Byte bySimulate;						//�Ƿ�ģ������: 1--ģ��, 0---ûģ��
	Byte bySimMode;						//ģ���㷨����
	Byte byDisable;						//�Ƿ񱻽�ֹ: 1--����ֹ, 0--û����ֹ
	Byte byHot;							//�Ƿ�Ϊ�Ȼ�: 1--���Ȼ�, 0--�����
	Byte byExtraRange;					//�Ƿ񱨳������¼�: 1--��, 0--����.
}tIOScheduleUnitState;

//!������.dbf��һ����¼���ýṹ
typedef struct tIOCfgUnitType
{
	char		pchFormat[128];				//��ǩ��ַģ���ʽ
	int		lLower;						//��ǩ��Ч��ַ������
	int		lUpper;						//��ǩ��Ч��ַ������
	int		iUnitType;					//UnitType
	int		iRawType;					//RawType
	int		iBitWidth;					//λ���
	int		lOffset;					//�����͵��������豸�������е���ƫ��
}tIOCfgUnitType;

//!�������ʽ�е�Ԫ��
typedef struct tBOLITEM
{
	Byte		byType;						//���ͱ�ʶ��0����ֵ��1��������2���������
	int		lValue;						//ֵ������0����ֵ��˵
	char		chOper;						//������ +-*/
	int		nPos;						//�����ڱ��ʽ�е�λ��
	//��ST%U(0,0,127)%*10:%+U(0,0,8)%*1000:%+U(0,0,500)%*1000:%+U(0,0,500),ST1:2:3:1���ֽ�ΪA,B,C,D�ĸ�����
	//��A��¼Ϊ0��BΪ1��CΪ2��DΪ3
	int		lUpper;						//��Χ�е�����
	int		lLower;						//��Χ�е�����
	int		lDiffer;					//У��ֵ
}tBOLITEM;

//!�ֽ�Ĳ������ʽ
typedef struct tBOLAN
{
	Byte		byType;						//0��ʾ%U��1��ʾ����,2��ʾ%S
	int		    nCount;						//���ʽ����ĸ���
	tBOLITEM		tItem[32];					//���ʽ�е���
}tBOLAN;

//!һ�ֱ�ǩ���͵������ṹ
typedef struct tIOCfgUnitDataDes
{
	char		pchFormat[128];
	char		pchID[16];					//��ʶ������AI%U(0.0.10)�е�AI
	tBOLAN		tBol;						//�ֽ�Ĳ������ʽ
	int			lLower;						//��ǩ�����ַ������
	int			lUpper;						//��ǩ�����ַ������
	int			lDataOffset;				//�����͵��������豸�����е���ƫ�Ƶ�ַ
	int			iUnitType;
	int			iRawType;
	int			iBitWidth;
	int			iLow;
	int			iHigh;
	char		pchComment[64];
}tIOCfgUnitDataDes;

//! �洢ClassName��DllFileName�ṹ��
typedef struct tExtInfo
{
	//! �ļ���������̬����,������׺.dll
	char pchFileName[64];
	//! ����
	char pchClassName[32];
}tExtInfo;

const int COMMDATALEN = 1024;		//!ͨ�ż������ݰ���󳤶�

//!ͨ�ż������ݽṹ
typedef struct tCommData
{
	char		pchDriverName[NAME_LENGTH];	//!��������
	long	dataTime;					//!ʱ��
	Byte	byDataType;					//!���ͻ���ܵı�ʶ:0Ϊ����,1Ϊ����
	int	nDataLen;					//!���ݳ���
	Byte	pbyData[COMMDATALEN];		//!��������
}tCommData;

//!ң������ṹFEPʹ��
typedef struct tIOCtrlRemoteCmd
{
	char pchUnitName[NAME_LENGTH];	//!�豸����
	Byte	byUnitType;					//!��ǩ����
	long long	lTagAddress;				//!��ǩ��ַ
	int nDataLen;					//!������Ч����
	Byte	byData[MAX_REMOTECMD_LEN];	//!����
}tIOCtrlRemoteCmd;

typedef struct tIORemoteWriteCmd
{
	char pchUnitName[NAME_LENGTH];	//!�豸����
	Byte	byUnitType;					//!��ǩ����
	long long	lTagAddress;				//!��ǩ��ַ
	int nDataLen;					//!������Ч����
	Byte  byData[0];					//!�ɱ䳤������
}tIORemoteWriteCmd;

//!��·����״̬�л���ʽ
typedef enum PortSwtichMode
{
	SLAVE2MASTER = 0,		//!������
	MASTER2SLAVE = 1,		//!������
	RECOVER2DEFALUT = 2		//!�ָ�Ĭ��
}PortSwtichMode;

typedef struct tIOCmdCallBackParam
{
	void* pMethodHandle;
	void* pExtendParam;
}tIOCmdCallBackParam;
//!���յ�ͨ�ż�������
const int COMMDATA_RCV = 0;
//!���͵�ͨ�ż�������
const int COMMDATA_SND = 1;

//! ������ص���Ϣ>>>From LF
const Byte DEVICE_ONLINE  = 1;		//! �豸����
const Byte DEVICE_OFFLINE = 0;		//! �豸����
const Byte DEVICE_INIT    = 2;		//! �豸��ʼ̬

//!ͨ����ϰ�����>>>From LF
const Byte IOPACKET_TYPE_ASKFOR_SENDPACKET  = 1;				//�������

//!�����ļ���
#define MACS_DRIVER_FILENAME "MacsDriver.ini"

//!����ͨ�ò����ļ���
#define MACS_COMM_DRIVER_FILENAME "CommDriver.ini"


//!����ͨѶ״̬��ǩ
#define DT_ISONLINE		"DEVCOMM"
#define DT_ISONLINE_A	"DEVCOMMA"
#define DT_ISONLINE_B	"DEVCOMMB"

//!�����ٷֱ�ת����λ
const double DEADZONE_UNIT = 0.00001;

//!�¼��ṹ����
typedef struct tIOEventElement
{
	Byte		byPrimType;			//0:�ⲿ�¼���1���ڲ��¼�
	Byte		byType;				//�¼�����
	int		    tTime;				//ʱ��
	char		pchTagName[16];		//��ǩ��
	int		    nVal;				//ֵ
	Byte		byValid;			//��Ч��־
	char			pchTagDes[64];		//��ǩ����
	char			pchValDes[64];		//ֵ����
}tIOEventElement;

//! ������Ϣ�����ݽṹ,�������е�����
typedef struct tDebugInfo
{
	char pchInfo[DEBUGINFOLEN];			//! ��Ϣ����
}tDebugInfo;

//! �����ṩ���ݷ���������Ϣ
typedef struct tIOServerInfo
{
	char pchServerName [64];				//!������������
	char pchCfgFileName[64];				//!��������Ӧ�������ļ���
}tIOServerInfo;

//!����FEP֮����·״̬��Ϣͬ������(�������ںͷ�������)
const uint PortInfoSyncPeriod = 200;

typedef struct tCommDriverCfg
{
	int		LogFileStorDays;		//��λ���졣��־�鵵����������0��ʾ���鵵��ֻ����һ���ļ���д�������ļ��滻���ļ���
	int		LogFileSize;			//��λM��������־��󳤶ȣ�Ĭ��2��
	bool	CommuMonitorFlag;		//����ͨѶ���ӿ���,Ĭ��0��0���رգ� 1:�򿪡�
	std::string		LogFilePath;			//��������
	std::string		SrlBoardPrefix;			//����������ʹ�õ��豸ǰ׺�ַ���.Ĭ��ֵ: ttyM
	bool	IsSendEvent2RT;			//FEP�Ƿ���ʵʱ�������¼�


	int		Count;					//Ҫ���صĶ������ĸ������������ҪFEP�ṩ��վ�������������Ϊ0
	std::string		Server1;				//Ҫ���صĶ�����������,104��վ����
	std::string		Server1Cfg;				//Ҫ���صĶ�����������,104��վ����
	bool	IOSub104NeedLog;		//104��վ�Ƿ�����־����
	bool	IOSub104NeedComm;		//104��վ�Ƿ���ͨ�ż��ӹ���

	std::string		Server2;				//Ҫ���صĶ�����������,Modbus��վ����
	std::string		Server2Cfg;				//Modbus��վ�����Ӧ�������ļ�����
	bool	IOSubModbusNeedLog;		//Modbus��վ�Ƿ�����־����
	bool	IOSubModbusNeedComm ;	//Modbus��վ�Ƿ���ͨ�ż��ӹ���

	tCommDriverCfg()
	{
		LogFileStorDays = 0;
		LogFileSize = 2;
		CommuMonitorFlag = false;
		LogFilePath = "";
		SrlBoardPrefix = "ttyM";
		IsSendEvent2RT = false;
		Count = 0;
		Server1 = "IOSub104Manager";
		Server1Cfg = "Sub104.xml";
		IOSub104NeedLog = false;
		IOSub104NeedComm = false;
		Server2 = "IOSubModbusMgr";
		Server2Cfg = "SubModbusCfg.xml";
		IOSubModbusNeedLog = false;
		IOSubModbusNeedComm = false;
	}
}tCommDriverCfg;

const static std::string MOD_DrvSchedule = "DBDrvScheduleMgr";
const static std::string MOD_DBFrameAccess = "DBFrameAccess";
const static std::string MOD_FepPlatformClass = "FEPPlatformClass";

}// End namespace

#endif // fepcommondef_h__
