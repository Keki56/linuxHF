#include "messagetypes.h"

Message* Message::readMessage(QDataStream& stream)
{
    Message* result;
    int typeVal;
    MessageType type;
    stream >> typeVal;
    type = (MessageType)typeVal;
    switch (type) {
        case MSGT_CONNECTION_REQUEST:
        case MSGT_PLAYER_DISCONNECTED:
        case MSGT_CHAT_MESSAGE:
            result = new StringMessage();
            result->type = type;
            stream >> *result;
        break;
        case MSGT_CONNECTION_ACCEPTED:
        case MSGT_CONNECTION_REFUSED:
            result = new SimpleMessage();
            result->type = type;
        break;
        default:
            return NULL;
        break;
    }
    return result;
}

QDataStream& SimpleMessage::operator<<(QDataStream& stream) const
{
    return stream;
}

QDataStream& SimpleMessage::operator>>(QDataStream& stream)
{
    return stream;
}


QDataStream& StringMessage::operator<<(QDataStream& stream) const
{
    return (stream << str);
}

QDataStream& StringMessage::operator>>(QDataStream& stream)
{
    return (stream >> str);
}

QDataStream& operator<<(QDataStream& stream, const Message& msg)
{
    return msg.operator<<(stream);
}

QDataStream& operator>>(QDataStream& stream, Message& msg)
{
    return msg.operator>>(stream);
}
