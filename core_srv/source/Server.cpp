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


    // poll setup
    std::vector<pollfd> fds;

    pollfd client_pfd;
    client_pfd.fd = connection;
    client_pfd.events = POLLIN;
    client_pfd.revents = 0;
    fds.push_back(client_pfd);

    // accept
    int client_fd;
    int poll_var;
    char  buffer[5000];
    while (true)
    {
        // need to split that to functions to make the code clean and readable
        poll_var = poll(fds.data(), fds.size(), 10);
        if (poll_var == -1)
        {
            std::cerr << "poll err\n";
            close(connection);
            return ;
        }
        for (int i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == connection)
                {
                    client_fd = accept(connection, NULL, NULL); // socket in ESTABLISHED state for theat specific client
                    client_pfd.fd = client_fd;
                    client_pfd.events = POLLIN;
                    client_pfd.revents = 0;
                    fds.push_back(client_pfd);
                }
                else
                {
                    recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    fds[i].events = POLLOUT;
                    std::cout << buffer << std::endl; 
                    std::memset(buffer, 0, 4096);

                }
            }
            else if (fds[i].revents & POLLOUT)
            {
                std::string response =
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: 48\r\n"
                        "\r\n"
                        "<html><body><h1>Hello from poll server</h1></body></html>";

                send(fds[i].fd, response.c_str(), response.size(), 0);
                close(fds[i].fd);
                fds.erase(fds.begin() + i);
                i--;
            }
        }
    }
    close(connection);
}