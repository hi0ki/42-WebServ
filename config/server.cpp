/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: felhafid <felhafid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:22 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/08 14:43:28 by felhafid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"


#include <string>
#include <cstdlib>
#include <stdexcept>
// a revoir ces headers

std::vector<std::string> split(const std::string &str, char c)
{
    std::vector<std::string>  result;
    std::string cont;
    
    for(size_t i = 0; i < str.size(); i++)
    {
        if(str[i] == '#')
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

    size_t pos = str.find(';'); // chercher la première occurrence de ';'
    if (pos == std::string::npos)
        throw std::runtime_error("Missing semicolon in: " + str);

    str = str.substr(0, pos); // garder seulement ce qui est avant le premier ';'
}

void server::pars_errPage()
{
    
}

size_t string_to_sizet(const std::string &str)
{
    if (str.empty())
        throw std::runtime_error("Empty string");

    char *endptr;
    unsigned long val = strtoul(str.c_str(), &endptr, 10);

    if (*endptr != '\0') // si un caractère non numérique est présent
        throw std::runtime_error("Invalid character in number");

    return static_cast<size_t>(val);
}

void server::pars_location(std::vector<std::string>::iterator &it, std::vector<std::string> &tmp, std::vector<std::string>::iterator end)
{
    //// initialise the variables
    std::vector<std::string> spl;
    Location loc;
    loc.type = UNDEFINED;
    if (tmp[1] == "/")
    {
       loc.path = tmp[1];
       loc.type = STATIC;
       while(it != end && *it != "}")
       {
            if((*it).empty())
                continue;
            spl = split(*it, ' ');
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if(spl.size() != 2)
                throw std::runtime_error("invalid location");
            check_semicolon(spl[1]);
            if(spl[0] == "root")
                loc.root = spl[1];
            else if (spl[0] == "index")
                loc.index = spl[1];
            else
                throw std::runtime_error("invalid key in location");
            it++;
            if(it == end)
                throw std::runtime_error(" '}' is missing ");
       }
        if(loc.index.empty())// check if index empty
            throw std::runtime_error("empty index in location");
    }
    else if (tmp[1] == "/api")
    {
       loc.path = tmp[1];
       loc.type = API;
       while(it != end && *it != "}")
       {
            if((*it).empty())
                continue;    
            spl = split(*it, ' ');
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if(spl.size() != 2)
                throw std::runtime_error("invalid location");
            check_semicolon(spl[1]);
            if(spl[0] == "root")
                loc.root = spl[1];
            else if (spl[0] == "index")
                loc.index = spl[1];
            else
                throw std::runtime_error("invalid key in location");
            it++;
            if(it == end)
                throw std::runtime_error(" '}' is missing ");
       }
    }
    else if (tmp[1] == "/upload")
    {
        loc.path = tmp[1];
        loc.type = UPLOAD;
        while(it != end && *it != "}")
        {
            if((*it).empty())
                continue;    
            spl = split(*it, ' ');
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if(spl.size() != 2)
                throw std::runtime_error("invalid location");
            check_semicolon(spl[1]);
            if(spl[0] == "root")
                loc.root = spl[1];
            else if (spl[0] == "max_upload_size")
                loc.max_upload_size = string_to_sizet(spl[1]);
            else
                throw std::runtime_error("invalid key in location");
            it++;
            if(it == end)
                throw std::runtime_error(" '}' is missing ");
        }
       if(loc.max_upload_size == 0)
            loc.max_upload_size = 1048576;   
    }
    else if (tmp[1][0] == '/')
    {
        while(it != end && *it != "}")
        {
            if((*it).empty())
                continue;
            spl = split(*it, ' ');
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if(spl.size() != 2)
                throw std::runtime_error("invalid location");
            check_semicolon(spl[1]);
            if(spl[0] == "redirect_url")
            {
                loc.redirect_url = spl[1];
                loc.path = tmp[1];
                loc.type = REDIRECT;
            }
            else if(spl[0] == "root" && loc.redirect_url.empty())
            {
                loc.root = spl[1];
                if(loc.path.empty())
                    loc.path = tmp[1];
                if(loc.type == UNDEFINED)
                    loc.type = CGI;
            }
            else if(spl[0] == "cgi_handler" && loc.redirect_url.empty())
            {
                loc.cgi_handler = spl[1];
                if(loc.path.empty())
                    loc.path = tmp[1];
                if(loc.type == UNDEFINED)
                    loc.type = CGI;
            }
            else
                throw std::runtime_error("invalid key in location");
            it++;
            if(it == end)
                throw std::runtime_error(" '}' is missing ");
        }
        if ((loc.type == REDIRECT && loc.redirect_url.empty())
            || (loc.type == CGI && (loc.cgi_handler.empty() || loc.root.empty())))
            throw std::runtime_error("invalid location");    
    }
    else
        throw std::runtime_error("path invalid");
    set_location(loc);
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
    
void server::set_errpage(ErrPage &errpage)
{
    error_page.push_back(errpage);
}

void server::set_location(Location &loc)
{
    location.push_back(loc);
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

std::vector<Location> server::get_location() const
{
    return(location);
}

bool server::get_autoindexEnabled() const
{
    return this->autoindexEnabled;
}