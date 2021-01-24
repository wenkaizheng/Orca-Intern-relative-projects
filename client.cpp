//  Created by wenkai zheng
// Client side using interrupt and it's handler to make receive and write won't stuck each other
#include <iostream>
#include <fcntl.h>
#include <libwebsockets.h>
#include <string.h>
#include <stdlib.h>
#include "utils.hpp"
#include <deque>
#include <pthread.h>
static struct lws *web_socket = NULL;
static struct lws_context *context = NULL;
static std::deque<char*> msg_queue;
static bool flag = false;
static char complete[129] = {0};
static char sign[3] = ": ";
static char search_record_name[65] = {0};
static int fd;
static int op;
static bool search_flag = false;
static pthread_t read_tid;
// if the packet size is exactly fix network buffer, we will try to read at most 7 times.
static int time_recv_packets = 0;
static int callback_example_client( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len ) {
    payload *client_received_payload = (payload *) user;
    switch (reason) {
        case LWS_CALLBACK_CLIENT_ESTABLISHED: // establish connection
            flag = true;
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:    // recv data
        {
           time_recv_packets += 1;
           if (receive_callback(wsi,client_received_payload,in,len,0,fd)){
               if(len != EXAMPLE_RX_BUFFER_BYTES){
                   search_flag = true;
               }
           }

           break;
        }
        case LWS_CALLBACK_CLIENT_WRITEABLE: //send data
        {
            unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
            unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
            char* msg = NULL;
            if(op == 0) {
            }
            else {
                strcat(complete, msg_queue.front());
                msg_queue.pop_front();
                memcpy(p,complete,strlen(complete));
                lws_write(wsi, p, strlen(complete), LWS_WRITE_TEXT);
            }
            break;
        }
        case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL:
            // when context is destroy
            printf("Client callback: %s\n", "LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL");
            exit(0);

        case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
            // duplicate id
             printf("Client callback: %s\n", "LWS_CALLBACK_WS_PEER_INITIATED_CLOSE");
             exit(1);
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            // server is down and reconnect will be attempted
            printf("Client callback: %s\n", "LWS_CALLBACK_CLIENT_CONNECTION_ERROR");
            web_socket = NULL;
            break;
        default:
            break;
    }

    return 0;
}

void shutdown(int signo) {
    lws_context_destroy(context);
    close(fd);
    exit(0);
}
static struct lws_protocols client_protocols[] =
        {
                {
                        "ws-protocol-example",
                        callback_example_client,
                              sizeof(payload),
                        EXAMPLE_RX_BUFFER_BYTES,
                },
                { NULL, NULL, 0, 0 } /* terminator */
        };
int main(int argc, char* argv[]) {
    delete_prev_log_file(logs);
    signal(SIGINT, shutdown);
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof(info));

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = client_protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    int count = 0;
    /* Connect if we are not connected to the server. */
    if (!web_socket) {
        struct lws_client_connect_info ccinfo = {0};
        ccinfo.context = context;
        ccinfo.address = "localhost";
        ccinfo.port = 8000;
        ccinfo.path = "/";
        ccinfo.host = lws_canonical_hostname(context);
        ccinfo.origin = "origin";
        ccinfo.protocol = client_protocols[WS_PROTOCOL_EXAMPLE].name;
        web_socket = lws_client_connect_via_info(&ccinfo);
    }
    // printf("114th %p\n",context);
    while (!flag) {
        lws_service(context,/* timeout_ms = */ 0);
    }
    if(argc == 2) {
    }
    else{
        op = 1;
        char search[65];
        strcpy(search,argv[1]);
        if(strcmp(search,"search")!=0){
            fprintf(stderr,"We only accept search function for now\n");
        }
        memcpy(search,search_syms,7);
        strcat(search," ");
        strcat(search,argv[2]);
        strcpy(search_record_name, argv[2]);

        for(int i =0; i< strlen(search_record_name); i++){
            if (search_record_name[i] == '/' ||search_record_name[i] == ' '){
                search_record_name[i] = '_';
            }
        }
        strcat(search_record_name,".txt");
        delete_prev_log_file(search_record_name);
        fd = open(search_record_name, O_WRONLY | O_CREAT, 0644);
        dup2(fd,1) ;
        msg_queue.push_back(search);
        int count = 0;
        int time_prev_recv_packet = time_recv_packets;
        while(!search_flag) {
          if(count == 0) {
              lws_callback_on_writable(web_socket);
          }
          lws_service(context,/* timeout_ms = */ 0);
          // exact size we assume no more packets
          if (time_prev_recv_packet == time_recv_packets && count >= 7){
              break;
          }else{
              time_prev_recv_packet = time_recv_packets;
          }
          count ++;
        }
    }

    close(fd);
    lws_context_destroy(context);
    return 0;
}