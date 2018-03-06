/*!
 *	\file	FepObjectType.h
 *
 *	\brief	FEP����Ļ���
 *
 *
 *	\author lingling.li
 *
 *	\date	2014��6��20��	11:23
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2013-2014, SCADA V4ϵͳ������,HollySys Co.,Ltd
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
	//! ��������ָ�룬����Ŀǰ���õĹ��췽ʽ������Ĺ��캯���д������Զ��������Ҫ
	//! ���Ӹýӿ�����¼���Զ����ָ�롣
	//virtual void SetItemPointer(UaQualifiedName& strItem, UaNode* pNode, NodeManagerConfig* pNodeConfig);
	//! ��ȡ��̬����ָ�룬���㸴�����͵Ļ�������
	//! ���Ӹýӿ�����ȡ��̬����ָ��
	//virtual UaNode* GetStaticItem(UaQualifiedName& strItem);

public:
	virtual void SetDeleteFlag(bool bDelete);
	virtual bool GetDeleteFlag();

private:
	//���߸���ʱ��ɾ����־
	bool m_bDelete;
	//UaMutex       m_bDeleteLock;
};
}// End namespace
#endif // __FEPOBJECTTYPE_H__

