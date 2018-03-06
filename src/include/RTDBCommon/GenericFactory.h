/*!
 *	\file	GenericFactory.h
 *
 *	\brief	通用类工厂的定义和实现文件
 *
 *	本文件定义和实现了一个通用类工厂类，并提供了方便使用的宏定义。
 *	使用时，在模块初始化时调用GENERIC_REGISTER_CLASS(基类, 子类, 类工厂对象指针)
 *	进行子类注册，创建子类时就可以通过子类的类名调用类工厂的Create()方法进行创建
 *
 *	\author
 *
 *	\date	2005-2-22	9:05
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */
#ifndef _GENERICFACTORY_H_
#define _GENERICFACTORY_H_

#include <string>
#include <memory>
#include <map>
#include <utility>
#include <vector>

//#include "ace/Synch_T.h"
#include "ace/Singleton.h"

/*!
 *	\class	GenericFactory
 *
 *	\brief	用模板定义的class factory，并实现单件模式
 *
 *	该类用于创建具有相同基类的多个派生类的对象，基类通过模板参数ManufacturedType
 *	指定，派生类以模板参数ClassIDKey进行区分。
 */
template <class ManufacturedType, typename ClassIDKey=std::string>
class GenericFactory
{
    typedef ManufacturedType* (*BaseCreateFn)();
    typedef std::map< ClassIDKey, BaseCreateFn > FnRegistry;

    FnRegistry m_registry;

public:
	/*!
	 *	注册子类
	 */
	void RegCreateFn(const ClassIDKey &id, BaseCreateFn fn) 
	{
		m_registry.insert( typename FnRegistry::value_type(id, fn) );
	}

	/*!
	 *	取消子类的注册
	 */
	void UnRegCreateFn(const ClassIDKey &id) 
	{
		m_registry.erase( id );
	}
	
	/*!
	 *	根据子类关键字调用相应的构建函数创建子类对象
	 */
	ManufacturedType* Create(const ClassIDKey &id)
	{
		ManufacturedType* theObject = 0;

		typename FnRegistry::iterator regEntry = m_registry.find(id);
		if (regEntry != m_registry.end()) 
		{
			theObject = regEntry->second();
		}

		return theObject;
	}

	/*!
	 *	获取全部注册关键字列表
	 */
	std::vector<ClassIDKey> getKeys()
	{
		std::vector<ClassIDKey> keys;

		typename FnRegistry::iterator it, last = m_registry.end();
		for( it = m_registry.begin(); it != last; it++ )
		{
			keys.push_back( it->first );
		}

		return keys;
	}

	//! 利用ACE提供的单件ACE_Singleton类提供单件模式的支持
	typedef ACE_Singleton<GenericFactory<ManufacturedType, ClassIDKey>, ACE_Thread_Mutex> Singleton_GF;
	friend class ACE_Singleton<GenericFactory<ManufacturedType, ClassIDKey>, ACE_Thread_Mutex>;
};

/*!
 *	\class	RegisterInFactory
 *
 *	\brief	提供以无参构造函数为构建函数调用通用类工厂的辅助类
 *
 *	该类提供对宏的实现支持，要求每个子类提供无参的缺省构造函数
 */
template <class BaseType, class DerivedType, typename ClassIDKey=std::string>
class RegisterInFactory
{
public:
	/*!
	 *	所有子类的公共构建函数接口
	 */
    static BaseType* CreateInstance()
    {
        return (BaseType*)(new DerivedType);
    }

	/*!
	 *	注册辅助函数，为宏的展开实体
	 */
    RegisterInFactory(const ClassIDKey &id, GenericFactory<BaseType,ClassIDKey> *gf, bool reg = true)
    {
		if( reg )
			gf->RegCreateFn(id, CreateInstance);
		else
			gf->UnRegCreateFn(id);
    }
};

/*!
 *	使用模板类厂
 *	展开后为：	RegisterInFactory<Base, Base> registerBase("Base");
 *				RegisterInFactory<Base, Derived> registerDerived("Derived");
 */
#define GENERIC_REGISTER_CLASS(BASE_CLASS, DERIVED_CLASS, pGenericFactory) \
	RegisterInFactory<BASE_CLASS, DERIVED_CLASS> reg##DERIVED_CLASS( \
		/*#DERIVED_CLASS*/DERIVED_CLASS::s_ClassName, pGenericFactory); 

#define GENERIC_UNREGISTER_CLASS(BASE_CLASS, DERIVED_CLASS, pGenericFactory) \
	RegisterInFactory<BASE_CLASS, DERIVED_CLASS> reg##DERIVED_CLASS( \
		/*#DERIVED_CLASS*/DERIVED_CLASS::s_ClassName, pGenericFactory, false); 

#endif
