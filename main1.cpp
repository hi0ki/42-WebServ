#include <iostream>
#include <map>
#include <string>
#include "CGI/HTTPCGI.hpp"

int main()
{
    HTTPCGI cgi;

    // Créer une fausse requête pour tester
    Httprequest req;
    Location loc;
    
    // Simuler les données de la requête
    req.setMethod("GET");
    // req.set_query_string("");
    req.setAbsolutePath("/Users/hanebaro/Desktop/webserv/www/html/cgi/test.py");
    
    // ✅ IMPORTANT : Initialiser l'environnement CGI AVANT execute()
    cgi.cgi_env(req, loc);

    // Maintenant execute() utilisera envr correctement
    std::string script = "/Users/hanebaro/Desktop/webserv/www/html/cgi/test.py";
    std::string output = cgi.execute(script);

    std::cout << "=== CGI Output ===" << std::endl;
    std::cout << output << std::endl;

    return 0;
}