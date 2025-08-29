#include "../include/Server.hpp"


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


    // poll setup
    std::vector<pollfd> fds;

    pollfd client_pfd;
    client_pfd.fd = connection;
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;
    fds.push_back(client_pfd);

    // accept
    int client_fd;
    char  *buffer;
    while (true)
    {
        for (int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == connection)
                {
                    client_fd = accept(connection, NULL, NULL); // socket in ESTABLISHED state for theat specific client
                }
                else
                {
                    recv(client_fd, buffer, sizeof(buffer), 0);
                    
                }
            }
        }
    }
    close(connection);

}