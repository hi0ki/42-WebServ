/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 15:24:58 by hanebaro          #+#    #+#             */
/*   Updated: 2025/09/01 16:50:16 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <algorithm>
#include <fstream>
#include "server.hpp"
#include <vector>

class config 
{
    private:
        std::ifstream File;
        std::vector<server> servs;
    public:
        config(std::string nameFile);
        void parse_configFile();
        void set_server(std::vector<std::string>::iterator &it, std::vector<std::string> &conf);
        std::vector<server> &get_servs()
        {
            return this->servs;
        }
};