#pragma once
#include <Windows.h>
#include <opencv\highgui.h>
#include <opencv\cv.h>
#include "CvvImg.h"
//#pragma comment(lib,"opencv_objdetect248d")
//#pragma comment(lib,"opencv_imgproc248d")


class CFaceTrace
{
public:
	CFaceTrace()
	{
		cascade = (CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt_tree.xml", 0, 0, 0);  //载入人脸检测级联分类器
	}

	template<typename _FaceProcCallBack>
	bool FindFace(IplImage* frame, _FaceProcCallBack faceProcCallBack)
	{
		double scale = 2.0;
		IplImage* gray = cvCreateImage(cvSize(frame->width, frame->height), 8, 1);
		IplImage* small_img = cvCreateImage(cvSize(cvRound(frame->width / scale),
			cvRound(frame->height / scale)),
			8, 1);


		cvCvtColor(frame, gray, CV_BGR2GRAY);//转换为灰度图。
		cvResize(gray, small_img, CV_INTER_LINEAR);//双线性插值法图像大小变换。
		cvEqualizeHist(small_img, small_img);//直方图均衡化
		CvMemStorage *f_storage = cvCreateMemStorage(0);

		if (cascade)
		{
			CvSeq* faces = cvHaarDetectObjects(small_img, cascade, f_storage,
				1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
				cvSize(30, 30));
			if (faces->total == 0)
			{
				cvReleaseImage(&gray);
				cvReleaseImage(&small_img);
				cvReleaseMemStorage(&f_storage);
				return false;
			}
			for (int i = 0; i < (faces ? faces->total : 0); i++)
			{
				
				CvRect* r = (CvRect*)cvGetSeqElem(faces, i);
				faceProcCallBack(r, scale);
			}
		}

		cvReleaseImage(&gray);
		cvReleaseImage(&small_img);
		cvReleaseMemStorage(&f_storage);
		return true;
	}
private:
	CvHaarClassifierCascade* cascade;
};


//此处是画方框
//m_CFaceTrace.FindFace(frame, [&](CvRect* r, double scale) {
//	cvRectangle(frame, cvPoint(r->x*scale, r->y*scale), cvPoint((r->x + r->width)*scale, (r->y + r->height)*scale), CV_RGB(255, 0, 0), 1);
//});
