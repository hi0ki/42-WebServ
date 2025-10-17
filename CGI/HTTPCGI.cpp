/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPCGI.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hanebaro <hanebaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:00:54 by hanebaro          #+#    #+#             */
/*   Updated: 2025/10/17 12:58:38 by hanebaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "HTTPCGI.hpp"

/// dont forget delete (free the memory)
HTTPCGI::HTTPCGI(Httprequest &req)
{
    cgi_env(req);
}

HTTPCGI &HTTPCGI::operator=(const HTTPCGI &obj)
{
    this->envr = obj.envr;
    return (*this);
}

void HTTPCGI::cgi_env(Httprequest &req)
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
    for (size_t i = 0; i < env.size(); i++)
    {
        envr.push_back(strdup(env[i].c_str())); // strdup car execve attend des pointeurs valides
    }
    envr.push_back(NULL); // fin du tableau
}

int HTTPCGI::can_execute(config &conf, int index, Httprequest &req)
{
    const std::vector<Location>& locations = conf.get_servs()[index].get_location();

    for(size_t i = 0; i < locations.size(); i++)
    {
        if(locations[i].type == CGI)
        {
            std::cout << RED << "CGI Location found!" << RESET << std::endl;
            
            // 1. Check if CGI is enabled
            if(locations[i].cgi_enabled == false)
            {
                std::cout << RED << "CGI not enabled" << RESET << std::endl;
                req.setStatus(403, "Forbidden");
                return (1);
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
                    req.setStatus(405, "Method Not Allowed");
                    return (1);
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
                req.setStatus(403, "Forbidden");
                return (1);
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
                    req.setStatus(403, "Forbidden");
                    return (1);
                }
                else
                {
                    std::cout << GREEN << "Extension " << ext << " is allowed" << RESET << std::endl;
                }
            }

            // 4. Check interpreters (cgi_handler or cgi_path)
            if (locations[i].cgi_path.empty())
            {
                std::cout << RED << "No CGI interpreter path configured" << RESET << std::endl;
                req.setStatus(500, "Internal Server Error");
                return (1);
            }

            for (size_t j = 0; j < locations[i].cgi_path.size(); ++j)
            {
                const std::string &path = locations[i].cgi_path[j];
                std::cout << "Checking interpreter: " << path << std::endl;

                struct stat st;
                if (stat(path.c_str(), &st) != 0)
                {
                    std::cout << RED << "Interpreter does not exist: " << path << RESET << std::endl;
                    req.setStatus(500, "Internal Server Error");
                    return (1);
                }

                if (!S_ISREG(st.st_mode))
                {
                    std::cout << RED << "Interpreter is not a regular file: " << path << RESET << std::endl;
                    req.setStatus(500, "Internal Server Error");
                    return (1);
                }

                if (access(path.c_str(), X_OK) != 0)
                {
                    std::cout << RED << "Interpreter is not executable: " << path << RESET << std::endl;
                    req.setStatus(500, "Internal Server Error");
                    return (1);
                }
            }

            std::cout << GREEN << "All CGI interpreters are valid and executable." << RESET << std::endl;

            
            
            // 5. Check if script file exists and is readable
            if (access(req.getAbsolutePath().c_str(), F_OK) != 0)
            {
                std::cout << RED << "CGI script not found: " << req.getAbsolutePath() << RESET << std::endl;
                req.setStatus(404, "Not Found");
                return (1);
            }
            
            if (access(req.getAbsolutePath().c_str(), R_OK) != 0)
            {
                std::cout << RED << "CGI script not readable: " << req.getAbsolutePath() << RESET << std::endl;
                req.setStatus(403, "Forbidden");
                return (1);
            }
            
            std::cout << GREEN << "All CGI checks passed!" << RESET << std::endl;
            return (0);  // ✅ Success!
        }
    }
    // Si on arrive ici, aucune location CGI trouvée
    req.setStatus(404, "Not Found");
    return (1);

    // return(1);
}

std::string clean_string(const std::string& str)
{
    std::string result = str;
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}

std::string HTTPCGI::execute(const std::string &script_path, std::map<std::string, std::string> post_data, Httprequest &req)
{
    std::string post_file = "/tmp/cgi_post_data.txt";
    std::string post_content;

    for (std::map<std::string, std::string>::iterator it = post_data.begin(); it != post_data.end(); ++it)
    {
        if (!post_content.empty())
            post_content += "&";
        
        std::string clean_key = clean_string(it->first);
        std::string clean_value = clean_string(it->second);
        
        post_content += clean_key + "=" + clean_value;
    }
    std::cout << "allll :::: " << post_content << std::endl; 
    
    // Step 2: Save to a file
    std::ofstream out(post_file.c_str());
    if (out.is_open())
    {
        out << post_content;
        out.close();
    }

    // Step 3: Create a pipe for CGI output
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        req.setStatus(500, "Internal Server Error");
        return ("");
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        req.setStatus(500, "Internal Server Error");
        return ("");
    }
        
    if (pid == 0)
    {
        // --- CHILD PROCESS ---

        // Redirect CGI stdout → pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        // --- Handle POST data ---
        if (!post_data.empty())
        {
            int input_pipe[2];
            pipe(input_pipe);
            pid_t writer = fork();
            if (writer == 0)
            {
                close(input_pipe[0]);
                int fd = open(post_file.c_str(), O_RDONLY);
                if (fd != -1)
                {
                    char buffer[4096];
                    ssize_t bytes;
                    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0)
                        write(input_pipe[1], buffer, bytes);
                    close(fd);
                }
                close(input_pipe[1]);
                _exit(0);
            }
            close(input_pipe[1]);
            dup2(input_pipe[0], STDIN_FILENO);
            close(input_pipe[0]);
        }

        // --- Prepare argv for execve ---
        char *argv[] = {
            strdup(script_path.c_str()), // script path
            NULL
        };

        // --- Execute CGI script ---
        execve(argv[0], argv, envr.data());
        perror("execve"); // if execve fails
        _exit(1);
    }
    // --- PARENT PROCESS ---
    close(pipefd[1]);
    std::string output;
    char buffer[4096];
    ssize_t n;
    
    // --- Timeout wait ---
    time_t start_time = time(NULL);
    int status;
    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
    while (true)
    {
        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0)
            output.append(buffer, n);
        pid_t result = waitpid(pid, &status, WNOHANG);
        if (result == -1)
            break;
        else if (result > 0)
            break;
        std::cout << "timee = " << std::difftime(time(NULL), start_time) << std::endl;
        if (std::difftime(time(NULL), start_time) > 3)
        {
            std::cout << "tiiiiimeouuuut" << std::endl;
            kill(pid, SIGKILL);
            waitpid(pid, &status, 0);
            req.setStatus(504, "Gateway Timeout");
            close(pipefd[0]);
            return ("");
        }
        // usleep(100000);
    }
    close(pipefd[0]);
    return (output);
}

void HTTPCGI::reset_cgi_obj()
{
    this->envr.clear();
}

