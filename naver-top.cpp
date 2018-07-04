// naver-top.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "winhttp.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <ctime>
#pragma comment(lib, "winhttp.lib")

using namespace std;

string get_text() {
	string r;

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL  bResults = FALSE;
	HINTERNET  hSession = NULL,
		hConnect = NULL,
		hRequest = NULL;

	// Use WinHttpOpen to obtain a session handle.
	hSession = WinHttpOpen(L"WinHTTP Example/1.0",
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);

	// Specify an HTTP server.
	if (hSession)
		hConnect = WinHttpConnect(hSession, L"www.naver.com",
			INTERNET_DEFAULT_HTTPS_PORT, 0);

	// Create an HTTP request handle.
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", NULL,
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			WINHTTP_FLAG_SECURE);

	// Send a request.
	if (hRequest)
		bResults = WinHttpSendRequest(hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS, 0,
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);


	// End the request.
	if (bResults)
		bResults = WinHttpReceiveResponse(hRequest, NULL);

	// Keep checking for data until there is nothing left.
	if (bResults)
	{
		do
		{
			// Check for available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				printf("Error %u in WinHttpQueryDataAvailable.\n",
					GetLastError());

			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			// Read the data.
			ZeroMemory(pszOutBuffer, dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
				dwSize, &dwDownloaded))
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			else {
				r += pszOutBuffer;
			}

			// Free the memory allocated to the buffer.
			delete[] pszOutBuffer;
		} while (dwSize > 0);
	}


	// Report any errors.
	if (!bResults)
		printf("Error %d has occurred.\n", GetLastError());

	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);

	return r;
}

string UTF8ToANSI(string s)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;
	const char *pszCode = s.c_str();

	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];

	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);

	string r(pszAnsi);
	delete[] pszAnsi;
	return r;
}

vector<string> list_naver() {
	string s(UTF8ToANSI(get_text()));

	static regex e("<span class=\"ah_k\">(.+?)</span>");
	regex_iterator<string::iterator> rit(s.begin(), s.end(), e);
	static regex_iterator<string::iterator> rend;

	vector<string> r;

	size_t count = 0;
	while (rit != rend && count < 20) {
		r.push_back((*rit)[1]);
		++rit;
		count++;
	}
	return r;
}

// followed by newline
string now() {
	time_t rawtime;

	time(&rawtime);
	return string(ctime(&rawtime));
}

int _tmain(int argc, _TCHAR* argv[])
{
	while (true) {
		cout << now();
		auto ln = list_naver();
		for (auto i = 0u; i < ln.size(); ++i) {
			if (i > 0) cout << ", ";
			cout << ln[i];
		}
		cout << endl << endl;
		Sleep(60 * 1000);
	}

	return 0;
}

