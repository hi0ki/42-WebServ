/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:26 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/01 17:28:18 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>

enum locationType {STATIC, CGI, REDIRECT, API, UPLOAD};
struct l_location
{
    std::string path;
    locationType type;
};

struct ErrPage
{
    int err;
    std::string red_page;
};

class server
{
    private:
        std::string IP;
        int  port;
        std::string servname;
        std::string root;
        std::string index;
        std::vector<ErrPage> error_page;
        std::vector<l_location> location;
    public:
        void pars_errPage();
        void pars_location();
        void pars_serv();
        void set_IP(std::string ip);
        void set_port(int  nport);
        std::string get_IP();
        int get_port();
        void set_name(std::string sname);
        void set_root(std::string nroot);
        void set_index(std::string nindex);
        void set_errpage(ErrPage errpage);//push_back
        std::vector<ErrPage> get_errpage();
        std::string get_name();
        std::string get_root();
        std::string get_index();
};