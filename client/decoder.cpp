#include "decoder.h"

#include <QPixmap>
#include <QScreen>
#include <QDebug>

Decoder::Decoder()
{
}

void Decoder::init()
{
    packet = av_packet_alloc();

    if(!packet)
    {
        qDebug() << "Could not init packet";
        return;
    }

    memset(inBuf + inBufSize, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    codec = avcodec_find_decoder(AV_CODEC_ID_H265);

    if(!codec)
    {
        qDebug() << "Codec not found";
        return;
    }

    parser = av_parser_init(codec->id);

    if(!parser)
    {
        qDebug() << "Parser not found";
        return;
    }

    codecContext = avcodec_alloc_context3(codec);

    if(!codecContext)
    {
        qDebug() << "Could not allocate video codec context";
        return;
    }

    if(avcodec_open2(codecContext, codec, NULL) < 0)
    {
        qDebug() << "Could not open codec";
        return;
    }

    frame = av_frame_alloc();

    if (!frame)
    {
        qDebug() << "Could not allocate video frame";
        return;
    }
}

void Decoder::decode(const QByteArray &dataByteArray)
{
    QDataStream currentDataStream;

    if(dataByteArray.isEmpty())
        return;

    int count = 0;

    do
    {
        const int from = inBufSize * count;
        const int to = from + inBufSize;
        QByteArray byteArray = dataByteArray.mid(from, to);

        int dataSize = inBufSize;

        ++count;

        if(dataSize == -1)
            break;

        uint8_t *data = reinterpret_cast<uint8_t*>(byteArray.data());

        while(dataSize > 0)
        {
            int bitstreamBytesNum = av_parser_parse2(parser, codecContext, &packet->data, &packet->size, data, dataSize, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

            if(bitstreamBytesNum < 0)
            {
                qDebug() << "Error while parsing.";
                return;
            }

            data += bitstreamBytesNum;
            dataSize -= bitstreamBytesNum;

            if(packet->size)
                decode();         
        }

        if(byteArray.size() < inBufSize)
            break;
    }
    while(true);
}

void Decoder::decode()
{
    int error = avcodec_send_packet(codecContext, packet);

    if(error < 0)
    {
        qDebug() << "Error sending a packet for decoding.";
        return;
    }

    while(error >= 0)
    {
        error = avcodec_receive_frame(codecContext, frame);

        if (error == AVERROR(EAGAIN) || error == AVERROR_EOF)
            return;
        else if(error < 0)
        {
            qDebug() << "Error during decoding.";
            return;
        }

       emit sendFrame(QPixmap::fromImage(QImage(frame->data[0], frame->width / bytesPerLine, frame->height, QImage::Format_RGB32)));
    }
}
