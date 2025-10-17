/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPCGI.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: felhafid <felhafid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:01:17 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/08 17:18:16 by felhafid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include<iostream>
#include<vector>
#include <cstring>     // for strdup
#include <sys/wait.h>  // for waitpid
#include <unistd.h>    // for fork, execve, pipe, etc. (if you use them)=
#include <map>
#include <fcntl.h>

#include "../request/Request.hpp"
// #include "../core_srv/include/ClientData.hpp"

class HTTPCGI
{
    private:
        std::vector<char*> envr;
    public:
        HTTPCGI() {}
        HTTPCGI(Httprequest &req);
        HTTPCGI &operator=(const HTTPCGI &obj);
        void cgi_env(Httprequest &req);
        //function to execute
        // bool canExecuteCGI(Httprequest &req, const Location &loc, std::string& errorMsg);
        int can_execute(config &conf, int index, Httprequest &req);
        std::string execute(const std::string &script_path, std::map<std::string, std::string> post_data);
        void reset_cgi_obj();
};