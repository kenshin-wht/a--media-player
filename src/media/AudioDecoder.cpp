

#include"media/AudioDecoder.h"


//AudioDecoder::AudioDecoder() {
    //stopFlag = false;
    //stop();
    //thread Audio{ &AudioDecoder::AudioThreadFunc, this };
    //threadMap["Audio"] = std::move(Audio);
//}

 void AudioDecoder::decoder(string name)
{
    // 源文件路径
    //const char* src_path = "C:/Users/jhc18/Desktop/1.mp3";
    const char* src_path = "D:/code/IMG_0939.mp4";
    // 生产的pcm文件路径
    const char* dst_path = "D:/code/test0807.pcm";
    // AVFormatContext 对象创建
    avFormatContext = avformat_alloc_context();
    // 打开音频文件
    int ret = avformat_open_input(&avFormatContext, name.c_str(), NULL, NULL);
    if (ret != 0) {
        std::cout << "打开文件失败 ";
        return;
    }
    // 输出音频文件的信息
    av_dump_format(avFormatContext, 0, name.c_str(), 0);
    // 获取音频文件的流信息
    ret = avformat_find_stream_info(avFormatContext, NULL);
    if (ret < 0) {
        std::cout << "获取流信息失败 " << std::endl;
        return;
    }
    // 查找音频流在文件的所有流集合中的位置
    streamIndex = 0;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        //cout << "***" << endl;
        enum AVMediaType avMediaType = avFormatContext->streams[i]->codecpar->codec_type;
        if (avMediaType == AVMEDIA_TYPE_AUDIO) {  //这边和视频不一样，是AUDIO
            //cout << "###" << endl;
            
            streamIndex = i;
        }
    }
    //cout << "streamIndex = " << streamIndex << endl;
    // 拿到对应音频流的参数
    o_s_rate = avFormatContext->streams[streamIndex]->codecpar->sample_rate;
    int i = avFormatContext->streams[streamIndex]->codecpar->bit_rate;
    //cout << "rate:" << i <<endl;
    AVCodecParameters* avCodecParameters = avFormatContext->streams[streamIndex]->codecpar;
    // 获取解码器的标识ID
    enum AVCodecID avCodecId = avCodecParameters->codec_id;
    // 通过获取的ID，获取对应的解码器
    AVCodec* avCodec = avcodec_find_decoder(avCodecId);
    // 创建一个解码器上下文对象
    avCodecContext = avcodec_alloc_context3(NULL);
    if (avCodecContext == NULL) {
        //创建解码器上下文失败
        std::cout << "创建解码器上下文失败 ";
        return;
    }
    // 将新的API中的 codecpar 转成 AVCodecContext
    avcodec_parameters_to_context(avCodecContext, avCodecParameters);
    ret = avcodec_open2(avCodecContext, avCodec, NULL);
    if (ret < 0) {
        std::cout << "打开解码器失败 ";
        return;
    }
    std::cout << "decodec name:" << avCodec->name;
    // av_log(NULL, AV_LOG_ERROR, "decodec name: %s", avCodec->name);

     //压缩数据包
    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    //解压缩后存放的数据帧的对象
    inFrame = av_frame_alloc();
    //frame->16bit 44100 PCM 统一音频采样格式与采样率
    //创建swrcontext上下文件
    swrContext = swr_alloc();
    //音频格式  输入的采样设置yinp参数
    AVSampleFormat inFormat = avCodecContext->sample_fmt;

    // 输入采样率
    int inSampleRate = avCodecContext->sample_rate;
    // 输出采样率
    int outSampleRate = o_s_rate;
    // 输入声道布局
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    //输出声道布局
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    //给Swrcontext 分配空间，设置公共参数
    swr_alloc_set_opts(swrContext, out_ch_layout, outFormat, outSampleRate,
        in_ch_layout, inFormat, inSampleRate, 0, NULL
    );
    // 初始化
    swr_init(swrContext);
    // 获取声道数量
    outChannelCount = av_get_channel_layout_nb_channels(out_ch_layout);

    currentIndex = 0;
    std::cout << "声道数量%d " << outChannelCount;
    // 设置音频缓冲区间 16bit   44100  PCM数据, 双声道
    out_buffer = (uint8_t*)av_malloc(2 * o_s_rate);
    // 创建pcm的文件对象
    //FILE* fp_pcm = fopen(dst_path, "wb");
    //开始读取源文件，进行解码
    //MediaPlayer* player = new MediaPlayer("", 44100);
    //测试******
    stopFlag = false;
    thread audio{ &AudioDecoder::AudioThreadFunc, this };
    threadMap["audio"] = std::move(audio);


    





    /*
    
     int number=0;
    while (av_read_frame(avFormatContext, packet) >= 0) {
        //压缩编码的数据包

        if (packet->stream_index == streamIndex) {
            avcodec_send_packet(avCodecContext, packet);
            //解码
            ret = avcodec_receive_frame(avCodecContext, inFrame);
            std::cout << ret <<"第几次 "<<++number << endl;
            //从解码器中获取解码的输出数据
            if (ret == 0) {
                //将每一帧数据转换成pcm
            //解码得到的数据类型为float 4bit，
            //而播放器播放的格式一般为S16（signed 16bit）,
            //就需要对解码得到的数据进行转换

                swr_convert(swrContext, &out_buffer, 2 * 44100,
                    (const uint8_t**)inFrame->data, inFrame->nb_samples);
                //获取实际的缓存大小
                out_buffer_size = av_samples_get_buffer_size(NULL, outChannelCount, inFrame->nb_samples, outFormat, 1);
                std::cout << "ret* " << std::endl;
                // 写入文件
               // fwrite(out_buffer, 1, out_buffer_size, fp_pcm);
                // std::cout << "正在解码 " << currentIndex++;

 // 及时释放
            }
        }
    }
   // fclose(fp_pcm);
    av_frame_free(&inFrame);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
    
    
    */

   
}

void AudioDecoder::free(AVFrame* inFrame, SwrContext* swrContext, 
    AVCodecContext* avCodecContext, AVFormatContext* avFormatContext)
{
    //fclose(fp_pcm);
    av_frame_free(&inFrame);
    //av_free(out_buffer);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);

}

void AudioDecoder::AudioThreadFunc() {
    MediaPlayer player;
    player.init("", o_s_rate *2);
    int ret = 0;
    while (av_read_frame(avFormatContext, packet) >= 0)
    //while (true)
    {

        if (packet->stream_index == streamIndex) {
            //cout << "$$$" << audiodecoder.packet->pts<<endl;
            avcodec_send_packet(avCodecContext, packet);
            ret = avcodec_receive_frame(avCodecContext, inFrame);
            //cout << "receive_ret  " << ret << endl;
            if (ret == 0) {
                swr_convert(swrContext, &out_buffer, 2 * o_s_rate,
                    (const uint8_t**)inFrame->data, inFrame->nb_samples);
                out_buffer_size = av_samples_get_buffer_size(NULL, outChannelCount,
                    inFrame->nb_samples, outFormat, 1);
                //AVRational time_base;
               
               // cout << "clock2:" << clock << endl;
                //0cout << "step_three" << audiodecoder.out_buffer_size << endl;
                player.workData(out_buffer, out_buffer_size);
                if (packet->pts != AV_NOPTS_VALUE) {
                    clock = av_q2d(avCodecContext->time_base) * packet->pts;
                }
                double time = out_buffer_size / ((double)o_s_rate * 2 * 2);
                clock = clock + time;
            }
         }
          
       
          //cout << "printf" << endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(23));
        

    }

    av_frame_free(&inFrame);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
}

void AudioDecoder::stop() {
    stopFlag = true;
    auto audioThread = threadMap.find("audio");
    if (audioThread != threadMap.end()) {
        if (audioThread->second.joinable()) {
            audioThread->second.join();
        }
    }
}

void AudioDecoder::run(MediaPlayer player)
{
    //decodermtx.lock();
    //std::unique_lock<std::mutex> locker(decodermtx);
    //decoder();
    while (av_read_frame(avFormatContext, packet) >= 0)
    {
        if (packet->stream_index == streamIndex) {
            avcodec_send_packet(avCodecContext, packet);
            int ret1 = avcodec_receive_frame(avCodecContext, inFrame);
            if (ret1 == 0) {
                swr_convert(swrContext, &out_buffer, 2 * o_s_rate,
                    (const uint8_t**)inFrame->data, inFrame->nb_samples);
                out_buffer_size = av_samples_get_buffer_size(NULL, outChannelCount,
                    inFrame->nb_samples, outFormat, 1);
                //cout << "decode success" << endl;
            }
            //else cout << "decode unsuccess" << endl;
        }
        player.workData(out_buffer,out_buffer_size);
        //std::this_thread::sleep_for(std::chrono::milliseconds(23));
    }
    //decodermtx.unlock();
    free(inFrame, swrContext,avCodecContext,avFormatContext);
}


AudioDecoder::~AudioDecoder(){
    if (codecCtx != NULL) 
    {avcodec_free_context(&codecCtx);}
}
