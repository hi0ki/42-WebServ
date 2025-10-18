#include "../include/Server.hpp"
#include "../../config/server.hpp"


uint32_t ip_convert(std::string ip)
{
	unsigned int b1, b2, b3, b4;
	char dot;

	std::stringstream ss(ip);
	ss >> b1 >> dot >> b2 >> dot >> b3 >> dot >> b4;

	uint32_t ipHostOrder = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;

	return (ipHostOrder);
}

std::string the_ip_port(config &config, int srv_index)
{
	std::string ip_port;

	ip_port = "[" + config.get_servs()[srv_index].get_IP() + ":" +
		std::to_string(config.get_servs()[srv_index].get_port()) + "]";
	return ip_port;
}

std::map<std::string, std::string> &Server::get_session(const std::string &id)
{
	return sessions[id];
}

void Server::set_session_data(const std::string &id, const std::map<std::string, std::string> &data)
{
	std::map<std::string, std::string> &session = sessions[id];

	for (std::map<std::string, std::string>::const_iterator it = data.begin(); it != data.end(); ++it)
	{
		session[it->first] = it->second;
	}
}

Server::Server(config &config) : myconfig(config) , sessions()
{
	std::cout << std::endl;
	this->server_start();
	if (this->myconfig.get_servs().size() == 0)
	{
		throw std::runtime_error("No server find\n");
		return ;
	}
	std::cout << "\t" << GREEN << "##########################################" << std::endl;
	std::cout << "\t" << "#                                        #" << std::endl;
	std::cout << "\t" << "#            Servers listening           #" << std::endl;
	std::cout << "\t" << "#                                        #" << std::endl;
	std::cout << "\t" << "##########################################" << RESET<< std::endl;
	this->start_connection();
}

void Server::server_start()
{
	for (size_t i = 0; i < this->myconfig.get_servs().size(); i++)
	{
		this->connection = socket(AF_INET, SOCK_STREAM, 0);
		if (this->connection == -1)
		{
			std::cerr<< "Connection Socket Error" << std::endl;
			continue ;
		}
		try
		{
			this->bind_socket(i);
			this->listen_socket();

			// add server fd to the container
			fcntl(this->connection, F_SETFL, O_NONBLOCK);
			pollfd pfd;
			pfd.fd = this->connection;
			pfd.events = POLLIN | POLLOUT;
			pfd.revents = 0;
			fds.push_back(pfd);
			std::cout << BLUE << "\t    [ " << this->myconfig.get_servs()[i].get_IP() << ":" << 
				this->myconfig.get_servs()[i].get_port() << " ]" << " : " <<
				"Listening..." << RESET << std::endl;
		}
		catch(const std::exception& e)
		{
			std::cerr << RED << "\t" << the_ip_port(myconfig, i) << " : " <<
				e.what() << RESET << std::endl;
			this->myconfig.get_servs().erase(this->myconfig.get_servs().begin() + i);
			i--;
			close(this->connection);
		}
	}
}

void Server::bind_socket(int srv_index)
{
	if (this->myconfig.get_servs()[srv_index].get_port() < 1024 || this->myconfig.get_servs()[srv_index].get_port() > 65535)
	{
		std::string err_msg = std::to_string(this->myconfig.get_servs()[srv_index].get_port()) + " : Can't use ports under 1024 or bigger then 65535, your process must have the necessary permissions\n";
		throw std::runtime_error(err_msg);
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip_convert(this->myconfig.get_servs()[srv_index].get_IP()));
	addr.sin_port = htons(this->myconfig.get_servs()[srv_index].get_port());
	
	int opt = 1;
	setsockopt(this->connection, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (bind(this->connection, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		throw std::runtime_error("Client Can't bind to socket");
}

void Server::listen_socket()
{
	if (listen(this->connection, SOMAXCONN) == -1)
		throw std::runtime_error("Client Can't listen to socket");
}

bool Server::is_server(int fd)
{
	for (size_t i = 0; i < this->myconfig.get_servs().size(); i++)
	{
		if (fd == this->fds[i].fd)
			return true;
	}
	return false;
}

bool check_timeout(time_t last_activity)
{
	time_t now  = time(NULL);

	double diff = difftime(now, last_activity);
	if (diff >= TIME_OUT)
		return false;
	return true;
}

void Server::start_connection()
{
	int poll_var;

	while (true)
	{
		poll_var = poll(fds.data(), fds.size(), 0);
		if (poll_var == -1)
			throw std::runtime_error("Poll Error");
		for (size_t i = 0; i < fds.size(); i++)
		{
			if (!is_server(fds[i].fd) && !check_timeout(this->clients[fds[i].fd].get_last_activity()))
			{
				std::cerr << RED << "\t   Client timed out : " << the_ip_port(myconfig, this->clients[fds[i].fd].get_srv_index()) << RESET << std::endl;
				close(fds[i].fd);
				this->clients.erase(fds[i].fd);
				this->fds.erase(fds.begin() + i);
				i--;
			}
			if (fds[i].revents & POLLIN)
			{
				if (is_server(fds[i].fd) == true)
					this->accept_client(i);
				else
				{
					this->handle_request(i);
				}
			}
			else if (fds[i].revents & POLLOUT)
			{
				this->handle_response(i);
			}
		}
	}
}


void Server::accept_client(int i)
{
	int client_fd;
	pollfd client_pfd;
	ClientData new_client;

	client_fd = accept(fds[i].fd, NULL, NULL);
	if (client_fd == -1)
	{
		std::cerr << "\tclient can't connect to [" << this->myconfig.get_servs()[i].get_IP() << "]" << std::endl;
		return ;
	}
	//    INIT  POLLFD
	client_pfd.fd = client_fd;
	client_pfd.events = POLLIN;
	client_pfd.revents = 0;
	fds.push_back(client_pfd);

	// INIT NEW CLIENT
	new_client.clean_client_data();
	new_client.set_srv_index(i);
	this->clients[client_fd] = new_client;
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	std::cout << YELLOW << "\tNew client connected to : " << the_ip_port(myconfig, i) << RESET << std::endl;
}

void Server::pars_post_req(int index)
{
	int request_length;
	post_algo(this->clients[index], request_length);
	if (request_length == this->clients[index].get_length())
	{
		this->clients[index].set_post_boyd(true);
		this->clients[index].set_reqs_done(true);
		this->clients[index].set_length(-1);
		// post cgi case
		if (this->clients[index].get_request_obj().getcgi_allowed())
		{
			this->clients[index].get_request_obj().setBody_cgi(
				this->clients[index].get_cgi().execute(
					this->clients[index].get_request_obj().getAbsolutePath(), 
					this->clients[index].get_body_map(),
					this->clients[index].get_request_obj()
				)
			);
			
			if (this->clients[index].get_request_obj().getBody_cgi() == "")
				check_Error_pages(this->clients[index].get_request_obj(), myconfig, this->clients[index]);	
		}
	}
}


void Server::handle_request(int i)
{
	char buffer[4096];

	this->clients[fds[i].fd].update_activity();
	int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		this->clients[fds[i].fd].get_request().insert(
			this->clients[fds[i].fd].get_request().end(), buffer, buffer + bytesRead
		);
	}
	else if (bytesRead == 0)
	{
		std::cerr << RED << "\tClient closed the connection : " << the_ip_port(myconfig, this->clients[fds[i].fd].get_srv_index()) << RESET << std::endl;
		close(fds[i].fd);
		this->clients.erase(fds[i].fd);
		this->fds.erase(fds.begin() + i);
		i--;
		return ;
	}
	if (this->clients[fds[i].fd].get_length() == -1)
	{
		this->clients[fds[i].fd].get_request_obj().request_pars(this->clients[fds[i].fd], this->myconfig);
		set_session_data(this->clients[fds[i].fd].get_sessionID(), this->clients[fds[i].fd].getSession_data());
	}
	if (this->clients[fds[i].fd].get_length() >= 0 && !this->clients[fds[i].fd].get_post_boolen())
		pars_post_req(fds[i].fd);
	if (this->clients[fds[i].fd].get_reqs_done())
		this->fds[i].events = POLLOUT;
}

void Server::handle_response(int i)
{
	std::string response;
	this->clients[fds[i].fd].update_activity();
	this->clients[fds[i].fd].setSession_data(get_session(clients[fds[i].fd].get_sessionID()));
	response = this->clients[fds[i].fd].get_request_obj().buildHttpResponse(
		this->clients[fds[i].fd].get_keep_alive(), this->clients[fds[i].fd]
	);
	send(fds[i].fd, response.c_str(), response.size(), MSG_DONTWAIT);
	if (this->clients[fds[i].fd].get_header_length() == -1 && !this->clients[fds[i].fd].get_keep_alive())
	{
		std::cout << RED << "\tClient closed the connection : " << the_ip_port(myconfig, this->clients[fds[i].fd].get_srv_index()) << RESET << std::endl;
		close(fds[i].fd);
		this->clients.erase(fds[i].fd);
		this->fds.erase(fds.begin() + i);
		return ;
	}
	else if (this->clients[fds[i].fd].get_header_length() == -1)
	{
		this->clients[fds[i].fd].clean_client_data();
		this->clients[fds[i].fd].get_request_obj().ft_clean();
		this->fds[i].events = POLLIN;
	}
	response.clear();
}
