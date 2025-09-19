/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 16:37:55 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/19 13:14:27 by hanebaro         ###   ########.fr       */
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
    this->print_servers();
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
    // check semi colon//verify if ligne end with ; && and if exist deleted it in every if 
    while(it != conf.end())
    {
        tmp = split(*it, ' ');
        if(!tmp.empty() && tmp[0] == "listen")// verify if it already exists
        {
            if(!serv.get_IP().empty())
                throw std::runtime_error("listen already exist");
            if(tmp.size() != 2 || check_char_count(tmp[1], ':'))
                throw std::runtime_error("----content invalid");
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
            serv.set_root(tmp[1]);
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
            //check if it == end and we dont foud }, if end throw exeption// if i need it
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
        if (it == conf.end())
            throw std::runtime_error(" '}' is missing ");
    }
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
        conf.push_back(ligne);
    }
    it = conf.begin();
    std::vector<std::string> tmp;
    
    while(it != conf.end())// apres split verif lesligne vide
    {
        tmp = split(*it, ' ');
        try
        {
            if(!tmp.empty() && tmp[0] == "server" && tmp[1] == "{")//check server
            {
                    
                if(tmp.size() >= 3)
                {
                    throw std::runtime_error("##content invalid");
                }
                else
                {
                    set_server(++it, conf);   
                    
                }       
            }
            else if (!tmp.empty())
                throw std::runtime_error("content invalid");
        }
        catch (std::exception &e)
        {
            std::cerr << RED << e.what() << RESET << std::endl;
        }
        it++;
        tmp.clear();
    }
}

// void config::print_servers() // print server
// {
//     std::vector<server>::iterator it = servs.begin();
//     int idx = 1;

//     while (it != servs.end())
//     {
//         std::cout << "===== Server " << idx++ << " =====\n";
//         std::cout << "IP: " << it->get_IP() << "\n";
//         std::cout << "Port: " << it->get_port() << "\n";
//         std::cout << "Server Name: " << it->get_name() << "\n";
//         std::cout << "Root: " << it->get_root() << "\n";
//         std::cout << "Index: " << it->get_index() << "\n";
//         std::cout << "Autoindex: " << it->get_autoindex() << "\n";

        
//         // --- Error Pages ---
//         std::vector<ErrPage> errpages = it->get_errpage();
//         std::vector<ErrPage>::iterator e_it = errpages.begin();
//         while (e_it != errpages.end())
//         {
//             std::cout << "Error Page [" << e_it->err << "] -> " << e_it->red_page << "\n";
//             ++e_it;
//         }

//         // --- Locations ---
//         std::vector<Location> locs = it->get_location(); // tu devras écrire get_location()
//         std::vector<Location>::iterator l_it = locs.begin();
//         while (l_it != locs.end())
//         {
//             std::cout << "Location path: " << l_it->path << " | Type: ";
//             switch (l_it->type)
//             {
//                 case STATIC:   std::cout << "STATIC"; break;
//                 case CGI:      std::cout << "CGI"; break;
//                 case REDIRECT: std::cout << "REDIRECT"; break;
//                 case API:      std::cout << "API"; break;
//                 case UPLOAD:   std::cout << "UPLOAD"; break;
//                 case UNDEFINED: std::cout << "UNDEFINED"; break;
                
//             }
//             std::cout << "\n";
//             ++l_it;
//         }

//         std::cout << "========================\n";
//         ++it;
//     }
// }

void config::print_servers() // print server
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
        std::cout << "Autoindex: " << it->get_autoindex() << "\n";

        // --- Server Methods ---
        std::vector<std::string> srv_methods = it->get_methods();
        if (!srv_methods.empty())
        {
            std::cout << "Methods: ";
            for (std::vector<std::string>::iterator m_it = srv_methods.begin(); m_it != srv_methods.end(); ++m_it)
            {
                std::cout << *m_it;
                if (m_it + 1 != srv_methods.end())
                    std::cout << ", ";
            }
            std::cout << "\n";
        }

        // --- Error Pages ---
        std::vector<ErrPage> errpages = it->get_errpage();
        std::vector<ErrPage>::iterator e_it = errpages.begin();
        while (e_it != errpages.end())
        {
            std::cout << "Error Page [" << e_it->err << "] -> " << e_it->red_page << "\n";
            ++e_it;
        }

        // --- Locations ---
        std::vector<Location> locs = it->get_location();
        std::vector<Location>::iterator l_it = locs.begin();
        while (l_it != locs.end())
        {
            std::cout << "Location path: " << l_it->path << " | Type: ";
            switch (l_it->type)
            {
                case STATIC:   std::cout << "STATIC"; break;
                case CGI:      std::cout << "CGI"; break;
                case REDIRECT: std::cout << "REDIRECT"; break;
                case API:      std::cout << "API"; break;
                case UPLOAD:   std::cout << "UPLOAD"; break;
                case UNDEFINED: std::cout << "UNDEFINED"; break;
            }
            std::cout << "\n";

            // --- Location Methods ---
            if (!l_it->methods.empty())
            {
                std::cout << "  Methods: ";
                for (std::vector<std::string>::iterator m_it = l_it->methods.begin(); m_it != l_it->methods.end(); ++m_it)
                {
                    std::cout << *m_it;
                    if (m_it + 1 != l_it->methods.end())
                        std::cout << ", ";
                }
                std::cout << "\n";
            }

            ++l_it;
        }

        std::cout << "========================\n";
        ++it;
    }
}
