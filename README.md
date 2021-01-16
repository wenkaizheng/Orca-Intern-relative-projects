# Orca intern relative project
Using websocket protocol to write a Mini Chat APP\
brew install libwebsockets\
make\
./server\
./client userid (normal chat) \
./client search Jan/11/2021 (request chat records on that day). And this command will create a file search_Jan/11/2021.txt in current working directory.
./client search today will give you current records.

Client side supports log (chat record in a file client_log.txt) for only one time using, if you try next 
time, the record will be overwritten.

Server side supports sqlite3 DataBase to store the chat record, if you want to 
check the record in one specific date, try the command above. Also note that server side is using LRU
cache to save answer .

For receiving the message in browser, http://localhost:8000.
