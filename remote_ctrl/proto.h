#ifndef PROTO_H
#define PROTO_H

#include <QObject>
#include <QUdpSocket>

#include <stdint.h>

class proto : public QObject
{
    Q_OBJECT
public:
    explicit proto(QObject *parent = 0);


    void sendWho();
    void sendListValue();
    void sendReadValue(quint32 group, quint32 id);
    void sendWriteValue(quint32 group, quint32 id, QByteArray *buf);
    void sendReadValueLimit(quint32 group, quint32 id, bool isLowLimit);
    void sendGetDescValue(quint32 group, quint32 id);

signals:
    void signalReceived(QByteArray byteArray);
public slots:
    void answerReceived();

private:
    void sendFrame(uint8_t cmd, const char *data, qint16 len);
    void sendDatagram(const char *data, qint16 len);
    QUdpSocket *_sock;
};

#endif // PROTOCOL_H
