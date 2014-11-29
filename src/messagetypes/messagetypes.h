#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H

#include <stdlib.h>
#include <QDataStream>

typedef enum {
    MSGT_CONNECTION_REQUEST,
    MSGT_CONNECTION_ACCEPTED,
    MSGT_CONNECTION_REFUSED,
    MSGT_DISCONNECTED,
    MSGT_PLAYER_DISCONNECTED,
    MSGT_GAME_CREATED,
    MSGT_GAME_JOINED,
    MSGT_GAME_STARTED,
    MSGT_CHAT_MESSAGE } MessageType;

class Message {
public:
    MessageType type;

    virtual QDataStream& operator<<(QDataStream& stream) const = 0;
    virtual QDataStream& operator>>(QDataStream& stream) = 0;
    static Message* readMessage(QDataStream& stream);
};

class SimpleMessage : public Message {
public:
    virtual QDataStream& operator<<(QDataStream& stream) const;
    virtual QDataStream& operator>>(QDataStream& stream);
};

class StringMessage : public Message {
public:
    QString str;

    virtual QDataStream& operator<<(QDataStream& stream) const;
    virtual QDataStream& operator>>(QDataStream& stream);
};

QDataStream& operator<<(QDataStream& stream, const Message& msg);
QDataStream& operator>>(QDataStream& stream, Message& msg);

#endif // MESSAGETYPES_H
