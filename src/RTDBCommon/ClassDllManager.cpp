
#include "RTDBCommon/ClassDllManager.h"
//#include "srvtrace.h"
#include "RTDBCommon/RuntimeCatlog.h"
#include "RTDBCommon/Debug_Msg.h"

typedef int (*fnInitDll)();
typedef int (*fnUnInitDll)( );

/////////////////////////////////////////////////////////////////////////////
// CEasClassDllMgrImpl

namespace MACS_SCADA_SUD
{

	CClassDllManager::CClassDllManager()
	{
	}

	/*!
	*	\brief	析构函数
	*
	*	卸载程序已加载的动态库，并清空类-动态库表
	*/
	CClassDllManager::~CClassDllManager()
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_MapMutex );

		fnUnInitDll uninitdll = 0;

		//! 卸载动态库
		DLL_HANDLE_MAP_TYPE::iterator it;
		for( it=m_dll_handle_map.begin(); it!=m_dll_handle_map.end(); it++ )
		{
			ACE_DLL* dll_handle =  it->second;

			if(dll_handle == 0)	continue;

			uninitdll = 0;
			uninitdll = (fnUnInitDll)dll_handle->symbol(ACE_TEXT("UnInitDll"));
			if( uninitdll != 0 )
			{
				uninitdll( );	
			}

			delete dll_handle;
			//ACE_DLL_Manager::instance()->close_dll((it->first).c_str());
		}

		//!清空map 
		m_dll_handle_map.clear();
	}

	/////////////////////////////////////////////////////////////////////////////
	// Override Functions
	int CClassDllManager::Init()
	{
		return 0;
	}

	int CClassDllManager::UnInit()
	{
		return 0;
	}

	int CClassDllManager::Test()
	{
		return 0;
	}

	/////////////////////////////////////////////////////////////////////////////
	// New Implement Functions

	/*!
	*	\brief	是否存在某动态库
	*
	*	判断是否已经加载了某动态库
	*
	*	\param	DllName	动态库名
	*
	*	\retval	ACE_DLL*	动态库的句柄指针,如果不存在,返回值为NULL
	*
	*	\note		注意事项（可选）
	*/
	ACE_DLL* CClassDllManager::Exist(std::string DllName)
	{
		ACE_Guard<ACE_Thread_Mutex> guard( m_MapMutex );

		DLL_HANDLE_MAP_TYPE::iterator it=m_dll_handle_map.find(DllName);
		if(it!=m_dll_handle_map.end())
			return it->second;
		return (ACE_DLL*)0;
	}

	/*!
	*	\brief	加载
	*
	*	详细的函数说明（可选）
	*
	*	\param	DllName	动态库名
	*
	*	\retval	ACE_DLL*	动态库的句柄指针,如果不存在,返回值为NULL
	*
	*	\note		注意事项（可选）
	*/
	ACE_DLL* CClassDllManager::LoadDll(std::string DllName)
	{
		std::string temp_dllname;
		//!获得正确的装载库名称
		temp_dllname = CRuntimeCatlogBase::GenerateModuleName(DllName.c_str());

		//! 先判断是否已经加载了
		ACE_DLL* dll_handle = NULL;
		dll_handle = Exist( DllName );
		if ( dll_handle )
		{
			return dll_handle;
		}

		ACE_Guard<ACE_Thread_Mutex> guard( m_MapMutex );

		fnInitDll initdll = 0;
#ifndef _WIN64
		std::string filepath = SingletonRuntimeCatlog::instance()->GetSetupDir()+"bin/"+temp_dllname;
#else
		std::string filepath = SingletonRuntimeCatlog::instance()->GetSetupDir()+"bin/x64/"+temp_dllname;
#endif

		//! 加载dll
		dll_handle = new ACE_DLL();
		if ( dll_handle )
		{
			if ( dll_handle->open( filepath.c_str(), RTLD_LAZY )!=0 )
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOClassDllMgrImpl::LoadDll LoadFile failed, FilePath:%s!!\n"), filepath.c_str()));
				return NULL;
			}
		}

		initdll = (fnInitDll)dll_handle->symbol(ACE_TEXT("InitDll"));
		if( initdll == 0 )
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOClassDllMgrImpl::LoadDll Call InitDll Failed， FileName=%s!\n"),filepath.c_str()));
			//MACS_ERROR(ERROR_PERFIX
			//	ACE_TEXT("CIOClassDllMgrImpl::LoadDll Call InitDll Failed， FileName=%s!\n"),filepath.c_str())

			delete dll_handle;
			//ACE_DLL_Manager::instance()->close_dll(DllName.c_str());
			return NULL;
		}

		initdll();

		//! 保存文件名和相应句柄
		m_dll_handle_map.insert(
			DLL_HANDLE_MAP_TYPE::value_type( DllName, dll_handle )
			);

		return dll_handle;
	}
	
	/*!
	*	\brief	加载
	*
	*	详细的函数说明（可选）
	*
	*	\param	DllName	动态库名
	*
	*	\retval	ACE_DLL*	动态库的句柄指针,如果不存在,返回值为NULL
	*
	*	\note		注意事项（可选）
	*/
	ACE_DLL* CClassDllManager::LoadDrvDll(std::string DllName)
	{
		std::string temp_dllname;
		//!获得正确的装载库名称
		temp_dllname = CRuntimeCatlogBase::GenerateModuleName(DllName.c_str());

		//! 先判断是否已经加载了
		ACE_DLL* dll_handle = NULL;
		dll_handle = Exist( DllName );
		if ( dll_handle )
		{
			return dll_handle;
		}

		ACE_Guard<ACE_Thread_Mutex> guard( m_MapMutex );

		fnInitDll initdll = 0;
#ifndef _WIN64
		std::string filepath = SingletonRuntimeCatlog::instance()->GetSetupDir()+"bin/Driver/"+temp_dllname;
#else
		std::string filepath = SingletonRuntimeCatlog::instance()->GetSetupDir()+"bin/x64/Driver/"+temp_dllname;
#endif

		//! 加载dll
		dll_handle = new ACE_DLL();
		if ( dll_handle )
		{
			if ( dll_handle->open( filepath.c_str(), RTLD_LAZY )!=0 )
			{
				MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOClassDllMgrImpl::LoadDll LoadFile failed, FilePath:%s!!\n"), filepath.c_str()));
				return NULL;
			}
		}

		initdll = (fnInitDll)dll_handle->symbol(ACE_TEXT("InitDll"));
		if( initdll == 0 )
		{
			MACS_ERROR((ERROR_PERFIX ACE_TEXT("CIOClassDllMgrImpl::LoadDll Call InitDll Failed， FileName=%s!\n"),filepath.c_str()));
			//MACS_ERROR(ERROR_PERFIX
			//	ACE_TEXT("CIOClassDllMgrImpl::LoadDll Call InitDll Failed， FileName=%s!\n"),filepath.c_str())

			delete dll_handle;
			//ACE_DLL_Manager::instance()->close_dll(DllName.c_str());
			return NULL;
		}

		initdll();

		//! 保存文件名和相应句柄
		m_dll_handle_map.insert(
			DLL_HANDLE_MAP_TYPE::value_type( DllName, dll_handle )
			);

		return dll_handle;
	}

	/*!
	*	\brief	获取动态库的句柄
	*
	*
	*	\param	DllName	动态库名
	*
	*	\retval	ACE_DLL*	动态库的句柄指针,如果不存在,返回值为NULL
	*
	*	\note		注意事项（可选）
	*/
	ACE_DLL* CClassDllManager::GetDll(std::string DllName)
	{
		return Exist(DllName);
	}

	/*!
	*	\brief	获取动态库所在路径
	*
	*	\retval	std::string	动态库所在路径
	*
	*	\note		注意事项（可选）
	*/
	std::string CClassDllManager::GetModulePath(void)
	{
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];

		_splitpath( __argv[0], drive, dir, fname, ext );

		std::string szModule;
		szModule = drive;
		szModule+= dir;

		return szModule;
	}
}
