/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPCGI.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:01:17 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/04 15:05:21 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include<iostream>
#include<vector>

#include "../request/request.hpp"
class HTTPCGI
{
    private:
        std::vector<char*> envr;
    public:
        HTTPCGI(Httprequest &req, const Location &loc);
        void cgi_env(Httprequest &req, const Location &loc);
        //function to execute
        // bool canExecuteCGI(Httprequest &req, const Location &loc, std::string& errorMsg);
        std::string execute(const std::string &script_path, const std::string &body);
};