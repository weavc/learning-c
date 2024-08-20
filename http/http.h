#ifndef HTTP_H
#define HTTP_H

#include <sys/socket.h>
#include <netinet/in.h>

// #define can be used as constants, can be used for functions aswell.
// Good to use for macros / regular ops
#define DEFAULT_PORT 8080

struct address {
    struct sockaddr_in addr;
    socklen_t size;
};

struct address new_address(uint16_t port);
int new_socket(struct address * addr);

#endif