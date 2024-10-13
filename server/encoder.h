#ifndef ENCODER_H
#define ENCODER_H

extern "C" {
#include <libavcodec/avcodec.h>
#include "libavformat/avformat.h"

#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#include <QGuiApplication>
#include <QScreen>
#include <QByteArray>
#include <QImage>

class Encoder
{
public:
    ~Encoder();

public:
    bool init();
    QByteArray encode();

private:
    void encodeFrame(AVCodecContext *codecContext, AVPacket* packet, AVFrame *frame = nullptr);
    const AVCodec* createCodec() const;
    AVCodecContext* createCodecContext(const AVCodec * const codec, qsizetype width, qsizetype height) const;
    AVPacket* createPacket() const;
    AVFrame* createFrame(AVPixelFormat pixelFormat, qsizetype width, qsizetype height) const;
    bool compareScreens(const QImage &currentScreenImage) const;

private:
    QScreen *const screen = QGuiApplication::primaryScreen();
    //QScreen *const screen = QGuiApplication::screens()[1];
    QImage previousScreenImage;
    const AVCodec *codec = nullptr;
    AVFrame* frame = nullptr;
    QByteArray block;
    quint64 frameCount = 0;
    const int minFramesPerPackage = 3;
    int sizeOfPixel = 0;
    int framesChunksCount = 0;
    int chunkSize = 0;
};

#endif // ENCODER_H
