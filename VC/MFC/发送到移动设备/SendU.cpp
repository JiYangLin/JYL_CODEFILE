
#include "stdafx.h"
#include "SendU.h"


CSendU::CSendU()
{

}

CSendU::~CSendU()
{
}

TCHAR   szMoveDiskName[33];	 ////存储USB盘符名,从下标1开始存储，下标0存储类型


TCHAR chFirstDriveFromMask(ULONG unitmask)
{
	TCHAR i;
	for (i = 0; i < 26; ++i)  
	{
		if (unitmask & 0x1) 
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}
BOOL GetDisksProperty(HANDLE hDevice, PSTORAGE_DEVICE_DESCRIPTOR pDevDesc)
{	
	STORAGE_PROPERTY_QUERY	Query;	// input param for query
	DWORD dwOutBytes;				// IOCTL output length
	BOOL bResult;					// IOCTL return val

	// specify the query type
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	// Query using IOCTL_STORAGE_QUERY_PROPERTY 
	bResult = ::DeviceIoControl(hDevice,			// device handle
		IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
		&Query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
		pDevDesc, pDevDesc->Size,				// output data buffer
		&dwOutBytes,							// out's length
		(LPOVERLAPPED)NULL);
	return bResult;
}
void ReInitUSB_Disk_Letter() //初始化U盘的盘符，放到szMoveDiskName
{
	int k = 0;
	DWORD			MaxDriveSet, CurDriveSet;
	DWORD			drive, drivetype;
	TCHAR			szBuf[300];

	TCHAR	szDrvName[33];
	HANDLE			hDevice;
	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;

	for(k=0; k<32; k++)
	{
		szMoveDiskName[k] = '\0';
	}
	k = 1;		
	// Get available drives we can monitor
	MaxDriveSet = CurDriveSet = 0;

	MaxDriveSet = GetLogicalDrives();
	CurDriveSet = MaxDriveSet;
	for ( drive = 0; drive < 32; ++drive )  
	{
		if ( MaxDriveSet & (1 << drive) )  
		{
			DWORD temp = 1<<drive;
			_stprintf( szDrvName, _T("%c:\\"), 'A'+drive );
			switch ( GetDriveType( szDrvName ) )  
			{
			case 0:					// The drive type cannot be determined.
			case 1:					// The root directory does not exist.
				drivetype = DRVUNKNOWN;
				break;
			case DRIVE_REMOVABLE:	// The drive can be removed from the drive.
				drivetype = DRVREMOVE;
				szMoveDiskName[k] = chFirstDriveFromMask(temp);
				szMoveDiskName[0]=k;
				k++;
				break;
			case DRIVE_CDROM:		// The drive is a CD-ROM drive.
				break;
			case DRIVE_FIXED:		// The disk cannot be removed from the drive.
				drivetype = DRVFIXED;
				_stprintf(szBuf, _T("\\\\?\\%c:"), _T('A')+drive);
				hDevice = CreateFile(szBuf, GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

				if (hDevice != INVALID_HANDLE_VALUE)
				{

					pDevDesc = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];

					pDevDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

					if(GetDisksProperty(hDevice, pDevDesc))
					{
						if(pDevDesc->BusType == BusTypeUsb)
						{
							szMoveDiskName[k] = chFirstDriveFromMask(temp);
							szMoveDiskName[0]=k;
							k++;
						}
					}

					delete pDevDesc;
					CloseHandle(hDevice);
				}

				break;
			case DRIVE_REMOTE:		// The drive is a remote (network) drive.
				drivetype = DRVREMOTE;
				szMoveDiskName[k] = chFirstDriveFromMask(temp);
				szMoveDiskName[0]=k;
				k++;
				break;
			case DRIVE_RAMDISK:		// The drive is a RAM disk.
				drivetype = DRVRAM;
				break;
			}
		}
	}
}

void CSendU::Send(CString strFilePath, CString pathName,CString fileName) //文件全路径，U盘中的文件夹名，发送到U盘后的文件名
{
	ReInitUSB_Disk_Letter();

	if (szMoveDiskName[1]!=NULL) //给第一个U盘发送文件
	{
		CString strSend=(CString)szMoveDiskName[1];
		strSend+=pathName;
		BOOL FileExitType= PathFileExists(strSend);  
		if (!FileExitType)
		{
			BOOL CreateType=CreateDirectory(strSend,NULL);   
			if (!CreateType)
			{
				CString strTip;
				strTip=(CString)_T("移动硬盘")+"("+(CString)szMoveDiskName[1]+_T(")")+_T("已经满或者有权限设置,不能发送");
				AfxMessageBox(strTip);
			}
		}


		CString strSendfile=strSend+_T("\\")+fileName;

		BOOL copyType= CopyFile(strFilePath,strSendfile,FALSE);
		if (!copyType)
		{
			CString strTip;
			strTip=(CString)_T("移动硬盘")+"("+(CString)szMoveDiskName[1]+_T(")")+_T("已经满或者有权限设置,不能发送");
			AfxMessageBox(strTip);
		}
	}
}