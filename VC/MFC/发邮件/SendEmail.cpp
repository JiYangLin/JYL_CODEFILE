// SendEmail.cpp : ʵ���ļ�

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
	//�ʼ����⡢���ݡ�����
	CString strTitle=(_T("�ʼ�����"));
	CString strBody=(_T("\r\n\
	----------------���ʼ��ɷ�������Զ����ͣ�����ظ�-------------------------------\r\n\
        "));
	CString strFile="d:\\1.txt"; //·������д��d:/1.swf������1.swf�������͵ĸ���������ļ������롣
        //�ļ�������ڣ�����ᷢ��ʧ�ܡ��ļ���д�ɾ���·��������.\\1.swf
	////����������Ĵ����ж�ε���msg.m_attach.AddFile(&strfile); ����Ӷ������


        
	CString strVerfiyEmail;
	if (strMailTo.Find(_T("@"))<1)
	{
		AfxMessageBox(_T("�����ʼ���ַ���Ϸ������������롣"));
		return 0L;
	}
	strVerfiyEmail=strMailTo.Right(strMailTo.GetLength()-strMailTo.Find(_T("@"))-1);
	if (strVerfiyEmail.Find(_T("."))<1||strVerfiyEmail.Find(_T("@"))>0)
	{
		AfxMessageBox(_T("�����ʼ���ַ���Ϸ������������롣"));
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
		AfxMessageBox(_T("�����ʼ���ַ���Ϸ������������롣"));
		return  0L;
	}


	CMailMessage msg;
	msg.m_attach.AddFile(strFile); //���Զ�ε��øú�������Ӷ������
	msg.SetBody(strBody);
	msg.SetTo(strMailTo);
	msg.SetFrom(CString(_T("Name")),strAddr);//���÷��������ƺ����䣬����ȷ�����������䣬�Լ�Ҫ��ʾ������
	msg.SetSubject(strTitle);


	CMailAgent ma;
	ma.SetAuthLoginParam(strAddr,strPassWord);//��¼
	ma.SetAuthLogin(true);
	ma.Connect(strSMTP);  //�������˵��
	ma.Hello(CString("******"));

	BOOL b=ma.SendMail(msg);
	if(b){
		AfxMessageBox("���ͳɹ�");
	}
	else{
		AfxMessageBox("����ʧ��");
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