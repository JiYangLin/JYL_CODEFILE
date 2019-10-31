/*===================================================*\
|                                                     |
|  EASY-SIZE Macros                                   |
|                                                     |
|  Copyright (c) 2001 - Marc Richarme                 |
|      devix@devix.cjb.net                            |
|      http://devix.cjb.net                           |
|                                                     |
|  License:                                           |
|                                                     |
|  You may use this code in any commersial or non-    |
|  commersial application, and you may redistribute   |
|  this file (and even modify it if you wish) as      |
|  long as you keep this notice untouched in any      |
|  version you redistribute.                          |
|                                                     |
|  Usage:                                             |
|                                                     |
|  - Insert 'DECLARE_EASYSIZE' somewhere in your      |
|    class declaration                                |
|  - Insert an easysize map in the beginning of your  |
|    class implementation (see documentation) and     |
|    outside of any function.                         |
|  - Insert 'INIT_EASYSIZE;' in your                  |
|    OnInitDialog handler.                            |
|  - Insert 'UPDATE_EASYSIZE' in your OnSize handler  |
|  - Optional: Insert 'EASYSIZE_MINSIZE(mx,my);' in   |
|    your OnSizing handler if you want to specify     |
|    a minimum size for your dialog                   |
|                                                     |
|        Check http://devix.cjb.net for the           |
|              docs and new versions                  |
|                                                     |
\*===================================================*/
/******************************************************************************
要让对话框中的控件随着对话框大小的变化进行自动调整，
是一件很头疼的事情，需要在WM_SIZE的响应函数中对所有的
控件用MoveWindow()或者SetWindowPos()设置其变化比例，极其繁琐。
EasySize是一个很好解决方案，用一些宏封装了内部的实现机制，
使用起来方便快捷。原文地址：http://www.codeproject.com/KB/dialog/easysize.aspx
使用方法如下：

1.首先将EasySize.h拷贝到工程文件夹中，然后在stdafx.h中包含该头文件：
    #include "EasySize.h"

2.在对话框类声明中添加DECLARE_EASYSIZE宏：
    class CEasySizeDemoDlg : public CDialog
    {
    DECLARE_EASYSIZE
    ...

3.在OnInitDialog()函数中添加INIT_EASYSIZE宏(注意有分号)：
    BOOL CEasySizeDemoDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();
        ...    
        INIT_EASYSIZE;
        return TRUE; // return TRUE  unless you set the focus to a control
    } 

4.响应WM_SIZE消息，在响应函数中添加UPDATE_EASYSIZE宏(注意有分号)：
    void CEasySizeDemoDlg::OnSize(UINT nType, int cx, int cy) 
    {    
        CDialog::OnSize(nType, cx, cy);
        UPDATE_EASYSIZE;
    } 

5.如果需要限制对话框有一个最小尺寸，首先在资源视图中为对话框添加上最大化最小化按钮，然后Class Wizard中选择Class Info标签，在Message fileter下拉列表中选择Window，然后再点击Message Maps标签，就出现WM_SIZING消息了，添加该消息的响应函数：
    void CEasySizeDemoDlg::OnSizing(UINT fwSide, LPRECT pRect) 
    {
        CDialog::OnSizing(fwSide, pRect);
        EASYSIZE_MINSIZE(280,250,fwSide,pRect);
    }
这样就把对话框的最小尺寸限制在(280, 250)上了

6.添加控件布局映射，放到消息映射之后，基本格式如下：
    BEGIN_EASYSIZE_MAP(class_name)
        ...
        EASYSIZE(control,left,top,right,bottom,options)
        ...
    END_EASYSIZE_MAP

class_name就是对话框类的类名

control是要设置的控件的ID

left、top、right、bottom分别指的是该控件左、上、右、下方的控件的ID，也可以为以下特殊值：
        ES_BORDER：保持与边框距离不变
        ES_KEEPSIZE：保持大小不变
                所放位置：
                         left -- 表示和right指定的控件右对齐
                         right -- 表示和left指定的控件左对齐
                         top -- 表示和bottom指定的控件上对齐
                         bottom -- 表示和top指定的控件下对齐

options可指定为0，也可指定为以下值：
        ES_HCENTER：保持宽度不变，水平位置在left和right指定的控件正中间
        ES_VCENTER：保持高度不变，垂直位置在left和right指定的控件正中间


***************************************************************************/

#ifndef __EASYSIZE_H_
#define __EASYSIZE_H_
#define ES_BORDER 0xffffffff
#define ES_KEEPSIZE 0xfffffffe
#define ES_HCENTER 0x00000001
#define ES_VCENTER 0x00000002
#define DECLARE_EASYSIZE \
void __ES__RepositionControls(BOOL bInit);\
void __ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect &rect, int clientbottomright);
#define INIT_EASYSIZE __ES__RepositionControls(TRUE); __ES__RepositionControls(FALSE)
#define UPDATE_EASYSIZE if(GetWindow(GW_CHILD)!=NULL) __ES__RepositionControls(FALSE)
#define EASYSIZE_MINSIZE(mx,my,s,r) if(r->right-r->left < mx) { if((s == WMSZ_BOTTOMLEFT)||(s == WMSZ_LEFT)||(s == WMSZ_TOPLEFT)) r->left = r->right-mx; else r->right = r->left+mx; } if(r->bottom-r->top < my) { if((s == WMSZ_TOP)||(s == WMSZ_TOPLEFT)||(s == WMSZ_TOPRIGHT)) r->top = r->bottom-my; else r->bottom = r->top+my; }
#define BEGIN_EASYSIZE_MAP(class) \
void class::__ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect &rect, int clientbottomright) {\
if(br==ES_BORDER) bottomright = clientbottomright-es_br;\
else if(br==ES_KEEPSIZE) bottomright = topleft+es_br;\
else { CRect rect2;\
pThis->GetDlgItem(br)->GetWindowRect(rect2); pThis->ScreenToClient(rect2);\
bottomright = (bBottom?rect2.top:rect2.left) - es_br;}}\
void class::__ES__RepositionControls(BOOL bInit) { CRect rect,rect2,client; GetClientRect(client);
#define END_EASYSIZE_MAP Invalidate(); UpdateWindow(); }
#define EASYSIZE(id,l,t,r,b,o) \
static int id##_es_l, id##_es_t, id##_es_r, id##_es_b;\
if(bInit) {\
GetDlgItem(id)->GetWindowRect(rect); ScreenToClient(rect);\
if(o & ES_HCENTER) id##_es_l = rect.Width()/2; else {\
if(l==ES_BORDER) id##_es_l = rect.left; else if(l==ES_KEEPSIZE) id##_es_l = rect.Width(); else {\
	GetDlgItem(l)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_l = rect.left-rect2.right;}}\
if(o & ES_VCENTER) id##_es_t = rect.Height()/2; else {\
if(t==ES_BORDER) id##_es_t = rect.top; else if(t==ES_KEEPSIZE) id##_es_t = rect.Height(); else {\
	GetDlgItem(t)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_t = rect.top-rect2.bottom;}}\
if(o & ES_HCENTER) id##_es_r = rect.Width(); else { if(r==ES_BORDER) id##_es_r = client.right-rect.right; else if(r==ES_KEEPSIZE) id##_es_r = rect.Width(); else {\
	GetDlgItem(r)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_r = rect2.left-rect.right;}}\
if(o & ES_VCENTER) id##_es_b = rect.Height(); else  { if(b==ES_BORDER) id##_es_b = client.bottom-rect.bottom; else if(b==ES_KEEPSIZE) id##_es_b = rect.Height(); else {\
	GetDlgItem(b)->GetWindowRect(rect2); ScreenToClient(rect2);\
	id##_es_b = rect2.top-rect.bottom;}}\
} else {\
int left,top,right,bottom; BOOL bR = FALSE,bB = FALSE;\
if(o & ES_HCENTER) { int _a,_b;\
if(l==ES_BORDER) _a = client.left; else { GetDlgItem(l)->GetWindowRect(rect2); ScreenToClient(rect2); _a = rect2.right; }\
if(r==ES_BORDER) _b = client.right; else { GetDlgItem(r)->GetWindowRect(rect2); ScreenToClient(rect2); _b = rect2.left; }\
left = _a+((_b-_a)/2-id##_es_l); right = left + id##_es_r;} else {\
if(l==ES_BORDER) left = id##_es_l;\
else if(l==ES_KEEPSIZE) { __ES__CalcBottomRight(this,FALSE,right,left,id,r,id##_es_r,rect,client.right); left = right-id##_es_l;\
} else { GetDlgItem(l)->GetWindowRect(rect2); ScreenToClient(rect2); left = rect2.right + id##_es_l; }\
if(l != ES_KEEPSIZE) __ES__CalcBottomRight(this,FALSE,right,left,id,r,id##_es_r,rect,client.right);}\
if(o & ES_VCENTER) { int _a,_b;\
if(t==ES_BORDER) _a = client.top; else { GetDlgItem(t)->GetWindowRect(rect2); ScreenToClient(rect2); _a = rect2.bottom; }\
if(b==ES_BORDER) _b = client.bottom; else { GetDlgItem(b)->GetWindowRect(rect2); ScreenToClient(rect2); _b = rect2.top; }\
top = _a+((_b-_a)/2-id##_es_t); bottom = top + id##_es_b;} else {\
if(t==ES_BORDER) top = id##_es_t;\
else if(t==ES_KEEPSIZE) { __ES__CalcBottomRight(this,TRUE,bottom,top,id,b,id##_es_b,rect,client.bottom); top = bottom-id##_es_t;\
} else { GetDlgItem(t)->GetWindowRect(rect2); ScreenToClient(rect2); top = rect2.bottom + id##_es_t; }\
if(t != ES_KEEPSIZE) __ES__CalcBottomRight(this,TRUE,bottom,top,id,b,id##_es_b,rect,client.bottom);}\
GetDlgItem(id)->MoveWindow(left,top,right-left,bottom-top,FALSE);\
}
#endif //__EASYSIZE_H_




//类内：              DECLARE_EASYSIZE;
//OnInitDialog：  	INIT_EASYSIZE;
//OnSize：            UPDATE_EASYSIZE;

//左上右下位置，先固定四周控件位置
//（四周控件可定义靠近边框为ES_BORDER，中间留位置ES_KEEPSIZE）
//中间控件写相对于四周控件的位置
//BEGIN_EASYSIZE_MAP(**Dlg)
//	EASYSIZE(IDC_BUTTON1,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_BORDER,0)
//	EASYSIZE(IDC_LIST1,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_BORDER,0)	
//	EASYSIZE(IDC_LIST2,ES_BORDER,IDC_BUTTON1,ES_BORDER,ES_BORDER,0)	
//	EASYSIZE(IDC_TREE1,IDC_BUTTON1,ES_BORDER,IDC_LIST1,IDC_LIST2,0)	
//END_EASYSIZE_MAP