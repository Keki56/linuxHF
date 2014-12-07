#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H

#include <stdlib.h>
#include <QDataStream>

//Kíváncsi komment: Miért nem tettük a MessageType-t a Message osztályon belülre. Miért jó, hogy a globális scope-ban létezik?
enum MessageType {
    MSGT_CONNECTION_REQUEST,
    MSGT_CONNECTION_ACCEPTED,
    MSGT_CONNECTION_REFUSED,
    MSGT_PLAYER_DISCONNECTED,
    MSGT_PLAYER_MOVED,
    MSGT_GAME_CREATED,
    MSGT_GAME_REMOVED,
    MSGT_GAME_STARTED,
    MSGT_GAME_CLOSED,
    MSGT_NEW_GAME,
    MSGT_JOIN_GAME,
    MSGT_CHAT_MESSAGE
};

/**
 * @brief The abstract base class of all message packets.
 */
class Message {
public:
    MessageType type;

    virtual QDataStream& operator<<(QDataStream& stream) const = 0;
    virtual QDataStream& operator>>(QDataStream& stream) = 0;
    static Message* readMessage(QDataStream& stream);

    virtual ~Message() {}
};

/**
 * @brief A message packet without any additional fields.
 */
class SimpleMessage : public Message {
public:
    virtual QDataStream& operator<<(QDataStream& stream) const;
    virtual QDataStream& operator>>(QDataStream& stream);
};

/**
 * @brief A message packet with a single string field.
 */
class StringMessage : public Message {
public:
    QString str;

    virtual QDataStream& operator<<(QDataStream& stream) const;
    virtual QDataStream& operator>>(QDataStream& stream);
};

/**
 * @brief A message packer with four double fields. | Általánosan megírhatnánk úgy is, hogy egy tetszőleges hosszúságú tömböt vigyen át, de elsőre így is jó.
 */
class QuadDoubleMessage : public Message {
public:
    double data[4];

    virtual QDataStream& operator<<(QDataStream& stream) const;
    virtual QDataStream& operator>>(QDataStream& stream);
};

QDataStream& operator<<(QDataStream& stream, const Message& msg);
QDataStream& operator>>(QDataStream& stream, Message& msg);

#endif // MESSAGETYPES_H
