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