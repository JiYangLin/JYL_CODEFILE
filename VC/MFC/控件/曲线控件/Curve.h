#pragma once
#include <math.h>
#include <vector>
#include <list>
#include <map>
using namespace std;

#define _TxtHeight   14//轴线上文字高度



#define  LinePtMaxCount  10000
//点数足够大，至少大于x轴宽度，否则影响CLine的GetPointY获取点
inline void SetPdcFont(CDC * pDC, int fontHeight)
{
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = fontHeight;
	VERIFY(font.CreateFontIndirect(&lf));
	pDC->SelectObject(&font);
}

class CLineDat
{
public:
	CLineDat(CString LineName, COLORREF color, int iStyle = PS_SOLID, int iThick = 1)
	{
		m_pLineEntirePt = NULL;

		m_LineName = LineName;
		m_color = color;
		m_eraseCount = 0;
		m_pen.CreatePen(PS_SOLID, iThick, color);
	}
	~CLineDat() { Clear(); }

	void AddPoint(double y)
	{
		m_pLineEntirePt = NULL;
		if (m_LinePtY.size() > LinePtMaxCount)  EraseDat(LinePtMaxCount / 2);
		m_LinePtY.push_back(y);
	}
	void Clear()
	{
		m_eraseCount = 0;
		m_LinePtY.swap(list<double>());
		vecPt.swap(vector<POINT>());
	}
	int GetPointCount()
	{
		return (int)m_LinePtY.size() + m_eraseCount;
	}
	void Draw(CDC *pDC, int ptStart, int left, double XdValuePerPixel, int bottom, double YdMinValue, double YdValuePerPixel)
	{
		if (NULL != m_pLineEntirePt)
		{
			DrawEntireLine(pDC,left,XdValuePerPixel,bottom,YdMinValue,YdValuePerPixel);
			return;
		}


		int iPointCount = GetPointCount();
		if (iPointCount <= 0) return;
		if (iPointCount < ptStart) return;

		int xCount = iPointCount - ptStart;

		vecPt.clear();
		for (int k = 0; k < xCount; k++)
		{
			int ptIndex = k + ptStart;

			int x = (int)(left + (k) / XdValuePerPixel);
			int y = (int)(bottom - ((GetPointY(ptIndex) - YdMinValue) / YdValuePerPixel));


			POINT pt = { x,y };
			vecPt.push_back(pt);
		}

		if (0 == vecPt.size()) return;

		CPen *oldPen = pDC->SelectObject(&m_pen);
		pDC->Polyline(&vecPt[0], (int)vecPt.size());
		pDC->SelectObject(oldPen);
	}
	void DrawLineInfo(CDC *pDC, CRect &InfoRect)
	{
		SetPdcFont(pDC, _TxtHeight);
		pDC->SetTextColor(m_color);
		pDC->DrawText(m_LineName, InfoRect, DT_PATH_ELLIPSIS);
	}


public:
	void AddEntireLine(const vector<POINT> *_pLineEntirePt)
	{
		m_pLineEntirePt = _pLineEntirePt;
	}
private:
	void DrawEntireLine(CDC *pDC, int left, double XdValuePerPixel, int bottom, double YdMinValue, double YdValuePerPixel)
	{
		if (NULL == m_pLineEntirePt) return;	
		const vector<POINT> line = *m_pLineEntirePt;
		if (line.size() == 0) return;


		vecPt.clear();
		for (size_t i = 0; i < line.size(); ++i)
		{

			int x = (int)(left + (line[i].x) / XdValuePerPixel);
			int y = (int)(bottom - (line[i].y - YdMinValue) / YdValuePerPixel);


			POINT pt = { x,y };
			vecPt.push_back(pt);
		}

		if (0 == vecPt.size()) return;

		CPen *oldPen = pDC->SelectObject(&m_pen);
		pDC->Polyline(&vecPt[0], (int)vecPt.size());
		pDC->SelectObject(oldPen);
	}
private:
	double GetPointY(int nIndex)
	{
		int indexActual = nIndex - m_eraseCount;
		if (indexActual >= (int)m_LinePtY.size()) return 0;

		int i = 0;
		for (list<double>::iterator it = m_LinePtY.begin(); it != m_LinePtY.end(); ++it, ++i)
		{
			if (i == indexActual) return *it;
		}
		return 0;
	}
	void EraseDat(int eraseCount)
	{
		m_eraseCount += eraseCount;
		list<double>::const_iterator it = m_LinePtY.begin();
		for (int i = 0; i < eraseCount; ++i) ++it;
		m_LinePtY.erase(m_LinePtY.begin(), it);
	}
private:
	COLORREF m_color;
	CString m_LineName;
	vector<POINT> vecPt;
	CPen m_pen;
	int m_eraseCount;
	list<double> m_LinePtY;

	//用于确定全部x，y值的单条曲线
	const vector<POINT> *m_pLineEntirePt;
};
class LineManager
{
public:
	void CreateLine(int lineID, CString lineName, COLORREF color, int iStyle = PS_SOLID, int iThick = 1)
	{
		map<int, CLineDat*>::iterator it = m_lines.find(lineID);
		if (it != m_lines.end())
		{
			(it->second)->Clear();
			delete (it->second);
		}


		CLineDat *pLine = new CLineDat(lineName, color, iStyle, iThick);
		m_lines[lineID] = pLine;
	}
	~LineManager()
	{
		Clear();
	}
	void Clear()
	{
		for (map<int, CLineDat*>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			(it->second)->Clear();
			delete (it->second);
		}
		m_lines.swap(map<int, CLineDat*>());
	}
	void Draw(CDC *pDC, int ptStart, int left, double XdValuePerPixel, int bottom, double YdMinValue, double YdValuePerPixel)
	{
		for (map<int, CLineDat*>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			(it->second)->Draw(pDC, ptStart, left, XdValuePerPixel, bottom, YdMinValue, YdValuePerPixel);
		}
	}
	void DrawLineInfo(CDC *pDC, CRect rectInfo)
	{
		int lineCount = (int)m_lines.size();
		if (0 == lineCount) return;


		int perWidth = rectInfo.Width() / lineCount;

		int i = 0;
		for (map<int, CLineDat*>::iterator it = m_lines.begin(); it != m_lines.end(); ++it, ++i)
		{
			CRect rect = rectInfo;
			rect.left += i*perWidth;
			rect.right = rect.left + perWidth;
			(it->second)->DrawLineInfo(pDC, rect);
		}
	}
	void InsertLineDat(int LineID, double val)
	{
		map<int, CLineDat*>::iterator it = m_lines.find(LineID);
		if (it == m_lines.end()) return;
		(it->second)->AddPoint(val);
	}
	void AddEntireLine(int LIneID,const vector<POINT> *_pLineEntirePt)
	{
		map<int, CLineDat*>::iterator it = m_lines.find(LIneID);
		if (it == m_lines.end()) return;
		(it->second)->AddEntireLine(_pLineEntirePt);
	}
	size_t GetMaxPointCount()
	{//获取最长曲线的点数
		if (m_lines.size() == 0) return 0;
		int maxPt = 0;
		for (map<int, CLineDat*>::iterator it = m_lines.begin(); it != m_lines.end(); ++it)
		{
			int ptCount = (it->second)->GetPointCount();
			if (ptCount > maxPt) maxPt = ptCount;
		}
		return maxPt;
	}
	void RemoveLinePt(int LIneID)
	{
		map<int, CLineDat*>::iterator it = m_lines.find(LIneID);
		if (it == m_lines.end()) return;
		(it->second)->Clear();
	}
	int GetLinePtCount(int LIneID)
	{
		map<int, CLineDat*>::iterator it = m_lines.find(LIneID);
		if (it == m_lines.end()) return 0;
		return (it->second)->GetPointCount();
	}
private:
	map<int, CLineDat*>  m_lines;
};



struct CAxis
{
	double      m_dMinValue;
	double      m_dMaxValue;
	int         m_GridCount;  //轴网格数
	double	    m_dValuePerPixel; //how much a pixel represented
	CAxis()
	{
		m_dMinValue = 0;
		m_dMaxValue = 0;
		m_GridCount = 0;
		m_dValuePerPixel = 0;
	}
	void Init(double dMinValue, double  dMaxValue, int  GridCount)
	{
		m_dMinValue = dMinValue;
		m_dMaxValue = dMaxValue;
		m_GridCount = GridCount;
	}
};
class AxisPos
{
public:
	AxisPos()
	{
		m_PixinterVal = 20;
		m_useRange = false;


		m_minVal = 0;
		m_Valueinterval = 1;

		m_RangeMin = 0;
		m_RangeMax = 100;
	}
	void SetRange(int _RangeMin, int _RangeMax,int _PixinterVal = 20)
	{//设置显示范围
		m_useRange = true;
		m_PixinterVal = _PixinterVal;
		m_RangeMin = _RangeMin;
		m_RangeMax = _RangeMax;
	}
	void SetMinVal(double _MinVal,int _PixinterVal = 20,int _ValueInterval=1)
	{//设置最小值，自动计算最大值
		m_useRange = false;
		m_PixinterVal = _PixinterVal;
		m_minVal = (int)_MinVal;
		m_Valueinterval = _ValueInterval;
	}
public:
	int SetAxis(CAxis &axis,int AxisLen)
	{//返回数据显示间隔
		int Valueinterval = 1;
		if (m_useRange)
		{
			double delta = m_RangeMax - m_RangeMin;
			if (AxisLen > delta)
			{
				double pixInterval = int(AxisLen/delta +0.5);
				if (pixInterval < m_PixinterVal)
					Valueinterval = int(m_PixinterVal/pixInterval + 0.5);
			}
			else
			{
				int numPerPix = int(delta/AxisLen);
				Valueinterval = numPerPix*m_PixinterVal;
			}

			SetAxis(axis,m_RangeMin,m_RangeMax,Valueinterval);
		}
		else
		{
			if (m_Valueinterval <= 0) m_Valueinterval = 1;
			double Delta = AxisLen/((double)m_PixinterVal*m_Valueinterval);
			int maxVal = int(m_minVal + Delta);
			Valueinterval = (int)m_Valueinterval;
			SetAxis(axis,m_minVal,maxVal,Valueinterval);
		}
		return Valueinterval;
	}
public:
	//计算能显示的点数量
	int GetAxisCount(int AxisLen)
	{
		if (m_useRange) return m_RangeMax - m_RangeMin + 1;

		if (m_Valueinterval <= 0) m_Valueinterval = 1;
		double Delta = AxisLen/((double)m_PixinterVal*m_Valueinterval);
		double maxVal =  int(m_minVal + Delta);
		return int(maxVal - m_minVal);
	}
private:
	void SetAxis(CAxis &axis,int minVal,int maxVal,int valInterval)
	{
		if (minVal > maxVal) maxVal = minVal;
		if (valInterval <= 0) valInterval = 1;
		int GridCount = int((maxVal - minVal) / valInterval);
		axis.Init(minVal, maxVal, GridCount);
	}
private:	
	bool   m_useRange;
	int    m_PixinterVal;

	int    m_minVal;
	int    m_Valueinterval;

	int    m_RangeMin;  
	int    m_RangeMax;  
};




class CurveCtl : public CStatic
{
	bool m_bHide;
	bool m_bHideContent;
	bool m_bHideXRange;

	LineManager m_LineManager;

	CAxis m_axisY;       //Y axis
	CAxis m_axisX;     //X axis

	AxisPos m_YAxisPos;
	AxisPos m_XAxisPos;
	int m_Xinterval;

	CRect m_rectPlot;
	CRect m_rectAxisY;
	CRect m_rectAxisX;
	CRect m_rectXText;

	//标题
	CString m_Title;
	COLORREF m_TitleColor;

	CPen m_PenAxisBorder;
	COLORREF m_ScaleColor;//刻度线颜色
	COLORREF m_TxtColor;//轴线上文字颜色
	CPen m_dotPen;
	CPen m_ZeroPen;
	CPen m_solidPen;



	DECLARE_DYNAMIC(CurveCtl)
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return FALSE; }
	afx_msg void OnPaint()
	{
		CPaintDC dc(this);
		Flush();
	}

protected:
	COLORREF    m_clrBkColor;
	int			m_iMarginLeft;			   // left margin in pixels
	int			m_iMarginRight;			   // right margin in pixels
	int			m_iMarginTop;			   // top margin in pixels
	int			m_iMarginBottom;		   // bottom margin in pixels
	bool        m_limitXrang;
private:
	void Draw(CDC* pDC, CRect &rectCtrl)
	{
		if(m_bHide) return;

		DrawBackground(pDC, rectCtrl);  

		if(!m_bHideContent)
		{
			CalcLayout(rectCtrl);	
			DrawAxisesBorder(pDC);
			DrawAxisesY(pDC);
			DrawLines(pDC, rectCtrl);
		}

		DrawTitle(pDC, rectCtrl);
	}
	void DrawBackground(CDC* pDC, CRect &rectCtrl)
	{
		pDC->FillSolidRect(rectCtrl, m_clrBkColor);
	}
	//给图添加标题
	void DrawTitle(CDC *pDC, CRect &rectCtrl)
	{
		pDC->SetTextColor(m_TitleColor);
		CFont font;
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = 20;
		VERIFY(font.CreateFontIndirect(&lf));
		CFont* def_font = pDC->SelectObject(&font);
		int leftPos = rectCtrl.Width() / 2 - m_Title.GetLength() * 8;

		pDC->FillSolidRect(CRect(0, 0, rectCtrl.right, m_rectPlot.top-10), m_clrBkColor);
		pDC->DrawText(m_Title, CRect(leftPos, 10, rectCtrl.right, 30), DT_PATH_ELLIPSIS);
		pDC->SelectObject(def_font);
	}
	void DrawAxisesBorder(CDC* pDC)
	{//画边框
		pDC->SelectObject(&m_PenAxisBorder);
		pDC->MoveTo(m_rectPlot.left, m_rectPlot.top);
		pDC->LineTo(m_rectPlot.left, m_rectPlot.bottom);
		pDC->LineTo(m_rectPlot.right, m_rectPlot.bottom);
	}
	void DrawAxisesY(CDC* pDC)
	{
		int iGridCountY = m_axisY.m_GridCount;		//设置网格数
		int iGridY = (int)floor((m_axisY.m_dMaxValue - m_axisY.m_dMinValue) / iGridCountY);//网格间距
		if (iGridY <= 0) return;


		pDC->SetTextColor(m_TxtColor);
		SetPdcFont(pDC, _TxtHeight);


		int y0 = (int)m_axisY.m_dMinValue;
		CString str;
		int Count = iGridCountY + 1;
		for (int i = 0;i < Count;i++)			//画网格线
		{
			int y = (int)(m_rectPlot.bottom - (y0 + i*iGridY - m_axisY.m_dMinValue) / m_axisY.m_dValuePerPixel);
			int yText = y0 + i*iGridY;
			str.Format(_T("%d"), yText);//显示网格标度
			pDC->DrawText(str, CRect(m_rectAxisY.left, y - _TxtHeight / 2, m_rectAxisY.right - 10, y + _TxtHeight / 2), DT_RIGHT);


			//画刻度线
			if (0 == yText)  pDC->SelectObject(&m_ZeroPen);
			else   pDC->SelectObject(&m_dotPen);
			pDC->MoveTo(m_rectPlot.left, y);
			pDC->LineTo(m_rectPlot.right, y);
			pDC->SelectObject(&m_solidPen);
		}
	}
	void DrawAxisesX(CDC* pDC, int xStart = 0)
	{
		if (m_Xinterval <= 0) m_Xinterval = 1;


		pDC->FillSolidRect(&m_rectXText, m_clrBkColor);

		int iGridCountX = m_axisX.m_GridCount;
		int iGridX = (int)floor(((m_axisX.m_dMaxValue - m_axisX.m_dMinValue) / iGridCountX) + 0.5);
		if (iGridX <= 0) return;


		int textHeight = 40;//标度显示高度
		pDC->SetTextColor(m_ScaleColor);
		SetPdcFont(pDC, _TxtHeight);

		//计算x轴最后一个显示的数字
		int lastNum = iGridCountX/m_Xinterval;
		lastNum *= m_Xinterval;

		int x0 = (int)m_axisX.m_dMinValue;
		CString strX;
		for (int i = 0;i <= iGridCountX;i+= m_Xinterval)			//画网格线
		{//////////
			int x = (int)(m_rectPlot.right - (x0 + (iGridCountX - i) * iGridX - m_axisX.m_dMinValue) / m_axisX.m_dValuePerPixel);

			//画网格
			pDC->SelectObject(&m_dotPen);
			pDC->MoveTo(x, m_rectPlot.bottom);
			pDC->LineTo(x, m_rectPlot.top);
			pDC->SelectObject(&m_solidPen);


			int xTmp = (x0 + i * iGridX) + xStart;
			strX.Format(_T("%d"), xTmp);

			
			if (m_bHideXRange)
			{//隐藏中间x值
				if (i == 0 || i == lastNum)
					pDC->DrawText(strX, CRect(x - textHeight / 2, m_rectAxisX.top + 8, x + textHeight / 2, m_rectAxisX.bottom), DT_CENTER);
			}
			else
			{
				pDC->DrawText(strX, CRect(x - textHeight / 2, m_rectAxisX.top + 8, x + textHeight / 2, m_rectAxisX.bottom), DT_CENTER);
			}
		}
	}
	void DrawLines(CDC *pDC, CRect &rectCtrl)
	{
		int pointCount = (int)m_LineManager.GetMaxPointCount();
		int ptXCount = (m_axisX.m_GridCount+1)*m_Xinterval;////
		int ptStart = pointCount > ptXCount ? pointCount - ptXCount : 0;

		m_LineManager.Draw(pDC, ptStart, m_rectPlot.left, m_axisX.m_dValuePerPixel, m_rectPlot.bottom, m_axisY.m_dMinValue, m_axisY.m_dValuePerPixel);

		DrawAxisesX(pDC, ptStart);

		m_LineManager.DrawLineInfo(pDC, CRect(m_rectAxisX.left + 10, m_rectAxisY.bottom + 10, rectCtrl.right, m_rectAxisY.bottom + 40));
	}

	void CalcLayout(CRect &rectCtrl)
	{
		(void)m_YAxisPos.SetAxis(m_axisY,rectCtrl.Height());
		m_Xinterval = m_XAxisPos.SetAxis(m_axisX,rectCtrl.Width());


		int rectAxisYWidth = 40;
		m_rectAxisY.left = rectCtrl.left + m_iMarginLeft;
		m_rectAxisY.right = m_rectAxisY.left + rectAxisYWidth;
		m_rectAxisY.top = rectCtrl.top + m_iMarginTop;
		m_rectAxisY.bottom = rectCtrl.bottom - m_iMarginBottom;

		int rectAxisXHight = 20;
		m_rectAxisX.left = m_rectAxisY.right;
		m_rectAxisX.right = rectCtrl.right - m_iMarginRight;
		m_rectAxisX.bottom = rectCtrl.bottom - m_iMarginBottom;
		m_rectAxisX.top = m_rectAxisX.bottom - rectAxisXHight;

		m_rectPlot.left = m_rectAxisY.right;
		m_rectPlot.right = rectCtrl.right - m_iMarginRight;
		m_rectPlot.top = rectCtrl.top + m_iMarginTop;
		m_rectPlot.bottom = m_rectAxisX.top;


		m_rectXText.left = rectCtrl.left;
		m_rectXText.right = rectCtrl.right;
		m_rectXText.top = m_rectAxisX.top + 4;
		m_rectXText.bottom = rectCtrl.bottom;

		//Y和X的数值转化为像素值的比例因子
		m_axisY.m_dValuePerPixel = (m_axisY.m_dMaxValue - m_axisY.m_dMinValue) / m_rectPlot.Height();
		m_axisX.m_dValuePerPixel = (m_axisX.m_dMaxValue - m_axisX.m_dMinValue) / m_rectPlot.Width();
	}



	/////////////接口操作/////////////////
public:
	CurveCtl()
	{
		m_clrBkColor = RGB(0, 0, 0);
		m_PenAxisBorder.CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
		m_ScaleColor = RGB(200, 200, 200);
		m_dotPen.CreatePen(PS_DOT, 1, RGB(60, 60, 60));
		m_ZeroPen.CreatePen(PS_SOLID, 1, RGB(70, 70, 70));
		m_solidPen.CreatePen(PS_SOLID, 1, RGB(60, 60, 60));
		m_TxtColor = RGB(255, 255, 255);

		m_bHide = false;
		m_bHideContent = false;
		m_bHideXRange = false;

		m_Xinterval = 1;
		m_limitXrang = false;

		SetMargin();
	}
public:
	void HideCTL(bool hide)
	{//隐藏控件
		if (hide) ShowWindow(SW_HIDE);
		else ShowWindow(SW_SHOW);

		m_bHide = hide;
		Flush();
	}
	void HideContent(bool hide)
	{//只显示标题
		m_bHideContent = hide;
		Flush();
	}
	void HideXRange(bool hide)
	{//只显示x轴第一个点和最后一个点
		m_bHideXRange = hide;
		Flush();
	}
	void SetBkColor(COLORREF BkColor)
	{//设置背景颜色
		m_clrBkColor = BkColor;
	}
	void SetMargin(int	iMarginTop = 40, int	iMarginBottom = 40, int	iMarginLeft = 0, int	iMarginRight = 30)
	{//设置边框位置
		m_iMarginLeft = iMarginLeft;
		m_iMarginRight = iMarginRight;
		m_iMarginTop = iMarginTop;
		m_iMarginBottom = iMarginBottom;
	}
	void SetTitle(CString Title, COLORREF TitleColor = RGB(255, 255, 255))
	{//设置标题信息
		m_Title = Title;
		m_TitleColor = TitleColor;
	}


	/////x，y轴值可以设置范围，也可以只设置最小值自动计算最大//////
public:
	void SetYRange(int RangeMin, int RangeMax,int PixinterVal = 20)
	{//设置Y轴固定显示范围
		m_YAxisPos.SetRange(RangeMin,RangeMax,PixinterVal);
	}
	void SetYMin(double MinVal,int PixinterVal = 20,int ValueInterval=1)
	{//设置Y轴最小值，会根据控件高度设置最大范围
		m_YAxisPos.SetMinVal(MinVal,PixinterVal,ValueInterval);
	}


	void SetXRange(int RangeMin, int RangeMax,int PixinterVal = 20)
	{//x值自动增加，根据axisX的RangeMax清空数据，重新写入
		//x范围大会有问题，适合插入整条曲线操作
		m_limitXrang = true;
		m_XAxisPos.SetRange(RangeMin,RangeMax,PixinterVal);
	}
	void SetXMin(int MinVal,int PixinterVal = 20,int ValueInterval=1)
	{//x轴显示会随x值增加而变化,运行过程中调用不能重新设置x最小值, 只能用改变x间距
		m_limitXrang = false;
		m_XAxisPos.SetMinVal(MinVal,PixinterVal,ValueInterval);
	}


	//计算x轴能显示的点数量
	int GetXAxisCount()
	{
		CRect rectCtrl;
		GetClientRect(&rectCtrl);
		return m_XAxisPos.GetAxisCount(rectCtrl.Width());
	}
public:
	void CreateLine(int lineID, CString lineName, COLORREF color, int iStyle = PS_SOLID, int iThick = 1)
	{//创建曲线
		m_LineManager.CreateLine(lineID, lineName, color, iStyle, iThick);
	}
	void RemoveLinePt(int lineID)
	{//删除某条曲线上的点
		m_LineManager.RemoveLinePt(lineID);
	}
	void InsertLineDat(int lineID, double val)
	{//为曲线插入数据
		if (m_limitXrang)  if(m_LineManager.GetLinePtCount(lineID) > m_axisX.m_dMaxValue) m_LineManager.RemoveLinePt(lineID);

		m_LineManager.InsertLineDat(lineID, val);
	}
	int GetLinePtCount(int lineID)
	{//获取曲线上的数据量
		return m_LineManager.GetLinePtCount(lineID);
	}
	void Flush()
	{//刷新显示
		CClientDC dc(this);

		CDC memDC;
		memDC.CreateCompatibleDC(&dc);

		CRect rectCtrl;
		GetClientRect(&rectCtrl);
		CBitmap memBitmap;
		memBitmap.CreateCompatibleBitmap(&dc, rectCtrl.Width(), rectCtrl.Height());
		memDC.SelectObject(&memBitmap);

		Draw(&memDC, rectCtrl);

		dc.BitBlt(0, 0, rectCtrl.Width(), rectCtrl.Height(), &memDC, 0, 0, SRCCOPY);
	}

	void Clear()
	{//清除所有数据
		m_LineManager.Clear();
	}


	void AddEntireLine(int LineID,const vector<POINT> *_pLineEntirePt)
	{//为曲线插入整条线的数据  _pLineEntirePt不要使用临时变量
		m_LineManager.AddEntireLine(LineID, _pLineEntirePt);
	}
};

#define  CURVE_IMPLEMENT   IMPLEMENT_DYNAMIC(CurveCtl, CStatic)\
	BEGIN_MESSAGE_MAP(CurveCtl, CStatic)\
	ON_WM_ERASEBKGND()\
	ON_WM_PAINT()\
END_MESSAGE_MAP()


//////////////控件邦定////////////////////
//任意cpp声明CURVE_IMPLEMENT
//图像控件邦定后类名从CStatic改为CurveCtl


//////////////线条ID定义////////////////////
//#define LINE1   1
//#define LINE2   2


//////////////初始化////////////////////////
////X坐标
//m_CurveShow.SetXMin(0);//设置x的最小值,最大值会根据控件宽度自动计算.x坐标会根据插入数据的量自动变化,运行过程中调用不能重新设置x最小值, 只能用改变x间距
//m_CurveShow.SetXRange(0,10);//设置x显示范围,插入数据超过范围会清空//x范围大会有问题


////Y坐标
//m_CurveShow.SetYMin(0);//设置Y轴最小值，会根据控件高度设置Y最大值
//m_CurveShow.SetYRange(0,10);//设置Y轴的显示范围


////设置标题
//m_CurveShow.SetTitle(_T("我是标题")); //传入空字符串或者不调用则不设置标题



////添加线条，重复添加为删除操作
//m_CurveShow.CreateLine(LINE1,_T("我是第一条"),RGB(0,255,0));
//m_CurveShow.CreateLine(LINE2,_T("我是第二条"),RGB(255,255,0));

//任何改变曲线的操作后，需要刷新显示时调用
//m_CurveShow.Flush();




//////////////插入单个数据////////////////////
//插入单个数据,x轴会随数量自动变化,适合随时间变化的曲线
//m_CurveShow.InsertLineDat(LINE1,val);
//m_CurveShow.Flush();




//////////////插入整条曲线////////////////////
//为曲线插入整条线的数据  _pLineEntirePt不要使用临时变量
//初始化使用SetXRange，适合x范围确定的曲线,x可以不连续

///===定义成员变量，设置x范围
//vector<POINT> m_vecPoint;
//m_vecPoint.resize(10);
//m_CurveShow.SetXRange(0,10);

///===数据赋值,x可以不连续
//for (int i=0; i< 10; ++i)
//{
//	m_vecPoint[i].x = x;
//	m_vecPoint[i].y = val; 
//}

///===显示曲线
//m_CurveShow.AddEntireLine(LINE1,&m_vecPoint);
//m_CurveShow.Flush();





//////////////清空曲线数据////////////////////
//m_CurveShow.RemoveLinePt(LINE2);
//m_CurveShow.Flush();





//////////////显示控制////////////////////
//隐藏曲线内容，不包括标题（可以设置空）和背景色
//m_CurveShow.HideContent(true);

//只显示x轴首尾两个数值
//m_CurveShow.HideXRange(true);