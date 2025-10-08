/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPCGI.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: felhafid <felhafid@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:00:54 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/07 14:03:25 by felhafid         ###   ########.fr       */
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
    
    for(std::vector<Location>::iterator it = conf.get_servs()[index].get_location().begin(); it != conf.get_servs()[index].get_location().end(); it++)
    {
        if(it->type == CGI)
        {
            std::cout << RED << "waaaaaa dkhllllllll \n" << std::endl;
            
            if(it->cgi_enabled == false)
                return(403);
            //// chech if method vide if yes check global methods
            std::vector<std::string>::iterator exist;
                // exit(1);
            std::cout << "heeeeeeeereeeeeee if     " << it->cgi_path << std::endl;
            if(it->methods.size() != 0)
            {
                
                exist = std::find(it->methods.begin(), it->methods.end(), req.getMethod());
            }
            else if (conf.get_servs()[index].get_methods().size())
            {
                std::cout << "heeeeeeeereeeeeee else     " << std::endl;
                exist = std::find(conf.get_servs()[index].get_methods().begin(), it->methods.end(), req.getMethod());
                std::cout << "heeeeeeeereeeeeee else     " << std::endl;
            
            }
            if(exist == it->methods.end())
            {
                std::cout << "heeeeeeeereeeeeee else     " << std::endl;
                return(405);
            }
            //cgi_extension
            std::string ext;
            for(int i = req.getAbsolutePath().size() - 1; i >= 0; --i)
            {
                if(req.getAbsolutePath()[i] == '.')
                    ext = req.getAbsolutePath().substr(i);
            }
            if(ext.empty())
                return(403);
            else
            {
                exist = std::find(it->cgi_extension.begin(), it->cgi_extension.end(), ext);
                if(exist == it->cgi_extension.end())
                return(403);
            }
            //cgi_handler or cgi_path
            struct stat st;
            if (stat(it->cgi_path.c_str(), &st) != 0) {
                // std::cerr << "Error: " << it->cgi_path << " does not exist.\n";
                return (500);
            }
            if (!S_ISREG(st.st_mode)) {
                // std::cerr << "Error: " << it->cgi_path << " is not a regular file.\n";
                return (500);
            }
            if (access(it->cgi_path.c_str(), X_OK | F_OK) != 0) {
                // std::cerr << "Error: " << it->cgi_path << " is not executable.\n";
                return (500);
            }
            /// a verifier
            return(0);
        }
    }
    return(1);
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