/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 16:37:55 by hanebaro          #+#    #+#             */
/*   Updated: 2025/08/31 20:33:36 by hanebaro         ###   ########.fr       */
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

int validnumber(std::string x)
{
    if (x.empty())
        return(0);
    for(size_t i = 0; i < x.size(); ++i)
    {
        if (i == x.size() - 1 && x[i] == ';')
            return (1);
        if (!std::isdigit(x[i]))
            return (0);
    }
    return (1);
}
void config::set_server(std::vector<std::string>::iterator it, std::vector<std::string> &conf)
{
    std::cout << "-------------server found \n";
    std::vector<std::string> tmp;
    server serv;
    while(it != conf.end())
    {
        tmp = split(*it, ' ');
        if(tmp[0] == "listen")
        {
            if(tmp.size() >= 3)
            {
                if(tmp[2][0] && tmp[2][0] != '#' )
                    throw std::runtime_error("content invalid");
            }
            std::vector<std::string> help;
            help = split(*it, ':');
            if(help.size() >= 3)
                throw std::runtime_error("listen content invalid");
            serv.set_IP(help[0]);//verify if ip is good??
            if(validnumber(help[1]))
            serv.set_port(atoi(help[1].c_str()));
            help.clear();
            tmp.clear();
            break;// do if else  
        }
        if(tmp[0] == "server_name")
        {
            
        }
        if(tmp[0] == "root")
        {
            if(tmp.size() >= 3)
            {
                if(tmp[2][0] && tmp[2][0] != '#' )
                    throw std::runtime_error("content invalid");
            }
            else
        }
        if(tmp[0] == "index")
        {

        }
        if(tmp[0] == "error_page ")
        {

        }
        if(tmp[0] == "location")
        {

        }
        servs.push_back(serv);
    }
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
                set_server(++it, conf);
                
        }
        // check }
        it++;
        tmp.clear();
    }
}
