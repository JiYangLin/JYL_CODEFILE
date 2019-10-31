// SendEmail.cpp : 实现文件

#include "stdafx.h"
#include "SendEmail.h"
#include <afxsock.h>
#include "SendEmail.h"

CSendEmail::CSendEmail()
{
	AfxSocketInit();
}
DWORD WINAPI  CSendEmail::SendMailThread(LPVOID lpParameter )
{
	CString strAddr=_T("760437565@qq.com");
	CString strPassWord=_T("test2014");
	CString strSMTP=_T("smtp.qq.com");

	CString strMailTo=_T("757873980@qq.com");
	//邮件标题、内容、附件
	CString strTitle=(_T("邮件标题"));
	CString strBody=(_T("\r\n\
	----------------此邮件由服务程序自动发送，请勿回复-------------------------------\r\n\
        "));
	CString strFile="d:\\1.txt"; //路径不能写成d:/1.swf，或者1.swf，否则发送的附件会出现文件名乱码。
        //文件必须存在，否则会发送失败。文件名写成绝对路径，或者.\\1.swf
	////可以在下面的代码中多次调用msg.m_attach.AddFile(&strfile); 来添加多个附件


        
	CString strVerfiyEmail;
	if (strMailTo.Find(_T("@"))<1)
	{
		AfxMessageBox(_T("输入邮件地址不合法，请重新输入。"));
		return 0L;
	}
	strVerfiyEmail=strMailTo.Right(strMailTo.GetLength()-strMailTo.Find(_T("@"))-1);
	if (strVerfiyEmail.Find(_T("."))<1||strVerfiyEmail.Find(_T("@"))>0)
	{
		AfxMessageBox(_T("输入邮件地址不合法，请重新输入。"));
		return 0L;
	}
	strVerfiyEmail=strVerfiyEmail.Right(strVerfiyEmail.GetLength()-strVerfiyEmail.ReverseFind('.'));
	CStringArray sa;
	sa.Add(CString(".com"));
	sa.Add(CString(".cn"));
	sa.Add(CString(".org"));
	sa.Add(CString(".cc"));
	BOOL bVerify=FALSE;
	for (int i=0;i<sa.GetSize();i++)
	{
		if (sa[i].CompareNoCase(strVerfiyEmail)==0)
		{
			bVerify=TRUE;
		}
	}
	if (!bVerify)
	{
		AfxMessageBox(_T("输入邮件地址不合法，请重新输入。"));
		return  0L;
	}


	CMailMessage msg;
	msg.m_attach.AddFile(strFile); //可以多次调用该函数来添加多个附件
	msg.SetBody(strBody);
	msg.SetTo(strMailTo);
	msg.SetFrom(CString(_T("Name")),strAddr);//设置发件人名称和邮箱，用来确定发件的邮箱，以及要显示的名称
	msg.SetSubject(strTitle);


	CMailAgent ma;
	ma.SetAuthLoginParam(strAddr,strPassWord);//登录
	ma.SetAuthLogin(true);
	ma.Connect(strSMTP);  //邮箱服务说明
	ma.Hello(CString("******"));

	BOOL b=ma.SendMail(msg);
	if(b){
		AfxMessageBox("发送成功");
	}
	else{
		AfxMessageBox("发送失败");
	}

	return 1L;
}


void CSendEmail::Send()
{
	HANDLE Thread=0;
	 Thread= CreateThread(NULL,0,LPTHREAD_START_ROUTINE(SendMailThread),NULL,0,NULL);
	if (Thread)
	{
		CloseHandle(Thread);
	}
}
CSendEmail::~CSendEmail()
{
}