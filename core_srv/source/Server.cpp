#include "../include/Server.hpp"
 #include <unistd.h>
Server::Server(int domain, int type, int protocol, int port , 
                                                        u_long ip_add)
{
    // create socket
    connection = socket(domain, type, protocol); //  the listening socket.
    if (connection == -1)
        std::cerr << "socket err\n";
    
    //define address
    struct sockaddr_in addr;
    addr.sin_family = domain;
    addr.sin_addr.s_addr = htonl(ip_add); // (host to network long).
    addr.sin_port = htons(port); // change it to bytes with htons bcs maching dont read the decimal
    
    //////////////////////////// remove it / search about it
    int opt = 1;
    setsockopt(connection, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    ////////////////////////////

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
        close(connection);
        return ;
    }
    std::cout << "Server listening on port 8080..." << std::endl;


    // accept
    while (true)
    {

        // struct sockaddr_in client_addr;
        // socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(connection, NULL, NULL); // socket in ESTABLISHED state for theat specific client
        if (client_fd == -1)
        {
            std::cerr << "accept err\n";
            close(connection);
            return ;
        }
        std::cout << "Client connected" << std::endl;
        send(client_fd, "Hello from server!\n", 20, 0);
        // std::cout << client_addr.sin_addr.s_addr << std::endl;
        close (client_fd);
    }
    close(connection);

}