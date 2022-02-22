#include "media/listshowvideo.h"
using::std::cout;
ListShowVideo::ListShowVideo()
{
	avdevice_register_all();
}
/*
void ListShowVideo::decodenew()
{

	// 源文件路径
	//const char* src_path = "C:/Users/jhc18/Desktop/1.mp3";
	const char* src_path = "D:/code/IMG_0939.mp4";
	// 生产的pcm文件路径
	const char* dst_path = "D:/code/test0807.pcm";
	// AVFormatContext 对象创建
	audio.avFormatContext = avformat_alloc_context();
	// 打开音频文件
	int ret = avformat_open_input(&audio.avFormatContext, src_path, NULL, NULL);
	if (ret != 0) {
		std::cout << "打开文件失败 ";
		return;
	}
	// 输出音频文件的信息
	av_dump_format(audio.avFormatContext, 0, src_path, 0);
	// 获取音频文件的流信息
	ret = avformat_find_stream_info(audio.avFormatContext, NULL);
	if (ret < 0) {
		std::cout << "获取流信息失败 " << std::endl;
		return;
	}
	// 查找音频流在文件的所有流集合中的位置
	audio.streamIndex = 0;
	for (int i = 0; i < audio.avFormatContext->nb_streams; ++i) {
		//cout << "***" << endl;
		enum AVMediaType avMediaType = audio.avFormatContext->streams[i]->codecpar->codec_type;
		if (avMediaType == AVMEDIA_TYPE_AUDIO) {  //这边和视频不一样，是AUDIO
			//cout << "###" << endl;
			audio.streamIndex = i;
		}
	}
	cout << "streamIndex = " << audio.streamIndex << endl;
	// 拿到对应音频流的参数
	AVCodecParameters* avCodecParameters = audio.avFormatContext->streams[audio.streamIndex]->codecpar;
	// 获取解码器的标识ID
	enum AVCodecID avCodecId = avCodecParameters->codec_id;
	// 通过获取的ID，获取对应的解码器
	AVCodec* avCodec = avcodec_find_decoder(avCodecId);
	// 创建一个解码器上下文对象
	audio.avCodecContext = avcodec_alloc_context3(NULL);
	if (audio.avCodecContext == NULL) {
		//创建解码器上下文失败
		std::cout << "创建解码器上下文失败 ";
		return;
	}
	// 将新的API中的 codecpar 转成 AVCodecContext
	avcodec_parameters_to_context(audio.avCodecContext, avCodecParameters);
	ret = avcodec_open2(audio.avCodecContext, avCodec, NULL);
	if (ret < 0) {
		std::cout << "打开解码器失败 ";
		return;
	}
	std::cout << "decodec name:" << avCodec->name;
	// av_log(NULL, AV_LOG_ERROR, "decodec name: %s", avCodec->name);

	 //压缩数据包
	audio.packet = (AVPacket*)av_malloc(sizeof(AVPacket));
	//解压缩后存放的数据帧的对象
	audio.inFrame = av_frame_alloc();
	//frame->16bit 44100 PCM 统一音频采样格式与采样率
	//创建swrcontext上下文件
	audio.swrContext = swr_alloc();
	//音频格式  输入的采样设置yinp参数
	AVSampleFormat inFormat = audio.avCodecContext->sample_fmt;

	// 输入采样率
	int inSampleRate = audio.avCodecContext->sample_rate;
	// 输出采样率
	int outSampleRate = 44100;
	// 输入声道布局
	uint64_t in_ch_layout = audio.avCodecContext->channel_layout;
	//输出声道布局
	uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
	//给Swrcontext 分配空间，设置公共参数
	swr_alloc_set_opts(audio.swrContext, out_ch_layout, audio.outFormat, outSampleRate,
		in_ch_layout, inFormat, inSampleRate, 0, NULL
	);
	// 初始化
	swr_init(audio.swrContext);
	// 获取声道数量
	audio.outChannelCount = av_get_channel_layout_nb_channels(out_ch_layout);

	audio.currentIndex = 0;
	std::cout << "声道数量%d " << audio.outChannelCount;
	// 设置音频缓冲区间 16bit   44100  PCM数据, 双声道
	audio.out_buffer = (uint8_t*)av_malloc(2 * 44100);
	// 创建pcm的文件对象
	//FILE* fp_pcm = fopen(dst_path, "wb");
	//开始读取源文件，进行解码
	//MediaPlayer* player = new MediaPlayer("", 44100);
	//测试******
	stopFlag = false;
	thread audio{ &AudioDecoder::AudioThreadFunc, this };
	threadMap["audio"] = std::move(audio);
}
*/


bool ListShowVideo::start(string fileName) {
	int ret = 0;
	av_register_all();
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
	pCodecCtx = pFormatCtx->streams[videoIndex]->codec;
	



	videoDecoder.setCodec(videoStream->codecpar);
	/*
	if (avcodec_open2(pCodecCtx, dec, NULL) != 0) {
		printf("codec open fail\n");
		return false;
	}
	*/
	
	//ret=init_filters("movie=\\'D\\:/code/my_logo.png\\'[wm];[in][wm]overlay=5:5[out]");
	//cout << "init_filters" << endl;
	

	int duration = pFormatCtx->duration / AV_TIME_BASE;//求出总时长
	noMoreFrame = false;
	//decodenew();
	audio.decoder(fileName);
	thread video{ &ListShowVideo::videoThreadFunc, this };
	threadMap["video"] = std::move(video);
	return true;
}


AVPacket* ListShowVideo::grabPkt() {
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
			//cout << "actual_pkt" << endl;
			return inputPkt;
		}
	}
}


void ListShowVideo::videoThreadFunc() {
	vector<AVFrame*> vec{};
	

	//int sleep = 1600 / getVideoFrameRate(videoStream);//1000帧所需要的时间？
	//I_LOG("sleep:{}", sleep);
	//int64_t time = Time::currentTime();
	//int64_t temp = time;

	int got_frame=0;
	int ret = 0;
	AVFrame* pFrame = av_frame_alloc();
	outputFrame = av_frame_alloc();
	while (!stopFlag)
		//while (1)
	{

		//std::this_thread::sleep_for(std::chrono::milliseconds(200));
		//int64_t current = Time::currentTime();
		//std::this_thread::sleep_for(std::chrono::milliseconds(sleep - (current - temp)));
		/*
		AVPacket* pkt = grabPkt();
		if (pkt == NULL) {
			printf("no more frame");
			if (fileEndCallBack != nullptr) {
				fileEndCallBack();
			}
			continue;
		}
		*/
		pkt = grabPkt();
		//ret = av_read_frame(pFormatCtx, pkt);
		//cout << "new_ret" << ret << endl;
		//if (ret < 0)
			//break;
		AVFrame* inputFrame = videoDecoder.decode(pkt, videoStream);
		/*
			if (pkt->stream_index == videoIndex) {
			cout << "**********" << endl;
			got_frame = 0;
			ret = avcodec_decode_video2(videoDecoder.codecCtx, pFrame, &got_frame, pkt);
			cout << got_frame << endl;
			cout << "*****ret*****" << ret << endl;
			if (ret < 0) {
				printf("Error decoding video\n");
				break;
			}
		}
		*/
		//cout << got_frame << endl;
	



			
			        start_time = av_gettime() / 1000000.0;
					if ((pts = av_frame_get_best_effort_timestamp(inputFrame)) == AV_NOPTS_VALUE) {
						pts = 0;
					}
					play = pts * av_q2d(videoStream->time_base);
					play = synchronize(inputFrame, play);
					delay = play - last_play;
					if (delay <= 0 || delay > 1) {
						delay = last_delay;
					}
					audio_clock = audio.clock;
					last_play = videoDecoder.play;
					//音频与视频的时间差
					last_delay = delay;
					diff = clock - audio_clock;
					//在合理范围外  才会延迟  加快
					sync_threshold = (delay > 0.01 ? 0.01 : delay);
					if (fabs(diff) < 10) {
						if (diff <= -sync_threshold) {
							delay = 0;
						}
						else if (diff >= sync_threshold) {
							delay = 2 * delay;
						}
					}
					start_time += delay;
					actual_delay = start_time - av_gettime() / 1000000.0;
					if (actual_delay < 0.01) {
						actual_delay = 0.01;
					}
					av_usleep(actual_delay * 10000.0 + 6000);
			

/*
//if (got_frame) {

				inputFrame->pts = av_frame_get_best_effort_timestamp(inputFrame);
				if (av_buffersrc_add_frame(buffersrc_ctx, inputFrame) < 0) {
					printf("Error while feeding the filtergraph\n");
					//cout << "prs_error" << av_buffersrc_add_frame_flags(buffersrc_ctx, pFrame, AV_BUFFERSRC_FLAG_NO_CHECK_FORMAT) << endl;
					//break;
				}
*/
			
				
				

				// pull filtered pictures from the filtergraph 
				//while (1) {

					/*
					ret = av_buffersink_get_frame_flags(buffersink_ctx, outputFrame,0);
					cout << "#####" << endl;
					if (ret < 0)
						break;
					//	cout << "process_error" << ret << endl;
					printf("Process 1 frame!\n");
					*/
					

					//**************************************


					width = videoDecoder.getWidth();
					height = videoDecoder.getHeight();
					img_convert_ctx = sws_getContext(videoDecoder.getWidth(), videoDecoder.getHeight(),
						videoDecoder.getPixFmt(), width, height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
					if (inputFrame != NULL) {
						//cout << "****" << endl;
						AVFrame* frameRGB = av_frame_alloc();
						frameRGB->width = width;
						//cout << "width2:" << width;
						frameRGB->height = height;
						//cout << "heigth:" << height;
						frameRGB->format = AV_PIX_FMT_RGB24;
						av_frame_get_buffer(frameRGB, 1);

						sws_scale(img_convert_ctx, (const uint8_t* const*)inputFrame->data,
							inputFrame->linesize, 0, inputFrame->height, frameRGB->data, frameRGB->linesize);
						if (dataCallBack != nullptr) {
							dataCallBack(frameRGB);
						}
						//std::this_thread::sleep_for(std::chrono::milliseconds(20));
						av_frame_free(&frameRGB);
					}

				//}
			//}

		


		//}
	}
	av_frame_free(&pFrame);
	av_packet_free(&pkt);
	av_frame_free(&outputFrame);

}


double ListShowVideo::synchronize(AVFrame* frame, double play) {
		//clock是当前播放的时间位置
		if (play != 0)
			clock = play;
		else //pst为0 则先把pts设为上一帧时间
			play = clock;
		//可能有pts为0 则主动增加clock
		//需要求出扩展延时：
		double repeat_pict = frame->repeat_pict;
		//使用AvCodecContext的而不是stream的
		double frame_delay = av_q2d(videoDecoder.codecCtx->time_base);
		//fps 
		double fps = 1 / frame_delay;
		//pts 加上 这个延迟 是显示时间  
		double extra_delay = repeat_pict / (2 * fps);
		double delay = extra_delay + frame_delay;
		clock += delay;
		return play;
}


 int ListShowVideo::init_filters(const char* filters_descr)
{
	 avfilter_register_all();
	char args[512];
	int ret;
	const AVFilter* buffersrc=avfilter_get_by_name("buffer");
	const AVFilter* buffersink = avfilter_get_by_name("buffersink");
	AVFilterInOut* outputs = avfilter_inout_alloc();
	AVFilterInOut* inputs = avfilter_inout_alloc();
	enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
	AVBufferSinkParams* buffersink_params;

	filter_graph = avfilter_graph_alloc();

	/* buffer video source: the decoded frames from the decoder will be inserted here. */
	snprintf(args, sizeof(args),
		"video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
		videoDecoder.codecCtx->width, videoDecoder.codecCtx->height, videoDecoder.codecCtx->pix_fmt,
		videoDecoder.codecCtx->time_base.num, videoDecoder.codecCtx->time_base.den,
		videoDecoder.codecCtx->sample_aspect_ratio.num, videoDecoder.codecCtx->sample_aspect_ratio.den);

	ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
		args, NULL, filter_graph);
	if (ret < 0) {
		printf("Cannot create buffer source\n");
		return ret;
	}

	/* buffer video sink: to terminate the filter chain. */
	buffersink_params = av_buffersink_params_alloc();
	buffersink_params->pixel_fmts = pix_fmts;
	ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
		NULL, buffersink_params, filter_graph);
	av_free(buffersink_params);
	if (ret < 0) {
		printf("Cannot create buffer sink\n");
		return ret;
	}

	/* Endpoints for the filter graph. */
	outputs->name = av_strdup("in");
	outputs->filter_ctx = buffersrc_ctx;
	outputs->pad_idx = 0;
	outputs->next = NULL;

	inputs->name = av_strdup("out");
	inputs->filter_ctx = buffersink_ctx;
	inputs->pad_idx = 0;
	inputs->next = NULL;

	if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
		&inputs, &outputs, NULL)) < 0)
		printf("error1\n");
		return ret;

	if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
		printf("error2\n");
		return ret;
	return 0;
}




void ListShowVideo::stop() {
	stopFlag = true;
	auto videoThread = threadMap.find("video");
	//auto audioThread = threadMap.find("audio");
	if (videoThread != threadMap.end()) {
		if (videoThread->second.joinable()) {
			videoThread->second.join();
		}
	}
	//if (audioThread != threadMap.end()) {
		//if (audioThread->second.joinable()) {
		//	audioThread->second.join();
		//}
	//}
}

int ListShowVideo::getVideoFrameRate(AVStream* inputVideoStream) {
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

ListShowVideo::~ListShowVideo()
{
	if (pFormatCtx != NULL) {
		avformat_free_context(pFormatCtx);
	}
}