#pragma once 
#include <vector>
using namespace std;

typedef void (*PROC_SPLIT)(vector<char*> &_split,void *puser);

class  TxtReader
{
	FILE *m_pf;
	vector<char> m_lineBuf;
	vector<char*> m_split;
public:
	TxtReader()
	{
		m_pf = NULL;
	}
	~TxtReader()
	{
		close();
	}
	bool open(const char *filePathName)
	{
		close();
		if (NULL != m_pf)
		{
			fclose(m_pf);
		}
		fopen_s(&m_pf,filePathName,"r");
		if (NULL == m_pf) return false;
		fseek(m_pf,0,SEEK_END);
		int len = ftell(m_pf);
		if (len <= 0)
		{
			fclose(m_pf);
			return false;
		}
		fseek(m_pf,0,SEEK_SET);
		m_lineBuf.resize(len,0);

		return true;
	}
	void Readline(PROC_SPLIT pcallBack,void *puser,const char *split = ",")
	{
		if (NULL == m_pf) return;
		char *pBuf = &m_lineBuf[0]; 
		size_t len = m_lineBuf.size();
		while(fgets(pBuf,len,m_pf))
		{
			m_split.clear();
			SplitStr(pBuf,split);
			pcallBack(m_split,puser);
			memset(pBuf,0,len);
		}
	}
	void close()
	{
		if (NULL == m_pf) return;
		m_split.swap(vector<char*>());
		m_lineBuf.swap(vector<char>());
		fclose(m_pf);
		m_pf = NULL;
	}
private:
	void SplitStr(char *str,const char *strFind)
	{
		char *p = NULL;
		while(NULL != (p = strstr(str,strFind)))
		{
			char *pStr = str;
			m_split.push_back(pStr);
			*p = 0;
			str = ++p;
		}
		if(strlen(str) != 0)
		{
			char *pStr = str;
			m_split.push_back(pStr);
		}
	}
};