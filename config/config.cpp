/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 16:37:55 by hanebaro          #+#    #+#             */
/*   Updated: 2025/08/30 19:41:02 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"
#include "server.hpp"


config::config(std::string nameFile) : File(nameFile)
{
    if(nameFile.size() < 5 || nameFile.compare(nameFile.size() - 5, 5, ".conf") != 0)
        throw std::runtime_error("config file invalid : " + nameFile);
    if (!File.is_open())
        throw std::runtime_error("Unable to open the file: " + nameFile);
}

std::vector<std::string> split(const std::string &str, char c)
{
    std::vector<std::string>  result;
    std::string cont;
    
    for(unsigned long i = 0; i < str.size(); i++)
    {
        if(str[i] == c && cont.size())
        {
            result.push_back(cont);
            // std::cout << cont << "\n" ;
            cont.clear();
        }
        if (str[i] != c)
            cont += str[i];
    }
    result.push_back(cont);
    // std::cout << cont << "\n" ;
    return(result);
}

void config::set_server(std::vector<std::string>::iterator it)
{
    (void)it;
    std::cout << "-------------server found \n";
    std::cout << *(it)++ << std::endl;
    std::cout << *(it)++ << std::endl;
    std::cout << *(it)++ << std::endl;
}

void config::parse_configFile()
{
    std::vector<std::string> conf;
    std::string ligne;
    std::vector<std::string>::iterator it;
    
    while(getline(File, ligne))
    {
        if(ligne == "\n")
            continue;//je peut la suppr
        // ligne += "\n";
        conf.push_back(ligne);
    }
    // for (it = conf.begin(); it != conf.end(); it++)
    //     std::cout << *it;
    it = conf.begin();
    std::vector<std::string> tmp;
    
    while(it != conf.end())// apres split verif lesligne vide
    {
        tmp = split(*it, ' ');
        // std::cout << "[ "<< *it << "]\n";
        if(tmp[0] == "server" && tmp[1] == "{")//check server
        {
            if(tmp.size() >= 3)
            {
                if(tmp[2][0] && tmp[2][0] != '#' )
                    throw std::runtime_error("content invalid");
            }
            else
                set_server(it);
                
        }
        // check }
        it++;
        tmp.clear();
    }
}
