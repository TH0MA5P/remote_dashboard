#include <QtEndian>
#include <QNetworkInterface>

#include <stdint.h>

#include "../common/udpProtocol.h"
#include "../common/values.h"
#include "../common/protocol.h"
#include "../common/commands.h"

#include "boardvalue.h"
#include "proto.h"

proto::proto(QObject *parent) : QObject(parent), _sock(new QUdpSocket(this))
{
    _sock->bind();
    connect(_sock, SIGNAL(readyRead()), this, SLOT(answerReceived()));
}

void proto::sendFrame(uint8_t cmd, const char *data, qint16 len)
{
    struct T_ACQ_ETH_CMD frame;
    frame.cmd = cmd;
    frame.ack = 0;
    frame.size = qToBigEndian(len);
    memcpy(frame.data, data, len);
    sendDatagram((const char *)&frame, sizeof(struct T_ACQ_ETH_REPLY) - sizeof(frame.data) + len);
}

void proto::sendDatagram(const char *data, qint16 len)
{
    QList<QNetworkInterface> list_iface = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface iter, list_iface) {
        if(iter.isValid()) {
            foreach(QNetworkAddressEntry addr, iter.addressEntries()) {
                if (((iter.flags() & QNetworkInterface::IsLoopBack) == 0) && (!addr.broadcast().isNull())) {
                    _sock->writeDatagram(data, len, addr.broadcast(), UDP_PORT);
                    qDebug() << "Send command to " << addr.broadcast().toString();
                }
            }
        }
    }
}

void proto::sendWho()
{
    sendFrame(CODE_CMD_WHO, (const char *)NULL, 0);
}

void proto::sendListValue()
{
    sendFrame(CODE_CMD_LIST_VALUES, (const char *)NULL, 0);
}

void proto::sendReadValue(quint32 group, quint32 id)
{
    struct T_READ_VALUE cmd_read;

    cmd_read.id.group = qToBigEndian(group);
    cmd_read.id.name = qToBigEndian(id);

    sendFrame(CODE_CMD_READ_VALUES, (const char *)&cmd_read, sizeof(cmd_read));
}

void proto::sendReadValueLimit(quint32 group, quint32 id, bool isLowLimit)
{
    struct T_READ_VALUE cmd_read;
    uint8_t cmd;

    cmd_read.id.group = qToBigEndian(group);
    cmd_read.id.name = qToBigEndian(id);

    if (isLowLimit)
    {
        cmd = CODE_CMD_READ_VALUES_LIMIT_LOW;
    }
    else
    {
        cmd = CODE_CMD_READ_VALUES_LIMIT_HIGH;
    }

    sendFrame(cmd, (const char *)&cmd_read, sizeof(cmd_read));
}

void proto::sendGetDescValue(quint32 group, quint32 id)
{
    struct T_GET_DESC_VALUE cmd_get_desc;

    cmd_get_desc.id.group = qToBigEndian(group);
    cmd_get_desc.id.name = qToBigEndian(id);

    sendFrame(CODE_CMD_DESC_VALUES, (const char *)&cmd_get_desc, sizeof(cmd_get_desc));
}

void proto::sendWriteValue(quint32 group, quint32 id, QByteArray *buf)
{
    struct T_WRITE_VALUE cmd_write;

    cmd_write.id.group = qToBigEndian(group);
    cmd_write.id.name = qToBigEndian(id);

    memcpy(cmd_write.data, buf->constData(), buf->size());

    sendFrame(CODE_CMD_WRITE_VALUES, (const char *)&cmd_write, sizeof(cmd_write) - sizeof(cmd_write.data) + buf->size());
}

void proto::answerReceived()
{
    while (_sock->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(_sock->pendingDatagramSize());
        QHostAddress board_ip;
        _sock->readDatagram(datagram.data(), datagram.size(), &board_ip);

        emit signalReceived(datagram);
    }
}

