#pragma once
#include "sqlite3.h"
#include <string>
#include <vector>
using   namespace   std;

class SelectPerLineCallbackProc
{
public:
	virtual  void GetSelectDat(int count,char** pValue) = 0;
};
class Sqlite3Opt
{
	sqlite3  *m_pDB;
	SelectPerLineCallbackProc* m_pSelectPerLineCallbackProc;
private:
	static int SelectPerLineCallback(void *user,int count,char** pValue,char**pColumName)
	{
		Sqlite3Opt *pSqlite3Opt = (Sqlite3Opt*)user;
		pSqlite3Opt->ProcCallback(count,pValue);
		return 0;
	}
	void ProcCallback(int count,char** pValue)
	{
		if (NULL == m_pSelectPerLineCallbackProc) return;
		m_pSelectPerLineCallbackProc->GetSelectDat(count,pValue);
	}
public:
	Sqlite3Opt():m_pDB(NULL),m_pstmt(NULL),m_pSelectPerLineCallbackProc(NULL){}

	bool Open(const char * dbPathName,SelectPerLineCallbackProc* _pSelectPerLineCallbackProc)
	{
		m_pSelectPerLineCallbackProc = _pSelectPerLineCallbackProc;
		if(SQLITE_OK != sqlite3_open(dbPathName,&m_pDB))
		{
			m_pDB = NULL;
			return false;
		}
		return true;
	}
	void Close()
	{
		if (NULL == m_pDB) return;
		sqlite3_close(m_pDB);
	}
	void Select(const char *sql)
	{
		if (NULL == m_pDB) return ;
		char *erro;
		sqlite3_exec(m_pDB,sql,SelectPerLineCallback,this,&erro);
	}
	bool Exec(const char *sql)
	{
		if (NULL == m_pDB) return false;
		char *erro;
		if(SQLITE_OK != sqlite3_exec(m_pDB,sql,0,0,&erro)) return false;
		return true;
	}
public:
	sqlite3_stmt* m_pstmt;
	sqlite3_stmt* MassInsertBegin(int insertColumnCount,string tableName)
	{
		m_pstmt = NULL;
		if (insertColumnCount < 1 ) return NULL;
		if (NULL == m_pDB) return NULL;
		sqlite3_exec(m_pDB,"begin",0,0,0);

		string str = "insert into "+ tableName + " values(?";
		for (int i = 1 ; i < insertColumnCount;++i)
		{
			str += ",?";
		}
		str += ")";

		sqlite3_prepare_v2(m_pDB,str.c_str(),str.length(),&m_pstmt,0);
		return m_pstmt;
	}
	void MassInsertEnd(sqlite3_stmt *pStmt)
	{
		if (NULL == pStmt) return;
		if (NULL == m_pDB) return ;

		sqlite3_finalize(pStmt);
		sqlite3_exec(m_pDB,"commit",0,0,0);
		m_pstmt = NULL;
	}
};


class SqliteMapBase:public SelectPerLineCallbackProc
{
protected:
	Sqlite3Opt m_Sqlite3Opt;
	sqlite3_stmt* m_pstmt;
public:
	void StartInsert(CString dbPathName)
	{
		m_pstmt = NULL;
		DeleteFile(dbPathName);
		if(!m_Sqlite3Opt.Open(CStringA(dbPathName),this)) throw CString(_T("db创建失败"));

		char *sql = "create table temp(id int,val double)";
		if(!m_Sqlite3Opt.Exec(sql))  throw CString(_T("表temp创建失败"));

		m_pstmt = m_Sqlite3Opt.MassInsertBegin(2,"temp");
		if (NULL == m_pstmt) throw CString(_T("db操作失败"));
	}
	void EndInsert()
	{
		m_Sqlite3Opt.MassInsertEnd(m_pstmt);
		m_pstmt = NULL;
	}
	void Insert(int id,double val)
	{
		sqlite3_reset(m_pstmt);
		sqlite3_bind_int(m_pstmt,1,id);
		sqlite3_bind_double(m_pstmt,2,val);
		sqlite3_step(m_pstmt);
	}

};





class SqliteMap:public SqliteMapBase
{
	double m_RetVal;


	bool m_getInterVal;

	bool   m_ProcLeft;
	int    m_leftId;
	double m_leftVal;
	int    m_rightId;
	double m_rightVal;

	virtual  void GetSelectDat(int count,char** pValue)
	{
		if (m_getInterVal)
		{
			if (m_ProcLeft)
			{
				m_leftId = atoi(pValue[0]);
				m_leftVal = atof(pValue[1]);

			}
			else
			{
				m_rightId = atoi(pValue[0]);
				m_rightVal = atof(pValue[1]);

			}
		}
		else
		{
			m_RetVal = atof(pValue[0]);
		}
	}





public:
	//返回查找到的值（两个值相等）或最近的两个值,或者两个值都是起始或结尾点
	void GetInterVal(int findKey,int &preKey,double &preVal,int &lastKey,double &lastVal)
	{
		m_getInterVal = true;
		m_ProcLeft = true;
		char sql[100] = {0};
		sprintf_s(sql,"select id,max(val) from temp where id<=%d",findKey);
		m_Sqlite3Opt.Select(sql);

		m_ProcLeft = false;
		sprintf_s(sql,"select id,min(val) from temp where id>=%d",findKey);
		m_Sqlite3Opt.Select(sql);

		preKey = m_leftId;
		preVal = m_leftVal;
		lastKey = m_rightId;
		lastVal = m_rightVal;
	}

	//查找不到返回0
	double GetVal(int id)
	{
		m_getInterVal = false;
		m_RetVal = 0;
		char sql[100] = {0};
		sprintf_s(sql,"select val from temp where id=%d",id);

		m_Sqlite3Opt.Select(sql);
		return m_RetVal;
	}
};