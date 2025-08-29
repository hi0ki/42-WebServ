/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/29 15:24:58 by hanebaro          #+#    #+#             */
/*   Updated: 2025/08/29 16:46:07 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

    // std::string path;
    // std::vector<std::string> allowed_methods;
    // std::string root;
    // bool autoindex;

    // LocationType type;

    // // spécifiques
    // std::string cgi_path;     // utilisé seulement si type == CGI
    // std::string redirect_url; // utilisé seulement si type == REDIRECT




//     try_files $uri $uri/ =404; :
// Cette directive vérifie si le fichier demandé existe et agit selon le résultat :

// $uri → essaye de trouver un fichier exact correspondant à l'URL demandée.
// Exemple : si l'utilisateur demande /index.html, nginx cherche un fichier index.html dans le root.

// $uri/ → si ce n'est pas un fichier, nginx cherche un dossier correspondant et tente d'utiliser le fichier index par défaut (souvent index.html ou index.php selon la config).
// Exemple : /blog/ devient /blog/index.html.

// =404 → si aucune des deux options précédentes ne marche, nginx renvoie une erreur 404 Not Found.

#pragma once
#include <iostream>
#include <fstream>
#include "server.hpp"

class config 
{
    private:
        std::ifstream File;
        server *servs;
    public:
        config(std::string nameFile);
        void parse_configFile();
};