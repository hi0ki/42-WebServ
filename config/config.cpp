/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 16:37:55 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/01 17:23:56 by hanebaro         ###   ########.fr       */
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
    // this->print_servers();
}

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
            // std::cout << cont << "\n" ;
            cont.clear();
        }
        if (str[i] != c)
            cont += str[i];
    }
    if(!cont.empty())
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
void config::set_server(std::vector<std::string>::iterator &it, std::vector<std::string> &conf)
{
    // (void)it;
    // (void)conf;
    // std::cout << "-------------server found \n";
    std::vector<std::string> tmp;
    server serv;
    while(it != conf.end())
    {
        // std::cout << "--------- "<<  *it << std::endl; 
        tmp = split(*it, ' ');
        if(!tmp.empty() && tmp[0] == "listen")// verify if it already exists
        {
            if(!serv.get_IP().empty())
                throw std::runtime_error("listen already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("----content invalid");
            std::vector<std::string> help;
            help = split(tmp[1], ':');
            if(help.size() != 2)
                throw std::runtime_error("listen content invalid");
            serv.set_IP(help[0]);//verify if ip is good??
            if(validnumber(help[1]))
            serv.set_port(atoi(help[1].c_str()));
            help.clear();
        }
        else if(!tmp.empty() && tmp[0] == "server_name")
        {
            if(!serv.get_name().empty())
                throw std::runtime_error("server name already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("server name content invalid");
            serv.set_name(tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "root")
        {
            if(!serv.get_root().empty())
                throw std::runtime_error("root already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("root content invalid");
            serv.set_root(tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "index")
        {
            if(!serv.get_index().empty())
                throw std::runtime_error("index already exist");
            if(tmp.size() != 2)
                throw std::runtime_error("index content invalid");
            serv.set_index(tmp[1]);
        }
        else if(!tmp.empty() && tmp[0] == "error_page")
        {
            if(tmp.size() != 3)
                throw std::runtime_error("error page content invalid");
            ErrPage errpage;
            errpage.err = atoi(tmp[1].c_str());
            if(err_exist(errpage.err, serv.get_errpage()))
                throw std::runtime_error("error page already exists");
            errpage.red_page = tmp[2];
            serv.set_errpage(errpage);
        }
        else if(!tmp.empty() && tmp[0] == "location")
        {

        }
        else if(!tmp.empty() && tmp[0] == "}")
        {
            tmp.clear();
            break;
        }
        else if(!tmp.empty())
        {
            tmp.clear();
            throw std::runtime_error("invalid key");
        }
        tmp.clear();
        it++;
    }
        // verify if listen && root are emtpy, or another things
    if(serv.get_IP().empty() || serv.get_root().empty())
        throw std::runtime_error("empty values");
    servs.push_back(serv);
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
        // exit(1);
        // std::cout << "[ "<< *it << "]\n";
        if(!tmp.empty() && tmp[0] == "server" && tmp[1] == "{")//check server
        {
            if(tmp.size() >= 3)
            {
                // std::cout << "here\n";
                    throw std::runtime_error("##content invalid");
            }
            else
            {
                // std::cout <<*it << "   --" << std::endl;
                set_server(++it, conf);   
            }
        }
        else if (!tmp.empty())
        {
            std::cout << *it << std::endl;
            throw std::runtime_error("content invalid");
        }
        // check }
        it++;
        tmp.clear();
    }
}

void config::print_servers()
{
    std::vector<server>::iterator it = servs.begin();
    int idx = 1;

    while (it != servs.end())
    {
        std::cout << "===== Server " << idx++ << " =====\n";
        std::cout << "IP: " << it->get_IP() << "\n";
        std::cout << "Port: " << it->get_port() << "\n";
        std::cout << "Server Name: " << it->get_name() << "\n";
        std::cout << "Root: " << it->get_root() << "\n";
        std::cout << "Index: " << it->get_index() << "\n";

        // --- Error Pages ---
        std::vector<ErrPage> errpages = it->get_errpage();
        std::vector<ErrPage>::iterator e_it = errpages.begin();
        while (e_it != errpages.end())
        {
            std::cout << "Error Page [" << e_it->err << "] -> " << e_it->red_page << "\n";
            ++e_it;
        }

        // // --- Locations ---
        // std::vector<l_location> locs = it->get_location(); // tu devras écrire get_location()
        // std::vector<l_location>::iterator l_it = locs.begin();
        // while (l_it != locs.end())
        // {
        //     std::cout << "Location path: " << l_it->path << " | Type: ";
        //     switch (l_it->type)
        //     {
        //         case STATIC:   std::cout << "STATIC"; break;
        //         case CGI:      std::cout << "CGI"; break;
        //         case REDIRECT: std::cout << "REDIRECT"; break;
        //         case API:      std::cout << "API"; break;
        //     }
        //     std::cout << "\n";
        //     ++l_it;
        // }

        std::cout << "========================\n";
        ++it;
    }
}