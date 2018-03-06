/*!
 *	\file	IOCfgDriverDbfParser.cpp
 *
 *	\brief	������Ӧ��������.dbf�ļ�������ʵ��
 *
 *	��ϸ���ļ�˵������ѡ��
 *
 *	\author LIFAN
 *
 *	\date	2014��4��21��	9:51
 *
 *	\version	1.0
 *
 *	\attention	Copyright (c) 2014, SCADA V4 Product Development,HollySys Co.,Ltd
 *	\attention	All rights reserved.
 */

#include "IOCfgDriverDbfParser.h"

namespace MACS_SCADA_SUD
{
	//!���캯��
	CIOCfgDriverDbfParser::CIOCfgDriverDbfParser()
	{

	}

	//!��������
	CIOCfgDriverDbfParser::~CIOCfgDriverDbfParser()
	{
		m_vecDes.clear();
	}

	//!����Template�ֽ����ɲ������ʽ
	int CIOCfgDriverDbfParser::MakeBolanExpress(const char* pchFormat, tBOLAN& tBol)
	{
		std::string szFormat = pchFormat;
		if ( szFormat.length()<=0 )
			return -1;
		tBol.nCount = 0;
		char chCell[32];
		char digitStr[32];
		std::string str;
		tBOLAN tbolan;
		memset( &tbolan, 0, sizeof(tBOLAN) );
		tbolan.nCount = 0;
		tBOLITEM tStack[32];
		memset( tStack, 0, sizeof(tStack) );
		int nPosCount = 0;
		int top = 0;
		int t = 0;
		for( int i=0; pchFormat[i]!='\0'; i++ )
		{
			if ( ('A'<=pchFormat[i] && pchFormat[i]<='Z') ||
				('a'<=pchFormat[i] && pchFormat[i]<='z') )
			{
				continue;
			}

			if ( pchFormat[i]=='%' )
				continue;
			if ( pchFormat[i]=='U' )
			{
				tbolan.byType = 0;
				continue;
			}
			if ( pchFormat[i]=='+' || pchFormat[i]=='-' || pchFormat[i]=='*' || pchFormat[i]=='/' )
			{
				while(top!=0)
				{
					tbolan.tItem[tbolan.nCount].byType = 2;
					tbolan.tItem[tbolan.nCount].chOper = tStack[top].chOper;
					tbolan.nCount ++;
					top--;
				}
				//������ѹ��ջstack��
				top++;
				tStack[top].byType = 2;
				tStack[top].chOper=pchFormat[i];
				continue;
			}
			if('0'<=pchFormat[i] && pchFormat[i]<='9') //��ֵ
			{
				t = 0;
				digitStr[t] = pchFormat[i];	//���ִ���Ҫת������ֵ���ַ���
				while('0'<=pchFormat[i+1] && pchFormat[i+1]<='9')
				{							//������������Ҫת������ֵ���ַ���
					digitStr[++t] = pchFormat[i+1];
					i++;
				}
				digitStr[++t]='\0';
				//�������ַ���ת������ֵ,������bolan����
				tbolan.tItem[tbolan.nCount].byType = 0;    //��ʶ�ýڵ�Ϊ��ֵ
				tbolan.tItem[tbolan.nCount].lValue = atol(digitStr);
				tbolan.nCount ++;
				continue;
			}

			if ( pchFormat[i]=='(' )
			{
				int t = i + 1;
				while( pchFormat[t]!=')' )
				{
					t ++;
				}
				memset( chCell, 0, sizeof(chCell) );
				memcpy( chCell, pchFormat + i + 1, t - i - 1 );
				str = chCell;
				int nDataPos[2];			
				int nPos = 0;
				int nIndex = 0;		
				char ch[32];
				while(1)
				{
					nPos = static_cast<int>( str.find(",", nPos + 1) );
					if (nPos==-1)
						break;
					nDataPos[nIndex] = nPos;
					nIndex ++;
				}
				if (nIndex!=2)
					return -2;
				strcpy(ch, chCell);
				ch[nDataPos[0]] = '\0';

				tbolan.tItem[tbolan.nCount].byType = 1;
				tbolan.tItem[tbolan.nCount].nPos = nPosCount;
				nPosCount ++;
				tbolan.tItem[tbolan.nCount].lDiffer = atol(ch);

				strcpy(ch, chCell + nDataPos[0] + 1);
				ch[nDataPos[1]-nDataPos[0]-1] = '\0';
				tbolan.tItem[tbolan.nCount].lLower = atol(ch);

				strcpy(ch, chCell + nDataPos[1] + 1);
				tbolan.tItem[tbolan.nCount].lUpper = atol(ch);

				tbolan.nCount ++;

				i = t;
			}
		}

		if ( tbolan.nCount == 0 )				//�õ�ַ���ֽܷ⣬��AI��ֱ�ӷ��� 
			return 0;

		while(top!=0)
		{
			tbolan.tItem[tbolan.nCount] = tStack[top];
			tbolan.nCount ++;
			top--;
		}
		memcpy(&tBol, &tbolan, sizeof(tBOLAN));

		return 0;
	}

	//!�����ǩ�ĵ�ַ
	int CIOCfgDriverDbfParser::CalcBolanExpress(tIOCfgUnitDataDes tDataDes, int* plItemVal, int& lTagAddr)
	{
		if ( !plItemVal )
			return -1;

		tBOLAN tbolan = tDataDes.tBol;
		int  dataStack[100];						//����м����ݵ�����ջ
		int top=0;									//����ջdataStack��ջ��

		if ( tbolan.nCount == 0 )			
		{
			plItemVal[0] = 0;
			return -1;
		}

		for( int i=0; i<tbolan.nCount; i++ )
		{
			if (tbolan.tItem[i].byType==0)
			{
				top++;
				dataStack[top] = tbolan.tItem[i].lValue;
				continue;
			}
			if (tbolan.tItem[i].byType==1)
			{
				top++;
				dataStack[top] = plItemVal[tbolan.tItem[i].nPos];
				continue;
			}
			if (tbolan.tItem[i].byType==2)
			{
				switch(tbolan.tItem[i].chOper)
				{
				case '+':
					dataStack[top-1] = dataStack[top-1] + dataStack[top];
					top --;
					break;
				case '-':
					dataStack[top-1] = dataStack[top-1] - dataStack[top];
					top --;
					break;
				case '*':
					dataStack[top-1] = dataStack[top-1] * dataStack[top];
					top --;
					break;
				case '/':
					dataStack[top-1] = dataStack[top-1] / dataStack[top];
					top --;
					break;
				}
			}
		}
		lTagAddr = dataStack[top];
		return 0;
	}

	//!�����ǩ��ַ����ֵ
	int CIOCfgDriverDbfParser::CalcTagAddrBound(tIOCfgUnitDataDes& tDataDes)
	{
		//! �����ַ������͵ı�ǩ
		if ( tDataDes.tBol.byType==1 )
		{
			tDataDes.lLower = tDataDes.iLow;
			tDataDes.lUpper = tDataDes.iHigh;
			return 0;
		}

		//! �����������͵ı�ǩ
		int nItemCount = 0;;
		int lLowData[32];
		int lHighData[32];
		tBOLAN tbolan = tDataDes.tBol;
		for( int i=0; i<tDataDes.tBol.nCount; i++ )
		{
			if ( tbolan.tItem[i].byType==1 )
			{
				lLowData[nItemCount] = tbolan.tItem[i].lLower;
				lHighData[nItemCount] = tbolan.tItem[i].lUpper;
				nItemCount ++;
			}
		}
		if ( nItemCount>0 )
		{
			if ( CalcBolanExpress( tDataDes, lLowData, tDataDes.lLower )!=0 )
				return -1;
			if ( CalcBolanExpress( tDataDes, lHighData, tDataDes.lUpper )!=0 )
				return -2;
		}
		else 
		{
			tDataDes.lLower = 0;
			tDataDes.lUpper = 0;
		}
		return 0;
	}

	//!����һ������
	void CIOCfgDriverDbfParser::AddDes(tIOCfgUnitDataDes& dataDes)
	{
		m_vecDes.push_back( dataDes );
	}

	//!��ȡ��ǩ��ַģ�������
	int CIOCfgDriverDbfParser::GetTagDesCount()
	{
		return (int)( m_vecDes.size() );
	}

	//!ͨ�������Ż�ȡ��ǩģ��ṹ
	int CIOCfgDriverDbfParser::GetTagDesByIndex(int iIndex, tIOCfgUnitDataDes& tDataDes)
	{
		//MOD for BUG63641 FEP��Ԫ���ԣ�IOCfgDriverDbParser.cpp�еĺ���GetTagDesByIndex ���������У�鲻��ȷ below//		
		if ( m_vecDes.empty() )
		{
			return -1;
		}
		if ( iIndex >= static_cast<int>(m_vecDes.size()) ||  iIndex < 0) 
		{
			return -2;
		}
		//MOD for BUG63641 FEP��Ԫ���ԣ�IOCfgDriverDbParser.cpp�еĺ���GetTagDesByIndex ���������У�鲻��ȷ above//

		T_VecDataDes::iterator it;
		it = m_vecDes.begin();
		for ( int i=0; i<iIndex; i++ )
		{
			it ++;
		}
		tDataDes = *it;
		return 0;
	}

	//!ͨ����ǩ����̬��ַ��ȡ��ǩ������ṹ
	int CIOCfgDriverDbfParser::GetTagDesByTagAddr(const char* pchTagAddr, tIOCfgUnitDataDes& tDataDes)
	{
		std::string strAddr = pchTagAddr;
		if ( strAddr.length()<=0 )
			return -1;
		
		char pchStr[32];
		strcpy( pchStr, pchTagAddr );

		//! �Ƿ����ַ������͵ı�ǩ
		int nPos = 0;
		nPos = static_cast<int>( strAddr.find( "_" ) );
		if ( nPos>0 )
		{
			pchStr[nPos+1] = '\0';
		}
		else
		{	//! �������͵�
			int i = 0;
			for( i=0; pchStr[i]!='\0'; i++)
			{
				if ( pchStr[i]>='0' && pchStr[i]<='9' )
				{
					break;
				}
			}
			pchStr[i] = '\0';	
		}

		T_VecDataDes::iterator it;
		it = m_vecDes.begin();
		for( it=m_vecDes.begin(); it!=m_vecDes.end(); it++ )
		{
			if ( stricmp( pchStr, (*it).pchID ) == 0 )
			{
				memcpy(&tDataDes, &(*it), sizeof(tIOCfgUnitDataDes));
				return 0;
			}
		}

		return -1;
	}
}