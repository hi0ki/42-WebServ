/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:22 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/04 20:15:27 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
std::vector<std::string> split(const std::string &str, char c)
{
    std::vector<std::string>  result;
    std::string cont;
    
    for(size_t i = 0; i < str.size(); i++)
    {
        if(str[i] == '#' || str[i] == ';')
            break;
        if(str[i] == c && cont.size())// a verifier cont.size()
        {
            result.push_back(cont);
            cont.clear();
        }
        if (str[i] != c)
            cont += str[i];
    }
    if(!cont.empty())
        result.push_back(cont);
    return(result);
}

void check_semicolon(std::string &str)
{
    if (str.empty())
        throw std::runtime_error("Empty string");

    if (str[str.size() - 1] == ';')
        str.erase(str.size() - 1); // supprime le dernier caractère
    else
        throw std::runtime_error("Missing semicolon at the end");
}

void server::pars_errPage()
{
    
}
    
void server::pars_location(std::vector<std::string>::iterator &it, std::vector<std::string> &tmp, std::vector<std::string>::iterator &end)
{
    std::vector<std::string> spl;
    Location loc;
    if (tmp[1] == "/")
    {
       loc.path = tmp[1];
       loc.type = STATIC;;
       while(it != end && *it != "}")
       {
           spl = split(*it, ' ');
           if(spl.size() != 2)
               throw std::runtime_error("invalid location");
           check_semicolon(spl[1]);
           if(spl[0] == "root")
               loc.root = spl[1];
           else if (spl[0] == "index")
               loc.index = spl[1];
           else
               throw std::runtime_error("invalid key in location");
           if(index.empty())// check if index empty
               throw std::runtime_error("empty index in location");
           it++;
           if(it == end)
               throw std::runtime_error(" '}' is missing ");
       }
    }
    else if (tmp[1] == "/api")
    {
        
       loc.path = tmp[1];
       loc.type = STATIC;;
       while(it != end && *it != "}")
       {
           spl = split(*it, ' ');
           if(spl.size() != 2)
               throw std::runtime_error("invalid location");
           check_semicolon(spl[1]);
           if(spl[0] == "root")
               loc.root = spl[1];
           else if (spl[0] == "index")
               loc.index = spl[1];
           else
               throw std::runtime_error("invalid key in location");
           if(index.empty())// check if index empty
               throw std::runtime_error("empty index in location");
           it++;
           if(it == end)
               throw std::runtime_error(" '}' is missing ");
       }
    }
    else if (tmp[1] == "/upload")
    {
        
    }
    else if (tmp[1][0] == '/')
    {
        
    }
    else
        throw std::runtime_error("path invalid");

    // it++;
}
    
void server::pars_serv()
{
    
}
    
void server::set_IP(std::string ip)
{
    IP = ip;
}
    
void server::set_port(int  nport)
{
    port = nport;
}
    
std::string server::get_IP()
{
    return(IP);
}
    
int server::get_port()
{
    return(port);
}
    
void server::set_name(std::string sname)
{
    servname = sname;
}
    
void server::set_root(std::string nroot)
{
    root = nroot;
}
    
void server::set_index(std::string nindex)
{
    index = nindex;
}
    
void server::set_errpage(ErrPage errpage)
{
    error_page.push_back(errpage);
}

std::vector<ErrPage> server::get_errpage()
{
    return(error_page);
}
    
std::string server::get_name()
{
    return(servname);
}
    
std::string server::get_root()
{
    return(root);
}
    
std::string server::get_index()
{
    return(index);
}
    