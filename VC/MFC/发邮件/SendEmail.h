#pragma once

//============================================================
//		附件名称管理，本类不包括编码 (Encode) 部分，编码
// (Base64 Encode) 可以使用 class CBase64Buffer 进行。
// 说明: Title 功能未添加，发送 title 使用 FileName
class CAttachment  
{
public:
	// 获取附件个数
	int GetCount() const
	{
		return m_attach.GetSize();
	}

	// 取得第 nIdx 个附件的文件全名(包含路径),
	// 以供外部编码部分获取文件路径
	CString GetFileFullName(int nIdx) const
	{
		ASSERT( (nIdx>=0) && (nIdx<=m_attach.GetUpperBound()));
		CString tmp = m_attach.GetAt(nIdx);

		return tmp;
	}

	// 获取第 nIdx 个附件的文件名称(不包含路径),
	// 用来填充 Mime 编码中的 name (本类中称作 Title，未实现)
	// 和 Mime 编码中的 filename 部分

	// 使用时，应检查 CString ret.IsEmpty()
	CString GetFileName(int nIdx) const
	{
		ASSERT( (nIdx>=0) && (nIdx<=m_attach.GetUpperBound()));
		CString tmp = m_attach.GetAt(nIdx);

		int nPos = tmp.ReverseFind('\\');
		if (nPos<0)
		{
			tmp.Empty();
			return tmp;
		}

		tmp = tmp.Right(tmp.GetLength()-nPos-1);
		return tmp;
	}

	// 删除附件数组中的 sFile 文件路径，如果数组中不
	// 包含该文件，则返回 false
	bool DelFile(CString & sFile)
	{
		for (int i=0;i<m_attach.GetSize();i++)
			if (sFile.CompareNoCase(LPCTSTR(m_attach[i]))==0)
			{
				m_attach.RemoveAt(i);
				return true;
			}

			return false;
	}

	// 添加附件文件到数组中，建议：sFile 包括文件的全部路径!!!
	int AddFile(CString & sFile)
	{
		ASSERT(sFile.GetLength());

		return m_attach.Add(sFile);
	}

	// 清除数组中的所有附件文件
	void Clear()
	{
		m_attach.RemoveAll();
	}

	// 构造/折构函数
	CAttachment()
	{
		m_attach.RemoveAll();
	}
	virtual ~CAttachment()
	{
		m_attach.RemoveAll();
	}

protected:

	// 附件文件保存数组
	CStringArray m_attach;
};


//============================================================
//		保存并生成传送主体信息，调用外部编码类 CBase64Buffer
// 进行编码，并使用 CAttachment 维护附件列表
class CMailMessage  
{
public:
	bool NeedEncode(CString & s)
	{
		if (m_charset.GetLength()<=0)
			return false;

		TCHAR ch;

		if (s.GetAt(0) & 0x80)
			return true;

		for (int i=1;i<s.GetLength()-1;i++)
		{
			ch = s.GetAt(i);

			if (ch & 0x80)
			{
				return true;
			}
			else
			{
				if (ch==_T('.'))
				{
					if (s.GetAt(i-1)==_T('\n') &&
						s.GetAt(i+1)==_T('\r'))
					{
						return true;
					}
				}
			}
		}

		if (s.GetAt(s.GetLength()-1) & 0x80)
			return true;

		return false;
	}
	CString GetMailHeader() const
	{
		CString tmpHeader, tmp, sCRLF;
		sCRLF = _T("\r\n");

		ASSERT(!m_fromEmail.IsEmpty());
		ASSERT(!m_to.IsEmpty());

		// Date
		tmpHeader = _T("Date: ");
		tmpHeader += GetMailDate();
		tmpHeader += sCRLF;

		// From
		if (m_fromName.IsEmpty())
			tmp.Format(_T("From: %s\r\n"), LPCTSTR(m_fromEmail));
		else
			tmp.Format(_T("From: \"%s\"<%s>\r\n"), 
			LPCTSTR(m_fromName), LPCTSTR(m_fromEmail));

		tmpHeader += tmp;

		// To
		tmp.Format(_T("To: %s\r\n"), LPCTSTR(m_to));	

		tmpHeader += tmp;


		return tmpHeader;
	}
	CString GetBoundary() const
	{
		return m_boundary;
	}
	// 设置 Content-Type
	void SetContentType(CString & contentType)
	{
		m_contentType = contentType;
	}

	// 设置 Content-Transfer-Encoding
	// 这个版本只适用于 Base64
	// void SetContentEncoding(CString & contentEncoding);

	// 设置信件主题
	void SetSubject(CString & subject)
	{
		m_subject = subject;
	}

	// 设置收件人
	void SetTo(CString & to)
	{
		m_to = to;
	}

	// 设置发送人
	void SetFrom(CString & fromName, CString & fromEmail)
	{
		m_fromName = fromName;
		m_fromEmail = fromEmail;
	}

	// 设置 charset (可选)
	void SetCharSet(CString & charset)
	{
		m_charset = charset;
	}

	// 设置邮件主体内容
	void SetBody(CString & body)
	{
		m_body = body;
	}

	// 获取 Charset
	CString GetCharSet() const
	{
		return m_charset;
	}

	// 获取 Content-Transfer-Encoding
	CString GetContentEncoding() const
	{
		return m_contentEncoding;
	}

	// 获取 Content-Type
	CString GetContentType() const
	{
		return m_contentType;
	}

	// 获取 X-mailer
	CString GetXMailer()const
	{
		return m_xMailer;
	}

	// 获取 收件人
	CString GetTo() const
	{
		return m_to;
	}

	// 主题
	CString GetSubject() const
	{
		return m_subject;
	}

	// 发件人姓名
	CString GetFromName()const
	{
		return m_fromName;
	}

	// 法件人地址
	CString GetFromEmail() const
	{
		return m_fromEmail;
	}

	// 邮件内容主体
	CString GetBody() const
	{
		return m_body;
	}

	// 构造/析构
	CMailMessage()
	{
		m_xMailer = _T("ZJMailer 0.1a");
		m_contentType = _T("text/plain");
		m_contentEncoding = _T("Base64");	// Please do not modify!!!
		m_boundary = _T("==ZJMailer_V001_codez==");

		m_attach.Clear();
	}
	virtual ~CMailMessage()
	{
		m_attach.Clear();
	}

	// 附件管理
	CAttachment m_attach;	// Email Attachment Manager

protected:
	CString GetMailDate() const
	{
		CString tmpDate, tmp;

		SYSTEMTIME st;
		TCHAR * szWeekOfDay[7]={_T("Sun"), _T("Mon"),
			_T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat")};

		TCHAR * szMonth[13]={NULL, _T("Jan"), _T("Feb"),
			_T("Mar"), _T("Apr"), _T("May"), _T("Jun"),
			_T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"),
			_T("Nov"), _T("Dec")
		};

		GetLocalTime(&st);

		if (st.wDayOfWeek<0 || st.wDayOfWeek>6)
			tmpDate = _T("???");
		else
			tmpDate = szWeekOfDay[st.wDayOfWeek];

		tmpDate += _T(", ");

		tmp.Format(_T("%d "), st.wDay);
		tmpDate += tmp;

		if (st.wMonth<1 || st.wMonth>12)
			tmpDate += _T("???");
		else
			tmpDate += szMonth[st.wMonth];

		tmp.Format(_T(" %d %d:%d:%d"), st.wYear, 
			st.wHour, st.wMinute, st.wSecond);

		tmpDate += tmp;

		return tmpDate;
	}
	CString m_boundary;
	// <BODY> FROM:"m_fromName"<m_fromEmail>
	// <CMD> MAIL FROM:<m_fromEmail>
	CString m_fromEmail;
	CString m_fromName;

	// <BODY> FROM:m_to;
	CString m_to;
	CString m_xMailer;
	CString m_subject;
	CString m_body;
	CString m_contentType;	// Content-Type
	CString m_contentEncoding; // Content-Transfer-Encoding
	CString m_charset;	// charset
};

//============================================================
#define BUFFER_SIZE	1024
class CMailSocket  
{
public:
	int SendRecv(LPTSTR szMsg, int len)
	{
		TCHAR szBuffer[1024];

		if (Send(szMsg, len)==false)
			return -1;

		if (Receive(szBuffer, 1024)==false)
			return -1;

		return GetRetCode(szBuffer);
	}

	// 发送 CRLF
	bool SendCRLF()
	{
		return Send("\r\n", 2);
	}

	// 获取响应码
	int GetRetCode(TCHAR * szBuffer) const
	{
		USES_CONVERSION;

		LPSTR szBuf = T2A(szBuffer);

		if (lstrlen(szBuffer)<3)
			return -1;

		return ((szBuf[0]-0x30)*100 +
			(szBuf[1]-0x30)*10 +
			(szBuf[2]-0x30));
	}

	// 查看是否准备好
	bool IsReady(bool & ready)
	{
		timeval timeout = {0,0};
		fd_set fd;

		FD_ZERO(&fd);
		FD_SET(m_socket, &fd);

		int ret = select(0, &fd, NULL, NULL, &timeout);
		if (ret == SOCKET_ERROR)
			return false;
		else
			return (ready = ret ? true:false), true;
	}

	// 收消息
	bool Receive(LPTSTR szBuffer, int max_len)
	{
		USES_CONVERSION;

		if (IsValidSocket() && IsConnected())
		{
			return (recv(m_socket, T2A(szBuffer), max_len, 0)!=SOCKET_ERROR) ? true:false;
		}

		return false;
	}

	// 发消息
	bool Send(LPTSTR szMsg, int len)
	{
		USES_CONVERSION;

		if (IsValidSocket() && IsConnected())
		{
			return (send(m_socket, T2A(szMsg), len, 0)!=SOCKET_ERROR) ? true:false;
		}

		return false;
	}

	// 连接
	bool Connect(LPTSTR szHost, int nPort)
	{
		USES_CONVERSION;

		ASSERT(szHost!=NULL);

		SOCKADDR_IN server;
		memset(&server, 0, sizeof(SOCKADDR_IN));

		LPSTR lpAddr = T2A(szHost);

		if (IsValidSocket() && (m_bConnected==false))
		{
			server.sin_family = AF_INET;
			server.sin_port = htons((u_short)nPort);
			server.sin_addr.s_addr = inet_addr(lpAddr);

			if (server.sin_addr.s_addr==INADDR_NONE)
			{
				LPHOSTENT lphost;
				lphost = gethostbyname(lpAddr);
				if (lphost==NULL)
				{
					Close();
					return false;
				}

				server.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
			}

			if (connect(m_socket, (SOCKADDR *)&server,
				sizeof(server))==SOCKET_ERROR)
			{
				Close();
				return false;
			}

			m_bConnected = true;

			return true;
		}

		return false;
	}

	// 查看连接是否有效
	bool IsConnected() const
	{
		return m_bConnected;
	}

	// 关闭连接
	void Close()
	{
		if (IsValidSocket())
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			m_bConnected = false;
		}
	}

	// 创建 socket 对象
	bool Create()
	{
		Close();

		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		return IsValidSocket();
	}

	// 构造/析构
	CMailSocket()
	{
		m_socket = INVALID_SOCKET;
		m_bConnected = false;
	}
	virtual ~CMailSocket()
	{
		Close();
	}

protected:
	// 查看 socket 对象是否有效
	bool IsValidSocket()
	{
		return (m_socket==INVALID_SOCKET)?false:true;
	}

	// 连接状态
	bool m_bConnected;

	// socket 对象
	SOCKET m_socket;
};

//============================================================
//		CBase64Buffer 基类，处理基本的数据操作。
// 自动管理缓冲区，包括自动分配，自动释放。
class CTextBuffer  
{
public:
	// 获取缓冲区字符长度
	int GetBufferLength() const
	{
		return m_innerLength;
	}

	// 分配缓冲区
	bool AllocBuffer(int size)
	{
		if (size<=0)
			return false;

		char * tmpBuffer = new char[size+1];

		if (tmpBuffer)
		{
			ReleaseBuffer();
			m_innerBuffer = tmpBuffer;
			m_innerLength = size;

			return true;

		}

		return false;	
	}

	// 获取缓冲区地址
	char * GetBuffer()
	{
		return m_innerBuffer;
	}

	// 释放缓冲区
	void ReleaseBuffer()
	{
		if (m_innerBuffer)
		{
			delete []m_innerBuffer;
			m_innerBuffer = NULL;
			m_innerLength = 0;
		}
	}

	// 设置缓冲区字符
	bool SetBuffer(TCHAR * text)
	{
		USES_CONVERSION;

		int len = _tcslen(text);

		if (AllocBuffer(len))
		{
			lstrcpy(m_innerBuffer, T2A(text));

			return true;
		}

		return false;
	}

	// 构造/析构
	CTextBuffer(TCHAR * text)
	{
		USES_CONVERSION;

		m_innerBuffer = NULL;
		m_innerLength = 0;

		SetBuffer(T2A(text));
	}

	CTextBuffer()
	{
		m_innerBuffer = NULL;
		m_innerLength = 0;
	}
	virtual ~CTextBuffer()
	{
		ReleaseBuffer();
	}

	// 纯虚函数，加码/解码 接口
	// 由派生类实现
	virtual bool Encode() = 0;
	virtual bool Decode() = 0;

protected:
	// 缓冲区长度
	int m_innerLength;

	// 缓冲区指针
	char * m_innerBuffer;
};

//============================================================
//		由 CTextBuffer 基类派生，实现了接口 Decode/Encode
// 用来实现 Base64 编码
class CBase64Buffer : public CTextBuffer  
{
public:
	virtual bool Encode()
	{
		char Base64Tab[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		int tmpLen, tmpLoop, tmpMod, tmpPad,
			tmpIdxSrc, tmpIdxObj;

		char * tmpBuf = NULL, tmpIdx;

		tmpLen = m_innerLength;

		if (tmpLen<=0)
			return false;

		// now, calculate Encoded Buffer param
		__asm{
			// save used register value
			push eax
				push ecx
				push edx

				// initialize the div number
				xor edx,edx
				mov eax, tmpLen

				// set dived number
				mov ecx, 3
				div ecx

				mov tmpLoop, eax
				mov tmpMod, edx

				// pop up
				pop edx
				pop ecx
				pop eax
		}

		switch(tmpMod)
		{
		case 0:
			tmpPad = 0; 
			tmpLen = 0;
			break;
		case 1:
			tmpPad = 2;
			tmpLen = 4;
			break;
		case 2:
			tmpPad = 1;
			tmpLen = 4;
			break;
		}

		tmpLen += (tmpLoop<<2);	// tmpLoop * 4;

		tmpBuf = new char[tmpLen+1];

		if (!tmpBuf)
			return false;

		tmpBuf[tmpLen]=0;

		for (int i=0;i<tmpLoop;i++)
		{
			tmpIdxObj = i<<2;			// i*4;
			tmpIdxSrc = tmpIdxObj-i;	// i*3;

			for (int j=0;j<4;j++)
			{
				tmpIdx = EncodeChar(tmpIdxSrc, j);
				tmpBuf[tmpIdxObj+j]=Base64Tab[tmpIdx];
			}
		}

		tmpIdxObj = tmpLoop<<2;	// tmpLoop * 4;
		tmpIdxSrc = tmpIdxObj - tmpLoop;	// tmpLoop * 3;

		switch(tmpMod)
		{
		case 1:
			tmpIdx = EncodeChar(tmpIdxSrc,0);
			tmpBuf[tmpIdxObj]=Base64Tab[tmpIdx];

			tmpIdx = (m_innerBuffer[tmpIdxSrc]<<4) & 0x30;
			tmpBuf[tmpIdxObj+1]=Base64Tab[tmpIdx];

			tmpBuf[tmpIdxObj+2]='=';
			tmpBuf[tmpIdxObj+3]='=';

			break;

		case 2:
			tmpIdx = EncodeChar(tmpIdxSrc,0);
			tmpBuf[tmpIdxObj]=Base64Tab[tmpIdx];

			tmpIdx = EncodeChar(tmpIdxSrc,1);
			tmpBuf[tmpIdxObj+1]=Base64Tab[tmpIdx];

			tmpIdx = (m_innerBuffer[tmpIdxSrc+1]<<2) & 0x3c;
			tmpBuf[tmpIdxObj+2]=Base64Tab[tmpIdx];

			tmpBuf[tmpIdxObj+3]='=';
			break;
		}

		delete []m_innerBuffer;

		m_innerBuffer = tmpBuf;
		m_innerLength = tmpLen;

		return true;
	}
	virtual bool Decode()
	{
		int tmpLen, tmpLoop, tmpIdxSrc, tmpIdxObj;
		BYTE szBt[4];

		if (m_innerLength<=0)
			return false;

		tmpLoop = m_innerLength>>2;		// m_innerLength / 4;
		tmpLen = (tmpLoop<<2) - tmpLoop;	// tmpLoop * 3;

		if (m_innerBuffer[m_innerLength-1]=='=')
		{
			tmpLen--;

			if (m_innerBuffer[m_innerLength-2]=='=')
				tmpLen--;
		}

		char * tmpBuf = new char[tmpLen+1];
		tmpBuf[tmpLen]=0;

		if (!tmpBuf)
			return false;

		for (int i=0;i<tmpLoop;i++)
		{
			tmpIdxSrc = i<<2;
			tmpIdxObj = tmpIdxSrc - i;

			for (int j=0;j<4;j++)
				szBt[j] = FindTab(m_innerBuffer[tmpIdxSrc+j]);

			if (szBt[0]==0xff || szBt[0]==0x80 ||
				szBt[1]==0xff || szBt[1]==0x80 ||
				szBt[2]==0xff || szBt[3]==0xff)
			{
				delete []tmpBuf;

				return false;
			}

			tmpBuf[tmpIdxObj] = ((szBt[0]<<2) & 0xfc) |
				((szBt[1]>>4) & 0x03);

			if (szBt[2]!=0x80)
			{
				tmpBuf[tmpIdxObj+1] = ((szBt[1]<<4) & 0xf0) |
					((szBt[2]>>2) & 0x0f);
			}
			else
			{
				tmpBuf[tmpIdxObj+1] = (szBt[1]<<4) & 0xf0;

				delete []m_innerBuffer;
				m_innerBuffer = tmpBuf;
				m_innerLength = tmpLen;

				return true;
			}

			if (szBt[3]!=0x80)
			{
				tmpBuf[tmpIdxObj+2] = ((szBt[2]<<6) & 0xc0) |
					(szBt[3] & 0x3f);
			}
			else
			{
				tmpBuf[tmpIdxObj+2] = (szBt[2]<<6) & 0xc0;

				delete []m_innerBuffer;
				m_innerBuffer = tmpBuf;
				m_innerLength = tmpLen;

				return true;
			}
		}

		delete []m_innerBuffer;
		m_innerBuffer = tmpBuf;
		m_innerLength = tmpLen;

		return true;
	}

	CBase64Buffer(TCHAR * text){}
	CBase64Buffer(){}
	virtual ~CBase64Buffer(){}

protected:
	BYTE FindTab(char ch)
	{
		char Base64Tab[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
		BYTE ret;

		if ( (ch>='A') && (ch<='Z') )
		{
			ret = ch-'A';
		}
		else if ( (ch>='a') && (ch<='z') )
		{
			ret = ch - 'a';
			ret += 26;
		}
		else if ( (ch>='0') && (ch<='9') )
		{
			ret = ch - '0';
			ret += 52;
		}
		else
		{
			switch(ch)
			{
			case '+':
				ret = 62;
				break;

			case '/':
				ret = 63;
				break;

			case '=':
				ret = 0x80;	// user defined code
				break;

			default:
				ret = 0xff; // user defined code
			}
		}

		return ret;
	}
	BYTE EncodeChar(int idx, int subIdx)
	{
		BYTE ret;

		switch(subIdx)
		{
		case 0:
			ret = (m_innerBuffer[idx]>>2) & 0x3f;
			break;
		case 1:
			ret = ((m_innerBuffer[idx]<<4) & 0x30) |
				((m_innerBuffer[idx+1]>>4) & 0x0f);
			break;
		case 2:
			ret = ((m_innerBuffer[idx+1]<<2) & 0x3c) |
				((m_innerBuffer[idx+2]>>6) &0x03);
			break;
		case 3:
			ret = m_innerBuffer[idx+2] & 0x3f;
		}

		return ret;
	}
};
#define	MAX_LINE_COUNT	76

//============================================================
class CMailAgent  
{
public:
	bool SendMailText(CMailMessage &msg)
	{
		CString tmp, tmp1;

		bool bNeedEncode = msg.NeedEncode(msg.GetBody());

		if (msg.GetContentType().GetLength()==0)
		{
			tmp = _T("Content-Type: text/plain; ");
		}
		else
		{
			tmp.Format(_T("Content-Type: %s; "),
				LPCTSTR(msg.GetContentType()));
		}

		if (msg.GetCharSet().GetLength())
		{
			tmp1.Format(_T("charset=\"%s\""),
				LPCTSTR(msg.GetCharSet()));

			tmp += tmp1;
		}

		tmp += _T("\r\n");

		if (bNeedEncode)
		{
			if (msg.GetContentEncoding().GetLength()==0)
			{
				tmp1 = _T("Content-Transfer-Encoding: Base64\r\n");
				//			encode = 2; // Base64
			}
			else
			{
				tmp1.Format(_T("Content-Transfer-Encoding: %s\r\n"),
					LPCTSTR(msg.GetContentEncoding()));

				// set encode
			}
		}
		else
		{
			tmp1 = _T("Content-Transfer-Encoding: 7bit\r\n");
		}

		tmp += tmp1;

		if (m_mail.Send((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
			return false;

		// Begin to Send body
		// prepare
		if (m_mail.SendCRLF()==false)
			return false;

		LPSTR p;
		int len;
		CBase64Buffer buf;


		buf.SetBuffer((LPTSTR)(LPCTSTR(msg.GetBody())));

		if (bNeedEncode)
		{
			if (buf.Encode()==false)
				return false;
		}

		len=buf.GetBufferLength();

		p = buf.GetBuffer();
		if (bNeedEncode)
		{
			while(len>0)
			{
				if (len>MAX_LINE_COUNT)
				{
					if (m_mail.Send(p, MAX_LINE_COUNT)==false)
						return false;

					p += MAX_LINE_COUNT;
				}
				else
				{
					if (m_mail.Send(p, len)==false)
						return false;
				}

				if (m_mail.SendCRLF()==false)
					return false;

				len -= MAX_LINE_COUNT;
			}
		}
		else
		{
			if (m_mail.Send(buf.GetBuffer(), buf.GetBufferLength())==false)
				return false;
		}

		return true;
	}
	// 连接完毕，Hello 完毕，验证完毕
	// 请先调用 Connect & Hello

	// 开始发送信息主体：MAIL FROM:...
	bool SendMail(CMailMessage & msg)
	{
		if (m_mail.IsConnected()==false)
			return false;	// invalid connection

		if (msg.GetTo().IsEmpty())
			return false;	// no send destination

		// begin to send data!
		CString tmp, tmp1;

		// MAIL FROM:<email@from.com>
		if (msg.GetFromEmail().IsEmpty())
			tmp = _T("MAIL FROM:<>\r\n");
		else
			tmp.Format(_T("MAIL FROM:<%s>\r\n"),
			LPCTSTR(msg.GetFromEmail()));

		if (m_mail.SendRecv( (LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=250)
			return false;

		// RCPT TO:<email@to.com>
		// Begin loop to [RCPT TO] command
		tmp.Format(_T("RCPT TO:<%s>\r\n"),LPCTSTR(msg.GetTo()));

		if (m_mail.SendRecv( (LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=250)
			return false;

		// End loop to [RCPT TO] command

		// DATA start
		tmp.Format(_T("DATA\r\n"));

		if (m_mail.SendRecv( (LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=354)
			return false;

		// header
		tmp = msg.GetMailHeader();
		if (m_mail.Send( (LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
			return false;

		// Subject
		tmp = _T("Subject: ");

		if (m_mail.Send( (LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
			return false;

		tmp = msg.GetSubject();

		if (msg.NeedEncode(tmp))
		{
			// Encode
			CBase64Buffer buf;

			if (buf.SetBuffer((LPTSTR)(LPCTSTR(tmp)))==false)
				return false;

			if (buf.Encode()==false)
				return false;

			int len = buf.GetBufferLength();
			LPSTR p = buf.GetBuffer();

			CString sHead, sTail = _T("?=");;

			sHead.Format(_T("=?%s?B?"), LPCTSTR(msg.GetCharSet()));

			if (m_mail.Send((LPTSTR)(LPCTSTR(sHead)), sHead.GetLength())==false)
				return false;

			if (m_mail.Send(buf.GetBuffer(), buf.GetBufferLength())==false)
				return false;

			if (m_mail.Send((LPTSTR)(LPCTSTR(sTail)), sTail.GetLength())==false)
				return false;		

			if (m_mail.SendCRLF()==false)
				return false;
		}
		else
		{
			if (m_mail.Send((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
				return false;

			if (m_mail.SendCRLF()==false)
				return false;
		}

		// X-mailer & Mime-Version
		tmp.Format(_T("X-mailer: %s\r\nMime-Version: 1.0\r\n"),
			LPCTSTR(msg.GetXMailer()));

		if (m_mail.Send( (LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
			return false;

		// write others content
		if (SendMailContent(msg)==false)
			return false;

		// End Msg
		tmp = _T("\r\n.\r\n");

		if (m_mail.SendRecv((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=250)
			return false;

		tmp = _T("QUIT\r\n");
		if (m_mail.SendRecv((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=221)
			return false;

		m_mail.Close();

		return true;
	}

	// 用于验证：发送 BASE64 编码的用户名称和用户密码
	int SendBase64Msg(CString & msg)
	{
		TCHAR szBuffer[BUFFER_SIZE];
		CBase64Buffer buf;

		buf.SetBuffer(LPTSTR(LPCTSTR(msg)));
		buf.Encode();

		if (m_mail.Send(buf.GetBuffer(), buf.GetBufferLength())==false)
			return -1;

		m_mail.SendCRLF();

		if (m_mail.Receive(szBuffer, BUFFER_SIZE)==false)
			return -1;

		return m_mail.GetRetCode(szBuffer);
	}

	// 断开连接
	void DisConnect()
	{
		m_mail.Close();
	}

	// 传送 HELO 或者 EHLO & AUTH LOGIN
	// 请先调用 Connect
	bool Hello(CString & who)
	{
		CString tmp;

		if (m_authLogin)
		{
			tmp = _T("EHLO ");
		}
		else
		{
			tmp = _T("HELO ");
		}

		tmp = tmp+who+_T("\r\n");

		if (m_mail.SendRecv((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=250)
			return false;

		if (m_authLogin)
		{
			tmp = _T("AUTH LOGIN\r\n");

			if (m_mail.SendRecv((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())!=334)
				return false;

			if (SendBase64Msg(m_loginName)!=334)
				return false;

			if (SendBase64Msg(m_loginPass)!=235)
				return false;
		}

		return true;
	}

	// 设置是否使用用户验证旗帜
	void SetAuthLogin(bool login)
	{
		m_authLogin = login;
	}

	// 设置用户验证数据
	// 注：是否开启用户验证，请使用 SetAuthLogin 函数
	void SetAuthLoginParam(CString & name, CString & pass)
	{
		if (name.IsEmpty() || pass.IsEmpty())
		{
			return;
		}

		m_loginName = name;
		m_loginPass = pass;
	}

	// 获取用户验证旗帜
	bool GetAuthLogin() const
	{
		return m_authLogin;
	}

	// 连接 smtp 主机服务器
	bool Connect(LPCTSTR szHost, int nPort=25)
	{
		TCHAR szBuffer[BUFFER_SIZE];

		if (m_mail.IsConnected())
			m_mail.Close();

		if (m_mail.Create()==false)
		{
			TRACE0("CMailSocket create error!\n");
			return false;
		}

		if (m_mail.Connect((LPTSTR)szHost, nPort)==false)
		{
			TRACE("CMailSocket connect error!\n");
			return false;
		}

		if (m_mail.Receive(szBuffer, BUFFER_SIZE)==false)
		{
			TRACE0("Received error!\n");
			return false;
		}

		if (m_mail.GetRetCode(szBuffer)!=220)
		{
			TRACE0("Invalidate Smtp server!\n");
			return false;
		}

		return true;
	}

	// 构造/析构
	CMailAgent()
	{
		m_authLogin = false;
	}
	virtual ~CMailAgent(){}

protected:
	bool SendAttachment(CString & path, CString & name, CString & boundary)
	{
		CString sCRLF(_T("\r\n"));
		CString sPreHeader(_T("--"));

		CString tmp, tmp1;

		tmp = sCRLF;
		tmp += sPreHeader;
		tmp += boundary;
		tmp += sCRLF;

		tmp1.Format(_T("Content-Type: application/octet-stream; name=\"%s\"\r\nContent-Transfer-Encoding: base64\r\nContent-Disposition: attachment; filename=\"%s\"\r\n"),
			LPCTSTR(name), LPCTSTR(name));

		tmp += tmp1;

		if (m_mail.Send((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
			return false;

		if (m_mail.SendCRLF()==false)
			return false;

		CBase64Buffer buf;

		CFile inFile;
		CFileException ex;

		if (inFile.Open(LPCTSTR(path), CFile::modeRead | CFile::shareDenyWrite, &ex)==FALSE)
		{
			return false;
		}

		int size = inFile.GetLength();
		if (buf.AllocBuffer(size)==false)
			return false;	// not enough buffer

		if (inFile.Read(buf.GetBuffer(), size)==0)
			return false;	// no read

		inFile.Close();

		if (buf.Encode()==false)
			return false;

		size = buf.GetBufferLength();
		LPSTR p = buf.GetBuffer();

		while(size>0)
		{
			if (size>MAX_LINE_COUNT)
			{
				if (m_mail.Send(p, MAX_LINE_COUNT)==false)
					return false;

				p += MAX_LINE_COUNT;
			}
			else
			{
				if (m_mail.Send(p, size)==false)
					return false;
			}

			if (m_mail.SendCRLF()==false)
				return false;

			size -= MAX_LINE_COUNT;
		}

		return true;
	}
	bool SendMailContent(CMailMessage & msg)
	{
		bool bAttachment = msg.m_attach.GetCount() ? true:false;

		//	USES_CONVERSION;	
		CString tmp, tmp1;

		if (bAttachment)
		{
			// Contain the Attachment
			// no implemetation
			CString sLogoText(_T("\r\nA multi-part message in MIME format, made by codez!\r\n"));
			CString sCRLF(_T("\r\n"));
			CString sPreHeader(_T("--"));

			if (msg.GetBoundary().GetLength()==0)
				return false;

			tmp.Format(_T("Content-Type: multipart/mixed; boundary=\"%s\"\r\n"),
				LPCTSTR(msg.GetBoundary()));

			tmp += sLogoText;

			// Insert a boundary
			tmp += sCRLF;
			tmp += sPreHeader;
			tmp += msg.GetBoundary();
			tmp += sCRLF;

			if (m_mail.Send((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength())==false)
				return false;

			// body content and text
			if (SendMailText(msg)==false)
				return false;

			for (int i=0;i<msg.m_attach.GetCount();i++)
				if (SendAttachment(msg.m_attach.GetFileFullName(i),
					msg.m_attach.GetFileName(i),
					msg.GetBoundary())==false)
				{
					return false;
				}

				tmp = sCRLF;
				tmp += sPreHeader;
				tmp += msg.GetBoundary();
				tmp += sPreHeader;
				tmp += sCRLF;

				return m_mail.Send((LPTSTR)(LPCTSTR(tmp)), tmp.GetLength());
		}
		else
		{
			// Do not contain the attachment
			// Mime header
			return SendMailText(msg);
		}
	}
	// 用户验证数据
	CString m_loginName;
	CString m_loginPass;

	// 验证旗帜
	bool m_authLogin;

	// 通讯 socket class
	CMailSocket m_mail;
};


//////////////////////////////////////////////////////
class CSendEmail
{
public:
	CSendEmail();
	virtual ~CSendEmail();
	void Send();
	static DWORD WINAPI SendMailThread(LPVOID lpParameter );
};


