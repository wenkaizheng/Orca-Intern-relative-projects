//
// Created by wenkai on 1/4/21.
//
#ifndef UTI
#define UTI
#include <libwebsockets.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#define FRONT_END    "front_end.html"
#define  FRONT_END_500 "front_end_500.html"
#define  one_byte 1024
#define EXAMPLE_RX_BUFFER_BYTES 1024*5
extern char search_syms[7];
extern char logs[32];
enum client_protocols_type
{
    WS_PROTOCOL_EXAMPLE,
    PROTOCOL_COUNT
};
enum sql_command_type
{
    CREATE_TABLE,
    DELETE_TABLE,
    INSERT_SQL,
    DELETE_SQL,
    SELECT_SQL
};
enum payload_op_type{
    NORMAL_OP,
    SEARCH_OP
};
typedef struct payload
{
    unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES];
    size_t len;
    enum payload_op_type op;
    bool cache_found;
    char date[65];
} payload;

char* current_time();
int receive_callback(struct lws *wsi,payload * received_payload,void* in, size_t len, int type, int fd = -1);
int compare_op(char* src, unsigned char* dst);
int compare_date(char* src, unsigned char* dst);
char* date_copy(char* original, int flag);
void name_copy(char* name, char* message);
void separate_data(unsigned char* data, char* time, char* real_data);
void delete_prev_log_file(char* file_name);
std::vector<char*>::iterator check_user_name(char* user_name, std::vector<char*>& name_list);
#endif