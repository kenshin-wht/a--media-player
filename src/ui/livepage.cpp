#include "ui/livepage.h"
#include <string>
#include <iostream>
#include "stdio.h"
using namespace std;
#include <WINDOWS.H>
#include<io.h>

ApolloLivePage::ApolloLivePage(wxWindow* parent)
	:
	MyFrame4(parent)
{
	
	/*
	//设置默认路径，是exe所在文件夹
	standPath = wxStandardPaths::Get().GetResourcesDir().ToStdString();
	backPath = standPath + "/video/meetingBack1.png";
	choseBack->SetPath(backPath);
	*/
	
	originTimer = new RenderTimer(m_panel30);
	originTimer->start();
	//videoCap = new VideoCap();
	//backgroundCap = new VideoCap();
//	backgroundWA = new WatchAgain();
	listvideo = new ListShowVideo();
	
	//audiodecoder.stop();
	

//这个
	auto videoDataCallBack = [&](AVFrame* frameRGB) {
		videoCallback(frameRGB->data[0], frameRGB->width, frameRGB->height, (AVPixelFormat)frameRGB->format);
	};

	//backgroundCap->setDataCallBack(videoDataCallBack);
	//backgroundWA->setDataCallBack(videoDataCallBack);
	listvideo->setDataCallBack(videoDataCallBack);
}



void ApolloLivePage::videoCallback(uint8_t* data, int width, int height, AVPixelFormat format) {
	//I_LOG("width:{}, fWidth:{}, height:{}, fHeight:{}", width, originFrame->width, height, originFrame->height);
	AVFrame* f = av_frame_alloc();
	int numBytes = av_image_get_buffer_size((AVPixelFormat)format, width, height, 1);
	uint8_t* outData = (uint8_t*)malloc(numBytes);
	memcpy(outData, data, numBytes);

	av_image_fill_arrays(f->data, f->linesize, outData,
		(AVPixelFormat)format, width, height, 1);
	f->width = width;
	f->height = height;
	f->format = format;

	int panelWidth, panelHeight;
	m_panel30->GetSize(&panelWidth, &panelHeight);

	float widthScale = (float)f->width / (float)panelWidth;
	float heightScale = (float)f->height / (float)panelHeight;

	int scaleWidth = 0;
	int scaleHeight = 0;
	float scale = 0;
	if (widthScale >= heightScale) {
		//原图像比较宽
		scale = widthScale;
		scaleWidth = panelWidth;
		scaleHeight = (float)f->height / scale;
		if (scaleHeight % 2 == 1) scaleHeight -= 1;
	}
	else {
		scale = heightScale;
		scaleHeight = panelHeight;
		scaleWidth = (float)f->width / scale;
		if (scaleWidth % 2 == 1) scaleWidth -= 1;
	}
	//scaleWidth = panelWidth;
	//scaleHeight = panelHeight;
	AVFrame* frameRGB = av_frame_alloc();
	frameRGB->width = scaleWidth;
	frameRGB->height = scaleHeight;
	frameRGB->format = AV_PIX_FMT_RGB24;
	av_frame_get_buffer(frameRGB, 1);
	SwsContext* video_convert_ctx = sws_getContext(f->width, f->height,
		(AVPixelFormat)format, frameRGB->width, frameRGB->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);


	int ret = sws_scale(video_convert_ctx, f->data, f->linesize, 0, f->height, frameRGB->data, frameRGB->linesize);

	sws_freeContext(video_convert_ctx);
	if (ret <= 0) {
		printf("error");
		av_frame_free(&frameRGB);
		if (f->data[0]) {
			free(f->data[0]);
		}
		av_frame_free(&f);
		return;
	}
	std::unique_lock<std::mutex> locker(originImgMtx);
	if (originImg != NULL)
		av_frame_free(&originImg);
	originImg = frameRGB;
	//convert();
	
	/*
	std::unique_lock<std::mutex> locker(originImgMtx);
	if (originImg != NULL)
		av_frame_free(&originImg);
	originImg = frameRGB;
	*/
	
	
	
	}




void ApolloLivePage:: convert()
	{
	std::unique_lock<std::mutex> locker(originImgMtx);

	if (originImg == NULL) {
		return;
	}
		w = originImg->width;
		h = originImg->height;
		struct SwsContext* sws_ctx_ori = NULL;
		sws_ctx_ori = sws_getContext(originImg->width, originImg->height, (enum AVPixelFormat)originImg->format,
			w, h, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);
		mat.create(cv::Size(w, h), CV_8UC3);
		AVFrame* bgr24frame = av_frame_alloc();
		bgr24frame->data[0] = (uint8_t*)mat.data;
		avpicture_fill((AVPicture*)bgr24frame, bgr24frame->data[0], AV_PIX_FMT_BGR24, w, h);
		sws_scale(sws_ctx_ori,
			(const uint8_t* const*)originImg->data, originImg->linesize,
			0, originImg->height, // from cols=0,all rows trans
			bgr24frame->data, bgr24frame->linesize);

		av_free(bgr24frame);
		sws_freeContext(sws_ctx_ori);


		Mat img_logo, imagesrc2;
		imagesrc2 = cv::imread("D:/code/logo_flower.jpg");//读取图像2
		//判断读入是否成功
		//调整image2的大小与image1的大小一致，融合函数addWeighted()要求输入的两个图形尺寸相同
		resize(imagesrc2, imagesrc2, Size(mat.cols, mat.rows));
		addWeighted(mat, 0.9, imagesrc2, 0.1, 0, img_logo);//0.5+0.5=1,0.3+0.7=1
		imwrite("C:/Users/jhc18/Desktop/test/x.png", img_logo);
		AVFrame* avframe = av_frame_alloc();
		frame_RGB = av_frame_alloc();
		//if (avframe && !img_logo.empty()) {

		avframe->format = AV_PIX_FMT_YUV420P;
		avframe->width = img_logo.cols;
		avframe->height = img_logo.rows;
		av_frame_get_buffer(avframe, 0);
		av_frame_make_writable(avframe);
		cv::Mat yuv; // convert to yuv420p first
		cv::cvtColor(img_logo, yuv, cv::COLOR_BGR2YUV_I420);
		// calc frame size
		int frame_size = img_logo.cols * img_logo.rows;
		unsigned char* pdata = yuv.data;
		// fill yuv420
		// yyy yyy yyy yyy
		// uuu
		// vvv
		avframe->data[0] = pdata; // fill y
		avframe->data[1] = pdata + frame_size; // fill u
		avframe->data[2] = pdata + frame_size * 5 / 4; // fill v
		struct SwsContext* img_convert_ctx;
		img_convert_ctx = sws_getContext(avframe->width, avframe->height,
			AV_PIX_FMT_YUV420P, width_s, height_s, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

		frame_RGB->width = width_s;
		//cout << "width2:" << width;
		frame_RGB->height = height_s;
		//cout << "heigth:" << height;
		frame_RGB->format = AV_PIX_FMT_RGB24;
		av_frame_get_buffer(frame_RGB, 1);

		sws_scale(img_convert_ctx, (const uint8_t* const*)avframe->data,
			avframe->linesize, 0, avframe->height, frame_RGB->data, frame_RGB->linesize);
}

	


void getFiles(string path, vector<string>& files)
{
	//文件句柄
	long long  hFile = 0;
	//文件信息
	struct _finddata_t fileinfo;
	string p;

	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之
			//如果不是,加入列表
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			
			else 
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

void ApolloLivePage::dirclickfuc(wxFileDirPickerEvent& event) {
	//先清屏 
	m_listBox1->Clear();
	 dirpath=event.GetPath();
	 vector<string> files;

	 ////获取该路径下的所有文件
	 getFiles(dirpath, files); int size = files.size();
	 char str[30];
	 for (int i = 0; i < size; i++)
	 {
		 if(files[i].substr(files[i].find_last_of('.'))== ".mp4"|| files[i].substr(files[i].find_last_of('.')) == ".flv"|| files[i].substr(files[i].find_last_of('.')) == ".mkv")
		 m_listBox1->Append(files[i].c_str());
	 }
	
	 
}

void ApolloLivePage::audioplayfunc(wxCommandEvent& event) {
	
	/*
	int w, h = 0;
	if (w <= 0) w = originImg->width;
	if (h <= 0) h = originImg->height;
	struct SwsContext* sws_ctx_ori = NULL;
	sws_ctx_ori = sws_getContext(originImg->width, originImg->height, (enum AVPixelFormat)originImg->format,
		w, h, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	cv::Mat mat;//视频的帧图
	mat.create(cv::Size(w, h), CV_8UC3);
	AVFrame* bgr24frame = av_frame_alloc();
	bgr24frame->data[0] = (uint8_t*)mat.data;
	avpicture_fill((AVPicture*)bgr24frame, bgr24frame->data[0], AV_PIX_FMT_BGR24, w, h);
	sws_scale(sws_ctx_ori,
		(const uint8_t* const*)originImg->data, originImg->linesize,
		0, originImg->height, // from cols=0,all rows trans
		bgr24frame->data, bgr24frame->linesize);

	av_free(bgr24frame);
	sws_freeContext(sws_ctx_ori);

	Mat image, imagesrc2;
	imagesrc2 = cv::imread("D:/code/my_logo.jpg");//读取图像2
	//判断读入是否成功
	//调整image2的大小与image1的大小一致，融合函数addWeighted()要求输入的两个图形尺寸相同
	resize(imagesrc2, imagesrc2, Size(mat.cols, mat.rows));
	addWeighted(mat, 0.3, imagesrc2, 0.7, 0, image);//0.5+0.5=1,0.3+0.7=1
	imwrite("C:/Users/jhc18/Desktop/test/xiaoguo.png", image);
	*/
	
	//imshow("效果图", image);
	//建立显示窗口
	//namedWindow("效果图");
	/*
	Mat img1;
	img1 = imread("D:/code/logo_flower.jpg");
	Mat img3;
	img3 = imread("D:/code/my_logo.jpg");
	cv::Mat gray;
	cv::cvtColor(img1, gray, cv::COLOR_BGR2GRAY);
	cv::Mat result;
	cv::threshold(gray, result, 240, 255, cv::THRESH_BINARY);
	imwrite("C:/Users/jhc18/Desktop/test/result.png", result);
	cv::Mat src_mask;
	bitwise_not(result, src_mask);
	imwrite("C:/Users/jhc18/Desktop/test/src_mask.png", src_mask);
	int x_start = p.x - img1.cols / 2;
	int y_start = p.y - img1.rows / 2;

	int x_diff = 0;
	if (x_start < 0)
	{
		x_diff = -x_start;
		x_start = 0;
		img1 = img1.colRange(x_diff, img1.cols);
		src_mask = src_mask.colRange(x_diff, src_mask.cols);
	}
	if ((x_start + src_mask.cols) > img3.cols) {
		x_diff = x_start + src_mask.cols - img3.cols;
		img1 = img1.colRange(0, img1.cols - x_diff);
		src_mask = src_mask.colRange(0, src_mask.cols - x_diff);
	}


	int y_diff = 0;
	if (y_start < 0)
	{
		y_diff = -y_start;
		y_start = 0;
		img1 = img1.rowRange(y_diff, img1.rows);
		src_mask = src_mask.rowRange(y_diff, src_mask.rows);
	}
	if ((y_start + src_mask.rows) > img3.rows) {
		y_diff = y_start + src_mask.rows - img3.rows;
		img1 = img1.rowRange(0, img1.rows - y_diff);
		src_mask = src_mask.rowRange(0, src_mask.rows - y_diff);
	}
	cv::Mat imageROI;

	imageROI = img(cv::Rect(x_start, y_start, src_mask.cols, src_mask.rows));
	imwrite("F:/桌面/test/img.png", img);
	imwrite("F:/桌面/test/imageROI.png", imageROI);
	img1.copyTo(imageROI, src_mask);
	imwrite("F:/桌面/test/imageROI2.png", imageROI);
	*/
	
}

void ApolloLivePage::listclickfuc(wxCommandEvent& event)
{
		//std::cout << originpath;
	    string str = event.GetString();
		//backgroundCap->stop();
		//listvideo->stop();
		listvideo->start(str);
		//audiodecoder.decoder();
		//audiodecoder.stop();
}







void ApolloLivePage::originImgPaintEvent(wxPaintEvent& event) {
	std::unique_lock<std::mutex> locker(originImgMtx);
	

	if (originImg == NULL) {
		return;
	}
	int panelWidth, panelHeight;
	m_panel30->GetSize(&panelWidth, &panelHeight);
	wxPaintDC  dc(m_panel30);
	unsigned char* data = originImg->data[0];

	wxSize size = wxSize(originImg->width, originImg->height);

	wxImage* image = new wxImage(size, data);

	wxBitmap* img = nullptr;
	if (imgMirror) {
		img = new wxBitmap(image->Mirror());
	}
	else {
		img = new wxBitmap(*image);
	}

	dc.DrawBitmap(*img, (panelWidth - originImg->width) / 2, (panelHeight - originImg->height) / 2);
	delete img;
}

	/*
	Mat img_logo;
	img_logo = imread("D:/code/logo_flower.jpg");
	AVFrame* avframe = av_frame_alloc();
	AVFrame* frameRGB = av_frame_alloc();
	if (avframe && !img_logo.empty()) {

		avframe->format = AV_PIX_FMT_YUV420P;
		avframe->width = img_logo.cols;
		avframe->height = img_logo.rows;
		av_frame_get_buffer(avframe, 0);
		av_frame_make_writable(avframe);
		cv::Mat yuv; // convert to yuv420p first
		cv::cvtColor(img_logo, yuv, cv::COLOR_BGR2YUV_I420);
		// calc frame size
		int frame_size = img_logo.cols * img_logo.rows;
		unsigned char* pdata = yuv.data;
		// fill yuv420
		// yyy yyy yyy yyy
		// uuu
		// vvv
		avframe->data[0] = pdata; // fill y
		avframe->data[1] = pdata + frame_size; // fill u
		avframe->data[2] = pdata + frame_size * 5 / 4; // fill v
		struct SwsContext* img_convert_ctx;
		img_convert_ctx = sws_getContext(avframe->width, avframe->height,
			AV_PIX_FMT_YUV420P, width_s, height_s, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
		
		frameRGB->width = width_s;
		//cout << "width2:" << width;
		frameRGB->height = height_s;
		//cout << "heigth:" << height;
		frameRGB->format = AV_PIX_FMT_RGB24;
		av_frame_get_buffer(frameRGB, 1);

		sws_scale(img_convert_ctx, (const uint8_t* const*)avframe->data,
			avframe->linesize, 0, avframe->height, frameRGB->data, frameRGB->linesize);
	}

	*/
	/*
	Mat img1;
	img1 = imread("D:/code/logo_flower.jpg");
	cv::Mat gray;
	cv::cvtColor(img1, gray, cv::COLOR_BGR2GRAY);
	cv::Mat result;
	cv::threshold(gray, result, 240, 255, cv::THRESH_BINARY);
	imwrite("F:/桌面/test/result.png", result);
	*/
	
	/*
	AVFrame* avframe = av_frame_alloc();
	if (avframe && !img1.empty()) {

		avframe->format = AV_PIX_FMT_YUV420P;
		avframe->width = img1.cols;
		avframe->height = img1.rows;
		av_frame_get_buffer(avframe, 0);
		av_frame_make_writable(avframe);
		cv::Mat yuv; // convert to yuv420p first
		cv::cvtColor(img1, yuv, cv::COLOR_BGR2YUV_I420);
		// calc frame size
		int frame_size = img1.cols * img1.rows;
		unsigned char* pdata = yuv.data;
		// fill yuv420
		// yyy yyy yyy yyy
		// uuu
		// vvv
		avframe->data[0] = pdata; // fill y
		avframe->data[1] = pdata + frame_size; // fill u
		avframe->data[2] = pdata + frame_size * 5 / 4; // fill v
		struct SwsContext* img_convert_ctx;
		img_convert_ctx = sws_getContext(avframe->width, avframe->height,
			AV_PIX_FMT_YUV420P, width_s, height_s, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
		AVFrame* frameRGB = av_frame_alloc();
		frameRGB->width = width_s;
		//cout << "width2:" << width;
		frameRGB->height = height_s;
		//cout << "heigth:" << height;
		frameRGB->format = AV_PIX_FMT_RGB24;
		av_frame_get_buffer(frameRGB, 1);

		sws_scale(img_convert_ctx, (const uint8_t* const*)avframe->data,
			avframe->linesize, 0, avframe->height, frameRGB->data, frameRGB->linesize);
		int panelWidth, panelHeight;
		m_panel30->GetSize(&panelWidth, &panelHeight);

		int x = (panelWidth - frameRGB->width), y = (panelHeight - frameRGB->height), w = frameRGB->width, h = frameRGB->height;
		if (x != oX || y != oY || w != oW || h != oH) {
			m_panel30->ClearBackground();
			oX = x;
			oY = y;
			oW = w;
			oH = h;
		}
		wxPaintDC  dc(m_panel30);
		unsigned char* data = frameRGB->data[0];

		wxSize size = wxSize(frameRGB->width, frameRGB->height);

		wxImage* image = new wxImage(size, data);

		wxBitmap* img = nullptr;
		if (imgMirror) {
			img = new wxBitmap(image->Mirror());
		}
		else {
			img = new wxBitmap(*image);
		}

		dc.DrawBitmap(*img, (panelWidth - frameRGB->width) / 2, (panelHeight - frameRGB->height) / 2);
		delete img;
	


	if (originImg == NULL) {
		return;
	}
	int panelWidth, panelHeight;
	m_panel30->GetSize(&panelWidth, &panelHeight);
	wxPaintDC  dc(m_panel30);
		unsigned char* data1 = originImg->data[0];
		//unsigned char* data2 = frameRGB->data[0];
		wxSize size1 = wxSize(originImg->width, originImg->height);
		//wxSize size2 = wxSize(frameRGB->width, frameRGB->height);
		wxImage* image1 = new wxImage(size1, data1);
		//wxImage* image2 = new wxImage(size2, data2);
		wxBitmap* img1 = nullptr;
		//wxBitmap* img2 = nullptr;
		if (imgMirror) {
			img1 = new wxBitmap(image1->Mirror());
		}
		else {
			img1 = new wxBitmap(*image1);
		}
		//if (imgMirror) {
			//img2 = new wxBitmap(image2->Mirror());
		//}
		//else {
			//img2 = new wxBitmap(*image2);
		//}

		dc.DrawBitmap(*img1, (panelWidth - originImg->width) / 2, (panelHeight - originImg->height) / 2);
		//dc.DrawBitmap(*img2, (panelWidth - frameRGB->width) / 2, (panelHeight - frameRGB->height) / 2);
		delete img1;
		//delete img2;
		locker.unlock();
}
	*/
	




//buyaol
	/*
	int x = (panelWidth - originImg->width), y = (panelHeight - originImg->height), w = originImg->width, h = originImg->height;
	if (x != oX || y != oY || w != oW || h != oH) {
		m_panel30->ClearBackground();
		oX = x;
		oY = y;
		oW = w;
		oH = h;
	}
	*/
	
	    
		/*
		if (originImg == NULL ) {
		return;
	}
	int panelWidth, panelHeight;
	m_panel30->GetSize(&panelWidth, &panelHeight);;

	int x = (panelWidth - originImg->width), y = (panelHeight - originImg->height), w = originImg->width, h = originImg->height;
	if (x != oX || y != oY || w != oW || h != oH) {
		m_panel30->ClearBackground();
		oX = x;
		oY = y;
		oW = w;
		oH = h;
	}
	wxPaintDC  dc(m_panel30);
	unsigned char* data = originImg->data[0];

	wxSize size = wxSize(originImg->width, originImg->height);

	wxImage* image = new wxImage(size, data);

	wxBitmap* img = nullptr;
	if (imgMirror) {
		img = new wxBitmap(image->Mirror());
	}
	else {
		img = new wxBitmap(*image);
	}

	//dc.DrawBitmap(*img, (panelWidth - originImg->width) / 2, (panelHeight - originImg->height) / 2);
	delete img;
		*/






		/*
		ifstream fp;
		ofstream r_sat, g_sat, b_sat;
		fp.open("D:/code/my_logo.jpg", ios::in | ios::binary);//打开down.rgb文件，之前使用fopen不行。。。。
		if (!fp)
		{
			cout << "down.rgb open failed" << endl;
			return ;
		}
		unsigned char r[logowidth * logoheight];//创建数组
		unsigned char g[logowidth * logoheight];
		unsigned char b[logowidth * logoheight];
		unsigned char d[logowidth*3 * logoheight];

		double rt[logosize] = { 0 }, gt[logosize] = { 0 }, bt[logosize] = { 0 };//r、g、b每个数值出现的次数

		for (int i = 0; i < logowidth * logoheight; i++)
		{
			fp.read((char*)(b + i), sizeof(unsigned char));//直接从rgb数值中读取r、g、b
			bt[b[i]]++;//该数值出现次数加一
			fp.read((char*)(g + i), sizeof(unsigned char));
			gt[g[i]]++;
			fp.read((char*)(r + i), sizeof(unsigned char));
			rt[r[i]]++;
		}

		for (int i = 0; i < logowidth * logoheight; i++)
		{
			d[i*3] = b[i];
			//cout << "d_r_printf" << r[i] << endl;
		}
		for (int i = 0; i < logowidth * logoheight; i++)
		{
			d[i*3+1] = g[i];
			//cout << "d_g_printf" << r[i] << endl;
		}
		for (int i = 0; i < logowidth * logoheight; i++)
		{
			d[i*3+2] = r[i];
			//cout << "d_b_printf" << r[i] << endl;
		}

		unsigned char* data_logo = d;
			wxSize size_logo = wxSize(logowidth, logoheight);

		wxImage* image_logo = new wxImage(size_logo, data_logo);

		wxBitmap* img_logo = nullptr;
		if (imgMirror) {
			img_logo = new wxBitmap(image_logo->Mirror());
		}
		else {
			img_logo = new wxBitmap(*image_logo);
		}

		dc.DrawBitmap(*img_logo, (panelWidth - logowidth) / 2, (panelHeight - logoheight) / 2);
		delete img_logo;
		*/








/*
void ApolloLivePage::Watchagainfuc(wxCommandEvent& event) {
	if (originfileType == ".mp4" || originfileType == ".flv" || originfileType == ".mkv") {
		//std::cout << originpath;
		backgroundCap->stop();
		backgroundWA->stop();
		backgroundWA->start(originpath);
	}
}

void ApolloLivePage::choosevi(wxFileDirPickerEvent& event)
{
	// TODO: Implement choosePic
	string path = event.GetPath();
	int position = path.find('.');
	string fileType = "";
	if (position == path.npos) {

	}
	else {
		size_t pos = path.find_last_of('.');
		fileType = path.substr(pos);
	}
	/*
	if (fileType == ".png" || fileType == ".PNG") {
		if (backgroundImg != NULL) {
			free(backgroundImg);
			backgroundImg = NULL;
			backgroundWidth = 0;
			backgroundHeight = 0;
		}
		backgroundCap->stop();
		if (PngUtils::readPngRgb(path.c_str(), backgroundWidth, backgroundHeight, backgroundImg)) {
			modnetMtxMask.lock();
			if (modNet != nullptr) {
				if (modNet->getOutputMode() == 2) {
					modnetMtxMask.unlock();
					resetModel(true);
				}
				else {
					modNet->inputMask(backgroundImg, backgroundWidth, backgroundHeight);

					modnetMtxMask.unlock();
				}
			}
			else {
				modnetMtxMask.unlock();
			}

		}
	}
	*/
	/*
	if (fileType == ".mp4" || fileType == ".flv" || fileType == ".mkv") {
			backgroundCap->stop();
			backgroundCap->start(path);
			originpath = path;
			originfileType = fileType;
			//?
		//choseBack->SetPath(backPath);
		//wxMessageBox(wxString("只能选择png格式图片或者mp4、flv、mkv格式的视频", wxConvUTF8), wxT("提示"), wxICON_ERROR, nullptr);
	}

}
	*/
	 







