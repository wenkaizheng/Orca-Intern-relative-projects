//
// Created by wenkai zheng on 4/30/21.
//

#define PORT 8888
#define SA struct sockaddr
#include "utils.hpp"
static std::map<int, char*> collector;
static int sockfd;
static int count = 0;
char path[9] = "./server";
void shutdown(int signo){
    // todo close socket in here
    printf("server_t exists in here\n");
    close(sockfd);
    for (int i = 0; i<count; i++){
        wait(NULL);
    }
    exit(0);
}
int main(int argc, char *argv[])
{
    signal(SIGINT,shutdown);
    int len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
    int kq = kqueue();
    struct kevent ev_set;

    EV_SET(&ev_set, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    assert(-1 != kevent(kq, &ev_set, 1, NULL, 0, NULL));

    struct kevent evList[32];
    int count = 0;
    while (1) {
        // returns number of events
        int nev = kevent(kq, NULL, 0, evList, 32, NULL);
        //printf("kqueue got %d events\n", nev);

        for (int i = 0; i < nev; i++) {
            int fd = (int)evList[i].ident;
            if (evList[i].flags & EV_EOF) {
                printf("Disconnect\n");
                close(fd);
                        // Socket is automatically removed from the kq by the kernel.
            } else if (fd == sockfd) {
                struct sockaddr_storage addr;
                socklen_t socklen = sizeof(addr);
                int connfd = accept(fd, (struct sockaddr *)&addr, &socklen);
                assert(connfd != -1);

                int flags = fcntl(connfd, F_GETFL, 0);
                assert(flags >= 0);
                fcntl(connfd, F_SETFL, flags | O_NONBLOCK);

                // register to read
                EV_SET(&ev_set, connfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kq, &ev_set, 1, NULL, 0, NULL);
                printf("Got connection!\n");

                // register to write
                EV_SET(&ev_set, connfd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
                kevent(kq, &ev_set, 1, NULL, 0, NULL);

            } else if (evList[i].filter == EVFILT_READ) {
                count += 1;
                // Read from socket.
                char buf[64] ={0};
                size_t bytes_read = read(fd, buf, 63);
                printf("read %zu bytes %s\n", bytes_read,buf);
                int port_number = find_port();
                char buffer[50];
                sprintf(buffer,"%d",port_number);
                char port[4];
                memcpy(port,&port_number,4);
                collector[fd] = strdup(port);
                int fork_rv = fork();
                if (fork_rv == 0){
                    // first argument port number
                    // second argument table name
                    printf("%s\n",buf);
                    char *argv[] = {path, buffer, buf, NULL};
                    int rv = execvp("./server",argv);
                    if(rv == -1)
                        perror("execl error");
                    printf("104th\n running server already\n");
                }else if (fork_rv == -1){
                    perror("can't fork in line 99th\n");
                }

            } else if (evList[i].filter == EVFILT_WRITE) {
                //printf("101th %d\n",fd);
                if (collector.find(fd) != collector.end()) {
                    size_t bytes_write = write(fd, collector[fd], 4);
                    printf("write %zu bytes %d\n", bytes_write, *((int*)collector[fd]));
                    free(collector[fd]);
                    collector.erase(fd);
                }else{
                    continue;
                }
            }
        }
    }
}