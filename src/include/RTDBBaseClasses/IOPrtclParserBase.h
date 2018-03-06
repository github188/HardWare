/*!
 *	\file	IOPrtclParserBase.h
 *
 *	\brief	协议基类接口定义文件
 *
 *	详细的文件说明（可选）
 *
 *	\author LIFAN
 *
 *	\date	2014年4月8日	10:42
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _IOPRTCLPARSERBASE_H_
#define _IOPRTCLPARSERBASE_H_


#include "RTDBCommon/GenericFactory.h"
#include "utilities/fepcommondef.h"
#include "RTDBCommon/OS_Ext.h"

#ifndef DBBASECLASSES_API
#ifdef DBBASECLASSES_EXPORTS
#define DBBASECLASSES_API MACS_DLL_EXPORT
#else
#define DBBASECLASSES_API MACS_DLL_IMPORT
#endif
#endif

namespace MACS_SCADA_SUD
{
	/*!
	 *	\class	CIOPrtclParserBase
	 *
	 *	\brief	协议处理类基类
	 *
	 *	详细的类说明（可选）
	 */
	class DBBASECLASSES_API CIOPrtclParserBase
	{

	public:
		/*!
		 *	\brief	缺省构造函数
		 */
		CIOPrtclParserBase();

		/*!
		 *	\brief	缺省析构函数
		 */
		virtual ~CIOPrtclParserBase();

		/*!
		 *	\brief	
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pbyData		数据指针
		 *	\param	nDataLen	数据长度
		 *
		 *	\retval	int	0为成功其他失败
		 *
		 *	\note		注意事项（可选）
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
		 *
		 *	\note		注意事项（可选）
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
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen, bool& bStopTimer, bool& bSendData);

		/*!
		 *	\brief	处理接收数据
		 *
		 *	\param	pDataBuf: 接收到的数据
		 *	\param	nLen	: 接收到的数据的长度
		 *
		 *	\retval	int	0为成功，其他失败
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int RxDataProc(Byte* pDataBuf, int& nLen);

		/*!
		 *	\brief	超时处理
		 *
		 *	本接口由驱动器定时器超时处理中调用,通过引用bSendData和bTrigTimer来控制是否发送数据和启动超时定时器
		 *	如果是使用COMX类型的需要重载本接口
		 *
		 *	\param	bSendData	: 本接口调用后,驱动器是否发送数据,true为发送数据,false不发送
		 *	\param	bTrigTimer	: 本接口调用后,驱动器是否启动超时定时器的标志，true则启动超时定时器，false则不启动
		 *	\param	nTimeOut	: 超时时间
		 *
		 *	\retval	返回值	对该返回值的说明（一个返回值对应一个）
		 *
		 *	\note		注意事项（可选）
		 */
		virtual int TimeOut(bool& bSendData, bool& bTrigTimer, int& nTimeOut);

		/*!
		 *	\brief	接收到通道诊断包
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	ptPacket: 诊断包指针
		 *
		 *	\retval	返回值	int 0为成功其他失败
		 */
		virtual int RcvPcketCmd(tIOCtrlPacketCmd* ptPacket);

		/*!
		 *	\brief	连接成功后由驱动器回调，用于网络连接类协议
		 *
		 *	详细的函数说明（可选）
		 *
		 *	\param	pPeerIp		：对端IP
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int OnConnSuccess(std::string pPeerIP);

		/*!
		 *	\brief	返回类名
		 */
		std::string getClassName();

		//!设置多TCP连接下数据下发处理指针
		void SetHandle(void* pHandle);

		//!获取多TCP连接下数据下发处理指针
		void* GetHandle();

		//!设置驱动对象指针
		void SetParam(void* pParam);

		//!得到驱动对象指针
		void* GetParam();

		/*!
		 *	\brief	停止协议解析类中的定时器
		 *
		 *	\retval	int	0为成功,其他失败
		 */
		virtual int StopTimer();

	public:
		//!类名
		static std::string s_ClassName;
			
	protected:
		//！参数指针，一般为驱动对象指针
		void* m_pParam;

	private:
		//!多TCP连接下数据下发处理指针(103G驱动用到)
		void* m_pHandle;
	};

	//!输出全局变量
	extern DBBASECLASSES_API GenericFactory<CIOPrtclParserBase> g_DrvPrtclFactory;
}

#endif