#include "encoder.h"

#include <QIODevice>
#include <QPixmap>
#include <QDebug>

Encoder::~Encoder()
{
    av_frame_free(&frame);
}

bool Encoder::init()
{
    const auto image = screen->grabWindow().toImage();
    const auto screenSize = image.size();

    sizeOfPixel = image.bytesPerLine() / screenSize.width();

    codec = createCodec();

    if (!codec)
    {
        qDebug() << "Codec not found codec.";
        return false;
    }

    frame = createFrame(AV_PIX_FMT_YUV420P, image.bytesPerLine(), image.height());

    if(!frame)
    {
        qDebug() << "Could not allocate the video frame data.";
        return false;
    }

    return true;
}

const AVCodec* Encoder::createCodec() const
{
    return avcodec_find_encoder(AV_CODEC_ID_H265);
}

AVCodecContext* Encoder::createCodecContext(const AVCodec* const codec, qsizetype width, qsizetype height) const
{
    AVCodecContext *const codecContext = avcodec_alloc_context3(codec);

    if(!codecContext)
        return nullptr;

    codecContext->bit_rate = 400000;
    codecContext->width = width;
    codecContext->height = height;
    codecContext->time_base = (AVRational) { 1, 25 };
    codecContext->framerate = (AVRational) { 25, 1 };

    codecContext->gop_size = 10;
    codecContext->max_b_frames = 1;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    int ret = avcodec_open2(codecContext, codec, NULL);

    if(ret < 0)
        return nullptr;

    return codecContext;
}

AVPacket* Encoder::createPacket() const
{
    AVPacket* const packet = av_packet_alloc();

    if (!packet)
        return nullptr;

    return packet;
}

AVFrame* Encoder::createFrame(AVPixelFormat pixelFormat, qsizetype width, qsizetype height) const
{
    AVFrame* const frame = av_frame_alloc();

    if(!frame)
    {
        qDebug() << "Could not allocate video frame";
        return nullptr;
    }

    frame->format = pixelFormat;
    frame->width  = width;
    frame->height = height;

    int ret = av_frame_get_buffer(frame, 0);

    if (ret < 0)
        return nullptr;

    ret = av_frame_make_writable(frame);

    if (ret < 0)
        return nullptr;

    return frame;
}

QByteArray Encoder::encode()
{
    const QImage &screenImage = screen->grabWindow().toImage();

    if(!previousScreenImage.isNull() && compareScreens(screenImage))
        return {};

    previousScreenImage = screenImage;

    block.clear();

    AVCodecContext *codecContext = createCodecContext(codec, screenImage.bytesPerLine(), screenImage.height());


    if(!codecContext)
    {
        qDebug() << "Could not create codec context.";
        return {};
    }

    AVPacket* packet = createPacket();

    if(!packet)
    {
        qDebug() << "Could not init packet";
        return {};
    }

    memcpy(frame->data[0], screenImage.bits(), screenImage.sizeInBytes());

    frame->pts = frameCount++;

    for(int i = 0; i < minFramesPerPackage; ++i)
        encodeFrame(codecContext, packet, frame);

    encodeFrame(codecContext, packet);

    avcodec_free_context(&codecContext);
    av_packet_free(&packet);

    return block;
}

void Encoder::encodeFrame(AVCodecContext *codecContext, AVPacket* packet, AVFrame* frame)
{
    int ret = avcodec_send_frame(codecContext, frame);

    if(ret < 0)
    {
        qDebug() << "Error sending a frame for encoding";
        return;
    }

    while(ret >= 0)
    {
        ret = avcodec_receive_packet(codecContext, packet);

        if(ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if(ret < 0)
        {
            qDebug() << "Error during encoding";
            return;
        }

        block.append(reinterpret_cast<const char*>(packet->data), packet->size);

        av_packet_unref(packet);
    }
}

bool Encoder::compareScreens(const QImage &currentScreenImage) const
{
    if(previousScreenImage.size() != currentScreenImage.size())
        return false;

    for(int y = 0; y < previousScreenImage.height(); ++y)
    {
        for (int x = 0; x < previousScreenImage.width(); ++x)
        {
            if (previousScreenImage.pixel(x, y) != currentScreenImage.pixel(x, y))
                return false;
        }
    }

    return true;
}
