/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:01:17 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/01 13:56:42 by hanebaro         ###   ########.fr       */
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
        void cgi_env(Httprequest &req, const Location &loc);
        //function to execute
        std::string execute(const std::string &script_path, const std::string &body = "");
};