#pragma once
#include <winioctl.h>
#define DRVUNKNOWN		0
#define DRVFIXED		1
#define DRVREMOTE		2
#define DRVRAM			3
#define DRVCD			4
#define DRVREMOVE		5
class CSendU
{
public:
	CSendU();
	virtual ~CSendU();
	void Send(CString strFilePath, CString pathName,CString fileName);
};


