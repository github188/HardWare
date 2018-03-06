#ifndef _KIWI_VARIANT_H_
#define _KIWI_VARIANT_H_


#include "KiwiVariant.h"
#include <map>
#include <string>

namespace MACS_SCADA_SUD
{
	class COMMONBASECODE_API CKiwiVariable
	{
	public:
		CKiwiVariable(long	nNodeID);
		virtual ~CKiwiVariable(void);

		std::string szName;//±êÇ©Ãû

		long	nodeId();

		void InitAttribute(void);

		long GetTimeStamp(void);

		float GetfValue();

		int GetValue();

		unsigned char GetByteValue();

		void AddAttribute(const std::string& szAttribute);

		bool DelAttribute(const std::string& szAttribute);

		int setAttributeValue(const std::string& szAttribute, const CKiwiVariant& value);

		int getAttributeValue(const std::string& szAttribute, CKiwiVariant& value);
	private:
		std::map<std::string, CKiwiVariant*> m_mapAttribute;
		long	m_nNodeID;
	};
	
};


#endif