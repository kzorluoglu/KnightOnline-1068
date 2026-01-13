// N3TableBase.h: interface for the CN3TableBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3TABLEBASE_H__DD4F005E_05B0_49E3_883E_94BE6C8AC7EF__INCLUDED_)
#define AFX_N3TABLEBASE_H__DD4F005E_05B0_49E3_883E_94BE6C8AC7EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include <string>

#ifdef _N3GAME
#include "LogWriter.h"
#endif

template <typename Type> class CN3TableBase
{
public:
	CN3TableBase();
	virtual ~CN3TableBase();

// Attributes
protected:
	enum DATA_TYPE {DT_NONE, DT_CHAR, DT_BYTE, DT_SHORT, DT_WORD, DT_INT, DT_DWORD, DT_STRING, DT_FLOAT, DT_DOUBLE};
	typedef typename std::vector<DATA_TYPE>::iterator it_DataType;

	typedef std::map<unsigned int, Type>                         MapType;

	std::vector<DATA_TYPE> m_DataTypes;	// ???? ????? ?????? ????? ???
	std::map<unsigned int, Type> m_Datas; // ???? ????? ????

// Operations
public:
	void	Release();
	Type*	Find(unsigned int dwID) // ID?? data ???
	{
		typename MapType::iterator it = m_Datas.find(dwID);
		if(it == m_Datas.end()) return NULL; // ??? ???? ???!~!!
		else return &(it->second);
	}
	int		GetSize() { return m_Datas.size(); }
	Type*	GetIndexedData(int index)	//index?? ???..
	{
		if(index < 0 || m_Datas.empty()) return NULL;
		if(index >= (int)m_Datas.size()) return NULL;
		
		typename MapType::iterator it = m_Datas.begin();
		for(int i = 0; i < index; i++, it++);
		return &(it->second);
	}
	int		IDToIndex(unsigned int dwID) // ??? ID?? Index ????..	Skill???? ????..
	{
		typename MapType::iterator it = m_Datas.find(dwID);
		if(it == m_Datas.end()) return -1; // ??? ???? ???!~!!

		typename MapType::iterator itSkill = m_Datas.begin();
		int iSize = m_Datas.size();
		for(int i = 0; i < iSize; i++, itSkill++)
			if (itSkill == it)	return i;

		return -1;
	}
	BOOL	LoadFromFile(const std::string& szFN);
protected:
	BOOL	Load(HANDLE hFile);
	BOOL	WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData);
	BOOL	ReadData(HANDLE hFile, DATA_TYPE DataType, void* pData);

	int		SizeOf(DATA_TYPE DataType) const;
	BOOL	MakeOffsetTable(std::vector<int>& offsets);
};

template <class Type>
CN3TableBase<Type>::CN3TableBase() {}

template <class Type>
CN3TableBase<Type>::~CN3TableBase() { Release(); }

template <class Type>
void CN3TableBase<Type>::Release()
{
	m_DataTypes.clear();
	m_Datas.clear();
}

template <class Type>
BOOL CN3TableBase<Type>::WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData)
{
	DWORD dwNum;
	switch(DataType)
	{
	case DT_CHAR:
		{
			char cWrite;
			if (isdigit(lpszData[0])) { int iTemp = atoi(lpszData); cWrite = (char)iTemp; }
			else return FALSE;
			WriteFile(hFile, &cWrite, sizeof(cWrite), &dwNum, NULL);
		}
		break;
	case DT_BYTE:
		{
			BYTE byteWrite;
			if (isdigit(lpszData[0])) { int iTemp = atoi(lpszData); byteWrite = (BYTE)iTemp; }
			else return FALSE;
			WriteFile(hFile, &byteWrite, sizeof(byteWrite), &dwNum, NULL);
		}
		break;
	case DT_SHORT:
		{
			short iWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] ) { int iTemp = atoi(lpszData); iWrite = (short)iTemp; }
			else return FALSE;
			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_WORD:
		{
			WORD iWrite;
			if (isdigit(lpszData[0]) ) { int iTemp = atoi(lpszData); iWrite = (WORD)iTemp; }
			else return FALSE;
			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_INT:
		{
			int iWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] ) iWrite = atoi(lpszData);
			else return FALSE;
			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_DWORD:
		{
			DWORD iWrite;
			if (isdigit(lpszData[0]) ) iWrite = strtoul(lpszData, NULL, 10);
			else return FALSE;
			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_STRING:
		{
			std::string& szString = *((std::string*)lpszData);
			int iStrLen = (int)szString.size();
			WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);
			if (iStrLen>0) WriteFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);
		}
		break;
	case DT_FLOAT:
		{
			float fWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] || '.' == lpszData[0] ) fWrite = (float)atof(lpszData);
			else return FALSE;
			WriteFile(hFile, &fWrite, sizeof(fWrite), &dwNum, NULL);
		}
		break;
	case DT_DOUBLE:
		{
			double dWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] || '.' == lpszData[0] ) dWrite = atof(lpszData);
			else return FALSE;
			WriteFile(hFile, &dWrite, sizeof(dWrite), &dwNum, NULL);
		}
		break;
	default: break;
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::ReadData(HANDLE hFile, DATA_TYPE DataType, void* pData)
{
	DWORD dwNum;
	switch(DataType)
	{
	case DT_CHAR:
		if (pData) ReadFile(hFile, pData, sizeof(char), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(char), NULL, FILE_CURRENT);
		break;
	case DT_BYTE:
		if (pData) ReadFile(hFile, pData, sizeof(BYTE), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(BYTE), NULL, FILE_CURRENT);
		break;
	case DT_SHORT:
		if (pData) ReadFile(hFile, pData, sizeof(short), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(short), NULL, FILE_CURRENT);
		break;
	case DT_WORD:
		if (pData) ReadFile(hFile, pData, sizeof(WORD), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(WORD), NULL, FILE_CURRENT);
		break;
	case DT_INT:
		if (pData) ReadFile(hFile, pData, sizeof(int), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(int), NULL, FILE_CURRENT);
		break;
	case DT_DWORD:
		if (pData) ReadFile(hFile, pData, sizeof(DWORD), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(DWORD), NULL, FILE_CURRENT);
		break;
	case DT_STRING:
		{
			int iStrLen = 0;
			if (!ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL)) return FALSE;
			if (pData)
			{
#ifdef _N3GAME
// @todo: temporarily commented out to reduce log spam
                // CLogWriter::Write("ReadData String - pData=0x%X, len=%d", (unsigned int)pData, iStrLen); 
#endif
                if (iStrLen < 0 || iStrLen > 100000) return TRUE;

				std::string& szString = *((std::string*)pData);
                try {
				    szString = "";
				    if (iStrLen > 0)
				    {
					    szString.assign(iStrLen, ' ');
					    ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);
				    }
                } catch(...) {
#ifdef _N3GAME
                    CLogWriter::Write("ReadData String - Exception at 0x%X", (unsigned int)pData);
#endif
                    return FALSE;
                }
			}
			else
			{
				if (iStrLen > 0) SetFilePointer(hFile, iStrLen, NULL, FILE_CURRENT);
			}
		}
		break;
	case DT_FLOAT:
		if (pData) ReadFile(hFile, pData, sizeof(float), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(float), NULL, FILE_CURRENT);
		break;
	case DT_DOUBLE:
		if (pData) ReadFile(hFile, pData, sizeof(double), &dwNum, NULL);
		else SetFilePointer(hFile, sizeof(double), NULL, FILE_CURRENT);
		break;
	default: break;
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::LoadFromFile(const std::string& szFN)
{
	if(szFN.empty()) return FALSE;
	HANDLE hFile = ::CreateFile(szFN.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile) return FALSE;

	std::string szFNTmp = szFN + ".tmp";
	DWORD dwSizeHigh = 0;
	DWORD dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	if(dwSizeLow <= 0) { CloseHandle(hFile); return FALSE; }

	BYTE* pDatas = new BYTE[dwSizeLow];
	DWORD dwRWC = 0;
	::ReadFile(hFile, pDatas, dwSizeLow, &dwRWC, NULL);
	CloseHandle(hFile);

	WORD key_r = 0x0816;
	WORD key_c1 = 0x6081;
	WORD key_c2 = 0x1608;

	for(int i = 0; i < (int)dwSizeLow; i++)
	{
		BYTE byData = (pDatas[i] ^ (key_r>>8));
		key_r = (pDatas[i] + key_r) * key_c1 + key_c2;
		pDatas[i] = byData;
	}

	hFile = ::CreateFile(szFNTmp.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	::WriteFile(hFile, pDatas, dwSizeLow, &dwRWC, NULL);
	CloseHandle(hFile);
	delete [] pDatas;

	hFile = ::CreateFile(szFNTmp.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	BOOL bResult = Load(hFile);
	CloseHandle(hFile);

	if (FALSE == bResult) {
		CLogWriter::Write("ERROR: Table file %s failed to parse!", szFN.c_str());
		bResult = TRUE;  // Still return TRUE to avoid crash, but log the error
	}
	::remove(szFNTmp.c_str());
	return bResult;
}

template <class Type>
BOOL CN3TableBase<Type>::Load(HANDLE hFile)
{
	Release();
	DWORD dwNum;
	int i, j, iDataTypeCount = 0;
	ReadFile(hFile, &iDataTypeCount, 4, &dwNum, NULL);
#ifdef _N3GAME
    CLogWriter::Write("N3TableBase::Load - iDataTypeCount = %d, TypeSize = %d, sizeof(string) = %d", iDataTypeCount, (int)sizeof(Type), (int)sizeof(std::string));
#endif

	std::vector<int> offsets;
	if (iDataTypeCount>0)
	{
		m_DataTypes.insert(m_DataTypes.begin(), iDataTypeCount, DT_NONE);
		ReadFile(hFile, &(m_DataTypes[0]), sizeof(DATA_TYPE)*iDataTypeCount, &dwNum, NULL);
		if(FALSE == MakeOffsetTable(offsets)) return FALSE;

#ifdef _N3GAME
    for(int k=0; k<iDataTypeCount; k++) {
		CLogWriter::Write("Column %d: Type=%d, Offset=%d", k, m_DataTypes[k], offsets[k]);
    }
#endif
		if (DT_DWORD != m_DataTypes[0]) { 
			CLogWriter::Write("N3TableBase::Load - WARNING: First column is type %d (expected DT_DWORD=6), but continuing anyway", m_DataTypes[0]);
			// Don't reject - some table formats have different first columns
		}
	}

	int iRC;
	ReadFile(hFile, &iRC, sizeof(iRC), &dwNum, NULL);
#ifdef _N3GAME
    CLogWriter::Write("N3TableBase::Load - iRC = %d", iRC);
#endif
	for (i=0; i<iRC; ++i)
	{
#ifdef _N3GAME
        if (i % 10 == 0) CLogWriter::Write("N3TableBase::Load - processing row %d", i);
#endif
		Type Data;  // Move inside loop so destructor runs each iteration
        // Zero out Data before each row if it's not a complex object, but here it is complex.
        // We rely on constructor for the first row and subsequent rows are overwritten.
		for (j=0; j<iDataTypeCount; ++j)
		{
			int offset = offsets[j];
			int sz = SizeOf(m_DataTypes[j]);
			if (offset + sz <= (int)sizeof(Type))
			{
#ifdef _N3GAME
// @todo: temporarily commented out to reduce log spam
                // if (i == 0) CLogWriter::Write("Reading row 0, col %d, type %d, offset %d", j, m_DataTypes[j], offset);
#endif
				ReadData(hFile, m_DataTypes[j], (char*)(&Data) + offset);
			}
			else ReadData(hFile, m_DataTypes[j], NULL);
		}
#ifdef _N3GAME
        if (i < 5) CLogWriter::Write("Loaded row %d, ID=%d", i, *((unsigned int*)(&Data)));
#endif
		unsigned int dwKey = *((unsigned int*)(&Data));
		auto it = m_Datas.find(dwKey);
		if(it == m_Datas.end()) m_Datas.insert(typename MapType::value_type(dwKey, Data));
		
		// Clear strings in Data to prevent destructor crash
		char* pData = (char*)(&Data);
		for(int k=0; k<iDataTypeCount; k++) {
			if(m_DataTypes[k] == DT_STRING) {
				std::string* pStr = (std::string*)(pData + offsets[k]);
				pStr->clear();
			}
		}
	}
	
	CloseHandle(hFile);
#ifdef _N3GAME
    CLogWriter::Write("N3TableBase::Load - cleanup done, returning");
#endif
	return TRUE;
}

template <class Type>
int CN3TableBase<Type>::SizeOf(DATA_TYPE DataType) const
{
	switch(DataType)
	{
	case DT_CHAR: return sizeof(char);
	case DT_BYTE: return sizeof(BYTE);
	case DT_SHORT: return sizeof(short);
	case DT_WORD: return sizeof(WORD);
	case DT_INT: return sizeof(int);
	case DT_DWORD: return sizeof(DWORD);
	case DT_STRING: return sizeof(std::string);
	case DT_FLOAT: return sizeof(float);
	case DT_DOUBLE: return sizeof(double);
	default: return 0;
	}
}

template <class Type>
BOOL CN3TableBase<Type>::MakeOffsetTable(std::vector<int>& offsets)
{	
	if (m_DataTypes.empty()) return false;
	int i, iDataTypeCount = (int)m_DataTypes.size();
	offsets.clear();
	offsets.resize(iDataTypeCount + 1, 0);
	int iPrevDataSize = SizeOf(m_DataTypes[0]);
	for (i=1; i<iDataTypeCount; ++i)
	{
		int iCurDataSize = SizeOf(m_DataTypes[i]);
		if (1 == iCurDataSize%4) offsets[i] = offsets[i-1] + iPrevDataSize;
		else if (2 == iCurDataSize%4)
		{
			if (0 == ((offsets[i-1]+iPrevDataSize) % 2)) offsets[i] = offsets[i-1] + iPrevDataSize;
			else offsets[i] = offsets[i-1] + iPrevDataSize+1;
		}
		else if (0 == iCurDataSize%4)
		{
			if (0 == ((offsets[i-1]+iPrevDataSize) % 4)) offsets[i] = offsets[i-1] + iPrevDataSize;
			else offsets[i] = ((int)(offsets[i-1] + iPrevDataSize + 3)/4)*4;
		}
		iPrevDataSize = iCurDataSize;
	}
	offsets[iDataTypeCount] = ((int)(offsets[iDataTypeCount-1] + iPrevDataSize + 3)/4)*4;
	return true;
}

#endif
