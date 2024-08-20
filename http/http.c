#include <asm-generic/socket.h>
#include <stdio.h>

#include "http.h"

struct address new_address(uint16_t port) {

    // struct keyword is needed when there is no typedef
    struct sockaddr_in sockaddr;
    
    // ipv4 family
    sockaddr.sin_family = AF_INET;

    // 0.0.0.0
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    // htons = host byte order -> network byte order
    sockaddr.sin_port = htons(port);

    struct address addr;
    addr.size = sizeof(sockaddr);
    addr.addr = sockaddr;

    return addr;
}

int new_socket(struct address * addr) {
    
    // file descriptor: i/o process id for our new socket instance
    int sock_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_file_descriptor == -1) {
        perror("error setting up socket");
        return -1;
    }

    // we can find out about system header files using `man 7 socket`
    // 7 = miscellaneous category 

    int sock_opt_val = 1;
    int sock_opts = setsockopt(
            sock_file_descriptor,
            SOL_SOCKET,
            // | = bitwise or 2 = 0010, 15 = 1111 -> 1111
            // unsure how this doesn't conflict with other 
            // values or if the idea is that they inherit
            // so SO_REUSEPORT would always enable everything below 15 as 1111 | <any> would always be 1111??
            SO_REUSEADDR | SO_REUSEPORT, 
            // set_opt_val is "generic (void *)" so we must provide the size aswell
            &sock_opt_val, 
            sizeof(sock_opt_val));

    if (sock_opts == -1) {
        perror("error setting up socket options");
        return -1;
    }

    int sock_bind = bind(sock_file_descriptor, (struct sockaddr *) &addr->addr, addr->size);
    if (sock_bind < 0) {
        perror("error binding");
        return -1;
    }

    return sock_file_descriptor;
}