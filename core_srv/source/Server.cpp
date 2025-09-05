#include "../include/Server.hpp"
#include <unistd.h>
#include <sstream>


/*
	1 -move the fds vector to the class ✅
	2 -split the code : ✅
	3 -handle multi servers : ✅
	4 -read the request and store it in vector

	// kifach t3rf client ach mn server mnin ja bach t3rf l path dyal files dyalo -> hit lamdrtich haka rah atjib files ghaltin ola may3rfch mol request ach mn path 3ndo
	----------err---------
	5 -close the servers fd before throwing the exception
	6 -handle errs and the proccess keep working


	// problem with using multi servers
*/

uint32_t ip_convert(std::string ip)
{
	unsigned int b1, b2, b3, b4;
	char dot;

	std::stringstream ss(ip);
	ss >> b1 >> dot >> b2 >> dot >> b3 >> dot >> b4;

	uint32_t ipHostOrder = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

	return (ipHostOrder);
}

Server::Server(config &config) : myconfig(config)
{
	this->server_start();
	
	std::cout << GREEN << "---------------------------------------" << std::endl;
	std::cout << "-----------Servers listening-----------" << std::endl;
	std::cout << "---------------------------------------\n" << RESET<< std::endl;
	this->start_connection();
}

void Server::server_start()
{
	for (int i = 0; i < this->myconfig.get_servs().size(); i++)
	{
		this->connection = socket(AF_INET, SOCK_STREAM, 0); //  the listening socket.
		if (this->connection == -1)
		{
			std::cerr<< "connection socket err" << std::endl;
			continue ;
		}
		try
		{
			this->bind_socket(i);
			this->listen_socket();

			// add server fd to the container
			pollfd pfd;
			pfd.fd = this->connection;
			pfd.events = POLLIN | POLLOUT; // give events both signs
			pfd.revents = 0;	
			fds.push_back(pfd);
			
		}
		catch(const std::exception& e)
		{
			std::cerr << RED << "[ " << this->myconfig.get_servs()[i].get_IP() << ":" << 
				this->myconfig.get_servs()[i].get_port() << " ]" << " : " <<
				e.what() << RESET << std::endl;
			close(this->connection);
		}
	}
}

void Server::bind_socket(int srv_index)
{
	std::cout << "------------------------biiindiiiing----------------------\n";
	std::cout << "server index : " << srv_index << std::endl;
	std::cout << "ip : " << this->myconfig.get_servs()[srv_index].get_IP() << std::endl;
	std::cout << "port : " << this->myconfig.get_servs()[srv_index].get_port() << std::endl;
	std::cout << "connection : " << this->myconfig.get_servs()[srv_index].get_port() << std::endl;
	std::cout << std::endl;

	if (this->myconfig.get_servs()[srv_index].get_port() < 1024 || this->myconfig.get_servs()[srv_index].get_port() > 65535)
	{
		std::string err_msg = std::to_string(this->myconfig.get_servs()[srv_index].get_port()) + " : Can't use ports under 1024 or bigger then 65535, your process must have the necessary permissions\n";
		throw std::runtime_error(err_msg);
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip_convert(this->myconfig.get_servs()[srv_index].get_IP())); // (host to network long).
	addr.sin_port = htons(this->myconfig.get_servs()[srv_index].get_port()); // change it to bytes with htons bcs maching dont read the decimal
	
	int opt = 1;
	setsockopt(this->connection, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(this->connection, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("bind err");
}
void Server::listen_socket()
{
	if (listen(this->connection, SOMAXCONN) == -1)
		throw std::runtime_error("listen err");
}

void Server::start_connection()
{
	pollfd client_pfd;
	int client_fd;
	int poll_var;
	char  buffer[5000];

	while (true)
	{
		// need to split that to functions to make the code clean and readable
		poll_var = poll(fds.data(), fds.size(), 10);
		if (poll_var == -1)
		{
			// close all fds
			throw std::runtime_error("poll err");
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