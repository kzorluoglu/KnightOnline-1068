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

template <typename Type> class CN3TableBase
{
public:
	CN3TableBase();
	virtual ~CN3TableBase();

// Attributes
protected:
	enum DATA_TYPE {DT_NONE, DT_CHAR, DT_BYTE, DT_SHORT, DT_WORD, DT_INT, DT_DWORD, DT_STRING, DT_FLOAT, DT_DOUBLE};
	typedef std::vector<DATA_TYPE>::iterator it_DataType;
	
	typedef std::map<unsigned int, Type>::iterator		it_Table;
	typedef std::map<unsigned int, Type>::value_type	val_Table;
	typedef std::pair<it_Table, bool> pair_Table;

	std::vector<DATA_TYPE> m_DataTypes;	// ���� ���Ǵ� ������ ����Ÿ Ÿ��
	std::map<unsigned int, Type> m_Datas; // ���� ���Ǵ� ����

// Operations
public:
	void	Release();
	Type*	Find(unsigned int dwID) // ID�� data ã��
	{
		it_Table it = m_Datas.find(dwID);
		if(it == m_Datas.end()) return NULL; // ã�⿡ ���� �ߴ�!~!!
		else return &(it->second);
	}
	int		GetSize() { return m_Datas.size(); }
	Type*	GetIndexedData(int index)	//index�� ã��..
	{
		if(index < 0 || m_Datas.empty()) return NULL;
		if(index >= m_Datas.size()) return NULL;
		
		it_Table it = m_Datas.begin();
		for(int i = 0; i < index; i++, it++);
		return &(it->second);
	}
	int		IDToIndex(unsigned int dwID) // �ش� ID�� Index ����..	Skill���� ����..
	{
		it_Table it = m_Datas.find(dwID);
		if(it == m_Datas.end()) return -1; // ã�⿡ ���� �ߴ�!~!!

		it_Table itSkill = m_Datas.begin();
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



// cpp���Ͽ� �����ϱ� link������ ����. �� �׷���?

template <class Type>
CN3TableBase<Type>::CN3TableBase()
{
}

template <class Type>
CN3TableBase<Type>::~CN3TableBase()
{
	Release();
}

template <class Type>
void CN3TableBase<Type>::Release()
{
	m_DataTypes.clear(); // data type �����Ѱ� �����
	m_Datas.clear(); // row ����Ÿ �����
}

// ���Ͽ� ����Ÿ Ÿ�Ժ��� ����..
template <class Type>
BOOL CN3TableBase<Type>::WriteData(HANDLE hFile, DATA_TYPE DataType, const char* lpszData)
{
	DWORD dwNum;
	switch(DataType)
	{
	case DT_CHAR:
		{
			char cWrite;
			if (isdigit(lpszData[0]))
			{
				int iTemp = atoi(lpszData);
				if (iTemp < -127 || iTemp > 128) return FALSE; // ������ �����~
				cWrite = (char)iTemp;
			}
			else return FALSE;		// ���ڴ� �ȵ�~!

			WriteFile(hFile, &cWrite, sizeof(cWrite), &dwNum, NULL);
		}
		break;
	case DT_BYTE:
		{
			BYTE byteWrite;
			if (isdigit(lpszData[0]))
			{
				int iTemp = atoi(lpszData);
				if (iTemp < 0 || iTemp > 255) return FALSE; // ������ �����~
				byteWrite = (BYTE)iTemp;
			}
			else return FALSE;		// ���ڴ� �ȵ�~!

			WriteFile(hFile, &byteWrite, sizeof(byteWrite), &dwNum, NULL);
		}
		break;
	case DT_SHORT:
		{
			short iWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] )
			{
				int iTemp = atoi(lpszData);
				if (iTemp < -32767 || iTemp > 32768) return FALSE; // ������ �����~
				iWrite = (short)iTemp;
			}
			else return FALSE;		// ���ڴ� �ȵ�~!

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_WORD:
		{
			WORD iWrite;
			if (isdigit(lpszData[0]) )
			{
				int iTemp = atoi(lpszData);
				if (iTemp < 0 || iTemp > 65535) return FALSE; // ������ �����~
				iWrite = (short)iTemp;
			}
			else return FALSE;		// ���ڴ� �ȵ�~!

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_INT:
		{
			int iWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] )	iWrite = atoi(lpszData);
			else return FALSE;		// ���ڴ� �ȵ�~!

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_DWORD:
		{
			DWORD iWrite;
			if (isdigit(lpszData[0]) )	iWrite = strtoul(lpszData, NULL, 10);
			else return FALSE;		// ���ڴ� �ȵ�~!

			WriteFile(hFile, &iWrite, sizeof(iWrite), &dwNum, NULL);
		}
		break;
	case DT_STRING:
		{
			std::string& szString = *((std::string*)lpszData);
			int iStrLen = szString.size();
			WriteFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);
			if (iStrLen>0) WriteFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);
		}
		break;
	case DT_FLOAT:
		{
			float fWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] ||
				'.' == lpszData[0] )	fWrite = (float)atof(lpszData);
			else return FALSE;	// ���ڴ� �ȵ�~!
			WriteFile(hFile, &fWrite, sizeof(fWrite), &dwNum, NULL);
		}
		break;
	case DT_DOUBLE:
		{
			double dWrite;
			if (isdigit(lpszData[0]) || '-' == lpszData[0] ||
				'.' == lpszData[0] )	dWrite = atof(lpszData);
			WriteFile(hFile, &dWrite, sizeof(dWrite), &dwNum, NULL);
		}
		break;

	case DT_NONE:
	default:
		__ASSERT(0,"");
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
		{
			ReadFile(hFile, pData, sizeof(char), &dwNum, NULL);
		}
		break;
	case DT_BYTE:
		{
			ReadFile(hFile, pData, sizeof(BYTE), &dwNum, NULL);
		}
		break;
	case DT_SHORT:
		{
			ReadFile(hFile, pData, sizeof(short), &dwNum, NULL);
		}
		break;
	case DT_WORD:
		{
			ReadFile(hFile, pData, sizeof(WORD), &dwNum, NULL);
		}
		break;
	case DT_INT:
		{
			ReadFile(hFile, pData, sizeof(int), &dwNum, NULL);
		}
		break;
	case DT_DWORD:
		{
			ReadFile(hFile, pData, sizeof(DWORD), &dwNum, NULL);
		}
		break;
	case DT_STRING:
		{
			std::string& szString = *((std::string*)pData);
			
			int iStrLen = 0;
			ReadFile(hFile, &iStrLen, sizeof(iStrLen), &dwNum, NULL);

			szString = "";
			if (iStrLen>0)
			{
				szString.assign(iStrLen, ' ');
				ReadFile(hFile, &(szString[0]), iStrLen, &dwNum, NULL);
			}
		}
		break;
	case DT_FLOAT:
		{
			ReadFile(hFile, pData, sizeof(float), &dwNum, NULL);
		}
		break;
	case DT_DOUBLE:
		{
			ReadFile(hFile, pData, sizeof(double), &dwNum, NULL);
		}
		break;

	case DT_NONE:
	default:
		__ASSERT(0,"");
		return FALSE;
	}
	return TRUE;
}

template <class Type>
BOOL CN3TableBase<Type>::LoadFromFile(const std::string& szFN)
{
	if(szFN.empty()) return FALSE;

	HANDLE hFile = ::CreateFile(szFN.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(INVALID_HANDLE_VALUE == hFile)
	{
#ifdef _N3GAME
		CLogWriter::Write("N3TableBase - Can't open file(read) File Handle error (%s)", szFN.c_str());
#endif
		return FALSE;
	}


	
	
	
	
	
	
	
	
	// ���� ��ȣȭ Ǯ��.. .. �ӽ� ���Ͽ��� ������ ..
	std::string szFNTmp = szFN + ".tmp";
	DWORD dwSizeHigh = 0;
	DWORD dwSizeLow = ::GetFileSize(hFile, &dwSizeHigh);
	if(dwSizeLow <= 0)
	{
		CloseHandle(hFile);
		::remove(szFNTmp.c_str()); // �ӽ� ���� �����..
		return FALSE;
	}

	// ���� ������ �а�..
	BYTE* pDatas = new BYTE[dwSizeLow];
	DWORD dwRWC = 0;
	::ReadFile(hFile, pDatas, dwSizeLow, &dwRWC, NULL); // ��ȣȭ�� ������ �а�..
	CloseHandle(hFile); // ���� ���� �ݰ�

// ���̺� ����� ������ ���� Ű�� ���� Ű..
	WORD key_r = 0x0816;
	WORD key_c1 = 0x6081;
	WORD key_c2 = 0x1608;

//BYTE Encrypt(BYTE plain)
//{
//	BYTE cipher;
//	cipher = (plain ^ (key_r>>8));
//	key_r = (cipher + key_r) * key_c1 + key_c2;
//	return cipher;
//}

//BYTE Decrypt(BYTE cipher)
//{
//	BYTE plain;
//	plain = (cipher ^ (m_r>>8));
//	m_r = (cipher + m_r) * m_c1 + m_c2;
//	return plain;
//}

	// ��ȣȭ Ǯ��..
	for(int i = 0; i < dwSizeLow; i++)
	{
		BYTE byData = (pDatas[i] ^ (key_r>>8));
		key_r = (pDatas[i] + key_r) * key_c1 + key_c2;
		pDatas[i] = byData;
	}

	// �ӽ� ���Ͽ� ������.. �ٽ� ����..
	hFile = ::CreateFile(szFNTmp.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	::WriteFile(hFile, pDatas, dwSizeLow, &dwRWC, NULL); // �ӽ����Ͽ� ��ȣȭ Ǯ�� ������ ����
	CloseHandle(hFile); // �ӽ� ���� �ݱ�
	delete [] pDatas; pDatas = NULL;

	hFile = ::CreateFile(szFNTmp.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); // �ӽ� ���� �б� ���� ����.

	




	
	
	
	
	BOOL bResult = Load(hFile);

	CloseHandle(hFile);

	if (FALSE == bResult)
	{
#ifdef _N3GAME
		CLogWriter::Write("N3TableBase - incorrect table (%s)", szFN.c_str());
#endif
	}


	// �ӽ� ���� �����..
	::remove(szFNTmp.c_str());

	return bResult;
}

template <class Type>
BOOL CN3TableBase<Type>::Load(HANDLE hFile)
{
	Release();

	// data(column) �� ������ ��� �Ǿ� �ִ��� �б�
	DWORD dwNum;
	int i, j, iDataTypeCount = 0;
	ReadFile(hFile, &iDataTypeCount, 4, &dwNum, NULL);			// (�������� column ��)

	std::vector<int> offsets;
	__ASSERT(iDataTypeCount>0, "Data Type �� 0 �����Դϴ�.");
	if (iDataTypeCount>0)
	{
		m_DataTypes.insert(m_DataTypes.begin(), iDataTypeCount, DT_NONE);
		ReadFile(hFile, &(m_DataTypes[0]), sizeof(DATA_TYPE)*iDataTypeCount, &dwNum, NULL);	// ������ column�� �ش��ϴ� data type

		if(FALSE == MakeOffsetTable(offsets))
		{
			__ASSERT(0, "can't make offset table");
			return FALSE;	// structure������ ���� offset table ������ֱ�
		}

		int iSize = offsets[iDataTypeCount];	// MakeOffstTable �Լ����� ���ϵǴ� ���� m_iDataTypeCount��°�� �� �Լ��� ���� ����� ����ִ�.
		if (sizeof(Type) != iSize ||		// ��ü type�� ũ��� ���� ����ü�� ũ��� �ٸ��ų� 
			DT_DWORD != m_DataTypes[0] )	// �� ó���� ����Ÿ�� DT_DWORD���� �ƴҶ�(��ó���� ������ ID�̹Ƿ�)
		{
			m_DataTypes.clear();
			__ASSERT(0, "DataType is mismatch or DataSize is incorrect!!");
			return FALSE;
		}
	}

	// row �� �������� �б�
	int iRC;
	ReadFile(hFile, &iRC, sizeof(iRC), &dwNum, NULL);
	Type Data;
	for (i=0; i<iRC; ++i)
	{
		for (j=0; j<iDataTypeCount; ++j)
		{
			ReadData(hFile, m_DataTypes[j], (char*)(&Data) + offsets[j]);
		}

		unsigned int dwKey = *((unsigned int*)(&Data));
		pair_Table pt = m_Datas.insert(val_Table(dwKey, Data));

		__ASSERT(pt.second, "CN3TableBase<Type> : Key �ߺ� ���.");
	}
	return TRUE;
}

template <class Type>
int CN3TableBase<Type>::SizeOf(DATA_TYPE DataType) const
{
	switch(DataType)
	{
	case DT_CHAR:
		return sizeof(char);
	case DT_BYTE:
		return sizeof(BYTE);
	case DT_SHORT:
		return sizeof(short);
	case DT_WORD:
		return sizeof(WORD);
	case DT_INT:
		return sizeof(int);
	case DT_DWORD:
		return sizeof(DWORD);
	case DT_STRING:
		return sizeof(std::string);
	case DT_FLOAT:
		return sizeof(float);
	case DT_DOUBLE:
		return sizeof(double);
	}
	__ASSERT(0,"");
	return 0;
}

// structure�� 4����Ʈ �����Ͽ��� �޸𸮸� ��´�. ���� �Ʒ� �Լ��� �ʿ��ϴ�.
// �Ʒ� �Լ��� OffsetTable�� ����� �� �Ŀ��� ����� ���ϰ��� delete [] �� ���־�� �Ѵ�.
template <class Type>
BOOL CN3TableBase<Type>::MakeOffsetTable(std::vector<int>& offsets)
{	
	if (m_DataTypes.empty()) return false;

	int i, iDataTypeCount = m_DataTypes.size();
	offsets.clear();
	offsets.reserve(iDataTypeCount+1);	// +1�� �� ������ �� ������ ���� Type�� ���� ����� �ֱ� ���ؼ�
	offsets[0] = 0;
	int iPrevDataSize = SizeOf(m_DataTypes[0]);
	for (i=1; i<iDataTypeCount; ++i)
	{
		int iCurDataSize = SizeOf(m_DataTypes[i]);
		if (1 == iCurDataSize%4)	// ���� �����Ͱ� 1����Ʈ�� �׳� ���� �����Ͱ� �����Ʈ�� ��� ����.
		{
			offsets[i] = offsets[i-1] + iPrevDataSize;
		}
		else if (2 == iCurDataSize%4) // ���� �����Ͱ� 2����Ʈ�� ¦�������� ��ġ�ؾ� �Ѵ�.
		{
			if (0 == ((offsets[i-1]+iPrevDataSize) % 2))
				offsets[i] = offsets[i-1] + iPrevDataSize;
			else
				offsets[i] = offsets[i-1] + iPrevDataSize+1;
		}
		else if (0 == iCurDataSize%4) // ���� �����Ͱ� 4����Ʈ�� 4�� ��������� ��ġ�ؾ� �Ѵ�.
		{
			if (0 == ((offsets[i-1]+iPrevDataSize) % 4))
				offsets[i] = offsets[i-1] + iPrevDataSize;
			else
				offsets[i] = ((int)(offsets[i-1] + iPrevDataSize + 3)/4)*4;	// 4�� ����� �����
		}
		else __ASSERT(0,"");
		iPrevDataSize = iCurDataSize;
	}

	// �� ������ ���� Type�� ���� ����� ����.
	offsets[iDataTypeCount] = ((int)(offsets[iDataTypeCount-1] + iPrevDataSize + 3)/4)*4;	// 4�� ����� �����

	return true;
}

#endif // !defined(AFX_N3TABLEBASE_H__DD4F005E_05B0_49E3_883E_94BE6C8AC7EF__INCLUDED_)
