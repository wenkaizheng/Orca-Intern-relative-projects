//
// Created by wenkai on 12/29/20.
//
#include "db.hpp"
namespace  db{
    std::vector<char*> answers;
    std::map<char*,std::vector<char*>> cache;
    std::list<char*> track;
    unsigned int cache_cap;
    // for tmp customer cmp function
    char cache_cmp_key[65];

    static bool cmp_track(char* i){
        if (strcmp(i,cache_cmp_key) == 0){
            return true;
        }
        return false;
    }
    static bool cmp_cache(std::pair<char*,std::vector<char*>> p){
        if (strcmp(p.first,cache_cmp_key) == 0){
            return true;
        }
        else{
            return false;
        }
    }

    std::vector<char*> db::DB::get_answer(){
        return answers;
    }
    std::vector<char*> db::DB::get_cache(){
        // remove it and then move to the back
        std::list<char*>::iterator it = find_if(track.begin(),track.end(),cmp_track);
        track.splice(track.end(),track,it);
        std::map<char*,std::vector<char*>>::iterator its = find_if(cache.begin(),cache.end(),cmp_cache);
        return its->second;
    }
    bool db::DB::contains_cache(unsigned char* key){
        strcpy(cache_cmp_key,(char*) key);
        std::list<char*>::iterator it = find_if(track.begin(),track.end(),cmp_track);
        if (it == track.end()){
            bzero(cache_cmp_key,65);
            return false;
        }
        return true;
    }
    void db::DB::set_answer_empty(char* date) {
       char* copy_date = strdup(date);
       bool push_flag = true;
       if (strcmp(date,"today") == 0){
           push_flag = false;
       }
       else {
           if (track.size() == cache_cap) {
               char *key = track.front();
               track.pop_front();
               strcpy(cache_cmp_key,key);
               std::map<char*,std::vector<char*>>::iterator it = find_if(cache.begin(),cache.end(),cmp_cache);
               bzero(cache_cmp_key,65);
               cache.erase(it);
           }
           track.push_back(strdup(date));
       }

        for(char* ch:answers){
            if (push_flag) {
                cache[copy_date].push_back(strdup(ch));
            }
            free(ch);
        }
        answers.clear();
    }
    void db::DB::set_cache_empty(){
        bzero(cache_cmp_key,65);
    }
    int callback_db(void *NotUsed, int argc, char **argv, char **azColName) {
        int i;
        for(i = 0; i<argc; i++) {
            if (i % 4 == 0){
                continue;
            }
            char result[128] = {0};
            //printf("-----------    %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            sprintf(result,"%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            char* tmp = strdup(result);
            answers.push_back(tmp);
        }
        return 0;

    }
    int db::DB::close_db() {
        int rc = sqlite3_close(db);
        if( rc ) {
            fprintf(stderr, "Can't close database: %s\n", sqlite3_errmsg(db));
            return 0;
        } else {
            fprintf(stdout, "Close database successfully\n");
            return 1;
        }

    }

    int db::DB::open_db() {
        int rc = sqlite3_open(db_name,&db);
        if( rc ) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
            return 0;
        } else {
            fprintf(stdout, "Opened database successfully\n");
            return 1;
        }
    }

    int db::DB::exec_db(char *sql, int sign) {
        int rc = sqlite3_exec(db, sql, callback_db, 0, &zErrMsg);
        if( rc ) {
            fprintf(stderr, "SQL ERROR %s: %s\n", operation[sign],sqlite3_errmsg(db));
            return 0;
        } else {
            fprintf(stdout, "SQL command %s executes successfully\n", operation[sign]);
            if (answers.empty() && sign == 4){
                answers.push_back(strdup("NO RECORD ON THAT DAY"));
            }
            return 1;
        }
    }
    int db::DB::create_table(int sign, char* tb_name){
            char sql2[256];
            sprintf(sql2, "CREATE TABLE IF NOT EXISTS %s("  \
            "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
            "TIME           TEXT    NOT NULL," \
            "MESSAGE        CHAR(65));",tb_name);
            return exec_db(sql2,sign);

    }
    db::DB::DB(char *name) {
        cache_cap = 10;
        strcpy(db_name,name);
        zErrMsg = 0;
        operation[0] = "create table";
        operation[1] = "delete table";
        operation[2] = "insert sql";
        operation[3] = "delete sql";
        operation[4] = "select sql";
    }

    db::DB::~DB() {}
}