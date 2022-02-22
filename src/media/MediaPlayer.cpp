#include"media/MediaPlayer.h"
using std::cout;
using std::endl;

MediaPlayer::MediaPlayer() {}
void MediaPlayer::init(string playerName, int pFreq) {
	freq = pFreq;
	playerDeviceName = playerName;
	//decoder = new MediaDecoder(echo, 22050, 1);

	playerSource = 0;
	if (playerName.length() != 0) {
		playerDevice = alcOpenDevice(playerName.c_str());
	}
	else {
		playerDevice = alcOpenDevice(nullptr);
	}

	if (playerDevice == nullptr) {
		E_LOG("open player device fail");
		return;
	}
	playerContext = alcCreateContext(playerDevice, nullptr);
	if (playerContext == nullptr) {
		E_LOG("create player context fail");
		return;
	}
	alcMakeContextCurrent(playerContext);

	if (checkALError()) {
		return;
	}
	alGenSources(1, &playerSource);
	alGenBuffers(MAX_CACHE, playerBuffer);
	if (checkALError()) {
		return;
	}
	for (auto buf : playerBuffer) {
		playerBufferQueue.push_back(buf);
		//cout << " in gouzao  " << playerBufferQueue.size() << endl;
	}

	I_LOG("MediaPlayer init success,player name:{}", playerName);
}

//void MediaPlayer::setDecoder(MediaDecoder *_decoder) {
//	decoder = _decoder;
//	auto decoderCallback = [&](uint8_t *data, int len) {
//		workData(data, len);
//	};
//
//	decoder->setCallback(decoderCallback);
//}

//void MediaPlayer::play(uint8_t *data, int len, bool useSsrc, uint32_t ssrc, uint32_t timeStamp) {
//	D_LOG("recv play data, len:{}", len);
//	//decoder->process(data, len, useSsrc, ssrc, timeStamp);
//	//workData(data, len);
//}






int MediaPlayer::workData( uint8_t *data, int len) {
//	unique_lock<mutex> locker{ mu };
	//std::unique_lock<std::mutex> locker(playermtx);
	//playermtx.lock();
//	I_LOG("workData1");
	std::this_thread::sleep_for(std::chrono::milliseconds(22));
    recycle();
//    I_LOG("workData2");
	//std::unique_lock<std::mutex> locker(playermtx);
	//cout << " in workdata 1 "  << endl;
    if (data != nullptr && !playerBufferQueue.empty()) {
//        I_LOG("workData while");
		//cout << " in workdata 1 " << playerBufferQueue.size() << endl;
        ALuint buffer = playerBufferQueue.front();
		//返回起始元素
		playerBufferQueue.pop_front();
		alBufferData(buffer, AL_FORMAT_MONO16, data, len, freq);
		alSourceQueueBuffers(playerSource, 1, &buffer);
		bufferIndex = 0;
		//cout << " in workdata 2 " << playerBufferQueue.size() << endl;
		//cout << "state  " << isPlaying()<<endl;
	}

//    I_LOG("workData4");

    resume();
	//playermtx.unlock();
//    I_LOG("workData5");

	//todo 播放结束清空该数据对应的ssrc的用户名称
//	locker.unlock();
//	locker.unlock();
	return 0;
}

/*
int MediaPlayer::getbuffersize()
{
	std::unique_lock<std::mutex> locker(playermtx);
	return playerBuffefrQueue.size();
}
*/


bool MediaPlayer::isPlaying() {
	return playState() == AL_PLAYING;
}

ALint MediaPlayer::playState() {
	ALint playState = 0;
	alGetSourcei(playerSource, AL_SOURCE_STATE, &playState);
	return playState;
}

void MediaPlayer::resume() {
	//std::unique_lock<std::mutex> locker(playermtx);
	//playermtx.lock();
	//cout << " resume 1 " << endl;
	if (!isPlaying()) {
		//检查在队列里面queue的buffer的数量
		alGetSourcei(playerSource, AL_BUFFERS_QUEUED, &bufferQueued);
		//cout << "bufferQueued:  " << bufferQueued << endl;
		if (bufferQueued != 0) {
			alSourcePlay(playerSource);
		}
	}
	//playermtx.unlock();
}

void MediaPlayer::pause() {
	if (isPlaying()) {
		alSourcePause(playerSource);
	}
}

void MediaPlayer::stop() {
	if (isPlaying()) {
		alSourceStop(playerSource);
	}
}

void MediaPlayer::recycle() {
	//std::unique_lock<std::mutex> locker(playermtx);
		  // playermtx.lock();
	//查询已经播放的buffer的数量，bufferProecssed就是已播放完的buffer数
	//cout << " recycle 1 " << endl;
	alGetSourcei(playerSource, AL_BUFFERS_PROCESSED, &proceBufNum);
	if (proceBufNum > 0) {
		ALuint buffers[MAX_CACHE];
		alSourceUnqueueBuffers(playerSource, proceBufNum, buffers);
		for (int i = 0; i < proceBufNum; ++i) {
			playerBufferQueue.push_back(buffers[i]);
		}
		//cout << " in recycle  " << playerBufferQueue.size() << endl;
	}
	//playermtx.unlock();
}

void MediaPlayer::makeALCurrent() {
	alcMakeContextCurrent(playerContext);
}


bool MediaPlayer::checkALError() {
	int loopCnt = 0;
	for (ALenum error = alGetError(); loopCnt < 32 && error; error = alGetError(), ++loopCnt) {
		const char* pMsg;
		switch (error)
		{
		case AL_INVALID_NAME:
			pMsg = "invalid name";
			break;
		case AL_INVALID_ENUM:
			pMsg = "invalid enum";
			break;
		case AL_INVALID_VALUE:
			pMsg = "invalid value";
			break;
		case AL_INVALID_OPERATION:
			pMsg = "invalid operation";
			break;
		case AL_OUT_OF_MEMORY:
			pMsg = "out of memory";
			break;
		default:
			pMsg = "unknown error";
		}
		E_LOG("alGetError:{}", pMsg);
	}
	return loopCnt != 0;
}

void MediaPlayer::run()
{
	while (!isPlaying()) {
		//检查在队列里面queue的buffer的数量
		alGetSourcei(playerSource, AL_BUFFERS_QUEUED, &bufferQueued);
		//cout << "bufferQueued:  " << bufferQueued << endl;
		if (bufferQueued != 0) {
			alSourcePlay(playerSource);
		}
	}
}

MediaPlayer::~MediaPlayer() {
	//std::unique_lock<std::mutex> locker(playermtx);
	if (playerContext != nullptr) {
		alDeleteSources(1, &playerSource);
		playerSource = 0;
		alDeleteBuffers(MAX_CACHE, playerBuffer);
		memset(playerBuffer, 0, sizeof(playerBuffer));
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(playerContext);
		playerContext = nullptr;
		alcCloseDevice(playerDevice);
		playerDevice = nullptr;
		playerBufferQueue.clear();
	}
	//if (decoder != nullptr) {
	//	delete decoder;
	//}
}