#ifndef DECODER_H
#define DECODER_H

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libavcodec/avcodec.h>
}

#include <QObject>

class Decoder: public QObject
{
    Q_OBJECT
public:
    Decoder();

public:
    void init();
    void decode(const QByteArray &dataByteArray);
    void decode();

signals:
    void sendFrame(const QPixmap &pixmap);

private:
    void save(unsigned char *buf, int wrap, int xsize, int ysize, char *filename);

private:
    static constexpr size_t inBufSize = 4096;
    const size_t bytesPerLine = 4;
    const AVCodec *codec= nullptr;
    AVCodecParserContext *parser= nullptr;
    AVCodecContext *codecContext = nullptr;
    AVFrame *frame= nullptr;
    char inBuf[inBufSize + AV_INPUT_BUFFER_PADDING_SIZE];
    AVPacket *packet = nullptr;
};

#endif // DECODER_H
