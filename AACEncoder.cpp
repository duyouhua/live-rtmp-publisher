#include "AACEncoder.h"

AACEncoder::AACEncoder(int sampleRate, int channals, int bitrate) {
    mEncoder = faacEncOpen(sampleRate, channals, &mMaxSample, &mBufSize);

    if (mEncoder != NULL) {
        faacEncConfigurationPtr conf = faacEncGetCurrentConfiguration(mEncoder);
        conf->inputFormat = FAAC_INPUT_16BIT;
        conf->outputFormat = 1;
        conf->aacObjectType = LOW;
        conf->allowMidside = 0;
        conf->useLfe = 0;
        conf->bitRate = bitrate;
        conf->bandWidth = 32000;
        faacEncSetConfiguration(mEncoder, conf);  

        mDataBuf = new char[mBufSize];
    }
}

AACEncoder::~AACEncoder() {
    if (mEncoder != NULL) {
        faacEncClose(mEncoder);  
        delete[] mDataBuf;
    }
}
    
std::pair<int, char*> AACEncoder::encode(int sample, char* data) {
    if (mEncoder == NULL || (int) mMaxSample < sample) {
        return std::make_pair<int, char*>(-1, NULL);
    }

    int size = faacEncEncode(mEncoder, reinterpret_cast<int*>(data), sample, reinterpret_cast<unsigned char*>(mDataBuf), mBufSize);

    if (size < 0) {
        return std::make_pair<int, char*>(-1, NULL);
    }

    return std::make_pair(size, mDataBuf);
}

std::pair<int, char*> AACEncoder::getMetadata() {
    unsigned char *buf;
    unsigned long size;

    faacEncGetDecoderSpecificInfo(mEncoder, &buf, &size);
    memcpy(mDataBuf, buf, size);

    delete[] buf;

    return std::make_pair(size, mDataBuf);
}