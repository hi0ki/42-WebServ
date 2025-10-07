/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: felhafid <felhafid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:22 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/07 13:32:09 by felhafid         ###   ########.fr       */
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

std::string getPWDwithWWW() {
    char buffer[PATH_MAX];
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        std::string path(buffer);
        // path += "/www";   // on ajoute /www à la fin
        return path;
    } else {
        perror("getcwd error");
        return "";
    }
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
            spl = split(*it, ' ');
            if(spl.empty())
            {
                it++;
                if(it == end)
                throw std::runtime_error(" '}' is missing ");
                continue;
            }
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if((spl.size() != 2 && spl[0] != "methods") || (spl.size() > 4 && spl[0] == "methods"))
                throw std::runtime_error("invalid location");
            if(spl[0] != "methods")
                check_semicolon(spl[1]);
            if(spl[0] == "root")
                loc.root = getPWDwithWWW() + spl[1];
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
            spl = split(*it, ' ');
            if(spl.empty())
            {
                it++;
                continue;
                if(it == end)
                throw std::runtime_error(" '}' is missing ");
            }
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if((spl.size() != 2 && spl[0] != "methods") || (spl.size() > 4 && spl[0] == "methods"))
                throw std::runtime_error("invalid location");
            if(spl[0] != "methods")
                check_semicolon(spl[1]);
            if(spl[0] == "root")
                loc.root = getPWDwithWWW() + spl[1];
            else if (spl[0] == "index")
                loc.index = spl[1];
            else if (spl[0] == "methods")
            {
                std::vector<std::string> meth;
                for (size_t i = 1; i < spl.size(); i++)
                {
                    if (i == spl.size() - 1)
                        check_semicolon(spl[i]);  // supprime le ';' si présent

                    if (spl[i] != "GET" && spl[i] != "POST" && spl[i] != "DELETE")
                        throw std::runtime_error("invalid method: " + spl[i]);

                    if (std::find(meth.begin(), meth.end(), spl[i]) != meth.end())
                        throw std::runtime_error("duplicate method: " + spl[i]);

                    meth.push_back(spl[i]);
                }
                loc.methods = meth;
            }
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
            spl = split(*it, ' ');
            if(spl.empty())
            {
                it++;
                if(it == end)
                throw std::runtime_error(" '}' is missing ");
                continue;
            }
            if (spl.size() == 1 && spl[0] == "}")
                break;
            if((spl.size() != 2 && spl[0] != "methods") || (spl.size() > 4 && spl[0] == "methods"))
                throw std::runtime_error("invalid location");
            if(spl[0] != "methods")
                check_semicolon(spl[1]);
            if(spl[0] == "root")
                loc.root = getPWDwithWWW() + spl[1];
            else if (spl[0] == "max_upload_size")
                loc.max_upload_size = string_to_sizet(spl[1]);
            else if (spl[0] == "methods")
            {
                std::vector<std::string> meth;
                for (size_t i = 1; i < spl.size(); i++)
                {
                    if (i == spl.size() - 1)
                        check_semicolon(spl[i]);  // supprime le ';' si présent

                    if (spl[i] != "GET" && spl[i] != "POST" && spl[i] != "DELETE")
                        throw std::runtime_error("invalid method: " + spl[i]);

                    if (std::find(meth.begin(), meth.end(), spl[i]) != meth.end())
                        throw std::runtime_error("duplicate method: " + spl[i]);

                    meth.push_back(spl[i]);
                }
                loc.methods = meth;
            }
            else
                throw std::runtime_error("invalid key in location");
            it++;
            if(it == end)
                throw std::runtime_error(" '}' is missing ");
        }
       if(loc.max_upload_size == 0)
            loc.max_upload_size = 1048576;   /// a verifier d ou vient 0
    }
    else if (tmp[1][0] == '/')
    {
        while(it != end && *it != "}")
        {
            spl = split(*it, ' ');
            if(spl.empty())
            {
                it++;
                if(it == end)
                throw std::runtime_error(" '}' is missing ");
                continue;
            }
            if (spl.size() == 1 && spl[0] == "}")
                break;
            // if((spl.size() != 2 && spl[0] != "methods") || (spl.size() > 4 && spl[0] == "methods"))
            //     throw std::runtime_error("invalid location");
            
            if (spl.empty())
                throw std::runtime_error("invalid location");

            // methods
            if (spl[0] == "methods")
            {
                if (spl.size() < 2 || spl.size() > 4)
                    throw std::runtime_error("invalid location (methods must have 2 to 4 args)");
            }
            // return
            else if (spl[0] == "return")
            {
                if (spl.size() != 3)
                    throw std::runtime_error("invalid location (return must have exactly 3 args)");
            }
            // tout le reste
            else if (spl[0] != "extension")
            {
                if (spl.size() != 2)
                    throw std::runtime_error("invalid location (directive must have exactly 2 args)");
            }
            if(spl[0] != "methods" && spl[0] != "return" && spl[0] != "extension")
                check_semicolon(spl[1]);
            // if(spl[0] == "redirect_url")
            // {
            //     loc.redirect_url = spl[1];
            //     loc.path = tmp[1];
            //     loc.type = REDIRECT;
            // }
            if (spl[0] == "return")
            {
                if (spl.size() != 3)
                    throw std::runtime_error("invalid return directive: expected 'return <code> <url>;'");

                // Vérifier que spl[1] est bien un nombre
                for (size_t i = 0; i < spl[1].size(); i++)
                {
                    if (!isdigit(static_cast<unsigned char>(spl[1][i])))
                        throw std::runtime_error("invalid return code: " + spl[1]);
                }

                int code = std::stoi(spl[1]);
                if (code < 100 || code > 599)  // seulement codes HTTP valides
                    throw std::runtime_error("invalid return code: must be between 100 and 599");

                // Vérifier et nettoyer l’URL
                check_semicolon(spl[2]);

                loc.return_r.err = code;
                loc.return_r.red_url = spl[2];
                loc.path = tmp[1];
                loc.type = REDIRECT;
            }
            else if(spl[0] == "root" && loc.return_r.red_url.empty())
            {
                loc.root = getPWDwithWWW() + spl[1];
                if(loc.path.empty())
                    loc.path = tmp[1];
                // if(loc.type == UNDEFINED)
                //     loc.type = CGI;
            }
            // else if(spl[0] == "cgi_handler" && loc.return_r.red_url.empty())
            // {
            //     loc.cgi_handler = spl[1];
            //     if(loc.path.empty())
            //         loc.path = tmp[1];
            //     if(loc.type == UNDEFINED)//why i use UNDEFINED
            //         loc.type = CGI;
            // }
            else if (spl[0] == "cgi_enabled")
            {
                if (spl[1] == "on")
                    loc.cgi_enabled = true;
                else if (spl[1] == "off")
                    loc.cgi_enabled = false;
                else
                    throw std::runtime_error("invalid value for cgi_enabled: " + spl[1]);
                if(loc.type == UNDEFINED)
                    loc.type = CGI; // important pour marquer que c'est une location CGI
                    ///// je dois verifier si j ai redirect et ja i autre key de CGI et le contraire
            }
            else if (spl[0] == "cgi_extension")
            {
                // check_semicolon(spl[1]);
                if (spl[1].empty() || spl[1][0] != '.')
                    throw std::runtime_error("invalid cgi_extension: " + spl[1]);
                loc.cgi_extension.push_back(spl[1]);
                if(loc.type == UNDEFINED)
                    loc.type = CGI;
            }
            else if (spl[0] == "cgi_path")///// todo/// if not existe throw exception
            {
                // check_semicolon(spl[1]);
                if (spl[1].empty())
                    throw std::runtime_error("cgi_path cannot be empty");
                loc.cgi_path = spl[1];  // tu avais déjà `cgi_handler`, tu peux utiliser `cgi_path` ou garder l'ancien
                if(loc.type == UNDEFINED)
                    loc.type = CGI;
            }
            else if (spl[0] == "methods")
            {
                std::vector<std::string> meth;
                for (size_t i = 1; i < spl.size(); i++)
                {
                    if (i == spl.size() - 1)
                        check_semicolon(spl[i]);  // supprime le ';' si présent

                    if (spl[i] != "GET" && spl[i] != "POST" && spl[i] != "DELETE")
                        throw std::runtime_error("invalid method: " + spl[i]);

                    if (std::find(meth.begin(), meth.end(), spl[i]) != meth.end())
                        throw std::runtime_error("duplicate method: " + spl[i]);

                    meth.push_back(spl[i]);
                }
                loc.methods = meth;
                std::cout << "-------------------" << loc.methods.size() << std::endl;
            }
            else
                throw std::runtime_error(spl[0] + "---invalid key in location");
            it++;
            if(it == end)
                throw std::runtime_error(" '}' is missing ");
        }
        if ((loc.type == REDIRECT && loc.return_r.red_url.empty())
            || (loc.type == CGI && (loc.cgi_path.empty() || loc.root.empty())))
            throw std::runtime_error("invalid location");    
    }
    else
        throw std::runtime_error("path invalid");
    this->set_location(loc);
    std::cout << location[location.size() - 1].methods.size() << "##############################"<< std::endl;
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

void server::set_autoindex(std::string nautoindex)
{
    autoindex = nautoindex;
}
  
void server::set_errpage(ErrPage &errpage)
{
    error_page.push_back(errpage);
}

void server::set_location(Location loc)
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

int server::get_autoindex()
{
    if(autoindex == "off")
        return(0);
    else if(autoindex == "on")
        return(1);
    return (-1);    
}

std::vector<Location> server::get_location() const
{
    return(location);
}

std::vector<std::string> server::get_methods() const
{
    return methods;
}

void server::set_methods(const std::vector<std::string> &med)
{
    methods = med;
}