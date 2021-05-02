# Websocket-chat
Using websocket protocol to write a Mini Chat APP\
brew install libwebsockets\
make\
./server_t\
./client_t room_name will return the port_num you need to go\
./client search Jan/11/2021 port_num (request chat records on that day). And this command will create a file search_Jan/11/2021.txt in current working directory.\
./client search today port_num will give you current records named today.txt.

Server side supports sqlite3 DataBase to store the chat record, if you want to 
check the record in one specific date, try the command above. Also note that server side is using LRU
cache to save answer.

For open the client side, please visit http://localhost:port_num/?name= YOUR NAME.
The owner will have the right to remove other users, once the owner logout session, the second order connection
user will become the new owner.
