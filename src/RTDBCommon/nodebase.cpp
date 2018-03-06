
#include "RTDBCommon/nodebase.h"

namespace MACS_SCADA_SUD
{
    CNode::CNode()
    :nNodeID(0)
    {

    }
    CNode::CNode(long nID)
    :nNodeID(nID)
    {

    }
    CNode::~CNode()
    {

    }
    long CNode::nodeId()
    {
        return nNodeID;
    }
    void CNode::SetNodeID(long lID)
    {
        nNodeID = lID;
    }

}