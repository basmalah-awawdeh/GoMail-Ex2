#include <iostream>
#include <string>
#include <sstream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include "../UrlCheck/url_blacklist.h" 
#include "../UrlCheck/url_validation.h"
#include "../FileManager/BlacklistFileManager.h"
#include "../HashFunctions/HashFunctionFactory.h"
#include "../BloomFilter/BloomFilter.h"
#include "../FileManager/BloomFilterFileManager.h"
#include "../server/server_arg_validation.h"
#include "../requests/requestCheck.h"

#define BUFFER_SIZE 4096
// in this func we handle the cleint's commands
void clientHandling(int clientSocket,
    BlacklistURL& blacklist,
    BlacklistFileManager& manager,
    BloomFilterFileManager& bloomFileManager) {
char buffer[BUFFER_SIZE];

while (true) {
    //giving zero values
    memset(buffer, 0, BUFFER_SIZE);
    // reding the client`s input data
    ssize_t bytesRead = read(clientSocket, buffer, BUFFER_SIZE-1);
    if (bytesRead <=0){
        break
    };
    std::string request(buffer);
    //check the client's request
    std::string response = requestCheck(request, blacklist, manager, bloomFileManager);
    send(clientSocket, response.c_str(), response.size(), 0);
}
close(clientSocket);
}

int main(int argc, char* argv[]) {
    int port, bloomFilter_size, id, server_fd;
    //chcking if our args are correct
    if (!validateArgs(argc, argv)) {
        return 1; 
    }
    //parsing the input arguments
    port = std::stoi(argv[1]);
    bloomFilter_size = std::stoi(argv[2]);
    //here we save the data
    BloomFilterFileManager bloomFilterManager("../data/bloom_filter.txt");
    BlacklistFileManager manager("../data/blacklist.txt");
    std::vector<std::shared_ptr<HashFunction>> hashFunctions;
    //creating hash funcs according to the args
    for (int i = 3; i < argc; ++i) {
        id = std::stoi(argv[i]);
        auto func = MakeHash::createHashFunction(id);
        hashFunctions.push_back(func); 
    }
    //checking that we have hash
    if (hashFunctions.empty()) {
        return 1;
    }

    std::cout << "Starting server with bloom size: " << bloomFilter_size
              << " and " << hashFunctions.size() << " hash functions.\n";

    auto filter = std::make_shared<BloomFilter>(bloomFilter_size, hashFunctions);
    BlacklistURL blacklist(filter);
    manager.load(blacklist);
    manager.save(blacklist);
    bloomFilterManager.load(blacklist);
    bloomFilterManager.save(blacklist);
    
//creating tcp socket and check if succeeded
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd== -1) {
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    //binding our socket to ip and check if succeeded
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        return 1;
    }
    //listening for the client and check if suceeded
    if (listen(server_fd, 1) < 0) {
        return 1;
    }
    //confirming that the server is listening
    std::cout << "Server listening on port " << port << "\n";
    //our loop to accept a client
    while (true) {
        clientSocket = accept(server_fd, nullptr, nullptr);
        if (clientSocket >= 0) {
            clientHandling(clientSocket, blacklist, manager, bloomFilterManager);
        }
    }

    // close(server_fd);
    // return 0;
}



/*
cd GoMail-main/GoMail-main
 rm -rf build
mkdir -p build
cd build
cmake ..
make
./server 12345 8 1 2
in another terminal-wsl: 
cd /mnt/c/Users/בסמלה/Desktop/part2sim/GoMail-main/GoMail-main/src/client
python3 client.py 127.0.0.1 12345
*/
/*
RUN IN DOCKER==>
-1st terminal:
docker-compose up --build
-2nd terminal:
 docker exec -it gomail-main-server-1 bash
 ./server 54321 8 1 2
 -3rd terminal:
docker exec -it gomail-main-client-1 bash
python3 client.py server 54321
RUNNING TESTS==>
docker exec -it gomail-main-server-1 bash
./tests


*/