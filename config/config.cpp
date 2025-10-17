/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 16:37:55 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/17 22:07:21 by hanebaro         ###   ########.fr       */
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
    this->parse_configFile();
    if(!servs.size())
        throw std::runtime_error("config file invalid");
    // this->print_servers();
}  
std::vector<server> &config::get_servs()
{
    return this->servs;
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

int err_exist(int err, const std::vector<ErrPage> &error_page)
{
    std::vector<ErrPage>::const_iterator it;
    it = error_page.begin();
    while(it != error_page.end())
    {
        if(it->err == err)
            return(1);
        it++;
    }
    return(0);
}

int check_char_count(const std::string &str, char c)
{
    int count = 0;
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (*it == c)
            count++;
    }
    return (count == 1) ? 0 : 1;
}

void config::set_server(std::vector<std::string>::iterator &it, std::vector<std::string> &conf)
{
    std::vector<std::string> tmp;
    server serv;
    while(it != conf.end())
    {
        tmp = split(*it, ' ');
        if(!tmp.empty() && tmp[0] == "listen")
        {
            if(!serv.get_IP().empty())
                throw std::runtime_error("listen already exist");
            if(tmp.size() != 2 || check_char_count(tmp[1], ':'))
                throw std::runtime_error("content invalid");
            check_semicolon(tmp[1]);
            std::vector<std::string> help;
            help = split(tmp[1], ':');// after that check if exist more than one :
            if(help.size() != 2)
                throw std::runtime_error("listen content invalid");
            serv.set_IP(help[0]);//verify if ip is good??
            if(validnumber(help[1]))
            serv.set_port(atoi(help[1].c_str()));
            // std::cout << serv.get_port();
            help.clear();
        }
        else if(!tmp.empty() && tmp[0] == "server_name")
        {
            if(!serv.get_name().empty())
                throw std::runtime_error("server name already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("server name content invalid");
            check_semicolon(tmp[1]);
            serv.set_name(tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "root")
        {
            if(!serv.get_root().empty())
                throw std::runtime_error("root already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("root content invalid");
            check_semicolon(tmp[1]);
            serv.set_root(getPWDwithWWW() + tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "index")
        {
            if(!serv.get_index().empty())
                throw std::runtime_error("index already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("index content invalid");
            check_semicolon(tmp[1]);
            serv.set_index(tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "autoindex")
        {
            if(serv.get_autoindex() != -1)
                throw std::runtime_error("autoindex already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("autoindex content invalid");
            check_semicolon(tmp[1]);
            serv.set_autoindex(tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "methods")
        {
            
            if(serv.get_methods().size() != 0)
                throw std::runtime_error("methods already exist");
            if(tmp.size() < 2 || tmp.size() > 4)
                throw std::runtime_error("methods content invalid");
            std::vector<std::string> meth;
            for (size_t i = 1; i < tmp.size(); i++)
            {
                if (i == tmp.size() - 1)
                    check_semicolon(tmp[i]);

                if (tmp[i] != "GET" && tmp[i] != "POST" && tmp[i] != "DELETE")
                    throw std::runtime_error("invalid method: " + tmp[i]);

                if (std::find(meth.begin(), meth.end(), tmp[i]) != meth.end())
                    throw std::runtime_error("duplicate method: " + tmp[i]);

                meth.push_back(tmp[i]);
            }
            serv.set_methods(meth);
        }
        else if(!tmp.empty() && tmp[0] == "error_page")
        {
            if(tmp.size() != 3)
                throw std::runtime_error("error page content invalid");
            check_semicolon(tmp[2]);
            ErrPage errpage;
            errpage.err = atoi(tmp[1].c_str());
            if(err_exist(errpage.err, serv.get_errpage()))
                throw std::runtime_error("error page already exists");
            errpage.red_page = tmp[2];
            serv.set_errpage(errpage);
        }
        else if(!tmp.empty() && tmp[0] == "location")
        {
            if(tmp.size() != 3 || tmp[2] != "{")
                throw std::runtime_error("location invalid");
            serv.pars_location(++it, tmp, conf.end());
        }
        else if(!tmp.empty() && tmp[0] == "}")
        {
            tmp.clear();
            break;
        }
        else if(tmp.size() == 2 && tmp[0] == "server" && tmp[1] == "{")
        {
            it--;
            return;
        }
        else if(!tmp.empty())
        {
            std::cout << tmp[0] << std::endl;
            tmp.clear();
            throw std::runtime_error("invalid key");
        }
        tmp.clear();
        it++;
        if (it == conf.end())
            throw std::runtime_error("'}' is missing ");
    }
    if(serv.get_IP().empty() || serv.get_root().empty())
        throw std::runtime_error("empty values");
    
    servs.push_back(serv);
}

bool is_comment(std::string ligne)
{
    size_t i = 0;
    while(i < ligne.size())
    {
        if(ligne[i] != ' ' && ligne[i] != '\t')
            break;
        i++;
    }
    
    if(ligne[i] == '#')
        return(1);
    return(0);
}


bool is_empty(const std::string &ligne)
{
    size_t i = 0;

    while (i < ligne.size() && (ligne[i] == ' ' || ligne[i] == '\t' || ligne[i] == '\n'))
        i++;
    if (i < ligne.size())
        return false;

    return true;
}

bool is_keyword(std::string word)
{
    std::string arr[] = {"listen", "server_name", "root", "index", "methods", "error_page", "autoindex", "location",
                        "return", "cgi_enabled", "cgi_extension", "cgi_path", "return", "}"};
                
    for (int i = 0; i < 14; i++)
    {
        if (arr[i] == word)
            return (true);
    }
    return (false);
}
void config::parse_configFile()
{
    std::vector<std::string> conf;
    std::string ligne;
    std::vector<std::string>::iterator it;
    
    while(getline(File, ligne))
    {
        if(is_empty(ligne) || is_comment(ligne))
            continue;
        conf.push_back(ligne);
    }
    it = conf.begin();
    std::vector<std::string> tmp;
    
    while(it != conf.end())
    {
        tmp = split(*it, ' ');
        try
        {
            if(!tmp.empty() && tmp.size() == 2 && tmp[0] == "server" && tmp[1] == "{")
            {
                    
                if(tmp.size() >= 3)
                {
                    throw std::runtime_error("content invalid\n");
                }
                else
                {
                    set_server(++it, conf);
                    if(it + 1 == conf.end())
                        break;
                    std::vector<std::string> tmp1;
                    tmp1 = split(*(it + 1), ' ');

                    if(!(*it == "}" && tmp1.size() == 2 && tmp1[0] == "server" && tmp1[1] == "{"))
                    {
                        std::cout << RED <<"content invalid" << RESET << std::endl;
                        if(!(tmp1.size() == 2 && tmp1[0] == "server" && tmp1[1] == "{"))
                        {
                            tmp1.clear();
                            while(servs.size() != 0)
                                servs.pop_back();
                            break;  
                        }                    }
                    tmp1.clear();
                }       
            }
            else if(!tmp.empty() && !is_keyword(tmp[0]))
            {
                while(servs.size() != 0)
                    servs.pop_back();
                break;
            }
            else if (!tmp.empty())
            {
                it++;
                continue;
            }
        }
        catch (std::exception &e)
        {
            std::cerr << RED << e.what() << RESET << std::endl;
        }
        if(it == conf.end())
            break;
        it++;
        tmp.clear();
    }
}
