#include "../core_srv/include/Server.hpp"

void post_algo(ClientData &client, int &request_length)
{
    if (!client.get_ftime_pars())
	{
		std::vector<char>::iterator find_index;

		find_index = std::search(client.get_request().begin(), client.get_request().end(),
			std::begin("\r\n\r\n"), std::end("\r\n\r\n") - 1);
		client.get_request().erase(client.get_request().begin(), find_index + 4);
		request_length = client.get_request().size();
		client.set_ftime_pars(true);
	}
	if (!client.get_body_struct().key.empty() && client.get_request().size() == static_cast<size_t>(client.get_length()))
	{
		size_t key_pos = 0;
		size_t fname_pos = 0;
		std::string boundry = client.get_body_struct().key;
		size_t boundry_size = boundry.size();
		std::string old_request(
			client.get_request().begin(),
			client.get_request().end()
		);

		request_length = client.get_request().size();
		while (true)
		{
			key_pos = old_request.find(boundry);
			if (key_pos != std::string::npos)
			{
				if (old_request[key_pos + boundry_size + 1] == '-')
				{
					old_request.erase(0, key_pos + boundry_size + 4);
					client.set_request(old_request);
					break ;
				}
				old_request.erase(0, boundry_size + key_pos + 2);
				key_pos = old_request.find(boundry) - 6;
				fname_pos = old_request.find("filename=\"");
				if (fname_pos != std::string::npos)
				{
					fname_pos += 10;
					client.get_body_struct().file_name.clear();
					for (; old_request[fname_pos] != '"'; fname_pos++)
						client.get_body_struct().file_name.push_back(old_request[fname_pos]);
					std::ofstream myfile(client.get_request_obj().getAbsolutePath() + "/" + client.get_body_struct().file_name.c_str());
					if (myfile.is_open())
					{
						int end = old_request.find("\r\n\r\n") + 4;
						myfile.write(&old_request[end], key_pos - end);
						old_request.erase(old_request.begin(), old_request.begin() + end);
						myfile.close();
					}
					else
						std::cout << RED << "File didn't open" << RESET << std::endl;
				}
				else
				{
					fname_pos = old_request.find("name=\"");
					std::string map_key;
					std::string map_value;

					if (fname_pos != std::string::npos)
					{
						fname_pos += 6;
						for (; old_request[fname_pos] != '"'; fname_pos++)
							map_key.push_back(old_request[fname_pos]);
						size_t end = old_request.find("\r\n\r\n") + 4;
						for (;end < key_pos - 1; end++)
							map_value.push_back(old_request[end]);
						old_request.erase(old_request.begin(), old_request.begin() + end);
						client.get_body_map()[map_key] = map_value;
						map_key.clear();
						map_value.clear();
					}
				}
			}
			else 
				break;
		}
	}
}