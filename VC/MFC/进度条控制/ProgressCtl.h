class ProgressCtl
{
public:
	ProgressCtl():m_StepNum(0),m_StepCount(0),m_bCountLess100(false){}
	bool Init(CProgressCtrl *pProgressCtrl,UINT32 Count)
	{
		if (NULL == pProgressCtrl) 
		{
			AfxMessageBox(_T("进度条控件指针为空"));
			return false;
		}

		m_pProgressCtrl = pProgressCtrl;

		m_pProgressCtrl->SetPos(0);

		InitProgressStep(Count);

		return true;
	}
	void Step()
	{
		if (m_bCountLess100)
		{
			m_pProgressCtrl->StepIt();
			return;
		}


		++m_StepCount;
		if (m_StepCount < m_StepNum) return;

		m_StepCount = 0;
		m_pProgressCtrl->StepIt();
	}
	void ToFull()
	{
		m_pProgressCtrl->SetPos(100);
	}
private:
	void  InitProgressStep(UINT32 Count)
	{
		m_bCountLess100 = Count < 100;
		if (m_bCountLess100)
		{
			m_pProgressCtrl->SetStep(100/Count);
		}
		else
		{
			m_pProgressCtrl->SetStep(1);
			m_StepCount = 0;
			m_StepNum =  Count/100;
		}
	}
private:
	CProgressCtrl *m_pProgressCtrl;
	UINT32 m_StepNum;//m_StepCount累计到m_StepNum时，增加进度
	UINT32 m_StepCount;
	bool  m_bCountLess100;
};