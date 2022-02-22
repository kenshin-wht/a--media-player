

#include"media/AudioDecoder.h"


//AudioDecoder::AudioDecoder() {
    //stopFlag = false;
    //stop();
    //thread Audio{ &AudioDecoder::AudioThreadFunc, this };
    //threadMap["Audio"] = std::move(Audio);
//}

 void AudioDecoder::decoder(string name)
{
    // Դ�ļ�·��
    //const char* src_path = "C:/Users/jhc18/Desktop/1.mp3";
    const char* src_path = "D:/code/IMG_0939.mp4";
    // ������pcm�ļ�·��
    const char* dst_path = "D:/code/test0807.pcm";
    // AVFormatContext ���󴴽�
    avFormatContext = avformat_alloc_context();
    // ����Ƶ�ļ�
    int ret = avformat_open_input(&avFormatContext, name.c_str(), NULL, NULL);
    if (ret != 0) {
        std::cout << "���ļ�ʧ�� ";
        return;
    }
    // �����Ƶ�ļ�����Ϣ
    av_dump_format(avFormatContext, 0, name.c_str(), 0);
    // ��ȡ��Ƶ�ļ�������Ϣ
    ret = avformat_find_stream_info(avFormatContext, NULL);
    if (ret < 0) {
        std::cout << "��ȡ����Ϣʧ�� " << std::endl;
        return;
    }
    // ������Ƶ�����ļ��������������е�λ��
    streamIndex = 0;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        //cout << "***" << endl;
        enum AVMediaType avMediaType = avFormatContext->streams[i]->codecpar->codec_type;
        if (avMediaType == AVMEDIA_TYPE_AUDIO) {  //��ߺ���Ƶ��һ������AUDIO
            //cout << "###" << endl;
            
            streamIndex = i;
        }
    }
    //cout << "streamIndex = " << streamIndex << endl;
    // �õ���Ӧ��Ƶ���Ĳ���
    o_s_rate = avFormatContext->streams[streamIndex]->codecpar->sample_rate;
    int i = avFormatContext->streams[streamIndex]->codecpar->bit_rate;
    //cout << "rate:" << i <<endl;
    AVCodecParameters* avCodecParameters = avFormatContext->streams[streamIndex]->codecpar;
    // ��ȡ�������ı�ʶID
    enum AVCodecID avCodecId = avCodecParameters->codec_id;
    // ͨ����ȡ��ID����ȡ��Ӧ�Ľ�����
    AVCodec* avCodec = avcodec_find_decoder(avCodecId);
    // ����һ�������������Ķ���
    avCodecContext = avcodec_alloc_context3(NULL);
    if (avCodecContext == NULL) {
        //����������������ʧ��
        std::cout << "����������������ʧ�� ";
        return;
    }
    // ���µ�API�е� codecpar ת�� AVCodecContext
    avcodec_parameters_to_context(avCodecContext, avCodecParameters);
    ret = avcodec_open2(avCodecContext, avCodec, NULL);
    if (ret < 0) {
        std::cout << "�򿪽�����ʧ�� ";
        return;
    }
    std::cout << "decodec name:" << avCodec->name;
    // av_log(NULL, AV_LOG_ERROR, "decodec name: %s", avCodec->name);

     //ѹ�����ݰ�
    packet = (AVPacket*)av_malloc(sizeof(AVPacket));
    //��ѹ�����ŵ�����֡�Ķ���
    inFrame = av_frame_alloc();
    //frame->16bit 44100 PCM ͳһ��Ƶ������ʽ�������
    //����swrcontext�����ļ�
    swrContext = swr_alloc();
    //��Ƶ��ʽ  ����Ĳ�������yinp����
    AVSampleFormat inFormat = avCodecContext->sample_fmt;

    // ���������
    int inSampleRate = avCodecContext->sample_rate;
    // ���������
    int outSampleRate = o_s_rate;
    // ������������
    uint64_t in_ch_layout = avCodecContext->channel_layout;
    //�����������
    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;
    //��Swrcontext ����ռ䣬���ù�������
    swr_alloc_set_opts(swrContext, out_ch_layout, outFormat, outSampleRate,
        in_ch_layout, inFormat, inSampleRate, 0, NULL
    );
    // ��ʼ��
    swr_init(swrContext);
    // ��ȡ��������
    outChannelCount = av_get_channel_layout_nb_channels(out_ch_layout);

    currentIndex = 0;
    std::cout << "��������%d " << outChannelCount;
    // ������Ƶ�������� 16bit   44100  PCM����, ˫����
    out_buffer = (uint8_t*)av_malloc(2 * o_s_rate);
    // ����pcm���ļ�����
    //FILE* fp_pcm = fopen(dst_path, "wb");
    //��ʼ��ȡԴ�ļ������н���
    //MediaPlayer* player = new MediaPlayer("", 44100);
    //����******
    stopFlag = false;
    thread audio{ &AudioDecoder::AudioThreadFunc, this };
    threadMap["audio"] = std::move(audio);


    





    /*
    
     int number=0;
    while (av_read_frame(avFormatContext, packet) >= 0) {
        //ѹ����������ݰ�

        if (packet->stream_index == streamIndex) {
            avcodec_send_packet(avCodecContext, packet);
            //����
            ret = avcodec_receive_frame(avCodecContext, inFrame);
            std::cout << ret <<"�ڼ��� "<<++number << endl;
            //�ӽ������л�ȡ������������
            if (ret == 0) {
                //��ÿһ֡����ת����pcm
            //����õ�����������Ϊfloat 4bit��
            //�����������ŵĸ�ʽһ��ΪS16��signed 16bit��,
            //����Ҫ�Խ���õ������ݽ���ת��

                swr_convert(swrContext, &out_buffer, 2 * 44100,
                    (const uint8_t**)inFrame->data, inFrame->nb_samples);
                //��ȡʵ�ʵĻ����С
                out_buffer_size = av_samples_get_buffer_size(NULL, outChannelCount, inFrame->nb_samples, outFormat, 1);
                std::cout << "ret* " << std::endl;
                // д���ļ�
               // fwrite(out_buffer, 1, out_buffer_size, fp_pcm);
                // std::cout << "���ڽ��� " << currentIndex++;

 // ��ʱ�ͷ�
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
