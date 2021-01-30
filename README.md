# Websocket-chat
Using websocket protocol to write a Mini Chat APP\
brew install libwebsockets\
make\
./server\
./client search Jan/11/2021 (request chat records on that day). And this command will create a file search_Jan/11/2021.txt in current working directory.
./client search today will give you current records.

Server side supports sqlite3 DataBase to store the chat record, if you want to 
check the record in one specific date, try the command above. Also note that server side is using LRU
cache to save answer.

For open the client side, please visit http://localhost:8000/?name= YOUR NAME.
The owner will have the right to remove other users, once the owner logout session, the newly connection
user will become the owner
