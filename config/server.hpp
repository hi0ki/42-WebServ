/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:26 by hanebaro          #+#    #+#             */
/*   Updated: 2025/08/31 20:10:43 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>

enum locationType {STATIC, CGI, REDIRECT, API};
struct l_location
{
    std::string path;
    locationType type;
};

struct ErrPage
{
    int *err;
    std::string red_page;
};

class server
{
    private:
        std::string IP;
        int  port;
        std::vector<std::string> servname;
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
};