#pragma once
#include <vector>
#include <map>
#include <string>
#include "PublicFileH.h"

#define COMTRADE_FILE_PATH "E:\\"

namespace MACS_SCADA_SUD
{
#pragma pack(push,1)
	typedef std::map<std::string, std::string> T_mapStringType;

	//! 
	typedef std::vector<std::string> T_vecString;
	
	
	typedef struct T_CtdChgInfo
	{
		std::string szName;			//ͨ������
		std::string szLineNo;		//��·��ʶ
	}T_CtdChgInfo;

	//! ������ͨ���ĵ��ʶ
	typedef struct T_DIFunInf
	{
		OpcUa_Byte nFun;		//��������
		OpcUa_Byte nInf;		//��Ϣ���
	}T_DIFunInf;

	//! ������ͨ����ֵ��¼
	typedef struct T_DIChannelValType
	{
		OpcUa_Int32 nOrgPos;	//��ֵ��λ�ã�ͨ��Ϊ0
		OpcUa_Byte nOrgVal;	//��ֵ
		OpcUa_Int32 nChgPos;	//��λ֮���λ��
		OpcUa_Byte nChgVal;	//��λ֮���ֵ
	}T_DIChannelValType;

	//! ����־��״̬��λ����Ϣ������
	typedef std::map<T_DIFunInf, T_CtdChgInfo> T_mapCtdChgIndexType;

	//! COMTRADEͨ����Ϣ
	typedef struct T_CtdChannelInfo
	{
		std::string szName;			//ͨ������
		std::string szPhase;		//ͨ�����
		std::string szUnits;		//ͨ��ֵ�ĵ�λ
	}T_CtdChannelInfo;

	//! COMTRADEͨ��������;<ͨ����ʵ����ţ�ͨ����Ϣ>
	typedef std::map<OpcUa_Int16, T_CtdChannelInfo> T_mapCtdChannelType;
///////////////////////////////////////////////
	//! ����¼����ʶ
	typedef struct T_FaultType
	{
		std::string szFaultNumber;		//�������
		std::string szOcurTime;			//���Ϸ���ʱ��
	}T_FaultType;
	/////////////////////////////////////



//! ģ����ͨ����������Ϣ
typedef struct T_AIChannelInfo
{
	//OpcUa_Byte byAcc;			//ʵ��ͨ�����
	OpcUa_Float	fPrim;			//�һ��ֵ
	OpcUa_Float   fSecond;		//�����ֵ
	OpcUa_Float	fRFA;			//�α�����

	OpcUa_Int32		nNFE;		//Ӧ�÷������ݵ�Ԫ�ĵ�һ����ϢԪ�ص����

	OpcUa_Int32		nDataCount;	//���յ����Ŷ�ֵ����Ŀ���ǿɱ��
}T_AIChannelInfo;

typedef std::vector<OpcUa_Int32> T_AIChannelVAL;

typedef std::map<OpcUa_Byte, T_AIChannelVAL> T_AIChannelValIndex;

//! ���ֽ�
typedef struct{
	OpcUa_Byte	MIN:6;
	OpcUa_Byte	RES:1;
	OpcUa_Byte	IV:1;
}T_MinByte;

//! ʱ�ֽ�
typedef struct{
	OpcUa_Byte HOUR:5;
	OpcUa_Byte RES:2;
	OpcUa_Byte SU:1;
}T_HourByte;

//! ���ֽ�
typedef struct{
	OpcUa_Byte DAY:5;
	OpcUa_Byte WEEK:3;
}T_DayByte;

//! ���ֽ�
typedef struct{
	OpcUa_Byte MONTH:4;
	OpcUa_Byte RES:4;
}T_MonthByte;

//! ���ֽ�
typedef struct{
	OpcUa_Byte YEAR:7;
	OpcUa_Byte RES:1;
}T_YearByte;
#pragma pack(pop)

class CFepIEC103Drv;
class CDriverItemType;

class CPRDFile103Dev
{
public:
	CPRDFile103Dev(void);
	~CPRDFile103Dev(void);
	CPRDFile103Dev( std::string strDrvName, std::string strDevName, CFepIEC103Drv* pDeal );

public:
	//! ����¼����Ӧ��
	OpcUa_Int32 DealRcvData( OpcUa_Byte* pbyData );

	//! ����CFG�ļ�
	OpcUa_Int32 ProduceCfgFile();

	//! ����dat�ļ�
	OpcUa_Int32 ProduceDatFile();

	//! ��ʼ��
	void Init();

private:
	//! �������ASDU23
	OpcUa_Int32 DealASDU23( OpcUa_Byte* pData );

	//! �������ASDU26
	OpcUa_Int32 DealASDU26( OpcUa_Byte* pData );

	//! �������ASDU28
	OpcUa_Int32 DealASDU28( OpcUa_Byte* pData );

	//! �������ASDU29
	OpcUa_Int32 DealASDU29( OpcUa_Byte* pData );

	//! �������ASDU31
	OpcUa_Int32 DealASDU31( OpcUa_Byte* pData );

	//! �������ASDU27
	OpcUa_Int32 DealASDU27( OpcUa_Byte* pData );

	//! �������ASDU30
	OpcUa_Int32 DealASDU30( OpcUa_Byte* pData );

	//! ���ڴ�
	void Clear();

	//! �����־
	void ClearFlag();

	//! �õ�7����λλ��Ķ�����ʱ����ַ���
	std::string Get7BytesTimeString( OpcUa_Byte* pData );

	//! �õ�4����λλ��Ķ�����ʱ����ַ���
	std::string Get4BytesTimeString( OpcUa_Byte* pData );

	//! �õ���ǰ������ŵ��ļ���
	OpcUa_Int32 GetComtradeFileName();
	
	//! �õ�ָ�����������ŵ�״̬��λ����Ϣ
	OpcUa_Int32 GetCtdChgInfo( OpcUa_Int16 nFun, OpcUa_Int16 nInf, T_CtdChgInfo& chgInfo );
	
	//! �õ�ָ��ʵ��ͨ����ŵ�ͨ����Ϣ
	OpcUa_Int32 GetCtdChannelInfo( OpcUa_Int16 nAcc, T_CtdChannelInfo& channelInfo );
	bool ExistDir( std::string strDirName );
	bool CreateDir( std::string strDirName );
	std::string GetParentDir( std::string sDir );
	void Parse103PRDCfg( CDriverItemType* pDrvItem, std::string szDevName );

private:
	//! ��������Ӧ���豸����
	std::string m_szDevName;

	////! ��һ�����Ե�����
	//std::string m_szDomainName;

	//! ���������������������������� ASDU23 ���������������������������� 
	//! ��װ�������еĹ�����źͷ���ʱ��Ķ�Ӧ���飬ʱ��ĸ�ʽΪ��dd/mm/yyyy,hh:mm:ss.ssssss 
	std::map<OpcUa_Int32, std::string> m_mapFaultNumberTimeIndex;

	//! ���������������������������� ASDU26 ���������������������������� 
	//! ��ǰ����Ĺ������ ���� ASDU26
	OpcUa_Int32 m_nCurFaultNumber;

	//! ����������ţ�ע����COMTRADE�ļ���û�м�¼�� ���� ASDU26
	OpcUa_Int32 m_nNetFaultNumber;

	//! ͨ����Ŀ��ע���Ƿ�ֻ��ģ����ͨ�����������������ģ��� ���� ASDU26
	OpcUa_Int32 m_nChannelCount;

	//! һ��ͨ����ϢԪ�ص���Ŀ ���� ASDU26
	OpcUa_Int32 m_nElementCount;

	//! ��ϢԪ�ؼ�ļ��
	OpcUa_Int32 m_nElementInterval;

	//! ��ʼ��¼���ϵ�ʱ�䴮:dd/mm/yyyy,hh:mm:ss.ssssss  ���� ASDU26
	std::string m_szRecordTime;

	//! ���������������������������� ASDU28 ���������������������������� 
	//! �жϹ�������Ƿ���Ч

	//! ���������������������������� ASDU29 ���������������������������� 
	std::map<T_DIFunInf, T_DIChannelValType> m_mapDIChannel;

	//! ���������������������������� ASDU31 ���������������������������� 
	//! ������ͨ���������

	//! ���������������������������� ��N��ͨ��������� ���������������������������� 
	//! ���������������������������� ASDU27 ����������������������������
	std::map<OpcUa_Byte, T_AIChannelInfo> m_mapAIChannel;

	//! ���������������������������� ASDU30 ����������������������������
	T_AIChannelValIndex m_mapAIVal;

	//! ���������������������������� ASDU31 ���������������������������� 
	//! ��N��ͨ���������

	//! ���������������������������� ASDU31 ���������������������������� 
	//! �Ŷ����ݴ������

	//! ��Сֵ
	OpcUa_Int32 m_nMinVal;

	//! ���ֵ
	OpcUa_Int32 m_nMaxVal;

private:
	//! ��ǰ���еĵڼ����ֵı�־��0Ϊ�����壻1Ϊ��ʼ�����̣�2Ϊ״̬��λ���̣�3Ϊͨ�����̡�
	OpcUa_Byte m_byPartType;

	//! ��ǰ����֡�ı�ʶ: m_byPartType=1��2ʱ��ΪASDU�ĺţ���m_byPartType=3ʱ��Ϊʵ��ͨ���š�
	OpcUa_Byte m_byFrameLabel;

	//! �������͵ı�־��2Ϊ״̬��λ���������3Ϊͨ�����������1Ϊ�Ŷ�����ȫ�����������0�����塣
	OpcUa_Byte m_byCompleteType;

	//! ���Ϸ������ļ��������豸��_����ʱ��_���Ϻ�+��׺����������ʱ��Ϊ��YYYYMMDDhhmmssssssss��
	std::string m_szCurFileName;

	//CPRDFile103Deal* m_pDealImpl;
	std::string m_szPrdFilePath; //����¼���ļ����·��

	T_mapCtdChannelType m_mapChannelInfo;	//ģ����ͨ����Ϣ
	T_mapCtdChgIndexType m_mapStateInfo;	//״̬��ͨ����Ϣ
	std::string m_strDrvName;				//!����������̬��
	CFepIEC103Drv* m_pIEC103Drv;

public:
	CIOLog* m_pLog;
};
}
