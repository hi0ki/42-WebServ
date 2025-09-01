/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:53:11 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/01 16:31:43 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

int main(int argc, char **argv)
{
    try
    {
        if(argc != 2)
        {
            std::cout << "invalid parameter\n";
            return(1);
        }
        config myconfig(argv[1]);
        myconfig.parse_configFile();
        myconfig.print_servers();
        return (0);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    
}