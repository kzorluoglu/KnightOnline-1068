// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7F6B8F8D_6B2B_47EE_A744_F49F1D4A2556__INCLUDED_)
#define AFX_STDAFX_H__7F6B8F8D_6B2B_47EE_A744_F49F1D4A2556__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <winsock2.h>		// Winsock 2
#include <afxtempl.h>
#include <afxdb.h>

//#include "Mmsystem.h"
//#include "Imm.h"
//#include "./N3Base/My_3DStruct.h"

#include "packet.h"			// packet 정의
#include "global.h"			// 전역 funtion
#include "define.h"			// define
//#include "extern.h"			// 전역 객체

inline CString KO_GetIniPath(LPCTSTR primaryName, LPCTSTR fallbackName)
{
	CString base = GetProgPath();
	CString primary = base + _T("\\") + primaryName;
	if (fallbackName && GetFileAttributes(primary) == INVALID_FILE_ATTRIBUTES) {
		CString fallback = base + _T("\\") + fallbackName;
		if (GetFileAttributes(fallback) != INVALID_FILE_ATTRIBUTES)
			return fallback;
	}
	return primary;
}

inline CString KO_BuildOdbcConnString(LPCTSTR dbName, LPCTSTR iniFileName, LPCTSTR fallbackIniName = NULL,
	LPCTSTR uidOverride = NULL, LPCTSTR pwdOverride = NULL)
{
	CString db = dbName ? dbName : _T("");
	CString lower = db;
	lower.MakeLower();
	if (lower.Find(_T("driver=")) >= 0 || lower.Find(_T("server=")) >= 0 ||
		lower.Find(_T("dsn=")) >= 0 || lower.Find(_T("odbc;")) >= 0) {
		return db;
	}

	CString iniPath = KO_GetIniPath(iniFileName, fallbackIniName);
	TCHAR driver[128] = _T("ODBC Driver 17 for SQL Server");
	TCHAR server[128] = _T("localhost");
	TCHAR port[16] = _T("1433");
	TCHAR uid[64] = _T("knight");
	TCHAR pwd[64] = _T("knight");
	TCHAR encrypt[8] = _T("NO");
	TCHAR trust[4] = _T("YES");

	GetPrivateProfileString(_T("ODBC"), _T("DRIVER"), driver, driver, 128, iniPath);
	GetPrivateProfileString(_T("ODBC"), _T("SERVER"), server, server, 128, iniPath);
	GetPrivateProfileString(_T("ODBC"), _T("PORT"), port, port, 16, iniPath);
	GetPrivateProfileString(_T("ODBC"), _T("UID"), uid, uid, 64, iniPath);
	GetPrivateProfileString(_T("ODBC"), _T("PWD"), pwd, pwd, 64, iniPath);
	GetPrivateProfileString(_T("ODBC"), _T("ENCRYPT"), encrypt, encrypt, 8, iniPath);
	GetPrivateProfileString(_T("ODBC"), _T("TRUSTSERVERCERTIFICATE"), trust, trust, 4, iniPath);

	if (uidOverride && uidOverride[0])
		lstrcpyn(uid, uidOverride, 64);
	if (pwdOverride && pwdOverride[0])
		lstrcpyn(pwd, pwdOverride, 64);

	CString conn;
	conn.Format(_T("Driver={%s};Server=%s,%s;Database=%s;UID=%s;PWD=%s;Trusted_Connection=No;Encrypt=%s;TrustServerCertificate=%s;"),
		driver, server, port, db, uid, pwd, encrypt, trust);
	return conn;
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7F6B8F8D_6B2B_47EE_A744_F49F1D4A2556__INCLUDED_)
