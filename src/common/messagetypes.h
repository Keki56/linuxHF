#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H

typedef enum {
    MSGT_CONNECTION_REQUEST,
    MSGT_CONNECTION_ACCEPTED,
    MSGT_CONNECTION_REFUSED,
    MSGT_DISCONNECTED,
    MSGT_PLAYER_DISCONNECTED,
    MSGT_GAME_CREATED,
    MSGT_GAME_JOINED,
    MSGT_GAME_STARTED } message_type;

struct Message {
    message_type type;
    size_t length;
};

#endif // MESSAGETYPES_H
