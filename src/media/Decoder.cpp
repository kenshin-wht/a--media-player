
#include<iostream>
#include"media/Decoder.h"


Decoder::Decoder() {
	
}

void Decoder::setCodec(AVCodecParameters *codecpar) {
	if (codecCtx != NULL) {
		avcodec_free_context(&codecCtx);
	}
	codec = avcodec_find_decoder(codecpar->codec_id);
	if (codec == NULL) {
		printf("open codec fail\n");
		return;
	}
	codecCtx = avcodec_alloc_context3(NULL);
	if (codecCtx == NULL) {
		printf("allow codec context fail\n");
		codec = NULL;
		return;
	}
	if ((avcodec_parameters_to_context(codecCtx, codecpar)) < 0) {
		fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
			av_get_media_type_string(AVMEDIA_TYPE_AUDIO));
		if (codecCtx != NULL) {
			avcodec_free_context(&codecCtx);
		}
	}
	if (avcodec_open2(codecCtx, codec, NULL) != 0) {
		printf("codec open fail\n");
		return;
	}
}

AVFrame * Decoder::decode(AVPacket *pkt, AVStream* stream) {
	int ret = avcodec_send_packet(codecCtx, pkt);
	if (ret < 0) {
		E_LOG("Error submitting the packet to the decoder\n");
		return NULL;
	}

	/* read all the output frames (in general there may be any number of them */

	AVFrame *d_frame = av_frame_alloc();
	//AVFrame* outputFrame = av_frame_alloc();
	ret = avcodec_receive_frame(codecCtx, d_frame);
	/*
	if (av_buffersrc_add_frame(buffersrc_ctx, d_frame) < 0) {
		printf("Error while feeding the filtergraph\n");
		//return;
	}

	// pull filtered pictures from the filtergraph 
	//while (1) {

	int ret = av_buffersink_get_frame(buffersink_ctx, outputFrame);
	//if (ret < 0)
		//return;
	*/
	
	return d_frame;
	/*
	if ((pts = av_frame_get_best_effort_timestamp(d_frame)) == AV_NOPTS_VALUE) {
		pts = 0;
	}

	play = pts * av_q2d(stream->time_base);
	*/
	
	//        纠正时间
	
	/*
	start_time = av_gettime() / 1000000.0;
		       //获取pts
		if ((pts = av_frame_get_best_effort_timestamp(d_frame)) == AV_NOPTS_VALUE) {
			pts = 0;
		}

		play = pts * av_q2d(stream->time_base);
		//        纠正时间
		play = synchronize(d_frame, play);
		delay = play - last_play;
		if (delay <= 0 || delay > 1) {
			delay = last_delay;
		}
		audio_clock = audio.clock;
		last_delay = delay;
		last_play = play;
		//音频与视频的时间差
		diff = clock - audio_clock;
		//        在合理范围外  才会延迟  加快
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

		//  休眠时间 ffmpeg 建议这样写  为什么 要这样写 有待研究
		av_usleep(actual_delay * 1000000.0 + 6000);
	*/
	/*
	if (ret == 0) {
		return d_frame;
	}
	else {
		printf("Error submitting the packet to the decoder 22:%d \n", ret);
		return NULL;
	}
	*/
	
		
}
/*
double Decoder::synchronize(AVFrame* frame, double play) {
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
*/


Decoder::~Decoder()
{
	if (codecCtx != NULL) {
		avcodec_free_context(&codecCtx);
	}
}