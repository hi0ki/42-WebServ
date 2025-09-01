/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:22 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/01 16:43:28 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
void server::pars_errPage()
{
    
}
    
void server::pars_location()
{
    
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
    