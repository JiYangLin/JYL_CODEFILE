#pragma once
#include "sqlite3.h"
#include <string>
using   namespace   std;

class Sqlite3Opt
{     
	sqlite3  *m_pDB;
private:
	static int SelectPerLineCallback(void *user,int count,char** pValue,char**pColumName)
	{
		Sqlite3Opt *pSqlite3Opt = (Sqlite3Opt*)user;
		pSqlite3Opt->SelectCallback(count,pValue);
		return 0;
	}
	virtual void SelectCallback(int count,char** pValue) = 0;

public:
	Sqlite3Opt():m_pDB(NULL),m_pstmt(NULL){}
	~Sqlite3Opt()
	{
		Close();
	}
	bool Open(const char * dbPathName)
	{
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





//class SqliteCreator:Sqlite3Opt
//{
//private:
//	sqlite3_stmt* m_pstmt;
//	Unicode_UTF8  m_UnicodeToUtf8;
//
//	virtual void SelectCallback(int count,char** pValue)
//	{
//
//	}
//
//
//public:
//	SqliteCreator()
//	{
//		m_pstmt = NULL;
//	}
//	BOOL OpenDB(wchar_t* dbPathName)
//	{
//		DeleteFile(dbPathName);
//		m_UnicodeToUtf8.UnicodeToUtf8(dbPathName);
//		if(!Open(&m_UnicodeToUtf8.m_strUtf8Buf[0]))
//		{
//			AfxMessageBox(CString(dbPathName) + _T(" 创建失败"));
//			return FALSE;
//		}
//		return TRUE;
//	}
//
//
//
//public:
//	BOOL StartInsertTable()
//	{
//		m_pstmt = NULL;
//		char *sql = "CREATE TABLE Table_Name(   \
//					ID   INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,  \
//					Name  TEXT NOT NULL)";
//
//		if(!Exec(sql))
//		{
//			AfxMessageBox(CString(_T("表创建失败")));
//			return FALSE;
//		}
//
//		int columnCount = 2;
//		m_pstmt = MassInsertBegin(columnCount,"Table_Name");
//		if (NULL == m_pstmt)
//		{
//			AfxMessageBox(CString(_T("MassInsertBegin 操作失败")));
//			return FALSE;
//		}
//
//		return TRUE;
//	}
//	void InsertRow(int ID,CString Name)
//	{
//		int pos = 1;//从1开始
//		sqlite3_reset(m_pstmt);
//		sqlite3_bind_int(m_pstmt,pos++,ID);
//		m_UnicodeToUtf8.UnicodeToUtf8(Name.GetBuffer());
//		sqlite3_bind_text(m_pstmt,pos++,&m_UnicodeToUtf8.m_strUtf8Buf[0],m_UnicodeToUtf8.m_strUtf8Len,SQLITE_STATIC);
//		sqlite3_step(m_pstmt);
//	}
//	void EndInsert()
//	{
//		MassInsertEnd(m_pstmt);
//		m_pstmt = NULL;
//	}
//};