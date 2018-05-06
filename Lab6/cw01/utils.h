#ifndef __UTILS__
#define __UTILS__

#define CLIENTS_LIMIT 1024
#define PROJECT_ID 999
#define CONTENT_SIZE 512

typedef enum Type {
    LOGIN  = 1,
    MIRROR = 2,
    CALC   = 3,
    TIME   = 4,
    END    = 5,
    STOP   = 6,
    INIT   = 7,
} Type;

typedef struct Message {
    long type;
    pid_t sender_pid;
    char content[CONTENT_SIZE];
} Message;

const size_t MESSAGE_SIZE = sizeof(Message);

#endif
