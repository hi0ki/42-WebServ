/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPCGI.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:00:54 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/13 16:58:18 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "HTTPCGI.hpp"

/// dont forget delete (free the memory)
HTTPCGI::HTTPCGI(Httprequest &req, const Location &loc)
{
    cgi_env(req, loc);
}

void HTTPCGI::cgi_env(Httprequest &req, const Location &loc)
{
    std::vector<std::string> env;
    env.push_back("REQUEST_METHOD=" + req.getMethod());
    env.push_back("QUERY_STRING=" + req.get_query_string());
    std::map<std::string, std::string> headers = req.getHeaders();
    if(req.getMethod() == "POST")
    {
        std::map<std::string, std::string>::const_iterator it = headers.find("Content-Length");
        if (it != headers.end())
            env.push_back("CONTENT_LENGTH=" + it->second);
        it = headers.find("Content-Type");
        if (it != headers.end())
            env.push_back("CONTENT_TYPE=" + it->second);
    }
    // i need location root biiiiiiiiiig waaaaaaaaarniiiiiiiing ????
    env.push_back("SCRIPT_FILENAME=" + req.getAbsolutePath());// without file name, ask fatima zahraa
    env.push_back("SERVER_SOFTWARE=webserv/1.0");
    env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    
    // HTTP headers → en variables HTTP_*
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::string key = it->first;
        // Transforme en majuscules et remplace '-' par '_'
        for (size_t i = 0; i < key.size(); i++) {
            if (key[i] == '-') key[i] = '_';
            else key[i] = toupper(key[i]);
        }
        env.push_back("HTTP_" + key + "=" + it->second);
    }
    // Conversion en `char*` pour execve
    // std::vector<char*> envr;
    for (size_t i = 0; i < envr.size(); i++)
    {
        envr.push_back(strdup(env[i].c_str())); // strdup car execve attend des pointeurs valides
    }
    envr.push_back(NULL); // fin du tableau

    // 🔹 Print pour debug
    // std::cout << "---- CGI ENV ----" << std::endl;
    // for (size_t i = 0; i < env.size(); i++) {
    //     std::cout << env[i] << std::endl;
    // }
    // std::cout << "-----------------" << std::endl;

    // return envr;
}

int HTTPCGI::can_execute(config &conf, int index, Httprequest req)
{
    const std::vector<Location>& locations = conf.get_servs()[index].get_location();
    // for(size_t i = 0; i < locations.size(); i++)
    // {
    //     if(locations[i].type == CGI)
    //     {
    //         std::cout << RED << "waaaaaa dkhllllllll \n" << std::endl;
            
    //         if(locations[i].cgi_enabled == false)
    //             return(403);
    //         //// chech if method vide if yes check global methods
    //         std::vector<std::string>::iterator exist;
            
    //             // exit(1);
    //             std::cout << RED << "------------------------------------------if     " << locations[i].methods.size() << std::endl;
    //         if(locations[i].methods.size())
    //         {
    //             std::cout << "heeeeeeeereeeeeee if     " << locations[i].cgi_path << std::endl;
                
    //             exist = std::find(locations[i].methods.begin(), locations[i].methods.end(), req.getMethod());
    //         }
    //         else if (conf.get_servs()[index].get_methods().size())
    //         {
    //             std::cout << "heeeeeeeereeeeeee else     " << std::endl;
    //             exist = std::find(conf.get_servs()[index].get_methods().begin(), locations[i].methods.end(), req.getMethod());
    //             std::cout << "heeeeeeeereeeeeee else     " << std::endl;
            
    //         }
    //         if(exist == locations[i].methods.end())
    //         {
    //             std::cout << "heeeeeeeereeeeeee else     " << std::endl;
    //             return(405);
    //         }
    //         //cgi_extension
    //         std::string ext;
    //         for(int i = req.getAbsolutePath().size() - 1; i >= 0; --i)
    //         {
    //             if(req.getAbsolutePath()[i] == '.')
    //                 ext = req.getAbsolutePath().substr(i);
    //         }
    //         if(ext.empty())
    //             return(403);
    //         else
    //         {
    //             exist = std::find(locations[i].cgi_extension.begin(), locations[i].cgi_extension.end(), ext);
    //             if(exist == locations[i].cgi_extension.end())
    //             return(403);
    //         }
    //         //cgi_handler or cgi_path
    //         struct stat st;
    //         if (stat(locations[i].cgi_path.c_str(), &st) != 0) {
    //             // std::cerr << "Error: " << locations[i].cgi_path << " does not exist.\n";
    //             return (500);
    //         }
    //         if (!S_ISREG(st.st_mode)) {
    //             // std::cerr << "Error: " << locations[i].cgi_path << " is not a regular file.\n";
    //             return (500);
    //         }
    //         if (access(locations[i].cgi_path.c_str(), X_OK | F_OK) != 0) {
    //             // std::cerr << "Error: " << locations[i].cgi_path << " is not executable.\n";
    //             return (500);
    //         }
    //         /// a verifier
    //         return(0);
    //     }
    // }
    // for(std::vector<Location>::iterator it = conf.get_servs()[index].get_location().begin(); it != conf.get_servs()[index].get_location().end(); it++)
    // {
        
    // }




    // Code corrigé avec tous les bugs fixés

    for(size_t i = 0; i < locations.size(); i++)
    {
        if(locations[i].type == CGI)
        {
            std::cout << RED << "CGI Location found!" << RESET << std::endl;
            
            // 1. Check if CGI is enabled
            if(locations[i].cgi_enabled == false)
            {
                std::cout << RED << "CGI not enabled" << RESET << std::endl;
                return 403;
            }
            
            // 2. Check if HTTP method is allowed
            std::vector<std::string>::const_iterator exist1;
            std::vector<std::string> methods_to_check;

            if (!locations[i].methods.empty())
            {
                std::cout << "in location" << std::endl;
                methods_to_check = locations[i].methods;
            }
            else if (!conf.get_servs()[index].get_methods().empty())
            {
                std::cout << "in global serv" << std::endl;
                methods_to_check = conf.get_servs()[index].get_methods(); // ✅ copie locale
            }

            if (!methods_to_check.empty())
            {
                std::vector<std::string>::const_iterator exist =
                    std::find(methods_to_check.begin(), methods_to_check.end(), req.getMethod());

                if (exist == methods_to_check.end())
                {
                    std::cout << RED << "Method " << req.getMethod() << " not allowed" << RESET << std::endl;
                    return 405;
                }
                else
                {
                    std::cout << GREEN << "Method " << req.getMethod() << " is allowed" << RESET << std::endl;
                }
            }

            
            // 3. Check file extension
            std::string ext;
            std::string path = req.getAbsolutePath();
            
            // ✅ FIX 3: Utiliser size_t au lieu de int pour éviter les warnings
            for(size_t j = path.size(); j > 0; --j)
            {
                if(path[j - 1] == '.')
                {
                    ext = path.substr(j - 1);
                    break;  // ✅ FIX 4: Ajouter break pour sortir dès qu'on trouve
                }
            }
            
            std::cout << "File extension: " << ext << std::endl;
            
            if(ext.empty())
            {
                std::cout << RED << "No file extension found" << RESET << std::endl;
                return 403;
            }
            
            // Check if extension is allowed
            if(locations[i].cgi_extension.size() > 0)
            {
                exist1 = std::find(locations[i].cgi_extension.begin(), 
                                locations[i].cgi_extension.end(), 
                                ext);
                
                if(exist1 == locations[i].cgi_extension.end())
                {
                    std::cout << RED << "Extension " << ext << " not allowed" << RESET << std::endl;
                    return 403;
                }
                else
                {
                    std::cout << GREEN << "Extension " << ext << " is allowed" << RESET << std::endl;
                }
            }
            
            // // 4. Check interpreter (cgi_handler or cgi_path)
            // // ✅ FIX 5: Vérifier que cgi_path n'est pas vide
            // if(locations[i].cgi_path.empty())
            // {
            //     std::cout << RED << "No CGI interpreter path configured" << RESET << std::endl;
            //     return 500;
            // }
            
            // std::cout << "Checking interpreter: " << locations[i].cgi_path[0] << std::endl;///// d ici 
            
            // struct stat st;
            // if (stat(locations[i].cgi_path[0].c_str(), &st) != 0)
            // {
            //     std::cout << RED << "Interpreter does not exist: " << locations[i].cgi_path[0] << RESET << std::endl;
            //     return 500;
            // }
            
            // if (!S_ISREG(st.st_mode))
            // {
            //     std::cout << RED << "Interpreter is not a regular file: " << locations[i].cgi_path[0] << RESET << std::endl;
            //     return 500;
            // }
            
            // if (access(locations[i].cgi_path[0].c_str(), X_OK) != 0)
            // {
            //     std::cout << RED << "Interpreter is not executable: " << locations[i].cgi_path[0] << RESET << std::endl;// jusqu a ici
            //     return 500;
            // }



            // 4. Check interpreters (cgi_handler or cgi_path)
            if (locations[i].cgi_path.empty())
            {
                std::cout << RED << "No CGI interpreter path configured" << RESET << std::endl;
                return 500;
            }

            for (size_t j = 0; j < locations[i].cgi_path.size(); ++j)
            {
                const std::string &path = locations[i].cgi_path[j];
                std::cout << "Checking interpreter: " << path << std::endl;

                struct stat st;
                if (stat(path.c_str(), &st) != 0)
                {
                    std::cout << RED << "Interpreter does not exist: " << path << RESET << std::endl;
                    return 500;
                }

                if (!S_ISREG(st.st_mode))
                {
                    std::cout << RED << "Interpreter is not a regular file: " << path << RESET << std::endl;
                    return 500;
                }

                if (access(path.c_str(), X_OK) != 0)
                {
                    std::cout << RED << "Interpreter is not executable: " << path << RESET << std::endl;
                    return 500;
                }
            }

            std::cout << GREEN << "All CGI interpreters are valid and executable." << RESET << std::endl;

            
            
            // 5. Check if script file exists and is readable
            if (access(req.getAbsolutePath().c_str(), F_OK) != 0)
            {
                std::cout << RED << "CGI script not found: " << req.getAbsolutePath() << RESET << std::endl;
                return 404;
            }
            
            if (access(req.getAbsolutePath().c_str(), R_OK) != 0)
            {
                std::cout << RED << "CGI script not readable: " << req.getAbsolutePath() << RESET << std::endl;
                return 403;
            }
            
            std::cout << GREEN << "All CGI checks passed!" << RESET << std::endl;
            return 0;  // ✅ Success!
        }
    }
    // Si on arrive ici, aucune location CGI trouvée
    return 404;

    // return(1);
}

std::string HTTPCGI::execute(const std::string &script_path, const std::string &body)
{
    
    // if(can_execute(conf, index))
    //     return;
    int pipefd[2];
    if (pipe(pipefd) == -1)
        return "500 Internal Server Error";

    pid_t pid = fork();
    if (pid < 0)
        return "500 Internal Server Error";
    if (pid == 0)
    {
        // Rediriger stdout vers pipe (sortie CGI → parent)
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // Si POST → donner body via STDIN
        if (!body.empty())
        {
            int input_pipe[2];
            pipe(input_pipe);
            pid_t writer = fork();
            if (writer == 0) {
                close(input_pipe[0]);
                write(input_pipe[1], body.c_str(), body.size());
                close(input_pipe[1]);
                _exit(0);
            }
            close(input_pipe[1]);
            dup2(input_pipe[0], STDIN_FILENO);
            close(input_pipe[0]);
        }
        // Préparer argv pour execve
        char *argv[] = {
            strdup(script_path.c_str()), // programme
            NULL
        };

        // char *argv[] = {
        //     strdup("/usr/bin/python3"),                 // argv[0] = programme
        //     strdup("/var/www/html/cgi/hello.py"),       // argv[1] = script
        //     NULL
        // };

        // Exécuter le script CGI
        execve(argv[0], argv, envr.data());
        perror("execve"); // si execve échoue
        _exit(1);
    }
    close(pipefd[1]);
    std::string output;
    char buffer[4096];
    ssize_t n;
    while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
    {
        output.append(buffer, n);
    }
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    return output;
    
}


// bool canExecuteCGI(Httprequest &req, const Location &loc, std::string& errorMsg)
// {
    
// }