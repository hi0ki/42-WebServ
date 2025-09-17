 #include "../include/Server.hpp"
#include "../../config/server.hpp"

#include <unistd.h>
#include <sstream>


/*
	0 -try to work in map with i nothe fd : impossible
	1 -move the fds vector to the class : ✅
	2 -split the code : ✅
	3 -handle multi servers : ✅
	4 -read the request and store it in vector : ✅
	6 -handle errs and the proccess keep working : ✅
	7 - check if keep alive or u need to close the client : waiting for request and response
	8 - problem = khsni nkhli client bla manms7hom o ib9a khdam server ✅
	8 - init the clientdata class : ✅
	infos:
		(pay attention to differences between HTTP versions)
		the virtual host
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
			this->myconfig.get_servs().erase(this->myconfig.get_servs().begin() + i);
			i--;
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

bool Server::is_server(int fd)
{
	for (int i = 0; i < this->myconfig.get_servs().size(); i++)
	{
		if (fd == this->fds[i].fd)
			return true;
	}
	return false;
}

void Server::start_connection()
{
	int poll_var;
	char  buffer[5000];

	while (true)
	{
		poll_var = poll(fds.data(), fds.size(), 0);
		if (poll_var == -1)
		{
			// close all fds
			throw std::runtime_error("poll err");
		}
		for (int i = 0; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				if (is_server(fds[i].fd) == true)
					this->accept_client(i);
				else
					this->handle_request(i);
			}
			else if (fds[i].revents & POLLOUT)
				this->handle_response(i);
		}
	}
	// close all fds.
}


void Server::accept_client(int i)
{
	int client_fd;
	pollfd client_pfd;
	ClientData new_client;

	std::cout << GREEN << "\n--------------------server-------------------" << RESET << std::endl;
	client_fd = accept(fds[i].fd, NULL, NULL); // socket in ESTABLISHED state for theat specific client
	// after accept i should create clientdata and give it the data from client like -> wich server + client fd.
	if (client_fd == -1)
	{
		std::cerr << "client from this server [" << fds[i].fd <<"] failed" << std::endl; // replace fds.[i].fd with ip from my config
		return ;
	}
	//    INIT  POLLFD
	client_pfd.fd = client_fd;
	client_pfd.events = POLLIN;
	client_pfd.revents = 0;
	fds.push_back(client_pfd);

	// INIT NEW CLIENT
	std::cout << BLUE << "--------writing clinet-----------" << RESET << std::endl;
	std::cout << "client fd = " << client_fd << std::endl;
	std::cout << "server fd = " << fds[i].fd << std::endl;
	std::cout << "server index = " << i << std::endl;
	new_client.clear();
	new_client.set_srv_index(i + 1);
	this->clients[client_fd] = new_client;
	std::cout << "size = " << this->clients.size() << std::endl;
	std::cout << BLUE << "----------writing end-------------" << RESET << std::endl;
	std::cout << GREEN << "---------------------------------------------" << RESET << std::endl;
}

void Server::handle_request(int i)
{
	// mn hnaaaa
	std::cout << YELLOW << "\n[" << fds[i].fd << "]" << " : Client Request" <<  RESET << std::endl;
	std::vector<char> request = this->clients[fds[i].fd].get_request();
	char buffer[4096];
	int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		request.insert(request.end(), buffer, buffer + bytesRead);
	}
	else if (bytesRead == 0)
	{
		// khli ta ngad req is done bach nzid hadi hit daba matsd9chhit khsni nkhlih idoz tal parsin o tlgah sala bach thyd lih pollin
		std::cerr << RED << "[" << fds[i].fd << "]" << " : Client disconnected: fd " << fds[i].fd << RESET << std::endl;
		close(fds[i].fd);
		this->clients.erase(i);
		this->fds.erase(fds.begin() + i);
		i--;
		return ;
	}

	for (int j = 0; j < request.size(); j++)
		std::cout << request[j];

	this->clients[fds[i].fd].set_request(request);
	this->clients[fds[i].fd].get_request_obj().request_pars(this->clients[fds[i].fd], this->myconfig);
	if (this->clients[fds[i].fd].get_reqs_done())
	{
		std::cout << BLUE << "Request is done" << RESET << std::endl;
		this->fds[i].events = POLLOUT;
	}
	std::memset(buffer, 0, 4096);
}

void Server::handle_response(int i)
{
	std::cout << GREEN << "[" << fds[i].fd << "]" << " : Clinet Response" <<  RESET << std::endl;
	std::string response = "";
	response = this->clients[fds[i].fd].get_request_obj().buildHttpResponse(this->clients[fds[i].fd].get_request_obj().getfullPath(), this->clients[fds[i].fd].get_request_obj());
	send(fds[i].fd, response.c_str(), response.size(), 0);// don't remove it 
	if (!this->clients[fds[i].fd].get_keep_alive())
	{
		std::cout << RED << ">>>>>>>> 'don't keep alive' <<<<<<<<" <<  RESET << std::endl;
		close(fds[i].fd);
		this->clients.erase(fds[i].fd);
		this->fds.erase(fds.begin() + i);
		return ;
	}
	std::cout << YELLOW << ">>>>>>>> 'keep alive' <<<<<<<<" <<  RESET << std::endl;
	this->clients[fds[i].fd].clean_request(); // don't remove it 
	this->clients[fds[i].fd].clean_response(); // don't remove it 
	// clear req obj
	this->clients[fds[i].fd].get_request_obj().ft_clean();
	this->fds[i].events = POLLIN; // don't remove it 
}