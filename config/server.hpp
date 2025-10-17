/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 10:38:26 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/17 11:32:30 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <unistd.h>   // getcwd
#include <limits.h>   // PATH_MAX
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"  
enum LocationType {UNDEFINED, STATIC, CGI, API, UPLOAD, REDIRECT};

struct ErrPage
{
    int err;
    std::string red_page;
};

struct return_red
{
    int err;
    std::string red_url ;
};

struct Location {
    std::string path;              // ex: "/blog" ou "/api"
    LocationType type;             // type de la location
    std::string root;              // pour STATIC, CGI, UPLOAD
    std::string index;             // pour STATIC
    // std::string cgi_handler;       // pour CGI (ex: "/usr/bin/php-cgi")
    bool cgi_enabled;
    std::vector<std::string> cgi_extension;
    std::vector<std::string> cgi_path; // pour CGI (ex: "/usr/bin/php-cgi")
    return_red return_r;      // pour REDIRECT (ex: "https://monsite.com/new")
    long long max_upload_size;        // utile pour UPLOAD (limite taille en bytes)
    std::vector<std::string> limit_except;//fatimazahra zadtha
     //client_max_body_size 5M;  
    std::vector<std::string> methods ;
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
        std::string autoindex;
        std::vector<ErrPage> error_page;
        std::vector<Location> location;
        std::vector<std::string> methods;
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
        void set_autoindex(std::string nindex);
        void set_errpage(ErrPage &errpage);//push_back
        void set_location(Location loc);
        std::vector<Location> get_location() const;
        std::vector<ErrPage> get_errpage();
        std::string get_name();
        std::string get_root();
        std::string get_index();
        int get_autoindex();
        std::vector<std::string> get_methods() const;
        void set_methods(const std::vector<std::string> &med);
        std::vector<std::string> get_cgi_ext();
};
void check_semicolon(std::string &str);
std::vector<std::string> split(const std::string &str, char c);
std::string getPWDwithWWW();