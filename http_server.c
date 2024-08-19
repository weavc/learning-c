#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <asm-generic/socket.h>

// we can find out about system header files using `man 7 socket`
// 7 = miscellaneous category 

// #define can be used as constants, can be used for functions aswell.
// Good to use for macros / regular ops
#define PORT 8080

struct address {
    struct sockaddr_in addr;
    socklen_t size;
};

static struct address get_address() {

    // struct keyword is needed when there is no typedef
    struct sockaddr_in sockaddr;
    
    // ipv4 family
    sockaddr.sin_family = AF_INET;

    // 0.0.0.0
    sockaddr.sin_addr.s_addr = INADDR_ANY;

    // htons = host byte order -> network byte order
    sockaddr.sin_port = htons(PORT);

    struct address addr;
    addr.size = sizeof(sockaddr);
    addr.addr = sockaddr;

    return addr;
}

static int get_server(struct address * addr) {
    
    // file descriptor: i/o process id for our new socket instance
    int sock_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_file_descriptor == -1) {
        perror("error setting up socket");
        return -1;
    }

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

    int l = listen(sock_file_descriptor, 3);
    if (l != 0) {
        perror("error occured listening to socket");
        return -1;
    }

    return sock_file_descriptor;
}

int main() {
    // note: because variables are freed when they go out of scope we cant
    // create and return a pointer in a single function.
    // Either: make the variable static || assign the memory in the caller || return the value 
    struct address addr = get_address();

    int server = get_server(&addr);
    if (server < 0) {
        perror("Error initializing server");
        
        // 2 good reasons to use goto in c:
        //   - jumping out of nested loops
        //   - many paths wanting to finish with executing the same code (similar to why you'd want to defer in go)
        goto stop;
    }

    int serve = 0;
    while (serve != -1) {
        serve = accept(server, (struct sockaddr *) &addr.addr, &addr.size);
        if (serve < 0) {
            goto stop;
        }

        ssize_t sr = 1;
        while (sr > 0) {
            char buffer[1024] = {};
            sr = read(serve, buffer, 1024-1);
            if (sr < 0) {
                printf("read returned %ld", sr);
                goto stop;
            }
            printf("%s\n", buffer);
        }
    }

stop:
    close(serve);
    close(server);
}