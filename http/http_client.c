#include <unistd.h>
#include <stdio.h>

#include "http.h"

int main() {
    struct address addr = new_address(DEFAULT_PORT);
    int sock = new_socket(&addr);

    


}
