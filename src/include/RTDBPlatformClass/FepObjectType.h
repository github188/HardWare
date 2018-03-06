/*!
 *	\file	FepObjectType.h
 *
 *	\brief	FEP对象的基类
 *
 *
 *	\author lingling.li
 *
 *	\date	2014年6月20日	11:23
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4系统开发组,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#ifndef __FEPOBJECTTYPE_H__
#define __FEPOBJECTTYPE_H__
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/nodebase.h"

#ifndef FEPPLATFORMCLASS_API
#ifdef FEPPLATFORMCLASS_EXPORTS
# define FEPPLATFORMCLASS_API MACS_DLL_EXPORT
#else
# define FEPPLATFORMCLASS_API MACS_DLL_IMPORT
#endif
#endif

// Namespace
namespace MACS_SCADA_SUD {

class FEPPLATFORMCLASS_API CFepObjectType : public CNode
{
protected:
	virtual ~CFepObjectType(void);

public:
    CFepObjectType(long nNodeID);
public:
	//! 设置属性指针，由于目前采用的构造方式不在类的构造函数中创建属性对象，因此需要
	//! 增加该接口来记录属性对象的指针。
	//virtual void SetItemPointer(UaQualifiedName& strItem, UaNode* pNode, NodeManagerConfig* pNodeConfig);
	//! 获取静态属性指针，方便复用类型的基本属性
	//! 增加该接口来获取静态属性指针
	//virtual UaNode* GetStaticItem(UaQualifiedName& strItem);

public:
	virtual void SetDeleteFlag(bool bDelete);
	virtual bool GetDeleteFlag();

private:
	//在线更新时的删除标志
	bool m_bDelete;
	//UaMutex       m_bDeleteLock;
};
}// End namespace
#endif // __FEPOBJECTTYPE_H__

