/*!
 *	\file	IOPrtclParser.h
 *
 *	\brief	协议类方法定义
 *
 *	详细的文件说明（可选）
 *
 *	\author JYF
 *
 *	\date	2014年4月8日	15:34
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, New NetGate Product Development,Update Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOPRTCLPARSER_H_
#define _IOPRTCLPARSER_H_

#include "RTDBBaseClasses/IOPrtclParserBase.h"
#include "RTDBDriverFrame/IODriverH.h"

namespace MACS_SCADA_SUD
{
	class CIODeviceBase;

	class IODRIVERFRAME_API CIOPrtclParser : public CIOPrtclParserBase
	{
	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOPrtclParser();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIOPrtclParser();

	public:
		
		/*!
		 *	\brief	获取类名
		 *
		 *	\param	无
		 *
		 *	\retval	std::string	协议解析类名
		 */
		
		std::string GetClassName();


		/*!
		 *	\brief	组请求帧,在驱动器发送数据前调用
		 *
		 *	\param	pbyData	: 要发送的数据
		 *	\param	nDataLen: 要发送的数据的长度
		 *
		 *	\retval	返回值	0为成功,其他失败
		 *
		 */
		virtual int BuildRequest(Byte* pbyData, int& nDataLen);
		
		/*!
		 *	\brief	组请求帧,供驱动器调用
		 *
		 *	本接口由驱动器在发送数据前调用,通过引用bTrigTimer来控制是否启动超时定时器
		 *	如果是使用COMX类型的需要重载本接口
		 *
		 *	\param	pbyData		: 要发送的数据
		 *	\param	nDataLen	: 要发送的数据的长度
		 *	\param	bTrigTimer	: 本接口返回０后，驱动器是否启动超时定时器的标志，true则启动超时定时器，false则不启动
		 *	\param  nTimeOut	: 超时时间
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 */
		virtual int BuildRequest(Byte* pbyData, int&nDataLen, bool& bTrigTimer, int& nTimeOut);

		/*!
		 *	\brief	处理接收数据,为驱动器中的数据处理
		 *
		 *	本接口由驱动器在接收到数据后调用,通过引用bStopTimer和bSendData来控制停止定时器和发送数据的流程
		 *	如果是使用COMX类型的需要重载本接口
		 *
		 *	\param	pDataBuf	: 接收到的数据
		 *	\param	nLen		: 接收到的数据的长度
		 *	\param	bStopTimer	: 本接口返回０后，驱动器是否关闭超时定时器的标志，true则关闭超时定时器，false则不关闭
		 *	\param	bSendData	: 本接口返回０后，驱动器是否发送数据的标志，true则发送数据，调用BuildRequest，false则不发送
		 *
		 *	\retval	int	0为成功，其他失败
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData);

		/*!
		 *	\brief	处理接收数据
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pDataBuf: 接收到的数据
		 *	\param	nLen	: 接收到的数据的长度
		 *
		 *	\retval	int	0为成功，其他失败
		 *
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen);

		/*!
		 *	\brief	接收到通道诊断包
		 *
		 *	\param	ptPacket	: 诊断包指针
		 *
		 *	\retval	返回值	int 0为成功其他失败
		 *
		 */
		virtual int RcvPcketCmd(tIOCtrlPacketCmd* ptPacket);

		/*!
		 *	\brief	从接收缓冲区中提取一帧完整数据包
		 *
		 *	各驱动都应重载本接口，本接口是根据具体协议采用滑动窗口的方法从接收缓冲区中析取一个有效帧
		 *
		 *	\param	pPacketBuffer	：	提取的帧数据的保存区
		 *	\param	pnPacketLength	：	为被处理掉数据的长度
		 *
		 *	\retval	bool	true为成功,false为失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual bool ExtractPacket(Byte* pPacketBuffer, int* pnPacketLength);

		/*!
		 *	\brief	处理提取出来的完整数据包
		 *
		 *	\param	pPacketBuffer	：有效数据的首指针
		 *	\param	nPacketLength	：有效数据的长度
		 *	\param	bFromLocal		：是否从本机获取的（控制是否发送ＳＯＥ）
		 *	\param	bStopTimer		：是否停止超时定时器：true为停止;false为不停止
		 *	\param	bSendData		：是否发送数据：true为发送;false为不发送
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		本接口是使用CIOComxBoard类型的驱动器时要调用的函数,驱动必须重载本接口。
		 *			
		 */
		virtual void ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal, bool& bStopTimer, bool& bSendData);

		/*!
		 *	\brief	设置接收处理中的 是否停止定时器标志 和 是否发送数据的标志
		 *
		 *	本函数在RxDataProc中调用
		 *	缺省实现是：如果设备为模拟态和从机则置标志为false.
		 *
		 *	\param	pDevice		:设备指针
		 *	\param	bStopTimer	:是否停止定时器标志
		 *	\param	bSendData	:是否发送数据的标志
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note	本接口是使用CIOComxBoard类型的驱动器时要调用的函数。
		 *			如果缺省实现不能满足要求，则可以重载。
		 */
		virtual int SetRcvProcFlag( CIODeviceBase* pDevice, bool& bStopTimer, bool& bSendData );

		/*!
		 *	\brief	给指定设备组通道诊断数据帧
		 *
		 *	\param	pDevice		:要操作的设备指针
		 *	\param	pbyData		:组好的通道诊断数据帧的首指针
		 *	\param	nReturnLen	:数据帧的总长度
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 */
		virtual void MakeChannelDiagFrame(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen);

		/*!
		 *	\brief	处理提取出来的完整数据包
		 *
		 *	\param	pPacketBuffer	：有效数据的首指针
		 *	\param	nPacketLength	：有效数据的长度
		 *	\param	bFromLocal		：是否从本机获取的（控制是否发送ＳＯＥ）
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 */
		virtual void ProcessRcvdPacket(Byte* pPacketBuffer, int nPacketLength, bool bFromLocal);

		/*!
		 *	\brief	得到帧数据缓冲区指针
		 *
		 *	\param	无
		 *
		 *	\retval	Byte* 缓冲区指针
		 *
		 */
		Byte* GetPacketBuffer();

		/*!
		 *	\brief	得到最小包的长度
		 *
		 *	\retval	int	最小包的长度
		 */
		virtual int GetMinPacketLength();

		/*!
		 *	\brief	停止协议解析类中的定时器
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int StopTimer();

	protected:
		/*!
		 *	\brief	生成16位sum校验
		 *
		 *	\param	Buf		: 要计算的数据区
		 *	\param	iWordLen: 计算的字节数
		 *
		 *	\retval	计算出的校验和
		 */
		WORD Get16SumCode(Byte * Buf, int iWordLen);

		/*!
		 *	\brief	返回16位WORD的高字节值
		 *
		 *	\param	wdCheck: 要计算的字
		 *
		 *	\retval	高字节值
		 */
		Byte GetWordHigh(WORD wdCheck);

		/*!
		 *	\brief	返回16位WORD的低字节值
		 *
		 *	\param	wdCheck: 要计算的字
		 *
		 *	\retval	低字节值
		 */
		Byte GetWordLow(WORD wdCheck);

		/*!
		 *	\brief	处理链路访问冲突,组包发送给另机
		 *
		 *	本接口是当处理串口链路访问冲突时调用,针对指定设备，组链路诊断数据包，并发送给另机
		 *
		 *	逻辑：判断双机通信正常、链路访问冲突、指定设备为主，且在状态诊断过程中
		 *		则清状态诊断标志，组诊断包，发送给另机。
		 *
		 *	\param	pDevice		:要处理的设备的指针
		 *	\param	pbyData		:通道诊断包的首指针
		 *	\param	nReturnLen	:通道诊断包的长度
		 *
		 *	\retval	0为成功,其他失败
		 */
		int DealLinkClash(CIODeviceBase* pDevice, Byte* pbyData, int& nReturnLen);

		/*!
		 *	\brief	将接收到的响应数据发送到另机
		 *
		 *	本机发送另机组的发送包,并发送后,接收到响应,将响应发送给另机
		 *
		 *	\param	bFromLocal	: 是否是本机收到的响应
		 *	\param	pbyData		: 收到的数据
		 *	\param	nDataLen	: 收到的数据长度
		 *
		 *	\retval	0为成功,其他失败
		 */
		int DealRcvLink( bool bFromLocal, Byte* pbyData, int nDataLen ); 

		/*!
		 *	\brief	转换longlong类型为时间
		 *
		 *	\param	const long:输入型变量，时间数据			
		 *
		 *	\retval　std::string 时间,格式为 年/月/日 时:分:秒
		 */	
		std::string llNum2Time( const long llNum2Time );

		/*!
		 *	\brief	把串口数据先放到m_pbyInnerBuf中，再象从串口读一样从这里取数
		 *
		 *	为兼容滑动窗口处理而设置的函数
		 *
		 *	\param	pBuf:要加的数据
		 *	\param	iLen:要加的数据的长度
		 *
		 *	\retval	void
		 */
		void InnerAddData( Byte* pBuf, int iLen );

		/*!
		 *	\brief	读取内部缓冲区中的数据,为滑动窗口服务
		 *
		 *	\param	pBuf	:读出的数据
		 *	\param	iBufLen	:希望读的长度
		 *	\param	iReadLen:实际读出数据的长度
		 *
		 *	\retval	bool		成功返回true，否则false
		 */
		bool InnerRead( Byte* pBuf, int iBufLen, int& iReadLen );


		
		virtual int TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut);
		
		virtual int OnConnSuccess(std::string pPeerIP);

	public:
		//!类名
		static std::string s_ClassName;
		
		//!有效包的最小长度
		int PRTCL_MIN_PACKET_LENGTH;

		//!有效包的最大长度
		int PRTCL_MAX_PACKET_LENGTH;

		//!接收数据缓冲区的最大长度
		int PRTCL_RCV_BUFFER_LENGTH;

		//!发送数据缓冲区的最大长度
		int PRTCL_SND_BUFFER_LENGTH;

		//!滑动窗口数据区的最大长度
		int PRTCL_MAX_INNER_LENGTH;

		//!超时时间
		int MACSDRIVER_MAX_TIMEOUT;

		//!发送缓冲区,长度为m_nSndBufLen
		Byte* m_pbySndBuffer;

	protected:
		//!协助滑动窗口用,内部数据区首指针
		Byte* m_pbyInnerBuf;

		//!协助滑动窗口用,内部数据长度
		int	 m_iInnerLen;

		//!接收缓冲区,长度为m_nRcvBufLen
		Byte* m_pbyRevBuffer;
		
		//!帧数据缓冲区,长度为m_nMaxPacketLen
		Byte* m_sPacketBuffer;

		//!当前接收缓冲区中有效数据的长度
		int m_nRevLength;

		//!区别发送包是本地组的还是另机组的,true为另机发送过来的
		bool m_bSendPacketIsFromPeer;

		//!对方发过来的诊断包,供本地缓存,在BuildRequest中发送
		tIOCtrlPacketCmd m_cmdPacket;
	};

	//!开辟内存空间
#define InitPrtclHelper( nMinPacketLen, nMaxPacketLen, nRcvBufLen, nSndBufLen, nInnerBufLen ) {\
	PRTCL_MIN_PACKET_LENGTH = nMinPacketLen;\
	PRTCL_MAX_PACKET_LENGTH = nMaxPacketLen;\
	PRTCL_RCV_BUFFER_LENGTH = nRcvBufLen;\
	PRTCL_SND_BUFFER_LENGTH = nSndBufLen;\
	PRTCL_MAX_INNER_LENGTH  = nInnerBufLen;\
	m_pbyRevBuffer = new Byte[(unsigned int)PRTCL_RCV_BUFFER_LENGTH];\
	m_pbySndBuffer = new Byte[(unsigned int)PRTCL_SND_BUFFER_LENGTH];\
	m_sPacketBuffer = new Byte[(unsigned int)PRTCL_MAX_PACKET_LENGTH];\
	m_pbyInnerBuf = new Byte[(unsigned int)PRTCL_MAX_INNER_LENGTH]; }

}

#endif