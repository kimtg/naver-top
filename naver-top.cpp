// naver-top.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>
#include <windows.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <regex>
#include <vector>
#include <ctime>
#include <memory>
#include "winhttpget.h"

using namespace std;

string UTF8ToANSI(string s)
{
	BSTR    bstrWide;
	int     nLength;
	const char *pszCode = s.c_str();

	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pszCode, strlen(pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);

	std::unique_ptr<char[]> pszAnsi{ new char[nLength] };
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi.get(), nLength, NULL, NULL);
	SysFreeString(bstrWide);

	return string(pszAnsi.get());
}

vector<string> list_naver() {
	string s(UTF8ToANSI(winhttpget(L"www.naver.com", L"/", true)));

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

