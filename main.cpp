#include "core_srv/include/Server.hpp"
#include "config/config.hpp"

int main(int ac, char **av)
{
    try
    {
        if (ac == 2)
        {
            std::string filenaem(av[1]);
            config config_obj(filenaem);
            Server var(config_obj);
        }
    }
    catch (std::exception &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
    return 1;
}

// for(std::vector<Location>::iterator it = myconfig.get_servs()[0].get_location().begin(); it != myconfig.get_servs()[0].get_location().end(); it++) 
// 	{
// 		if(it->type == CGI)
//         {
// 			std::cout << it->methods.size() << std::endl;
// 		}
// 	}
// 	exit(1);