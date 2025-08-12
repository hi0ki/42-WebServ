#include "../include/Server.hpp"
 #include <unistd.h>
Server::Server(int domain, int type, int protocol, int port , 
                                                        u_long ip_add)
{
    // create socket
    connection = socket(domain, type, protocol);
    if (connection == -1)
        std::cerr << "socket err\n";
    
    //define address
    struct sockaddr_in addr;
    addr.sin_family = domain;
    addr.sin_addr.s_addr = htonl(ip_add);
    addr.sin_port = htons(port); // change it to bytes with htons bcs maching dont read the decimal
    
    // bind
    if (bind(connection, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cerr << "bind err\n";
        close(this->connection);
        return ;
    }

    // listen
    if (listen(connection, SOMAXCONN) == -1)
    {
        std::cerr << "listen err\n";
        close(this->connection);
        return ;
    }
    std::cout << "Server listening on port 8080..." << std::endl;


    while(1);
    // accept
    socklen_t client_len = sizeof(addr);
    int accept_fd = accept(connection, (struct sockaddr*)&addr, &client_len);
    if (accept_fd == -1)
    {
        std::cerr << "accept err\n";
        close(this->connection);
        return ;
    }



}