/*!
*	\file	DataTrans.h
*
*	\brief	数据转换操作类
*
*	$Author: Wangyuanxing $
*	$Date: 14-04-01 9:09 $
*	$Revision: 1.0 $
*	\attention	Copyright (c) 2002-2008, MACS-SCADA System Development, HollySys Co.,Ltd
*	\attention	All rights reserved.
*/
#ifndef DATATRANS_H
#define DATATRANS_H

//#include "CharTrans.h"
#include "RTDBCommon/OS_Ext.h"
#include "RTDBCommon/KiwiVariable.h"
#include "RTDBCommon/CommonInfo.h"
#include "RTDBPlatformClass/GeneralConfig.h"

#include <string.h>
using namespace std;

//class UaXmlElement;

namespace MACS_SCADA_SUD{

	//const OpcUa_StatusCode StatusCode_Special = 16;

	//typedef enum RangeFlag
	//{
	//	Low = 0,
	//	Normal,
	//	High
	//};

	//OpcUa_Int32 static g_isBitMask[] = {
	//	0x00000001,0x00000002,0x00000004,0x00000008,
	//	0x00000010,0x00000020,0x00000040,0x00000080,
	//	0x00000100,0x00000200,0x00000400,0x00000800,
	//	0x00001000,0x00002000,0x00004000,0x00008000,
	//	0x00010000,0x00020000,0x00040000,0x00080000,
	//	0x00100000,0x00200000,0x00400000,0x00800000,
	//	0x01000000,0x02000000,0x04000000,0x08000000,
	//	0x10000000,0x20000000,0x40000000,0x80000000
	//};

	///*!
	//*	\brief	判定两个BrowseName是否相同
	//*			由于UaQualifiedName中既包含namespaceindex 又包含string名字
	//*			这里不关心namespaceindex 而只比较string名字
	//*	\param	nodeName	[in]		配置文件中的浏览名
	//*	\param	snodeName	[in]		静态成员变量中的浏览名
	//*	\retval	OpcUa_Boolean			相同返回true 否则返回false
	//*	
	//*/
	//OpcUa_Boolean static CheckBrowseNameSame(UaQualifiedName nodeName, UaQualifiedName snodeName)
	//{
	//	return nodeName.toString() == snodeName.toString();
	//}

	///*!
	//*	\brief	判定BrowseName中字符串是否
	//*			由于UaQualifiedName中既包含namespaceindex 又包含string名字
	//*			这里不关心namespaceindex 而只比较string名字
	//*	\param	nodeName	[in]		配置文件中的浏览名
	//*	\param	sName		[in]		特定的字符串
	//*	\retval	OpcUa_Boolean			查找到返回true 否则返回false
	//*	
	//*/
	//OpcUa_Boolean static IsInBrowseName(UaQualifiedName nodeName, string sName)
	//{
	//	return string(nodeName.toString().toUtf8()).find(sName) != string::npos;
	//}

	//OpcUa_Boolean static ParseStringInfo(const std::string& strCxt, char _ch, std::string& sfront, std::string& sback)
	//{
	//	OpcUa_Boolean ret = OpcUa_False;
	//	if(strCxt.length() < 1)
	//		return ret;

	//	string::size_type len = strCxt.size();
	//	string::size_type pos = strCxt.find(_ch);
	//	if(pos != string::npos)
	//	{
	//		sback = strCxt.substr(pos + 1, len-pos-1);
	//		sfront = strCxt.substr(0, pos);
	//		ret = OpcUa_True;
	//	}
	//	else
	//	{
	//		//! 没有分割符的话全当后者
	//		sback = strCxt;
	//	}

	//	return ret;
	//}

	//void static ParseUserRoleAndPos(const std::string& strCxt, std::string& sUserName, std::string& sRoleName, std::string& sPosName)
	//{
	//	std::string sTmp;
	//	ParseStringInfo(strCxt, '+', sUserName, sTmp);
	//	OpcUa_Boolean bFind = ParseStringInfo(sTmp, '*', sRoleName, sPosName);
	//	if(!bFind)
	//	{
	//		sRoleName = sPosName;
	//		sPosName = "";
	//	}
	//}

	//OpcUa_Int32 static GetVariantDataLength(UaVariant value)
	//{
	//	OpcUa_Int32 ret = 0;

	//	UaNodeId nodeId = value.dataType();
	//	OpcUa_NodeId NodeId;
	//	nodeId.copyTo(&NodeId);

	//	if (NodeId.IdentifierType == OpcUa_IdentifierType_Numeric)
	//	{
	//		switch(NodeId.Identifier.Numeric)
	//		{
	//		case OpcUaId_Boolean:
	//			ret = sizeof(OpcUa_Boolean);
	//			break;
	//		case OpcUaId_Byte:
	//			ret = sizeof(OpcUa_Byte);
	//			break;
	//		case OpcUaId_Double:
	//			ret = sizeof(OpcUa_Double);
	//			break;
	//		case OpcUaId_Float:
	//			ret = sizeof(OpcUa_Float);
	//			break;
	//		case OpcUaId_Int16:
	//			ret = sizeof(OpcUa_Int16);
	//			break;
	//		case OpcUaId_Int32:
	//			ret = sizeof(OpcUa_Int32);
	//			break;
	//		case OpcUaId_Int64:
	//			ret = sizeof(OpcUa_Int64);
	//			break;
	//		case OpcUaId_UInt16:
	//			ret = sizeof(OpcUa_UInt16);
	//			break;
	//		case OpcUaId_UInt32:
	//			ret = sizeof(OpcUa_UInt32);
	//			break;
	//		case OpcUaId_UInt64:
	//			ret = sizeof(OpcUa_UInt64);
	//			break;
	//		case OpcUaId_LocalizedText:
	//			break;
	//		case OpcUaId_String:
	//			ret = value.toString().length();
	//			break;
	//		case OpcUaId_SByte:
	//			ret = sizeof(OpcUa_SByte);
	//			break;
	//		case OpcUaId_ByteString:
	//			{
	//				UaByteString uatmpByteStr;
	//				value.toByteString(uatmpByteStr);
	//				ret = uatmpByteStr.length();				
	//			}
	//			break;
	//		case OpcUaId_QualifiedName:

	//			break;
	//		}
	//	}
	//	OpcUa_NodeId_Clear(&NodeId);   //!释放资源

	//	return ret;
	//}

	//static UaString TransferByteString2String(const UaByteString& bs)
	//{
	//	UaString str;

	//	if (bs.length() > 0)
	//	{
	//		int len = bs.length();
	//		OpcUa_Byte* pData = new OpcUa_Byte[len + 1];
	//		memset(pData, 0, len + 1);
	//		memcpy(pData, bs.data(), len);
	//		str = UaString((char*)pData);

	//		if(NULL != pData)
	//		{
	//			delete []pData;
	//			pData = NULL;
	//		}
	//	}

	//	return str;
	//}

	//static UaByteString TransferString2ByteString(const UaString& sData)
	//{
	//	UaByteString bsval;

	//	int len = sData.length();
	//	if(len > 0)
	//	{
	//		OpcUa_Byte* pData = new OpcUa_Byte[len + 1];
	//		memset(pData, 0, len + 1);
	//		memcpy(pData, sData.toUtf8(), len);
	//		
	//		bsval.setByteString(len, pData);

	//		if(NULL != pData)
	//		{
	//			delete []pData;
	//			pData = NULL;
	//		}
	//	}

	//	return bsval;
	//}

	//static void FormatArrayValues(const vector<UaString>& values, UaNodeId datatype, UaVariant& value)
	//{
	//	UaStatus ret;
	//	UaVariant variant;

	//	OpcUa_NodeId NodeId;
	//	datatype.copyTo(&NodeId);

	//	if (NodeId.IdentifierType == OpcUa_IdentifierType_Numeric)
	//	{
	//		switch(NodeId.Identifier.Numeric)
	//		{
	//		case OpcUaId_Boolean:
	//			{
	//				UaBooleanArray boolArray;
	//				value.toBoolArray(boolArray);

	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Boolean bval;
	//					ret = variant.toBool(bval);
	//					if(ret.isGood())
	//						boolArray[i] = bval;
	//				}

	//				value.setBoolArray(boolArray);
	//			}
	//			break;
	//		case OpcUaId_SByte:
	//			{
	//				UaSByteArray sbArray;
	//				value.toSByteArray(sbArray);

	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_SByte sbval;
	//					ret = variant.toSByte(sbval);
	//					if(ret.isGood())
	//						sbArray[i] = sbval;
	//				}

	//				value.setSByteArray(sbArray);
	//			}

	//			break;
	//		case OpcUaId_Byte:
	//			{
	//				UaByteArray byteArray;
	//				value.toByteArray(byteArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Byte bval;
	//					ret = variant.toByte(bval);
	//					if(ret.isGood())
	//						byteArray[i] = bval;
	//				}
	//				value.setByteArray(byteArray);
	//			}
	//			break;
	//		case OpcUaId_Int16:
	//			{
	//				UaInt16Array iArray;
	//				value.toInt16Array(iArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Int16 ival;
	//					ret = variant.toInt16(ival);
	//					if(ret.isGood())
	//						iArray[i] = ival;
	//				}
	//				value.setInt16Array(iArray);
	//			}
	//			break;
	//		case OpcUaId_UInt16:
	//			{
	//				UaUInt16Array iArray;
	//				value.toUInt16Array(iArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_UInt16 ival;
	//					ret = variant.toUInt16(ival);
	//					if(ret.isGood())
	//						iArray[i] = ival;
	//				}
	//				value.setUInt16Array(iArray);
	//			}
	//			break;
	//		case OpcUaId_Int32:
	//			{
	//				UaInt32Array iArray;
	//				value.toInt32Array(iArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Int32 ival;
	//					ret = variant.toInt32(ival);
	//					if(ret.isGood())
	//						iArray[i] = ival;
	//				}
	//				value.setInt32Array(iArray);
	//			}
	//			break;
	//		case OpcUaId_UInt32:
	//			{
	//				UaUInt32Array iArray;
	//				value.toUInt32Array(iArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_UInt32 ival;
	//					ret = variant.toUInt32(ival);
	//					if(ret.isGood())
	//						iArray[i] = ival;
	//				}
	//				value.setUInt32Array(iArray);
	//			}
	//			break;
	//		case OpcUaId_Int64:
	//			{
	//				UaInt64Array iArray;
	//				value.toInt64Array(iArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Int64 ival;
	//					ret = variant.toInt64(ival);
	//					if(ret.isGood())
	//						iArray[i] = ival;
	//				}
	//				value.setInt64Array(iArray);
	//			}
	//			break;
	//		case OpcUaId_UInt64:
	//			{
	//				UaUInt64Array iArray;
	//				value.toUInt64Array(iArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_UInt64 ival;
	//					ret = variant.toUInt64(ival);
	//					if(ret.isGood())
	//						iArray[i] = ival;
	//				}
	//				value.setUInt64Array(iArray);
	//			}
	//			break;
	//		case OpcUaId_Float:
	//			{
	//				UaFloatArray fArray;
	//				value.toFloatArray(fArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Float fval;
	//					ret = variant.toFloat(fval);
	//					if(ret.isGood())
	//						fArray[i] = fval;
	//				}
	//				value.setFloatArray(fArray);
	//			}
	//			break;
	//		case OpcUaId_Double:
	//			{
	//				UaDoubleArray dArray;
	//				value.toDoubleArray(dArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					OpcUa_Double dval;
	//					ret = variant.toDouble(dval);
	//					if(ret.isGood())
	//						dArray[i] = dval;
	//				}
	//				value.setDoubleArray(dArray);
	//			}

	//			break;
	//		case OpcUaId_String:
	//			{
	//				UaStringArray sArray;
	//				value.toStringArray(sArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].copyTo(&sArray[i]);
	//				}
	//				value.setStringArray(sArray);
	//			}			
	//			break;
	//		case OpcUaId_LocalizedText:
	//			{
	//				UaLocalizedTextArray sArray;
	//				value.toLocalizedTextArray(sArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					UaLocalizedText lt("en", values[i]);
	//					lt.copyTo(&sArray[i]);
	//				}
	//				value.setLocalizedTextArray(sArray);
	//			}
	//			break;
	//		case OpcUaId_DateTime:
	//		case OpcUaId_UtcTime:
	//			{
	//				UaDateTimeArray sArray;
	//				value.toDateTimeArray(sArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					values[i].toVariant(variant);
	//					UaDateTime val;
	//					ret = variant.toDateTime(val);
	//					if(ret.isGood())
	//						sArray[i] = val;
	//				}
	//				value.setDateTimeArray(sArray);
	//			}

	//			break;
	//		case OpcUaId_Guid:
	//			{
	//				UaGuidArray sArray;
	//				value.toGuidArray(sArray);
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					UaGuid val;
	//					ret = val.fromString(values[i]);
	//					if(ret.isGood())
	//						val.copyTo(&sArray[i]);
	//				}
	//				value.setGuidArray(sArray);
	//			}

	//			break;
	//		case OpcUaId_ByteString:
	//			{
	//				/*
	//				UaByteStringArray sArray;
	//				sArray.resize((OpcUa_UInt32) values.size());
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					UaByteString val;
	//					const char *szContent = values[i].toUtf8();
	//					val.setByteString(values[i].length(), (OpcUa_Byte*)szContent);
	//					if(ret.isGood())
	//						val.copyTo(&sArray[i]);
	//				}
	//				value.setByteStringArray(sArray);
	//				*/

	//				UaByteStringArray sArray;
	//				sArray.resize((OpcUa_UInt32) values.size());

	//				for(int i=0; i < values.size(); i++)
	//				{
	//					UaByteString val = TransferString2ByteString(values[i]);
	//					val.copyTo(&sArray[i]);
	//				}

	//				value.setByteStringArray(sArray);
	//			}

	//			break;
	//		case OpcUaId_XmlElement:
	//			{
	//				UaByteStringArray sArray;
	//				sArray.resize((OpcUa_UInt32) values.size());
	//				for(int i=0; i < values.size(); i++)
	//				{
	//					UaByteString val;
	//					const char *szContent = values[i].toUtf8();
	//					val.setByteString(values[i].length(), (OpcUa_Byte*)szContent);
	//					if(ret.isGood())
	//						val.copyTo(&sArray[i]);
	//				}
	//				value.setXmlElementArray(sArray, OpcUa_True);
	//			}
	//			break;

	//		default:
	//			// ToDo: OpcUaType_Variant
	//			// ToDo: OpcUaType_DiagnosticInfo
	//			break;
	//		}
	//	}
	//	else if(NodeId.IdentifierType == OpcUa_IdentifierType_Guid)
	//	{
	//		UaGuidArray sArray;
	//		value.toGuidArray(sArray);
	//		for(int i=0; i < values.size(); i++)
	//		{
	//			values[i].toVariant(variant);
	//			UaGuid val;
	//			ret = variant.toGuid(val);
	//			if(ret.isGood())
	//				val.copyTo(&sArray[i]);
	//		}
	//		value.setGuidArray(sArray);
	//	}

	//	OpcUa_NodeId_Clear(&NodeId);   //!释放资源
	//}

	///*
	//*	将客户端发来的改写数组内某一下标的数改为全数据
	//*	param1: targetValue	数组型UaVariant 里面仅含一个元素 是客户端指定index下标的元素的目标值
	//*	param2: oldValue	数组型UaVariant 里面包含数组的所有下标元素
	//*	param3: index		客户端想改写的下标
	//*
	//*/
	//static void AdjustArrayIndexValue(UaVariant& targetValue, const UaVariant& oldValue, OpcUa_Int32 index)
	//{
	//	if(targetValue.arrayType() == OpcUa_VariantArrayType_Array 
	//		&&  oldValue.arrayType() == OpcUa_VariantArrayType_Array
	//		&& index >= 0)
	//	{
	//		switch (oldValue.type())
	//		{
	//		case OpcUaType_Boolean:
	//			{
	//				UaBoolArray barray;
	//				targetValue.toBoolArray(barray);
	//				UaBoolArray barray2;
	//				oldValue.toBoolArray(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setBoolArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_SByte:
	//			{
	//				UaSByteArray barray;
	//				targetValue.toSByteArray(barray);
	//				UaSByteArray barray2;
	//				oldValue.toSByteArray(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setSByteArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_Byte:
	//			{
	//				UaByteArray barray;
	//				targetValue.toByteArray(barray);
	//				UaByteArray barray2;
	//				oldValue.toByteArray(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setByteArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_Int16:
	//			{
	//				UaInt16Array barray;
	//				targetValue.toInt16Array(barray);
	//				UaInt16Array barray2;
	//				oldValue.toInt16Array(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setInt16Array(barray2);
	//				break;
	//			}
	//		case OpcUaType_UInt16:
	//			{
	//				UaUInt16Array barray;
	//				targetValue.toUInt16Array(barray);
	//				UaUInt16Array barray2;
	//				oldValue.toUInt16Array(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setUInt16Array(barray2);
	//				break;
	//			}
	//		case OpcUaType_Int32:
	//			{
	//				UaInt32Array barray;
	//				targetValue.toInt32Array(barray);
	//				UaInt32Array barray2;
	//				oldValue.toInt32Array(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setInt32Array(barray2);
	//				break;
	//			}
	//		case OpcUaType_UInt32:
	//			{
	//				UaUInt32Array barray;
	//				targetValue.toUInt32Array(barray);
	//				UaUInt32Array barray2;
	//				oldValue.toUInt32Array(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setUInt32Array(barray2);
	//				break;
	//			}
	//		case OpcUaType_Int64:
	//			{
	//				UaInt64Array barray;
	//				targetValue.toInt64Array(barray);
	//				UaInt64Array barray2;
	//				oldValue.toInt64Array(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setInt64Array(barray2);
	//				break;
	//			}
	//		case OpcUaType_UInt64:
	//			{
	//				UaUInt64Array barray;
	//				targetValue.toUInt64Array(barray);
	//				UaUInt64Array barray2;
	//				oldValue.toUInt64Array(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setUInt64Array(barray2);
	//				break;
	//			}
	//		case OpcUaType_Float:
	//			{
	//				UaFloatArray barray;
	//				targetValue.toFloatArray(barray);
	//				UaFloatArray barray2;
	//				oldValue.toFloatArray(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setFloatArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_Double:
	//			{
	//				UaDoubleArray barray;
	//				targetValue.toDoubleArray(barray);
	//				UaDoubleArray barray2;
	//				oldValue.toDoubleArray(barray2);
	//				barray2[index] = barray[0];
	//				targetValue.setDoubleArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_String:
	//			{
	//				UaStringArray barray;
	//				targetValue.toStringArray(barray);
	//				UaStringArray barray2;
	//				oldValue.toStringArray(barray2);
	//				//barray2[index] = barray[0];

	//				UaString writeValue = barray[0];
	//				writeValue.copyTo(&barray2[index]);
	//				targetValue.setStringArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_DateTime:
	//			{
	//				break;
	//			}
	//		case OpcUaType_Guid:
	//			{
	//				break;
	//			}
	//		case OpcUaType_NodeId:
	//			{
	//				break;
	//			}
	//		case OpcUaType_ByteString:
	//			{
	//				break;
	//			}
	//		case OpcUaType_XmlElement:
	//			{
	//				break;
	//			}
	//		case OpcUaType_ExpandedNodeId:
	//			{
	//				break;
	//			}
	//		case OpcUaType_StatusCode:
	//			{
	//				break;
	//			}
	//		case OpcUaType_QualifiedName:
	//			{
	//				break;
	//			}
	//		case OpcUaType_LocalizedText:
	//			{
	//				UaLocalizedTextArray barray;
	//				targetValue.toLocalizedTextArray(barray);
	//				UaLocalizedTextArray barray2;
	//				oldValue.toLocalizedTextArray(barray2);
	//				//barray2[index] = barray[0];

	//				UaLocalizedText writeValue = barray[0];
	//				writeValue.copyTo(&barray2[index]);
	//				targetValue.setLocalizedTextArray(barray2);
	//				break;
	//			}
	//		case OpcUaType_ExtensionObject:
	//			{
	//				break;
	//			}
	//		case OpcUaType_DataValue:
	//			{
	//				break;
	//			}
	//		case OpcUaType_Variant:
	//			{
	//				break;
	//			}
	//		default:
	//			{

	//			}
	//		}
	//	}
	//}

	//UaStatus static TranslateStringDataToVariant(UaNodeId datatype, UaString sData, UaVariant& value)
	//{
	//	UaStatus ret;
	//	OpcUa_Boolean bFind = OpcUa_True;

	//	OpcUa_NodeId NodeId;
	//	datatype.copyTo(&NodeId);

	//	UaVariant variant;
	//	sData.toVariant(variant);

	//	if (NodeId.IdentifierType == OpcUa_IdentifierType_Numeric)
	//	{
	//		switch(NodeId.Identifier.Numeric)
	//		{
	//		case OpcUaId_Boolean:
	//			//value.setBool((OpcUa_Boolean)atoi(pData));
	//			{
	//				OpcUa_Boolean bval;
	//				ret = variant.toBool(bval);
	//				if(ret.isGood())
	//					value.setBool(bval);
	//			}
	//			break;
	//		case OpcUaId_Byte:
	//			//value.setByte((OpcUa_Byte)atoi(pData));
	//			{
	//				OpcUa_Byte bval;
	//				ret = variant.toByte(bval);
	//				if(ret.isGood())
	//					value.setByte(bval);
	//			}
	//			break;
	//		case OpcUaId_Double:
	//			//value.setDouble(atol(pData));
	//			{	
	//				OpcUa_Double dval;
	//				ret = variant.toDouble(dval);
	//				if(ret.isGood())
	//					value.setDouble(dval);
	//			}
	//			break;
	//		case OpcUaId_Float:
	//			//value.setFloat(atof(pData));
	//			{
	//				OpcUa_Float fval;
	//				ret = variant.toFloat(fval);
	//				if(ret.isGood())
	//					value.setFloat(fval);
	//			}
	//			break;
	//		case OpcUaId_Int16:
	//			//value.setInt16((OpcUa_Int16)atoi(pData));
	//			{
	//				OpcUa_Int16 ival;
	//				ret = variant.toInt16(ival);
	//				if(ret.isGood())
	//					value.setInt16(ival);
	//			}
	//			break;
	//		case OpcUaId_Int32:
	//			//value.setInt32((OpcUa_Int32)atoi(pData));
	//			{
	//				OpcUa_Int32 ival;
	//				ret = variant.toInt32(ival);
	//				if(ret.isGood())
	//					value.setInt32(ival);
	//			}
	//			break;
	//		case OpcUaId_Int64:
	//			//value.setInt64((OpcUa_Int64)atoi(pData));
	//			{
	//				OpcUa_Int64 ival;
	//				ret = variant.toInt64(ival);
	//				if(ret.isGood())
	//					value.setInt64(ival);
	//			}
	//			break;
	//		case OpcUaId_UInt16:
	//			//value.setUInt16((OpcUa_UInt16)atoi(pData));
	//			{
	//				OpcUa_UInt16 ival;
	//				ret = variant.toUInt16(ival);
	//				if(ret.isGood())
	//					value.setUInt16(ival);
	//			}
	//			break;
	//		case OpcUaId_UInt32:
	//			//value.setUInt32((OpcUa_UInt32)atoi(pData));
	//			{
	//				OpcUa_UInt32 ival;
	//				ret = variant.toUInt32(ival);
	//				if(ret.isGood())
	//					value.setUInt32(ival);
	//			}
	//			break;
	//		case OpcUaId_UInt64:
	//			//value.setUInt64((OpcUa_UInt64)atoi(pData));
	//			{
	//				OpcUa_UInt64 ival;
	//				ret = variant.toUInt64(ival);
	//				if(ret.isGood())
	//					value.setUInt64(ival);
	//			}
	//			break;
	//		case OpcUaId_LocalizedText:
	//			value.setLocalizedText(UaLocalizedText("en", sData));
	//			break;
	//		case OpcUaId_String:
	//			value.setString(sData);
	//			break;
	//		case OpcUaId_SByte:
	//			//value.setSByte(OpcUa_SByte(pData));
	//			{
	//				OpcUa_SByte sbval;
	//				ret = variant.toSByte(sbval);
	//				if(ret.isGood())
	//					value.setSByte(sbval);
	//			}
	//			break;
	//		case OpcUaId_ByteString:
	//			//value.setByteString(UaByteString(sData.length(),  (OpcUa_Byte*)pData), OpcUa_True);
	//			{
	//				UaByteString bsval = TransferString2ByteString(sData);	
	//				
	//				value.setByteString(bsval, OpcUa_True);
	//			}
	//			break;
	//		case OpcUaId_QualifiedName:
	//			value.setQualifiedName(UaQualifiedName(sData, 0));
	//			break;

	//		case OpcUaId_DateTime:
	//		case OpcUaId_UtcTime:
	//			{
	//			
	//				UaDateTime time = UaDateTime::fromString(sData);
	//				if(time.isNull())
	//					ret = OpcUa_BadTypeMismatch;
	//				else
	//					value.setDateTime(time);
	//			}
	//		
	//			break;
	//		case OpcUaId_Guid:
	//			{
	//				UaGuid g;
	//				ret = g.fromString(sData);
	//				if(ret.isGood())
	//				{
	//					value.setGuid(g);
	//				}				
	//			}
	//		
	//			break;
	//		case OpcUaId_XmlElement:
	//			{
	//				value.setXmlElement(sData);
	//			}
	//			
	//			break;
	//		default:
	//			bFind = OpcUa_False;
	//		}
	//	}
	//	else if (NodeId.IdentifierType == OpcUa_IdentifierType_Guid)	
	//	{

	//	}
	//	else if(NodeId.IdentifierType == OpcUa_IdentifierType_String)
	//	{
	//		//! 视为枚举类型
	//		OpcUa_UInt16 ival;
	//		ret = variant.toUInt16(ival);
	//		if(ret.isGood())
	//		{
	//			value.setUInt16(ival);
	//		}
	//	}

	//	if(!bFind)
	//		ret = OpcUa_BadTypeMismatch;

	//	OpcUa_NodeId_Clear(&NodeId);   //!释放资源
	//	return ret;
	//}

	//UaStatus static TranslateData(UaNodeId datatype, OpcUa_Byte* pData, OpcUa_Int32 iDataLen, UaVariant& value)
	//{
	//	UaStatus ret = OpcUa_Good;
	//	OpcUa_Boolean bFind = OpcUa_True;
	//	if (NULL == pData)
	//	{
	//		ret = OpcUa_Bad;
	//		return ret;
	//	}

	//	OpcUa_NodeId NodeId;
	//	datatype.copyTo(&NodeId);

	//	if (NodeId.IdentifierType == OpcUa_IdentifierType_Numeric)
	//	{
	//		switch(NodeId.Identifier.Numeric)
	//		{
	//		case OpcUaId_Boolean:
	//			value.setBool((OpcUa_Boolean)(*pData) & 1);
	//			break;
	//		case OpcUaId_Byte:
	//			value.setByte((OpcUa_Byte)(*pData));
	//			break;
	//		case OpcUaId_Double:
	//			{
	//				OpcUa_Double dVal = 0;		
	//				if (sizeof(OpcUa_Float) == iDataLen)
	//				{
	//					OpcUa_Float fVal = *(OpcUa_Float*)(pData);
	//					dVal = fVal;
	//				}
	//				else if (sizeof(OpcUa_Double) == iDataLen)
	//				{				
	//					dVal = *(OpcUa_Double*)pData;
	//				}
	//				else if (sizeof(OpcUa_Int16) == iDataLen)
	//				{
	//					OpcUa_Int16 nVal = *(OpcUa_Int16*)pData;
	//					dVal = nVal;
	//				}
	//				else if (sizeof(OpcUa_Byte) == iDataLen)
	//				{
	//					OpcUa_Byte bVal = *(OpcUa_Byte*)pData;
	//					dVal = bVal;
	//				}
	//				value.setDouble(dVal);
	//			}
	//			break;
	//		case OpcUaId_Float:
	//			{
	//				OpcUa_Float fVal = 0;
	//				if (sizeof(OpcUa_Double) == iDataLen)
	//				{
	//					OpcUa_Double dValue = *(OpcUa_Double*)pData;
	//					fVal = dValue;
	//				}
	//				else if (sizeof(OpcUa_Float) == iDataLen)
	//				{
	//					fVal = *(OpcUa_Float*)pData;
	//				}
	//				else if (sizeof(OpcUa_Int16) == iDataLen)
	//				{
	//					OpcUa_Int16 iVaU16 = *(OpcUa_Int16*)pData;
	//					fVal = iVaU16;
	//				}
	//				else if (sizeof(OpcUa_Byte) == iDataLen)
	//				{
	//					OpcUa_Byte bValue = *(OpcUa_Byte*)pData;
	//					fVal = bValue;
	//				}
	//				value.setFloat(fVal);
	//			}
	//			break;
	//		case OpcUaId_Int16:
	//			{
	//				OpcUa_Int16 iVal16 = 0;
	//				if (sizeof(OpcUa_Int32) == iDataLen)
	//				{
	//					OpcUa_Int32 nVal = *(OpcUa_Int32*)pData;
	//					iVal16 = nVal;
	//				}
	//				else if (sizeof(OpcUa_Int64) == iDataLen)
	//				{
	//					OpcUa_Int64 nVal = *(OpcUa_Int64*)pData;
	//					iVal16 = nVal;
	//				}
	//				else if (sizeof(OpcUa_Int16) == iDataLen)
	//				{
	//					iVal16 = *(OpcUa_Int16*)pData;
	//				}
	//				else if (sizeof(OpcUa_Byte) == iDataLen)
	//				{
	//					OpcUa_Byte bValue = *(OpcUa_Byte*)pData;
	//					iVal16 = bValue;
	//				}
	//				value.setInt16(iVal16);
	//			}
	//			break;
	//		case OpcUaId_Int32:
	//			{
	//				OpcUa_Int32 iVal32 = 0;
	//				if (iDataLen == sizeof(OpcUa_Int16))
	//				{
	//					OpcUa_UInt16 iVal16 = *(OpcUa_Int16*)pData;
	//					iVal32 = iVal16;
	//				}
	//				else if (iDataLen == sizeof(OpcUa_Byte))
	//				{
	//					OpcUa_Byte bValue = *(OpcUa_Byte*)pData;
	//					iVal32 = bValue;
	//				}
	//				else if(sizeof(OpcUa_Int32) == iDataLen)
	//				{
	//					iVal32 = *(OpcUa_Int32*)pData;
	//				}
	//				else if (sizeof(OpcUa_Int64) == iDataLen)
	//				{
	//					OpcUa_Int64 nVal = *(OpcUa_Int64*)pData;
	//					iVal32 = nVal;
	//				}
	//				value.setInt32(iVal32);
	//			}
	//			break;
	//		case OpcUaId_Int64:
	//			{
	//				OpcUa_Int64 iVal64 = 0;
 //                   if (iDataLen == sizeof(OpcUa_Int32))
	//				{
	//					OpcUa_Int32 iVal32 = *(OpcUa_Int32*)pData;
	//					iVal64 = iVal32;
	//				}
	//				else if (iDataLen == sizeof(OpcUa_Int16))
	//				{
	//					OpcUa_Int16 iVal16 = *(OpcUa_Int16*)pData;
	//					iVal64 = iVal16;
	//				}
	//				else if (iDataLen == sizeof(OpcUa_Byte))
	//				{
	//					OpcUa_Byte bValue = *(OpcUa_Byte*)pData;
	//					iVal64 = bValue;
	//				}
	//				else if(sizeof(OpcUa_Int64) == iDataLen)
	//				{
	//					iVal64 = *(OpcUa_Int64*)pData;
	//				}
	//				value.setInt64(iVal64);
	//			}
	//			break;
	//		case OpcUaId_UInt16:
	//			{
	//				OpcUa_UInt16 iVal16 = 0;
	//				if (sizeof(OpcUa_UInt32) == iDataLen)
	//				{
	//					OpcUa_UInt32 nVal = *(OpcUa_UInt32*)pData;
	//					iVal16 = nVal;
	//				}
	//				else if (sizeof(OpcUa_UInt64) == iDataLen)
	//				{
	//					OpcUa_UInt64 nVal = *(OpcUa_UInt64*)pData;
	//					iVal16 = nVal;
	//				}
	//				else if (sizeof(OpcUa_Byte) == iDataLen)
	//				{
	//					OpcUa_Byte byVal = *(OpcUa_Byte*)pData;
	//					iVal16 = byVal;
	//				}
	//				else if(sizeof(OpcUa_UInt16) == iDataLen)
	//				{
	//					iVal16 = *(OpcUa_UInt16*)pData;
	//				}
	//				value.setUInt16(iVal16);
	//			}
	//			break;
	//		case OpcUaId_UInt32:
	//			{
	//				OpcUa_UInt32 iValU32 = 0;
 //                   if (iDataLen == sizeof(OpcUa_UInt16))
	//				{
	//					OpcUa_UInt16 uVal16 = *(OpcUa_UInt16*)pData;
	//					iValU32 = uVal16;
	//				}
	//				else if (iDataLen == sizeof(OpcUa_Byte))
	//				{
	//					OpcUa_Byte bValue = *(OpcUa_Byte*)pData;
	//					iValU32 = bValue;
	//				}
	//				else if(sizeof(OpcUa_UInt32) == iDataLen)
	//				{
	//					iValU32 = *(OpcUa_UInt32*)(pData);
	//				}
	//				else if (sizeof(OpcUa_UInt64) == iDataLen)
	//				{
	//					OpcUa_UInt64 nVal = *(OpcUa_UInt64*)pData;
	//					iValU32 = nVal;
	//				}
	//				value.setUInt32(iValU32);
	//			}
	//			break;
	//		case OpcUaId_UInt64:
	//			{
	//				OpcUa_UInt64 iValU64 = 0;					
 //                   if (iDataLen == sizeof(OpcUa_UInt32))
	//				{
	//					OpcUa_UInt32 uValue = *(OpcUa_UInt32*)pData;
	//					iValU64 = uValue;
	//				}
	//				else if (iDataLen == sizeof(OpcUa_UInt16))
	//				{
	//					OpcUa_UInt16 uVal16 = *(OpcUa_UInt16*)pData;
	//					iValU64 = uVal16;
	//				}
	//				else if (iDataLen == sizeof(OpcUa_Byte))
	//				{
	//					OpcUa_Byte bValue = *(OpcUa_Byte*)pData;
	//					iValU64 = bValue;
	//				}
	//				else if(sizeof(OpcUa_UInt64) == iDataLen)
	//				{
	//					iValU64 = *(OpcUa_UInt64*)pData;
	//				}
	//				value.setUInt64(iValU64);
	//			}
	//			break;
	//		case OpcUaId_LocalizedText:

	//			break;
	//		case OpcUaId_String:
	//			{
	//				std::string strContent = std::string((char*)pData,iDataLen);
	//				std::string strT = GetStringGBK2UTF8(strContent);
	//				value.setString((const char*)strT.c_str());
	//			}
	//			break;
	//		case OpcUaId_SByte:
	//			value.setSByte(OpcUa_SByte(*(const char*)pData));
	//			break;
	//		case OpcUaId_ByteString:
	//			{
 //                      UaByteString ubsTemp = UaByteString(iDataLen, pData);
	//			       value.setByteString(ubsTemp, OpcUa_True);
 //               }
	//			break;
	//		case OpcUaId_QualifiedName:
	//			value.setQualifiedName(UaQualifiedName((const char*)pData, 0));
	//			break;
	//		default:
	//			bFind = OpcUa_False;
	//		}
	//	}
	//	else if (NodeId.IdentifierType == OpcUa_IdentifierType_Guid)	
	//	{
	//		//
	//	}

	//	if(!bFind)
	//		ret = OpcUa_BadTypeMismatch;

	//	OpcUa_NodeId_Clear(&NodeId);   //!释放资源
	//	return ret;
	//}
	////MOD for Bug64725：串口Modbus，FEP无法从设备读取AIEA点类型值 on 20160309 above

	//OpcUa_Double static TransValuePrecision(OpcUa_Double value, OpcUa_UInt32 length)
	//{
	//	OpcUa_Double ret;

	//	if(value > OpcUa_Int32_Max || value < OpcUa_Int32_Min)
	//		ret = value;
	//	else
	//	{
	//		OpcUa_Double x;
	//		OpcUa_Int64 y;
	//		OpcUa_Double tmp = 1;
	//		for(OpcUa_Int32 i=0; i < length; i++)
	//		{
	//			tmp *= 10;
	//		}

	//		x = value * tmp;
	//		if(x >= 0.0)
	//			y = OpcUa_Int64(x + 0.5);
	//		else
	//			y = OpcUa_Int64(x - 0.5);

	//		x = y / tmp;
	//		ret = x;
	//	}

	//	//! double最大数到1.7e+308 所以这里字符串保存的长度应该大于308
	//	char strVal[1000];

	//	char sFormat[100] = "%0.";
	//	if(length > 0)
	//	{
	//		char sTmp[50];
	//		sprintf(sTmp, "%dlf", length);
	//		sprintf(sFormat, "%s%s", sFormat, sTmp);

	//		sprintf(strVal, sFormat, ret);
	//	}
	//	else
	//		sprintf(strVal, "%lld", (OpcUa_Int64)ret);

	//	UaVariant var(strVal);

	//	var.toDouble(ret);
	//	
	//	
	//	return ret;
	//}

	///*
	//*	\brief	将源数据类型值转为目标数据类型值
	//*			适用情况:比如AI的PV是double,对应的InChannel是float
	//*			发送强制命令时就得把double数据转为float再下发 否则是乱码
	//*
	//*	\param	srcDataType		源数据类型
	//*	\param	targetDataType	目标数据类型
	//*	\param	value			源数据值
	//*	\retval	UaVariant		目标数据值
	//*
	//*/
	//UaVariant static TransDataValueToTargetType(UaNodeId srcDataType, UaNodeId targetDataType, const UaVariant& value)
	//{
	//	UaVariant targetValue = value;

	//	if(srcDataType != targetDataType)
	//	{
	//		//TranslateStringDataToVariant(targetDataType, value.toString(), targetValue);
	//		targetValue.changeType(UaStructureField::builtInTypeFromDataTypeId(targetDataType), OpcUa_False);
	//	}

	//	return targetValue;
	//}

	//UaVariant static TransStringVariantToTargetVariant(OpcUa_BuiltInType type, const UaVariant& value)
	//{
	//	UaVariant targetValue = value;
	//	targetValue.changeType(type, OpcUa_False);
	//	return targetValue;
	//}

	//UaVariant static TransVariantToStringArrayType(const UaVariant& value)
	//{
	//	UaVariant targetValue = value;

	//	if(value.dataType() != OpcUaId_String)
	//	{
	//		UaStatus ret = targetValue.changeType(UaStructureField::builtInTypeFromDataTypeId(OpcUaId_String), OpcUa_True);
	//		if(ret.isBad())
	//		{

	//		}
	//	}

	//	return targetValue;
	//}

	//UaVariant static TransStringArrayToTargetVariant(const UaVariant& value, OpcUa_BuiltInType targetType)
	//{
	//	UaVariant targetValue = value;
	//	UaStatus ret = targetValue.changeType(targetType, OpcUa_True);
	//	if(ret.isBad())
	//	{
	//		//! UaVariant虽然是StringArray类型 但是changeType接口处理OpcUaType_LocalizedText目标类型不处理会报数据类型不匹配
	//		if(targetType == OpcUaType_LocalizedText)
	//		{
	//			UaStringArray sArray;
	//			value.toStringArray(sArray);
	//			OpcUa_UInt32 length = sArray.length();

	//			UaLocalizedTextArray lArray;
	//			lArray.create(length);

	//			for(int i=0; i < length; i++)
	//			{
	//				UaLocalizedText lt("en", sArray[i]);
	//				lt.copyTo(&lArray[i]);
	//			}
	//			targetValue.setLocalizedTextArray(lArray);
	//		}
	//	}

	//	return targetValue;
	//}

	////!进行量程转换
	//UaStatus static RangeConver(OpcUa_Byte* pbyValue, OpcUa_Int32 nDataLen, UaNodeId dataType, OpcUa_Float fRawZero, OpcUa_Float fRawFull, OpcUa_Float fEngZero, OpcUa_Float fEngFull, OpcUa_Boolean isRawToEng)
	//{
	//	UaStatus ret = OpcUa_Good;
	//	if (pbyValue != NULL && (fabs(fRawFull - fRawZero) > 0.001 && fabs(fEngFull - fEngZero) > 0.001))
	//	{
	//		OpcUa_Double dValue = 0;
	//		OpcUa_UInt64  lValue = 0;
	//		OpcUa_Int32  nDataType = 0;		//!1浮点数,2 整型
	//		//OpcUa_Int32	 nDataLen = 0;
	//		OpcUa_NodeId typeId;
	//		dataType.copyTo(&typeId);
	//		if (OpcUa_IdentifierType_Numeric == typeId.IdentifierType)
	//		{
	//			switch(typeId.Identifier.Numeric)
	//			{
	//			case OpcUaId_Boolean:
	//			case OpcUaId_Byte:
	//			case OpcUaId_Int16:
	//			case OpcUaId_Int32:
	//			case OpcUaId_Int64:
	//			case OpcUaId_UInt16:
	//			case OpcUaId_UInt32:
	//			case OpcUaId_UInt64:
	//				{
	//					if (nDataLen == sizeof(OpcUa_Byte))
	//					{
	//						OpcUa_Byte nTempVal = *(OpcUa_Byte*)pbyValue;
	//						lValue = nTempVal;
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt16))
	//					{
	//						OpcUa_UInt16 nTempVal = *(OpcUa_UInt16*)pbyValue;
	//						lValue = nTempVal;
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt32))
	//					{
	//						OpcUa_UInt32 nTempVal = *(OpcUa_UInt32*)pbyValue;
	//						lValue = nTempVal;
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt64))
	//					{
	//						OpcUa_UInt64 nTempVal = *(OpcUa_UInt64*)pbyValue;
	//						lValue = nTempVal;
	//					}
	//					nDataType = 2;
	//				}
	//				break;
	//			case OpcUaId_Double:
	//			case OpcUaId_Float:
	//				{
	//					if (nDataLen == sizeof(OpcUa_Double))
	//					{
	//						dValue = *(OpcUa_Double*)(pbyValue);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_Float))
	//					{
	//						OpcUa_Float fValue = *(OpcUa_Float*)(pbyValue);
	//						dValue = fValue;
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt16))
	//					{
	//						OpcUa_UInt16 nTempVal = *(OpcUa_UInt16*)(pbyValue);
	//						dValue = nTempVal;
	//					}
	//					else if (nDataLen == sizeof(OpcUa_Byte))
	//					{
	//						OpcUa_Byte nTempVal = *(OpcUa_Byte*)(pbyValue);
	//						dValue = nTempVal;
	//					}
	//					nDataType = 1;
	//				}
	//				break;
	//			default:
	//				break;
	//			}
	//		}

	//		//!浮点型
	//		if (1 == nDataType)
	//		{
	//			if (isRawToEng)
	//			{
	//				dValue = (dValue - fRawZero)*(fEngFull - fEngZero)/(fRawFull - fRawZero) + fEngZero;
	//			}
	//			else
	//			{
	//				dValue = (dValue - fEngZero)*(fRawFull - fRawZero) / (fEngFull - fEngZero) + fRawZero;
	//			}
	//		}
	//		//!整形
	//		else if (2 == nDataType)
	//		{
	//			if (isRawToEng)
	//			{
	//				dValue = ((OpcUa_Double)lValue - fRawZero)*(fEngFull - fEngZero)/(fRawFull - fRawZero) + fEngZero;
	//				lValue = (OpcUa_UInt64)dValue;
	//			}
	//			else
	//			{
	//				dValue = ((OpcUa_Double)lValue - fEngZero)*(fRawFull - fRawZero) / (fEngFull - fEngZero) + fRawZero;
	//				lValue = (OpcUa_UInt64)dValue;
	//			}
	//		}
	//		if (OpcUa_IdentifierType_Numeric == typeId.IdentifierType)
	//		{
	//			switch(typeId.Identifier.Numeric)
	//			{
	//			case OpcUaId_Boolean:
	//			case OpcUaId_Byte:
	//			case OpcUaId_Int16:
	//			case OpcUaId_Int32:
	//			case OpcUaId_Int64:
	//			case OpcUaId_UInt16:
	//			case OpcUaId_UInt32:
	//			case OpcUaId_UInt64:
	//				{
	//					if (nDataLen == sizeof(OpcUa_Byte))
	//					{
	//						OpcUa_Byte nTempVal = (OpcUa_Byte)lValue;
	//						memcpy(pbyValue, &nTempVal, nDataLen);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt16))
	//					{
	//						OpcUa_UInt16 nTempVal = (OpcUa_UInt16)lValue;
	//						memcpy(pbyValue, &nTempVal, nDataLen);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt32))
	//					{
	//						OpcUa_UInt32 nTempVal = (OpcUa_UInt32)lValue;
	//						memcpy(pbyValue, &nTempVal, nDataLen);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt64))
	//					{
	//						memcpy(pbyValue, &lValue, nDataLen);
	//					}
	//				}
	//				break;
	//			case OpcUaId_Double:
	//			case OpcUaId_Float:
	//				{
	//					if (nDataLen == sizeof(OpcUa_Float))
	//					{
	//						OpcUa_Float fVal = (OpcUa_Float)dValue;
	//						memcpy(pbyValue, &fVal, nDataLen);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_Double))
	//					{
	//						memcpy(pbyValue, &dValue, nDataLen);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_UInt16))
	//					{
	//						OpcUa_UInt16 nTempVal = (OpcUa_UInt16)dValue;
	//						memcpy(pbyValue, &nTempVal, nDataLen);
	//					}
	//					else if (nDataLen == sizeof(OpcUa_Byte))
	//					{
	//						OpcUa_Byte nTempVal = (OpcUa_Byte)dValue;
	//						memcpy(pbyValue, &nTempVal, nDataLen);
	//					}
	//				}
	//				break;
	//			default:
	//				break;
	//			}
	//		}
	//		OpcUa_NodeId_Clear(&typeId);
	//	}
	//	return ret;
	//}

	///*!
	//*	\brief	 给定一个数据变量，将其转换为UaVariant类型变量
	//*			 此给定数据变量需要前两个参数
	//*			参数1标识该变量的数据类型，比如float double uint32......
	//*			参数2标识该变量的数据区，以ByteString类型封装
	//*/
	//UaStatus static TranslateByteStringData(UaNodeId datatype, UaByteString dataString, UaVariant& value)
	//{
	//	OpcUa_Byte* pData = (OpcUa_Byte*)dataString.data();
	//	return TranslateData(datatype, pData, dataString.length(), value);
	//}

	///*!
	//*	\brief	 给定一个节点和数据值，给该变量赋值
	//*			 注意该节点必须是叶子结点Variable类型，直接使用setValue接口赋值
	//*/
	//UaStatus static SetVariableValue(UaNode* pNode, UaVariant value)
	//{
	//	UaStatus ret;

	//	if(pNode)
	//	{
	//		if(pNode->nodeClass() == OpcUa_NodeClass_Variable)
	//		{
	//			UaVariable* pVariable = (UaVariable*)pNode;
	//			//ret = TranslateData(pVariable->dataType(), data, value);
	//			//if(ret.isGood())
	//			{
	//				UaDataValue dataValue;
	//				dataValue.setValue(value, OpcUa_True, OpcUa_True);
	//				ret = pVariable->setValue(NULL, dataValue, OpcUa_True);
	//			}
	//		}
	//		else
	//			ret = OpcUa_BadArgumentsMissing;
	//	}

	//	return ret;
	//}

	///*!
	//*	\brief		给UaDataValue设定时间戳
	//*
	//*/
	//void static setSourceTimestamp(UaDataValue &dataValue, OpcUa_Int64 nTime)
	//{
	//	//! 设定时间戳
	//	UaDateTime uaDataTime;
	//	uaDataTime.fromTime_t(time_t(nTime));
	//	dataValue.setSourceTimestamp(OpcUa_DateTime(uaDataTime));
	//}

	///*!
	//*	\brief		获取PV项当前值时间戳 为数据持久化VQT服务
	//*
	//*/
	//OpcUa_Int64 static GetProcessTime(UaVariable* pVariable)
	//{
	//	UaDataValue dataValue = pVariable->value(NULL);
	//	OpcUa_DateTime time = dataValue.sourceTimestamp();
	//	UaDateTime dataTime(time);
	//	time_t tm = dataTime.toTime_t();
	//	return (OpcUa_Int64)tm;
	//}

	///*!
	//*	\brief		设定质量位
	//*
	//*/
	//void static SetQualityDetail(UaVariable* pVariable, OpcUa_UInt32 setValue)
	//{
	//	if(pVariable)
	//	{
	//		UaVariant value;
	//		value.setUInt32(setValue);
	//		UaDataValue dataValue;
	//		dataValue.setValue(value, OpcUa_True, OpcUa_True);
	//		pVariable->setValue(NULL, dataValue, OpcUa_False);
	//	}
	//}

	///*!
	//*	\brief		获取质量位
	//*
	//*/
	//OpcUa_UInt32 static GetQualityDetail(UaVariable* pVariable)
	//{
	//	OpcUa_UInt32 ret = 0;

	//	if(pVariable)
	//	{
	//		UaDataValue dataValue = pVariable->value(NULL);
	//		UaVariant value = *dataValue.value();
	//		value.toUInt32(ret);
	//	}
	//
	//	return ret;
	//}

	///*
	//*	\brief	获取模拟量量程范围
	//*
	//*	\param	OpcUa::AnalogItemType*		模拟量PV项
	//*	\retval	UaRange						量程范围
	//*
	//*/
	//static UaRange GetUaRange(OpcUa::AnalogItemType* pAnalogItem)
	//{
	//	UaRange tempRange;

	//	if(NULL != pAnalogItem)
	//	{
	//		OpcUa::PropertyType* pEURange = pAnalogItem->getEURangeNode();
	//		if(NULL != pEURange)
	//		{
	//			UaDataValue rangeValue = pEURange->value(NULL);
	//			if ( OpcUa_IsGood(rangeValue.statusCode()) &&
	//				(rangeValue.value()->Datatype == OpcUaType_ExtensionObject) &&
	//				(rangeValue.value()->ArrayType == OpcUa_VariantArrayType_Scalar) )
	//			{
	//				tempRange.setRange(*rangeValue.value()->Value.ExtensionObject);
	//			}
	//		}
	//	}

	//	return tempRange;
	//}

	//static UaLocalizedText GetEUInformation(OpcUa::AnalogItemType* pAnalogItem)
	//{
	//	UaEUInformation euInfo;

	//	if(NULL != pAnalogItem)
	//	{
	//		OpcUa::PropertyType* pEUInfo = pAnalogItem->getEngineeringUnitsNode();
	//		if(NULL != pEUInfo)
	//		{
	//			UaDataValue euinfoValue = pEUInfo->value(NULL);
	//			if ( OpcUa_IsGood(euinfoValue.statusCode()) &&
	//				(euinfoValue.value()->Datatype == OpcUaType_ExtensionObject) &&
	//				(euinfoValue.value()->ArrayType == OpcUa_VariantArrayType_Scalar) )
	//			{
	//				euInfo.setEUInformation(*euinfoValue.value()->Value.ExtensionObject);
	//			}
	//		}
	//	}

	//	return euInfo.getDisplayName();
	//}

	//static RangeFlag CheckWithinRange(OpcUa_Double curValue, UaRange range)
	//{
	//	RangeFlag flag = Normal;

	//	if(curValue < range.getLow())
	//		flag = Low;
	//	else if(curValue > range.getHigh())
	//		flag = High;
	//	else
	//		flag = Normal;

	//	return flag;
	//}

	static void StringReplace(string &strBase, string strSrc, string strDes)
	{
		string::size_type pos = 0;
		string::size_type srcLen = strSrc.size();
		string::size_type desLen = strDes.size();
		pos=strBase.find(strSrc, pos); 
		while ((pos != string::npos))
		{
			strBase.replace(pos, srcLen, strDes);
			pos=strBase.find(strSrc, (pos+desLen));
		}
	}

	static void TranslateTagItem2Varient(const T_TagItem &tTagItem, CKiwiVariable& variable)
	{
		variable.InitAttribute();
		variable.szName = tTagItem.szTagName;

		variable.AddAttribute(DEF_FieldPoint_DeviceAddress);
		variable.AddAttribute(DEF_FieldPoint_Offset);

		CKiwiVariant var(tTagItem.szAddr);
		variable.setAttributeValue(DEF_FieldPoint_DeviceAddress, var);
		variable.setAttributeValue(DEF_FieldPoint_Offset, var);

	}
	static void StrToUpper( char* str )
	{
	#ifdef _WINDOWS
		strupr( str );
	#else
		char *ptr = str;
		while (*ptr != 0)
		{
			if (islower(*ptr))
			{
				*ptr = toupper(*ptr);
			}
			ptr++;
		}
	#endif
	}

	static void StrToUpper(std::string& str)
	{
		std::basic_string<char>::iterator it;
		for (it=str.begin(); it!=str.end(); it++)
		{
			if (islower(*it))
			{
				*it = toupper(*it);
			}
		}
	}

}

#endif
