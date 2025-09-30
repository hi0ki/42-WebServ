/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:00:54 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/30 18:25:49 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

std::vector<std::string> CGI::cgi_env(const Httprequest &req, const Location &loc)
{
    env.push_back("REQUEST_METHOD=" + req.method);
    
}