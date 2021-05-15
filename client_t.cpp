//
// Created by Wenkai Zheng on 4/30/21.
//

#define PORT 8888
#define SA struct sockaddr
#include "utils.hpp"


int main(int argc, char *argv[])
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
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
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    int flags = fcntl(sockfd, F_GETFL, 0);
    assert(flags >= 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    int kq = kqueue();
    struct kevent ev_set;

    EV_SET(&ev_set, sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    assert(-1 != kevent(kq, &ev_set, 1, NULL, 0, NULL));

    EV_SET(&ev_set, sockfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
    kevent(kq, &ev_set, 1, NULL, 0, NULL);
    struct kevent evList[32];
    bool  flag = true;
    bool write_already = false;
    while (flag) {
        // returns number of events
        int nev = kevent(kq, NULL, 0, evList, 32, NULL);
        printf("kqueue got %d events\n", nev);

        for (int i = 0; i < nev; i++) {
            int fd = (int) evList[i].ident;

            if (evList[i].flags & EV_EOF) {
                printf("Disconnect\n");
                close(fd);
                // Socket is automatically removed from the kq by the kernel.
            }
            else if (evList[i].filter == EVFILT_WRITE) {
                if(write_already){
                    continue;
                }
                char complete_msg[64] = {0};
                char* time = current_time();
                time[strlen(time)-1] = '\0';
                std::string str(time);
                str.erase(remove(str.begin(), str.end(), ' '), str.end());
                str.erase(remove(str.begin(), str.end(), ':'), str.end());
                char* c = const_cast<char*>(str.c_str());
                strcat(complete_msg,c);
                strcat(complete_msg,argv[1]);
                size_t bytes_write = write(fd, complete_msg, strlen(complete_msg));
                printf("write %zu bytes %s\n", bytes_write, complete_msg);
                write_already = true;

            } else if (evList[i].filter == EVFILT_READ) {
                // Read from socket.
                char buf[4];
                size_t bytes_read = read(fd, buf, 4);
                printf("read %zu bytes and please go to port %d\n", bytes_read, *((int*)buf));
                char port_arg[6];
                sprintf(port_arg,"%d",*((int*)buf));
                write_log_file(port_arg,argv[1],2);
                flag = false;
            }else if (fd == sockfd) {
                // never occurs
                continue;
            }
        }
    }
    close(sockfd);
    return 0;
}