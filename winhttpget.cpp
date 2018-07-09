#include <windows.h>
#include "winhttpget.h"
#include <memory>

std::string winhttpget(LPCWSTR host, LPCWSTR object, bool isHTTPS) {
	std::string r;

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
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
		hConnect = WinHttpConnect(hSession, host,
			isHTTPS ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT, 0);

	// Create an HTTP request handle.
	if (hConnect)
		hRequest = WinHttpOpenRequest(hConnect, L"GET", object,
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			isHTTPS ? WINHTTP_FLAG_SECURE : 0);

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
			std::unique_ptr<char[]> pszOutBuffer{ new char[dwSize + 1] };
			// Read the data.
			ZeroMemory(pszOutBuffer.get(), dwSize + 1);

			if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer.get(),
				dwSize, &dwDownloaded))
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			else {
				r += pszOutBuffer.get();
			}

			// Free the memory allocated to the buffer automatically.
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