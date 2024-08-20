#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "http.h"

char * response(int status_code, char * status_reason, char * content_type, char * body) {

    int http_ln_size = ((13+strlen(content_type))*sizeof(char))+sizeof(int);
    int ct_ln_size = (17+strlen(content_type))*sizeof(char);
    int cl_ln_size = (20*sizeof(char))+sizeof(size_t);
    int date_ln_size = 40*sizeof(char);
    int body_size = strlen(body)*sizeof(char);

    int size = http_ln_size + ct_ln_size + cl_ln_size + date_ln_size + body_size;
    char * buff = (char *) malloc(size);
    sprintf(buff, "HTTP/1.0 %d %s\n", status_code, status_reason);

    char * ct_ln = (char *) malloc(ct_ln_size);
    sprintf(ct_ln, "Content-Type: %s\n", content_type);
    strcat(buff, ct_ln);

    char * cl_ln = (char *) malloc(cl_ln_size);
    sprintf(cl_ln, "Content-Length: %ld\n", strlen(body));
    strcat(buff, cl_ln);

    strcat(buff, "Date: Tue, 20 Aug 2024 12:55:50 GMT\n\n");
    strcat(buff, body);

    free(ct_ln);
    free(cl_ln);

    return buff;
}

int listen_and_serve(int fd, struct address *addr) {
    printf("waiting on requests...\n");
    int s = accept(fd, (struct sockaddr *) &addr->addr, &addr->size);
    if (s < 0) {
        return -1;
    }

    int len = 0;
    ioctl(s, FIONREAD, &len);
    char * req = (char *) malloc(len+1);
    if (len > 0) {
        len = read(s, req, len);
    }

    printf("writing response...\n");
    char * res = response(200, "OK", "text/plain", "hello world");
    write(s, res, strlen(res)*sizeof(char));
    
    free(req);
    free(res);
    close(s);
    return 1;
}

int main() {
    // note: because variables are freed when they go out of scope we cant
    // create and return a pointer in a single function.
    // Either: make the variable static || assign the memory in the caller || return the value 
    struct address addr = new_address(DEFAULT_PORT);

    int server = new_socket(&addr);
    if (server < 0) {
        perror("Error initializing server");
        
        // 2 good reasons to use goto in c:
        //   - jumping out of nested loops
        //   - many paths wanting to finish with executing the same code (similar to why you'd want to defer in go)
        goto stop;
    }

    int l = listen(server, 3);
    if (l != 0) {
        perror("error occured listening to socket");
        goto stop;
    }


    int serve = 0;
    while (serve != -1) {
        serve = listen_and_serve(server, &addr);
    }

stop:
    close(server);
}