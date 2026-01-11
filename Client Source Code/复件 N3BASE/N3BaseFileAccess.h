// N3BaseFileAccess.h: interface for the CN3BaseFileAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_N3BASEFILEACCESS_H__C99953BD_12BE_4B37_823F_4F4B2379FF74__INCLUDED_)
#define AFX_N3BASEFILEACCESS_H__C99953BD_12BE_4B37_823F_4F4B2379FF74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include "N3Base.h"
#include <string>

class CN3BaseFileAccess : public CN3Base
{
protected:
	std::string		m_szFileName; // Base Path �� ������ ���� ��� + ���� �̸�

public:
	int m_iLOD; // �ε��Ҷ� �� LOD

public:
	const std::string& FileName() const { return m_szFileName; } // Full Path
	void FileNameSet(const std::string& szFileName);

	bool LoadFromFile(); // ���Ͽ��� �о����.
	virtual bool LoadFromFile(const std::string& szFileName); // ���Ͽ��� �о����.
	virtual bool Load(HANDLE hFile); // �ڵ鿡�� �о����..

	virtual bool SaveToFile(); // ���� ���� �̸���� ����.
	virtual bool SaveToFile(const std::string& szFileName); // ���̸����� ����.
	virtual bool Save(HANDLE hFile); // �ڵ��� ���� ����..

public:
	void Release();

	CN3BaseFileAccess();
	virtual ~CN3BaseFileAccess();
};

#endif // !defined(AFX_N3BASEFILEACCESS_H__C99953BD_12BE_4B37_823F_4F4B2379FF74__INCLUDED_)
