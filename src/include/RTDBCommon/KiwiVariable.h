#ifndef _KIWI_VARIANT_H_
#define _KIWI_VARIANT_H_


#include "KiwiVariant.h"
#include <map>
#include <string>
#include "RTDBCommon/nodebase.h"

namespace MACS_SCADA_SUD
{
	class COMMONBASECODE_API CKiwiVariable
		: public CNode
	{
	public:
		CKiwiVariable(long	nNodeID);
		virtual ~CKiwiVariable(void);

		std::string szName;//±êÇ©Ãû

		void InitAttribute(void);

		long GetTimeStamp(void);

		float GetfValue();

		int GetValue();

		unsigned char GetByteValue();

		void AddAttribute(const std::string& szAttribute);

		bool DelAttribute(const std::string& szAttribute);

		int setAttributeValue(const std::string& szAttribute, const CKiwiVariant& value);

		CKiwiVariant  getAttributeValue(const std::string& szAttribute);
	private:
		std::map<std::string, CKiwiVariant*> m_mapAttribute;
	};
	
};


#endif