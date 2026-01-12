#pragma once

#include <stdio.h>
#include <time.h>
#include <string>
#include <windows.h>

class UIEDebugLog
{
private:
	static FILE*& GetLogFile()
	{
		static FILE* pFile = NULL;
		return pFile;
	}

	static bool& GetInitialized()
	{
		static bool bInit = false;
		return bInit;
	}

public:
	static void Initialize()
	{
		FILE*& pFile = GetLogFile();
		bool& bInitialized = GetInitialized();
		if (bInitialized) return;
		bInitialized = true;

		// Create log file in the UIE directory
		char szLogPath[MAX_PATH];
		GetModuleFileNameA(NULL, szLogPath, MAX_PATH);

		// Get directory of exe
		char* pLastSlash = strrchr(szLogPath, '\\');
		if (pLastSlash) {
			*(pLastSlash + 1) = '\0';
		}

		// Create UIEDebug.log in the same directory as UIE.exe
		strcat_s(szLogPath, MAX_PATH, "UIEDebug.log");

		// Append mode - keep previous logs
		fopen_s(&pFile, szLogPath, "a");
		if (pFile) {
			fprintf(pFile, "\n");
			fprintf(pFile, "=== UIE Debug Log Started ===\n");
			time_t now = time(NULL);
			struct tm timeinfo;
			localtime_s(&timeinfo, &now);
			char szTime[128];
			strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &timeinfo);
			fprintf(pFile, "Time: %s\n", szTime);
			fprintf(pFile, "================================\n\n");
			fflush(pFile);
		}
	}

	static void Shutdown()
	{
		FILE*& pFile = GetLogFile();
		if (pFile) {
			fprintf(pFile, "\n=== UIE Debug Log Ended ===\n");
			fflush(pFile);
			fclose(pFile);
			pFile = NULL;
		}
	}

	static void Log(const char* szFormat, ...)
	{
		Initialize();
		FILE*& pFile = GetLogFile();
		if (!pFile) return;

		va_list args;
		va_start(args, szFormat);

		// Add timestamp
		time_t now = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char szTime[64];
		strftime(szTime, sizeof(szTime), "[%H:%M:%S]", &timeinfo);

		fprintf(pFile, "%s ", szTime);
		vfprintf(pFile, szFormat, args);
		fprintf(pFile, "\n");
		fflush(pFile);

		va_end(args);
	}

	static void LogError(const char* szFunction, const char* szMessage, DWORD dwError = 0)
	{
		Initialize();
		FILE*& pFile = GetLogFile();
		if (!pFile) return;

		time_t now = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char szTime[64];
		strftime(szTime, sizeof(szTime), "[%H:%M:%S]", &timeinfo);

		fprintf(pFile, "%s [ERROR] %s: %s", szTime, szFunction, szMessage);
		if (dwError != 0) {
			fprintf(pFile, " (Error Code: %lu)", dwError);
		}
		fprintf(pFile, "\n");
		fflush(pFile);
	}

	static void LogFileInfo(const char* szFilePath, DWORD dwFileSize)
	{
		Initialize();
		FILE*& pFile = GetLogFile();
		if (!pFile) return;

		time_t now = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char szTime[64];
		strftime(szTime, sizeof(szTime), "[%H:%M:%S]", &timeinfo);

		fprintf(pFile, "%s [FILE] Path: %s\n", szTime, szFilePath);
		fprintf(pFile, "%s [FILE] Size: %lu bytes\n", szTime, dwFileSize);
		fflush(pFile);
	}

	static void LogReadOperation(const char* szWhat, int iExpected, int iActual, DWORD dwFilePosition)
	{
		Initialize();
		FILE*& pFile = GetLogFile();
		if (!pFile) return;

		time_t now = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char szTime[64];
		strftime(szTime, sizeof(szTime), "[%H:%M:%S]", &timeinfo);

		if (iExpected == iActual) {
			fprintf(pFile, "%s [READ] OK: %s (expected %d bytes, got %d) at offset %lu\n",
				szTime, szWhat, iExpected, iActual, dwFilePosition);
		} else {
			fprintf(pFile, "%s [READ] FAIL: %s (expected %d bytes, got %d) at offset %lu\n",
				szTime, szWhat, iExpected, iActual, dwFilePosition);
		}
		fflush(pFile);
	}

	static void LogUIElement(int iChildNum, int iTotalChildren, const char* szUIType, const char* szUIID)
	{
		Initialize();
		FILE*& pFile = GetLogFile();
		if (!pFile) return;

		time_t now = time(NULL);
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char szTime[64];
		strftime(szTime, sizeof(szTime), "[%H:%M:%S]", &timeinfo);

		fprintf(pFile, "%s [UI] Loading child %d/%d: Type=%s, ID=%s\n",
			szTime, iChildNum, iTotalChildren, szUIType, szUIID ? szUIID : "(none)");
		fflush(pFile);
	}
};
