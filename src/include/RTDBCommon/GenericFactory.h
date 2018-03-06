/*!
 *	\file	GenericFactory.h
 *
 *	\brief	ͨ���๤���Ķ����ʵ���ļ�
 *
 *	���ļ������ʵ����һ��ͨ���๤���࣬���ṩ�˷���ʹ�õĺ궨�塣
 *	ʹ��ʱ����ģ���ʼ��ʱ����GENERIC_REGISTER_CLASS(����, ����, �๤������ָ��)
 *	��������ע�ᣬ��������ʱ�Ϳ���ͨ����������������๤����Create()�������д���
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
 *	\brief	��ģ�嶨���class factory����ʵ�ֵ���ģʽ
 *
 *	�������ڴ���������ͬ����Ķ��������Ķ��󣬻���ͨ��ģ�����ManufacturedType
 *	ָ������������ģ�����ClassIDKey�������֡�
 */
template <class ManufacturedType, typename ClassIDKey=std::string>
class GenericFactory
{
    typedef ManufacturedType* (*BaseCreateFn)();
    typedef std::map< ClassIDKey, BaseCreateFn > FnRegistry;

    FnRegistry m_registry;

public:
	/*!
	 *	ע������
	 */
	void RegCreateFn(const ClassIDKey &id, BaseCreateFn fn) 
	{
		m_registry.insert( typename FnRegistry::value_type(id, fn) );
	}

	/*!
	 *	ȡ�������ע��
	 */
	void UnRegCreateFn(const ClassIDKey &id) 
	{
		m_registry.erase( id );
	}
	
	/*!
	 *	��������ؼ��ֵ�����Ӧ�Ĺ������������������
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
	 *	��ȡȫ��ע��ؼ����б�
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

	//! ����ACE�ṩ�ĵ���ACE_Singleton���ṩ����ģʽ��֧��
	typedef ACE_Singleton<GenericFactory<ManufacturedType, ClassIDKey>, ACE_Thread_Mutex> Singleton_GF;
	friend class ACE_Singleton<GenericFactory<ManufacturedType, ClassIDKey>, ACE_Thread_Mutex>;
};

/*!
 *	\class	RegisterInFactory
 *
 *	\brief	�ṩ���޲ι��캯��Ϊ������������ͨ���๤���ĸ�����
 *
 *	�����ṩ�Ժ��ʵ��֧�֣�Ҫ��ÿ�������ṩ�޲ε�ȱʡ���캯��
 */
template <class BaseType, class DerivedType, typename ClassIDKey=std::string>
class RegisterInFactory
{
public:
	/*!
	 *	��������Ĺ������������ӿ�
	 */
    static BaseType* CreateInstance()
    {
        return (BaseType*)(new DerivedType);
    }

	/*!
	 *	ע�Ḩ��������Ϊ���չ��ʵ��
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
 *	ʹ��ģ���೧
 *	չ����Ϊ��	RegisterInFactory<Base, Base> registerBase("Base");
 *				RegisterInFactory<Base, Derived> registerDerived("Derived");
 */
#define GENERIC_REGISTER_CLASS(BASE_CLASS, DERIVED_CLASS, pGenericFactory) \
	RegisterInFactory<BASE_CLASS, DERIVED_CLASS> reg##DERIVED_CLASS( \
		/*#DERIVED_CLASS*/DERIVED_CLASS::s_ClassName, pGenericFactory); 

#define GENERIC_UNREGISTER_CLASS(BASE_CLASS, DERIVED_CLASS, pGenericFactory) \
	RegisterInFactory<BASE_CLASS, DERIVED_CLASS> reg##DERIVED_CLASS( \
		/*#DERIVED_CLASS*/DERIVED_CLASS::s_ClassName, pGenericFactory, false); 

#endif
