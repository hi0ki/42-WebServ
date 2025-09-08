/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: felhafid <felhafid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:26 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/08 15:00:32 by felhafid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <vector>

enum LocationType {UNDEFINED, STATIC, CGI, API, UPLOAD, REDIRECT};

struct ErrPage
{
    int err;
    std::string red_page;
};

struct Location {
    std::string path;              // ex: "/blog" ou "/api"
    LocationType type;             // type de la location
    std::string root;              // pour STATIC, CGI, UPLOAD
    std::string index;             // pour STATIC
    std::string cgi_handler;       // pour CGI (ex: "/usr/bin/php-cgi")
    std::string redirect_url;      // pour REDIRECT (ex: "https://monsite.com/new")
    size_t max_upload_size;        // utile pour UPLOAD (limite taille en bytes)
};

// struct Location {
//     std::string path;
//     LocationType type;
//     std::string root;
//     std::string index;
//     std::string cgi_handler;
//     std::string redirect_url;
//     size_t max_upload_size;
// };

class server
{
    private:
        std::string IP;
        int  port;
        std::string servname;
        std::string root;
        std::string index;
        std::vector<ErrPage> error_page;
        std::vector<Location> location;
        bool autoindexEnabled;
    public:
        void pars_errPage();
        void pars_location(std::vector<std::string>::iterator &it, std::vector<std::string> &tmp, std::vector<std::string>::iterator end);
        void pars_serv();
        void set_IP(std::string ip);
        void set_port(int  nport);
        std::string get_IP();
        int get_port();
        void set_name(std::string sname);
        void set_root(std::string nroot);
        void set_index(std::string nindex);
        void set_errpage(ErrPage &errpage);//push_back
        void set_location(Location &loc);
        std::vector<Location> get_location() const;
        std::vector<ErrPage> get_errpage();
        std::string get_name();
        std::string get_root();
        std::string get_index();
        bool get_autoindexEnabled() const;
};
void check_semicolon(std::string &str);
std::vector<std::string> split(const std::string &str, char c);