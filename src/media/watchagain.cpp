#include "media/watchagain.h"
using::std::cout;
WatchAgain::WatchAgain()
{
	avdevice_register_all();
}

bool WatchAgain::start(string fileName) {
	width = 640;
	height = 480;
	stopFlag = false;
	if (pFormatCtx != NULL) {
		avformat_free_context(pFormatCtx);
	}
	/*
		if (img_convert_ctx != NULL) {
		sws_freeContext(img_convert_ctx);
		img_convert_ctx = NULL;

	}
	*/


	pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pFormatCtx, fileName.c_str(), NULL, NULL) != 0) {
		printf("could not find video");
		return false;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("could not find stream info\n");
		return false;
	}

	av_dump_format(pFormatCtx, 0, fileName.c_str(), 0);
	for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
		if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStream = pFormatCtx->streams[i];
			I_LOG("video========{}\n", videoStream->codecpar->codec_id);
			videoIndex = i;
		}
	}
	if (videoIndex == -1) {
		printf("not find video or audio index\n");
		return false;
	}



	videoDecoder.setCodec(videoStream->codecpar);


	width = videoDecoder.getWidth();
	cout << "width:" << width;
	height = videoDecoder.getHeight();
	cout << "heigth:" << height;
	img_convert_ctx = sws_getContext(videoDecoder.getWidth(), videoDecoder.getHeight(),
		videoDecoder.getPixFmt(), width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

	int duration = pFormatCtx->duration / AV_TIME_BASE;//求出总时长
	noMoreFrame = false;
	thread video{ &WatchAgain::videoThreadFunc, this };
	threadMap["video"] = std::move(video);





	return true;
}


AVPacket* WatchAgain::grabPkt() {
	while (true)
	{
		AVPacket* inputPkt = av_packet_alloc();
		int ret = av_read_frame(pFormatCtx, inputPkt);
		if (ret < 0) {
			printf("read frame fail:%d\n", ret);
			noMoreFrame = true;
			av_packet_free(&inputPkt);
			return NULL;
		}
		if (inputPkt->stream_index == videoIndex) {
			return inputPkt;
		}
	}
}


void WatchAgain::videoThreadFunc() {
	vector<AVFrame*> vec{};
	int sleep = 1000 / getVideoFrameRate(videoStream);
	I_LOG("sleep:{}", sleep);
	int64_t time = Time::currentTime();
	int64_t temp = time;

	while (!stopFlag)
	{
		int64_t current = Time::currentTime();
		std::this_thread::sleep_for(std::chrono::milliseconds(sleep - (current - temp)));


		AVPacket* pkt = grabPkt();
		if (pkt == NULL) {
			printf("no more frame");
			if (fileEndCallBack != nullptr) {
				fileEndCallBack();
			}
			continue;
		}

		AVFrame* inputFrame = videoDecoder.decode(pkt, videoStream);
		av_packet_free(&pkt);
		if (inputFrame != NULL) {
			AVFrame* frameRGB = av_frame_alloc();
			frameRGB->width = width;
			cout << "width2:" << width;
			frameRGB->height = height;
			cout << "heigth:" << height;
			frameRGB->format = AV_PIX_FMT_RGB24;
			av_frame_get_buffer(frameRGB, 1);

			sws_scale(img_convert_ctx, (const uint8_t* const*)inputFrame->data,
				inputFrame->linesize, 0, inputFrame->height, frameRGB->data, frameRGB->linesize);
			if (dataCallBack != nullptr) {
				dataCallBack(frameRGB);
			}
			av_frame_free(&inputFrame);
			av_frame_free(&frameRGB);
		}
		time = Time::currentTime();
		temp = current;
		
	}
}





void WatchAgain::stop() {
	stopFlag = true;
	auto videoThread = threadMap.find("video");
	if (videoThread != threadMap.end()) {
		if (videoThread->second.joinable()) {
			videoThread->second.join();
		}
	}
}

int WatchAgain::getVideoFrameRate(AVStream* inputVideoStream) {
	int frameRate = 40;
	if (inputVideoStream != nullptr && inputVideoStream->r_frame_rate.den > 0)
	{
		frameRate = inputVideoStream->r_frame_rate.num / inputVideoStream->r_frame_rate.den;
	}
	else if (inputVideoStream != nullptr && inputVideoStream->r_frame_rate.den > 0)
	{

		frameRate = inputVideoStream->r_frame_rate.num / inputVideoStream->r_frame_rate.den;
	}
	return frameRate;
}

WatchAgain::~WatchAgain()
{
	if (pFormatCtx != NULL) {
		avformat_free_context(pFormatCtx);
	}
}