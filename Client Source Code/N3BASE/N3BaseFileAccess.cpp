// N3BaseFileAccess.cpp: implementation of the CN3BaseFileAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfxBase.h"
#include "N3BaseFileAccess.h"
#include "UIEDebugLog.h"
#include <vector>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CN3BaseFileAccess::CN3BaseFileAccess()
{
	m_dwType |= OBJ_BASE_FILEACCESS;
	m_szFileName = "";
	m_iLOD = 0; // ?????? ?? LOD
}

CN3BaseFileAccess::~CN3BaseFileAccess()
{
}

void CN3BaseFileAccess::Release()
{
	m_szFileName = "";
	m_iLOD = 0; // ?????? ?? LOD
	CN3Base::Release();
}

void CN3BaseFileAccess::FileNameSet(const std::string& szFileName)
{
	std::string szTmpFN = szFileName;

	// Convert to lowercase using standard C++ for better Unicode and Windows 11 compatibility
	if(!szTmpFN.empty()) {
		for(size_t i = 0; i < szTmpFN.length(); i++) {
			szTmpFN[i] = (char)tolower((unsigned char)szTmpFN[i]);
		}
	}
 	int iPos = szTmpFN.find(s_szPath); // ??????? Base Path ?? ?????? ????? ????? ????.
	if(iPos >= 0) m_szFileName = szTmpFN.substr(s_szPath.size()); // ???? ??????.. ????? ??????..
	else m_szFileName = szTmpFN;
}

bool CN3BaseFileAccess::Load(HANDLE hFile)
{
	m_szName = "";

	DWORD dwRWC = 0;
	int nL = 0;
	ReadFile(hFile, &nL, 4, &dwRWC, NULL);
#ifdef _N3GAME
    if (nL > 1000) CLogWriter::Write("CN3BaseFileAccess::Load - WARNING: massive string length %d", nL);
#endif

    // Sanity check string length against file size to avoid overruns from corrupt/misaligned data.
    DWORD dwFileSize = GetFileSize(hFile, NULL);
    DWORD dwPosAfterLen = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
    if (dwFileSize != INVALID_FILE_SIZE && dwPosAfterLen != INVALID_SET_FILE_POINTER && nL > 0)
    {
        DWORD dwRemaining = (dwPosAfterLen < dwFileSize) ? (dwFileSize - dwPosAfterLen) : 0;
        if ((DWORD)nL > dwRemaining)
        {
            int nLSwapped = (nL >> 16); // handle values like 0x000F0000
            if (nLSwapped > 0 && (DWORD)nLSwapped <= dwRemaining)
            {
#ifdef _N3GAME
                CLogWriter::Write("CN3BaseFileAccess::Load - clamping swapped string length %d -> %d (remaining %u)", nL, nLSwapped, dwRemaining);
#endif
                nL = nLSwapped;
            }
            else
            {
#ifdef _N3GAME
                CLogWriter::Write("CN3BaseFileAccess::Load - clamping string length %d to remaining %u", nL, dwRemaining);
#endif
                nL = (int)dwRemaining;
            }
        }
    }

    if (nL < 0 || nL > 10000) nL = 0;

	if(nL > 0) 
	{
		std::vector<char> buffer(nL + 1, 0);
		ReadFile(hFile, buffer.data(), nL, &dwRWC, NULL);
		buffer[nL] = 0;
		m_szName = buffer.data();
	}

	return true;
}

bool CN3BaseFileAccess::LoadFromFile()
{
	if(m_szFileName.size() <= 0)
	{
		UIEDebugLog::Log("FILE_LOAD: Cannot open file - filename is empty");
		return false;
	}

	std::string szFullPath;
	if(-1 != m_szFileName.find(':') || -1 != m_szFileName.find("\\\\") || -1 != m_szFileName.find("//")) // ??????? ':', '\\', '//' ?? ??? ?????? ??? ??????..
	{
		szFullPath = m_szFileName;
	}
	else
	{
		if(s_szPath.size() > 0) szFullPath = s_szPath;
		szFullPath += m_szFileName;
	}

	UIEDebugLog::Log("FILE_LOAD: Opening file: %s", szFullPath.c_str());

	DWORD dwRWC = 0;
	HANDLE hFile = ::CreateFile(szFullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(INVALID_HANDLE_VALUE == hFile)
	{
		DWORD dwError = GetLastError();
		UIEDebugLog::Log("FILE_LOAD_ERROR: Cannot open file: %s (Error: %lu)", szFullPath.c_str(), dwError);
		return false;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	UIEDebugLog::Log("FILE_LOAD_OK: File opened successfully, size: %lu bytes", dwFileSize);

	bool bSuccess =	this->Load(hFile);

	CloseHandle(hFile);

	return bSuccess;
}

bool CN3BaseFileAccess::LoadFromFile(const std::string& szFileName)
{
	this->FileNameSet(szFileName);
	return this->LoadFromFile();
}

bool CN3BaseFileAccess::SaveToFile()
{
	if(m_szFileName.size() <= 0)
	{
		std::string szErr = m_szName + " Can't open file (write) - NULL String";
		MessageBox(::GetActiveWindow(), szErr.c_str(), "File Open Error", MB_OK);
		return false;
	}

	std::string szFullPath;
	if(-1 != m_szFileName.find(':') || -1 != m_szFileName.find("\\\\") || -1 != m_szFileName.find("//")) // ??????? ':', '\\', '//' ?? ??? ?????? ??? ??????..
	{
		szFullPath = m_szFileName;
	}
	else
	{
		if(s_szPath.size() > 0) szFullPath = s_szPath;
		szFullPath += m_szFileName;
	}

	DWORD dwRWC = 0;
	HANDLE hFile = ::CreateFile(szFullPath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if(hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwError = GetLastError();
		char szErrCode[64];
		sprintf_s(szErrCode, sizeof(szErrCode), " (Error: %lu)", dwError);
		std::string szErr = szFullPath + " - Can't open file(write)" + szErrCode;
		MessageBox(::GetActiveWindow(), szErr.c_str(), "File Handle error", MB_OK);
		return false;
	}

	this->Save(hFile);

	CloseHandle(hFile);
	return true;
}

bool CN3BaseFileAccess::SaveToFile(const std::string& szFileName)
{
	this->FileNameSet(szFileName);
	return this->SaveToFile();
}

bool CN3BaseFileAccess::Save(HANDLE hFile)
{
	DWORD dwRWC = 0;

	int nL = m_szName.size();
	WriteFile(hFile, &nL, 4, &dwRWC, NULL);
	if(nL > 0) WriteFile(hFile, m_szName.c_str(), nL, &dwRWC, NULL);

	return true;
}

