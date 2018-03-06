#include "RTDBPlatformClass/FepObjectType.h"

using namespace MACS_SCADA_SUD;


/**  Constructs an instance of the class CIOUnit with all components
*/
CFepObjectType::CFepObjectType(long nNodeID)
	: CNode(nNodeID)
{
}


CFepObjectType::~CFepObjectType(void)
{
}

// void CFepObjectType::SetItemPointer(UaQualifiedName& strItem, UaNode* pNode, NodeManagerConfig* pNodeConfig)
// {
// }

//! 获取静态属性指针，方便复用类型的基本属性
//! 增加该接口来获取静态属性指针
// UaNode* CFepObjectType::GetStaticItem(UaQualifiedName& strItem)
// {
// 	return NULL;
// }

void CFepObjectType::SetDeleteFlag(bool bDelete)
{
	// UaMutexLocker lock(&m_bDeleteLock);
	m_bDelete = bDelete;
}

bool CFepObjectType::GetDeleteFlag()
{
	// UaMutexLocker lock(&m_bDeleteLock);
	return m_bDelete;
}
