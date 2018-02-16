Server provides the list of towns inside 20km radius of Client's point.

Server loads towns from geo.txt database in hashtable (takes 5 minutes on 2GHz Intel Core 2 Duo)
on the very first Client's request.

Compiled with clang 3.4

ip = 127.0.0.1 port = 2018 by default
Usage:

cd server && make && ./server 
cd ../client && make && ./client 40.7143 -74.0060 

or ./server 192.168.25.25 2020
   ./client 40.7143 -74.0060 192.168.25.25 2020
