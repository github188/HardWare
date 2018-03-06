#ifndef _NODE_BASE_H_
#define _NODE_BASE_H_

#include "OS_Ext.h"

namespace MACS_SCADA_SUD 
{
    class COMMONBASECODE_API CNode
    {
    public:
        CNode();
        CNode(long nID);
        virtual ~CNode();
    public:
        virtual long nodeId();
        virtual void SetNodeID(long);
    private:
        long nNodeID;
    };
}
#endif //_NODE_BASE_H_
