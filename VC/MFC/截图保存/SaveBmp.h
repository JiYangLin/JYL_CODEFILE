#ifndef CSAVEBMP_H 
#define CSAVEBMP_H
class CSaveBmp
{
public:
	CSaveBmp();
	virtual ~CSaveBmp();
    void DlgSavebmp(HWND hWnd,CRect &Clientrect); 
	void SaveBmp(HWND hWnd,CRect &Clientrect,CString path);
private:
	int SaveBitmapToFile(HBITMAP hBitmap, LPSTR lpFileName); //hBitmap 为刚才的屏幕位图句柄

};
#endif 
