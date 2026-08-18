
#include "main.h"
#include <unistd.h>

int main() {
    int server_fd = createServerSocket(PORT);

    while (true) {
        ConnectionInfo info;
        int client_fd = acceptClient(server_fd, info);
        readMessage(client_fd, info);
        close(client_fd);


    }

    close(server_fd);
    return 0;
}