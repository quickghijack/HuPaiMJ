/********************************************************************
created:	pinorr
file base:	HuPaiMJ.h
author:		pinorr	Q 505971450
purpose:	�齫������ʾ�㷨(��4��)
*********************************************************************/

#ifndef __HU_PAI_MJ3_H__
#define __HU_PAI_MJ3_H__
#include <set>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <unordered_map>  

using namespace std;

namespace ArrayMJ
{	
#define MAX_TOTAL_TYPE					34
#define MAX_VAL_NUM						9
#define MAX_KEY_NUM						(MAX_VAL_NUM+1)		//9+����
#define MAX_NAI_NUM						4					//����������
#define BIT_VAL_NUM						3					//һ��ֵռ��bit��
#define BIT_VAL_FLAG					0x07				//

	//�齫��ɫ�����ࣩ����
	enum enColorMJ
	{
		enColorMJ_WAN = 0,  //��
		enColorMJ_TONG,     //Ͳ
		enColorMJ_TIAO,     //��
		enColorMJ_FenZi,    //�硢�� ��
		enColorMJ_Max,
	};

	set<int>							g_setSingle;		//����˳��+����		50��
	set<int>							g_setSingleFZ;		//����˳��+����		22��
	set<int>							g_setSingleJiang;	//������			19��
	set<int>							g_setSingleJiangFZ;	//������			15��

	BYTE								g_byArray[262144];
	BYTE								g_byArrayFZ[262144];
	BYTE								g_byError[262144];

	unordered_map<int, BYTE>			g_mapHuAll[15];
	unordered_map<int, BYTE>			g_mapHuAllFZ[15];

	inline int getKeyByIndex(BYTE byIndex[MAX_KEY_NUM], BYTE byNum = MAX_KEY_NUM)
	{
		int nKey = 0;
		for (int i = 0; i < byNum; ++i)
			nKey |= (int)(byIndex[i] & BIT_VAL_FLAG) << (BIT_VAL_NUM*i);
		return nKey;
	}
	inline int getArrayIndex(BYTE byIndex[MAX_VAL_NUM], BYTE byNum = MAX_VAL_NUM)
	{
		int nKey = 0;
		for (int i = 0; i < byNum; ++i)
		{
			if ((byIndex[i] & BIT_VAL_FLAG) > 3) byIndex[i] -= 3;
			nKey |= (int)(byIndex[i] & 0x03) << (2 * i);
		}
		return nKey;
	}
	inline int getArrayIndex(int llVal)
	{
		BYTE byIndex[MAX_VAL_NUM] = {};
		for (int i = 0; i < MAX_VAL_NUM; ++i)
			byIndex[i] = (llVal >> (BIT_VAL_NUM*i))&BIT_VAL_FLAG;

		return getArrayIndex(byIndex);
	}
	inline bool isValidKey(int llVal)
	{
		BYTE byIndex[MAX_KEY_NUM] = {};
		for (int i = 0; i < MAX_KEY_NUM; ++i)
			byIndex[i] = (llVal >> (BIT_VAL_NUM*i))&BIT_VAL_FLAG;

		if (byIndex[9] > MAX_NAI_NUM)	return false;
		int nNum = 0;
		for (int i = 0; i < MAX_KEY_NUM; ++i)
		{
			nNum += byIndex[i];
			if (byIndex[i] > 4 || nNum > 14)	//
				return false;
		}
		return nNum > 0;
	}
	inline BYTE getNumByKey(int llVal, BYTE byNum = MAX_KEY_NUM)
	{
		BYTE byIndex[MAX_KEY_NUM] = {};
		for (int i = 0; i < MAX_KEY_NUM; ++i)
			byIndex[i] = (llVal >> (BIT_VAL_NUM*i))&BIT_VAL_FLAG;

		BYTE nNum = 0;
		for (int i = 0; i < byNum; ++i)
			nNum += byIndex[i];
		return nNum;
	}
	inline void addMap(unordered_map<int, BYTE> mapTemp[], int llVal)
	{
		BYTE nNum = getNumByKey(llVal, MAX_VAL_NUM);
		BYTE byNum = (llVal >> (BIT_VAL_NUM * 9))&BIT_VAL_FLAG;
		int  val = (llVal & 0x7FFFFFF);
		unordered_map<int, BYTE>::iterator iter = mapTemp[nNum].find(val);
		if (iter != mapTemp[nNum].end())
			iter->second = min(byNum, iter->second);
		else
			mapTemp[nNum][val] = byNum;		
	}

	class CHuPaiArrayMJ
	{
	private:
		static void TrainSingle()
		{
			BYTE byTemp[MAX_KEY_NUM] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3 };
			g_setSingle.insert(getKeyByIndex(byTemp));
			g_setSingleFZ.insert(getKeyByIndex(byTemp));
			// 1.1 ����
			for (int i = 0; i < MAX_VAL_NUM; ++i)
			{
				memset(byTemp, 0, MAX_KEY_NUM);
				for (int n = 0; n < 3; ++n)
				{
					byTemp[i] = 3 - n;	byTemp[9] = n;
					g_setSingle.insert(getKeyByIndex(byTemp));
					if (i < 7)	//�硢�������7��
						g_setSingleFZ.insert(getKeyByIndex(byTemp));
				}
			}
			// 1.2 ˳�� û����
			for (int i = 0; i < MAX_VAL_NUM - 2; ++i)
			{
				memset(byTemp, 0, MAX_KEY_NUM);
				byTemp[i] = 1;	byTemp[i + 1] = 1;	byTemp[i + 2] = 1;
				g_setSingle.insert(getKeyByIndex(byTemp));
			}
			// 1.3 ˳�� 1������ (2������ʱҲ���ǿ���)
			for (int i = 0; i < MAX_VAL_NUM - 2; ++i)
			{
				for (int n = 0; n < 3; ++n)
				{
					memset(byTemp, 0, MAX_KEY_NUM);
					byTemp[i] = 1;	byTemp[i + 1] = 1;	byTemp[i + 2] = 1;
					byTemp[i + n] = 0;	byTemp[9] = 1;
					g_setSingle.insert(getKeyByIndex(byTemp));
				}
			}
			// 2.1 ����
			memset(byTemp, 0, MAX_KEY_NUM);
			byTemp[9] = 2;
			g_setSingleJiang.insert(getKeyByIndex(byTemp));
			g_setSingleJiangFZ.insert(getKeyByIndex(byTemp));
			for (int i = 0; i < MAX_VAL_NUM; ++i)
			{
				memset(byTemp, 0, MAX_KEY_NUM);
				for (int n = 0; n < 2; ++n)
				{
					byTemp[i] = 2 - n;	byTemp[9] = n;
					g_setSingleJiang.insert(getKeyByIndex(byTemp));
					if (i < 7)	//�硢�������7��
						g_setSingleJiangFZ.insert(getKeyByIndex(byTemp));
				}
			}
		};

		static void TrainAllComb(const set<int> &setSingle, unordered_map<int, BYTE> mapOut[])
		{
			int nAll = 0;
			int nSingle[100] = {};
			set<int>::iterator iter = setSingle.begin();
			for (; iter != setSingle.end(); ++iter)
				nSingle[nAll++] = *iter;

			for (int i1 = 0; i1 < nAll; ++i1)
			{
				addMap(mapOut, nSingle[i1]);
				for (int i2 = i1; i2 < nAll; ++i2)
				{
					int nTemp = nSingle[i1] + nSingle[i2];
					if (!isValidKey(nTemp))	continue;
					addMap(mapOut, nTemp);
					for (int i3 = i2; i3 < nAll; ++i3)
					{
						int nTemp = nSingle[i1] + nSingle[i2] + nSingle[i3];
						if (!isValidKey(nTemp))	continue;
						addMap(mapOut, nTemp);
						for (int i4 = i3; i4 < nAll; ++i4)
						{
							int nTemp = nSingle[i1] + nSingle[i2] + nSingle[i3] + nSingle[i4];
							if (!isValidKey(nTemp))	continue;
							addMap(mapOut, nTemp);
						}
					}
				}
			}
		}

		static void TrainAllComb_Jiang(const set<int> &setSingle, unordered_map<int, BYTE> mapOut[])
		{
			int nAll = 0;
			int nSingle[100] = {};

			set<int>::iterator iter = setSingle.begin();
			for (; iter != setSingle.end(); ++iter)
				nSingle[nAll++] = *iter;

			unordered_map<int, BYTE> mapTemp[15];
			for (int j = 0; j < 15; ++j)
				mapTemp[j] = mapOut[j];

			for (int i = 0; i < nAll; ++i)
			{
				for (int j = 0; j < 15; ++j)
				{
					addMap(mapOut, nSingle[i]);
					unordered_map<int, BYTE>::iterator iter_u = mapTemp[j].begin();
					for (; iter_u != mapTemp[j].end(); ++iter_u)
					{
						int nTemp = nSingle[i] + iter_u->first + (int(iter_u->second & BIT_VAL_FLAG) << 27);
						if (isValidKey(nTemp))
							addMap(mapOut, nTemp);
					}
				}
			}
		}

	public:
		static void TrainAll()
		{
			if (g_setSingle.empty())
			{
				memset(g_byArray, 0, sizeof(g_byArray));
				memset(g_byArrayFZ, 0, sizeof(g_byArrayFZ));
				memset(g_byError, 0, sizeof(g_byError));
								
				DWORD dwFlag = GetTickCount();
				TrainSingle();
				TrainAllComb(g_setSingle, g_mapHuAll);
				TrainAllComb(g_setSingleFZ, g_mapHuAllFZ);
				TrainAllComb_Jiang(g_setSingleJiang, g_mapHuAll);
				TrainAllComb_Jiang(g_setSingleJiangFZ, g_mapHuAllFZ);

				int numAll = 0;
				for (int i = 0; i < 15; ++i)
				{
					numAll += g_mapHuAll[i].size();
					numAll += g_mapHuAllFZ[i].size();
				}
				cout << "train cost:" << GetTickCount() - dwFlag << "ms numAll=" << numAll << endl;
				for (int i = 0; i < 15; ++i)
				{
					unordered_map<int, BYTE>::iterator iter = g_mapHuAll[i].begin();
					for (; iter != g_mapHuAll[i].end(); ++iter)
					{
						int nArrayIndex = getArrayIndex(iter->first);
						ArrayMJ::g_byArray[nArrayIndex] = max(ArrayMJ::g_byArray[nArrayIndex], iter->second + 1);
					}
					iter = g_mapHuAllFZ[i].begin();
					for (; iter != g_mapHuAllFZ[i].end(); ++iter)
					{
						int nArrayIndex = getArrayIndex(iter->first);						
						ArrayMJ::g_byArrayFZ[nArrayIndex] = max(ArrayMJ::g_byArrayFZ[nArrayIndex], iter->second + 1);
					}
					g_mapHuAllFZ[i].clear();
				}
				
				for (int n = 0; n < sizeof(g_byArray); ++n)
				{
					int nNum = 0;
					BYTE byIndex[MAX_VAL_NUM] = {};
					for (int i = 0; i < MAX_VAL_NUM; ++i)
					{
						byIndex[i] = (n >> (2 * i)) & 0x03;
						nNum += byIndex[i];
					}						
					if (nNum >= 15) continue;

					int nVal = getKeyByIndex(byIndex, MAX_VAL_NUM);
					unordered_map<int, BYTE>::iterator iter = g_mapHuAll[nNum].find(nVal);
					if (iter == g_mapHuAll[nNum].end())
						g_byError[n] = 1;					
				}
				for (int i = 0; i < 15; ++i)
					g_mapHuAll[i].clear();

				// just show info
				int nZero = 0, nZeroFZ = 0, nError = 0, nAll = 0;
				for (int i = 0; i < sizeof(g_byArray); ++i)
				{
					int nNum = 0;
					BYTE byIndex[MAX_VAL_NUM] = {};
					for (int n = 0; n < MAX_VAL_NUM; ++n)
					{
						byIndex[n] = (i >> (2 * n)) & 0x03;
						nNum += byIndex[n];
					}
					if (nNum >= 15) continue;

					++nAll;
					if (g_byArray[i] == 0) ++nZero;
					if (g_byArrayFZ[i] == 0) ++nZeroFZ;
					if (g_byError[i] == 0) ++nError;
				}
				cout << "nAll = " << nAll << endl;
				cout << "nZero = " << nAll - nZero << " / " << nZero << endl;
				cout << "nZeroFZ = " << nAll - nZeroFZ << " / " << nZeroFZ << endl;
				cout << "nError = " << nAll - nError << " / " << nError << endl;
			}
			else
				cout << "already trained!" << endl;
		}

		static bool CheckCanHu(BYTE byCardSrc[], BYTE byNaiIndex)
		{
			BYTE byCards[MAX_TOTAL_TYPE];
			memcpy(byCards, byCardSrc, MAX_TOTAL_TYPE);
			int nNaiZiNum = 0;
			if (byNaiIndex < MAX_TOTAL_TYPE)
			{
				nNaiZiNum = byCards[byNaiIndex];
				byCards[byNaiIndex] = 0;
			}

			BYTE byJiangNum = 0;
			BYTE nNaiTry;
			for (int cor = 0; cor < enColorMJ_Max; ++cor)
			{				
				int nMax = (cor == enColorMJ_FenZi) ? 7 : 9;
				int nVal = 0, nNum = 0;
				BYTE byDel = 0, byTemp = 0;
				for (int i = 0; i < nMax; ++i)
				{
					byTemp = byCards[9 * cor + i];
					nNum += byTemp;
					if (byTemp > 3)
					{
						++byDel;
						nVal |= (int)(byTemp - 3) << (2 * i);
					}						
					else
						nVal |= (int)(byTemp) << (2 * i);
				}

				if (nNum == 0) continue;

				if (byDel == 0 && g_byError[nVal])
					return false;

				nNaiTry = (cor == enColorMJ_FenZi) ? g_byArrayFZ[nVal] - 1 : g_byArray[nVal] - 1;
				if (nNaiTry == 0xFF) return false;

				nNaiZiNum -= nNaiTry;
				byJiangNum += ((nNum + nNaiTry) % 3 == 2);
				if (nNaiZiNum < 0 || byJiangNum > nNaiZiNum + 1)
					return false;
			}
			return byJiangNum > 0 || nNaiZiNum >= 2;
		}
	};
}

#endif //__HU_PAI_MJ3_H__