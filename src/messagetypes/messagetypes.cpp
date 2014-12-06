#include "messagetypes.h"

/**
 * @brief Message::readMessage Read a message packet from a stream.
 * @param stream The stream to read from.
 * @return A dynamically constructed message packet: the caller is responsible for deleting.
 */
Message* Message::readMessage(QDataStream& stream) {
    Message* result;
    int typeVal;
    MessageType type;
    stream >> typeVal;
    type = (MessageType)typeVal;
    switch (type) {
        case MSGT_CONNECTION_REQUEST:
        case MSGT_PLAYER_DISCONNECTED:
        case MSGT_CHAT_MESSAGE:
        case MSGT_GAME_CREATED:
        case MSGT_GAME_REMOVED:
        case MSGT_GAME_STARTED:
        case MSGT_NEW_GAME:
        case MSGT_JOIN_GAME:
            result = new StringMessage();
            result->type = type;
            stream >> *result;
        break;
        case MSGT_CONNECTION_ACCEPTED:
        case MSGT_CONNECTION_REFUSED:
        case MSGT_GAME_CLOSED:
            result = new SimpleMessage();
            result->type = type;
        break;
        default:
            return NULL;
        break;
    }
    return result;
}

QDataStream& SimpleMessage::operator<<(QDataStream& stream) const {
    return (stream << type);
}

QDataStream& SimpleMessage::operator>>(QDataStream& stream) {
    return stream;
}


QDataStream& StringMessage::operator<<(QDataStream& stream) const {
    return (stream << type << str);
}

QDataStream& StringMessage::operator>>(QDataStream& stream) {
    return (stream >> str);
}

QDataStream& operator<<(QDataStream& stream, const Message& msg) {
    return msg.operator<<(stream);
}

QDataStream& operator>>(QDataStream& stream, Message& msg) {
    return msg.operator>>(stream);
}
