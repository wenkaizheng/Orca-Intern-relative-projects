//
// Created by wenkai on 12/29/20.
//
#ifndef DBS
#define DBS
#include <iostream>
#include <sqlite3.h>
#include <string.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
namespace db {
    class DB {
    public:
        int open_db();

        int close_db();

        int exec_db(char *sql, int sign);

        int create_table(int sign, char* tb_name);

        std::vector<char*> get_answer();

        void set_answer_empty(char* date);

        bool contains_cache(unsigned char* key);

        std::vector<char*> get_cache();

        void set_cache_empty();
        DB(
           char *name
        );

        ~DB();

    private:
        sqlite3 *db;
        char *zErrMsg;
        char db_name[32];
        const char* operation [6];
    };
}
#endif