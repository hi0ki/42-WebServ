/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:01:17 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/30 18:26:34 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include<iostream>
#include<vector>

#include "../request/request.hpp"
class CGI
{
    private:
        std::vector<std::string> env;
    public:
        std::vector<std::string> cgi_env(const Httprequest &req, const Location &loc);
        //function to execute
    
};