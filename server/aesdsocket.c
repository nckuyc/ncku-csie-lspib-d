#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

#define PORT "9000"
#define DATA_FILE "/var/tmp/aesdsocketdata"
#define BUFFER_SIZE 1024

int server_fd = -1;

void handle_signal(int sig) {
    syslog(LOG_INFO, "Caught signal, exiting");
    if (server_fd != -1) close(server_fd);
    remove(DATA_FILE);
    exit(0);
}

int main(int argc, char *argv[]) {
    int daemon_mode = 0;
    if (argc > 1 && strcmp(argv[1], "-d") == 0) daemon_mode = 1;

    openlog("aesdsocket", LOG_PID, LOG_USER);
    
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, PORT, &hints, &res) != 0) return -1;

    server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, res->ai_addr, res->ai_addrlen) == -1) {
        freeaddrinfo(res); return -1;
    }
    freeaddrinfo(res);

    // Daemonize AFTER bind but BEFORE listen
    if (daemon_mode) {
        if (fork() > 0) exit(0);
        setsid();
        chdir("/");
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    if (listen(server_fd, 10) == -1) return -1;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_size = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        
        if (client_fd != -1) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);
            syslog(LOG_INFO, "Accepted connection from %s", ip);

            FILE *fp = fopen(DATA_FILE, "a+");
            char *recv_buf = malloc(BUFFER_SIZE);
            ssize_t bytes_recv;

            while ((bytes_recv = recv(client_fd, recv_buf, BUFFER_SIZE, 0)) > 0) {
                fwrite(recv_buf, 1, bytes_recv, fp);
                if (memchr(recv_buf, '\n', bytes_recv)) break;
            }

            rewind(fp);
            while ((bytes_recv = fread(recv_buf, 1, BUFFER_SIZE, fp)) > 0) {
                send(client_fd, recv_buf, bytes_recv, 0);
            }

            fclose(fp);
            free(recv_buf);
            close(client_fd);
            syslog(LOG_INFO, "Closed connection from %s", ip);
        }
    }
}
