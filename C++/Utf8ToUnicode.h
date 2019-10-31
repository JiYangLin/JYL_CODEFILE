#pragma once

#include <string>
using namespace std;


class Unicode_UTF8
{
	int cU8xU(wchar_t* pOut, char *pText)
	{// --------- UTF-8转Unicode，单字 ---------------- 
		char* uchar = (char *)pOut;
		unsigned char cIn = (unsigned char)pText[0];
		if (cIn<0x80) { //1000 0000
			pOut[0] = pText[0];
			return 0;
		}
		else if (cIn<0xDF) {//1101 1111
			uchar[0] = (pText[0] << 6) | (pText[1] & 0x3F);
			uchar[1] = (pText[0] >> 2) & 0x0f;
			return 1;
		}
		else if (cIn <= 0xEF) {//1110 1111
			uchar[0] = (pText[1] << 6) | (pText[2] & 0x3F);
			uchar[1] = (pText[0] << 4) | ((pText[1] >> 2) & 0x0F);
			return 2;
		}
		else if (cIn<0xF7) {//1111 0111
			uchar[0] = (pText[2] << 6) | (pText[3] & 0x3F);
			uchar[1] = (pText[1] << 4) | ((pText[2] >> 2) & 0x0F);
			uchar[2] = ((pText[0] << 2) & 0x1c) | ((pText[1] >> 4) & 0x03);
			return 3;
		}
		return 0;
	}
	
	int cUxU8(char* pOut, wchar_t* pText)
	{// ---------- Unicode转UTF-8，单字 ------------------ 
		unsigned char* pchar = (unsigned char *)pText;
		if (pText[0] <= 0x7f) {         // ASCII  0x00 ~ 0x7f 
			pOut[0] = (char)pchar[0];
			return 0;
		}
		else if (pText[0] <= 0x7ff) {  // 0x080 ~ 0x7ff 
			pOut[0] = 0xc0 | (pchar[1] << 2) | (pchar[0] >> 6);
			pOut[1] = 0x80 | (pchar[0] & 0x3f);
			return 1;
		}
		else {                      // 0x0800 ~ 0xFFFF 
			pOut[0] = 0xe0 | (pchar[1] >> 4);
			pOut[1] = 0x80 | ((pchar[1] & 0x0f) << 2) | (pchar[0] >> 6);
			pOut[2] = 0x80 | (pchar[0] & 0x3f);
			return 2;
		}
		return 0;
	}

private:
	// ------------ UTF-8转Unicode，字符串 ---------------- 
	int Utf8ToUnicode(wchar_t* pOut, char *pText, int Len) {
		int i, j;
		for (i = 0, j = 0; i<Len; i++, j++) {
			i += cU8xU(&pOut[j], &pText[i]);
		}
		pOut[j] = 0;
		return j;
	}
	// ------------ Unicode转UTF-8，字符串 ---------------- 
	int UnicodeToUtf8(char* pOut, wchar_t* pText, int Len) {
		int i, j;
		for (i = 0, j = 0; i<Len; i++, j++) {
			j += cUxU8(&pOut[j], &pText[i]);
		}
		pOut[j] = 0;
		return j;
	}





public:
	string m_strUtf8Buf;
	int    m_strUtf8Len;
	void UnicodeToUtf8(wchar_t *pUnicodesStr)
	{
		int UnicodesStrLen = lstrlenW(pUnicodesStr);
		m_strUtf8Buf.resize(UnicodesStrLen*4 + 4);
		m_strUtf8Len = UnicodeToUtf8(&m_strUtf8Buf[0], pUnicodesStr, UnicodesStrLen);
	}

public:
	wstring   m_strUnicodeStr;
	void  Utf8ToUnicode(char *pUtf8StrBuf, int Utf8StrBufLen)
	{
		m_strUnicodeStr.resize(Utf8StrBufLen *2+2);
		Utf8ToUnicode(&m_strUnicodeStr[0], pUtf8StrBuf, Utf8StrBufLen);
	}
};