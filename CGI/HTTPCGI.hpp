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

#include "../request/Request.hpp"
#include "../core_srv/include/ClientData.hpp"

class HTTPCGI
{
    private:
        std::vector<char*> envr;
    public:
        HTTPCGI(Httprequest &req, const Location &loc);
        void cgi_env(Httprequest &req, const Location &loc);
        //function to execute
        // bool canExecuteCGI(Httprequest &req, const Location &loc, std::string& errorMsg);
        int can_execute(config &conf, int index, Httprequest &req);
        std::string execute(const std::string &script_path, ClientData &client);
};