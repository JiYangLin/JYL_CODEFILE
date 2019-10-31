#pragma once
#include <Windows.h>
#include <opencv\highgui.h>
#include <opencv\cxcore.h>
#include <opencv\cv.h>

#pragma comment(lib,"opencv_video248d")
#pragma comment(lib,"opencv_objdetect248d")
#pragma comment(lib,"opencv_imgproc248d")


class CActionTrace
{
public:
	IplImage* Run(IplImage* frame)
	{

		int cx = 1000;
		int cy = 1000;

		
		static int win_size = 10;
		const int MAX_COUNT = 5;
		static CvPoint2D32f* points[2] = { 0,0 }, *swap_points;//两个point指针，分别指向max_count个连续的内存空间
		static char* status = 0;
		static int count = 0;
		static int flags = 0;
		static CvPoint pt = cvPoint(100, 100);//起始点,当前点


		IplImage* dst = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
		cvResize(frame, dst);

		static IplImage *image = 0;
		static IplImage  *grey = 0;
		static IplImage  *prev_grey = 0;
		static IplImage   *pyramid = 0;
		static IplImage   *prev_pyramid = 0;
		if (!image)//一开始的时候image是空的，该if内的初始化只在开始时候运行一次
		{
			image = cvCreateImage(cvGetSize(dst), 8, 3);
			image->origin = frame->origin;
			grey = cvCreateImage(cvGetSize(dst), 8, 1);
			prev_grey = cvCreateImage(cvGetSize(dst), 8, 1);
			pyramid = cvCreateImage(cvGetSize(dst), 8, 1);
			prev_pyramid = cvCreateImage(cvGetSize(dst), 8, 1);
			//为两个point指针分别分配500个点的空间
			points[0] = (CvPoint2D32f*)cvAlloc(MAX_COUNT * sizeof(points[0][0]));
			points[1] = (CvPoint2D32f*)cvAlloc(MAX_COUNT * sizeof(points[0][0]));
			status = (char*)cvAlloc(MAX_COUNT);
			flags = 0;
		}
		cvCopy(dst, image, 0);
		cvCvtColor(image, grey, CV_BGR2GRAY);


		if (count > 0)
		{
			cvCalcOpticalFlowPyrLK(prev_grey, grey, prev_pyramid, pyramid,
				points[0], points[1], count, cvSize(win_size, win_size), 3, status, 0,
				cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03), flags);
			flags |= CV_LKFLOW_PYR_A_READY;

			int i, k;
			for (i = k = 0; i < count; i++)
			{
				if (!status[i])
				{
					continue;
				}
				points[1][k++] = points[1][i];

				//将点的范围进行限制
				BOOL b_change = FALSE;
				if (points[1][i].x<100)
				{
					points[1][i].x = 100;
					b_change = TRUE;
				}
				if (points[1][i].x>cx - 100)
				{
					points[1][i].x = cx - 100;
					b_change = TRUE;
				}
				if (points[1][i].y<100)
				{
					points[1][i].y = 100;
					b_change = TRUE;
				}
				if (points[1][i].y>cy - 100)
				{
					points[1][i].y = cy - 100;
					b_change = TRUE;
				}
				if (b_change)
				{
					k--;
				}
				pt.x = points[1][i].x;
				pt.y = points[1][i].y;
				cvCircle(image, pt, 10, CV_RGB(0, 255, 0), -1, 8, 0);
			}
			count = k;
		}

		if (count < MAX_COUNT)//增加点
		{
			points[1][count++] = cvPointTo32f(pt);
			cvFindCornerSubPix(grey, points[1] + count - 1, 1, cvSize(win_size, win_size), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));
		}

		IplImage   *swap_temp;
		CV_SWAP(prev_grey, grey, swap_temp);
		CV_SWAP(prev_pyramid, pyramid, swap_temp);
		CV_SWAP(points[0], points[1], swap_points);

		cvReleaseImage(&dst);

		cvFlip(image, image, 1);
		return image;
	}

};
