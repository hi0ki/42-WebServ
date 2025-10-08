#include "Request.hpp"
#include "../core_srv/include/ClientData.hpp" 
#include "../CGI/HTTPCGI.hpp"

// bool is_valid_url(const std::string &uri) 
// {
//     (void) uri;
//     const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_-~/?#[]@!$()'*+,'=%.&";
//     const std::vector<char> allowedChars(chars.begin(), chars.end());
//     // if (uri.find("%20"))
//     // {
//     //     return false;
//     // }
//     // for(size_t i = 0; i < uri.size(); i++) 
//     // {
//     //     if(uri[i] != '\0' && std::find(allowedChars.begin(), allowedChars.end(), uri[i]) == allowedChars.end())
//     //         return false;
//     // }
//         std::cout << "yeeeees\n";

//     return true;
// }

bool isValidURIChar(char c) {

    std::string unreserved = "-._~";
    if (isalnum(c) || unreserved.find(c) != std::string::npos || c == '%')
        return true;
    std::string reserved = ":/?#[]@!$&'()*+,;=";
    if (reserved.find(c) != std::string::npos)
        return true;
    return false;
}

bool is_valid_url(const std::string &uri) 
{
   for (size_t i = 0; i < uri.size(); ++i) 
   {
        if (!isValidURIChar(uri[i]))
            return false;
        if (uri[i] == '%') 
        {
            if (i + 2 >= uri.size() || !isxdigit(uri[i+1]) || !isxdigit(uri[i+2]))
                return false;
            i += 2;
        }
    }
    return true;
}

bool checkBodySize(Httprequest &req)
{
    if (req.getBody().size() > 5242880)
    {
        req.setStatus(413, "Request Entity Too Large");
        return false;
    }
    return true;
}

bool is_req_well_formed(Httprequest &req)
{
    if(req.getPath().find("//") != std::string::npos)
    {
        req.setStatus(404, "Not Found");
        return false;
    }
    std::map<std::string, std::string>::iterator it = req.getHeaders().find("Transfer-Encoding");
    if (it != req.getHeaders().end())
    {
        if(it->second != "chunked")
        {
            req.setStatus(501, "Not Implemented");
            return false;
        }
    }
    if ((req.getHeaders().find("Transfer-Encoding") == req.getHeaders().end() && req.getHeaders().find("Content-Length") == req.getHeaders().end() \
       && req.getMethod() == "POST") || !is_valid_url(req.getPath()) || req.getHeaders().find("Host") == req.getHeaders().end())
    {
        req.setStatus(400, "Bad Request");
        return false;
    }
    // if (req.getHeaders().find("Content-Length") != req.getHeaders().end() && req.getHeaders()["Content-Length"] != uintToString(req.getBody().size()))
    // {
    //     std::cout << uintToString(req.getBody().size()) << std::endl;
    //     std::cout << "2222222\n";
    //     req.setStatus(400, "Bad Request");
    //     return false;
    // }
    if (req.getPath().size() > 2048)
    {
        req.setStatus(414, "Request-URI Too Long");
        return false;
    }
    if (!checkBodySize(req))
        return false;
    std::cout << "dazet shiha mn errors\n";
    //if =>Request body larger han client max body size in config file
    return true;
}

Location findMatchingLocation(Httprequest &req, config &config) 
{
    std::string s = "/www" + req.getPath();
    std::vector<Location> locations = config.get_servs()[req.get_index()].get_location();
    static Location dummy; 
    Location best_match;
    size_t longest_match = 0;

    for (size_t i = 0; i < locations.size(); ++i) 
    {
        if (s.compare(0, locations[i].path.size(), locations[i].path) == 0)
        {
            bool valid_match = false;
            if (locations[i].path == "/" || s.size() == locations[i].path.size())
                valid_match = true;
            else if (s.size() > locations[i].path.size() && s[locations[i].path.size()] == '/')
                valid_match = true;
            if (valid_match && locations[i].path.size() > longest_match) 
            {
                best_match = locations[i];
                longest_match = locations[i].path.size();
            }
        }
    }
    if (best_match.path.size() && best_match.path == "/" && s.find('/', 1) != std::string::npos && !req.getError())
    {
        req.setStatus(404, "Not Found");
        return dummy;
    }
    if (best_match.path.size())
        return best_match;
    if (req.getError() == false && req.getError_page_found() == false)
        req.setStatus(404, "Not Found");   ///
    return dummy;
}

bool pathExists(const std::string& path, Httprequest &req, char &c) 
{
    struct stat info;
    if (stat(path.c_str(), &info) == 0) 
    {
        if (S_ISDIR(info.st_mode)) 
            c = 'D';
        else
            c = 'F';
        return true;
    } 
    else {
        req.setStatus(404, "Not Found");
        return false;
    }
}

bool fileExists(const std::string& path) 
{
    struct stat info;
    if (stat(path.c_str(), &info) == 0) 
        return true;
    return false;
}

bool findIndexFile(Httprequest &req) 
{
    std::vector<std::string> indexFiles;

    indexFiles.push_back("index.html");
    indexFiles.push_back("index.htm");
    indexFiles.push_back("index.php");
    for (size_t i = 0; i < indexFiles.size(); ++i) 
    {
        std::string fullPath = req.getAbsolutePath() + indexFiles[i];
        if (fileExists(fullPath))
        {
            req.setAbsolutePath(fullPath);

            req.setStatus(200, "OK");
            return true;
        } 
    }
    return false; 
}

void resolvePath(config &config, Httprequest &req)
{
    std::cout << "yahlili\n";
    Location loc = findMatchingLocation(req, config);
    if (loc.root != "")
    {
        req.setAbsolutePath(loc.root + "www" + req.getPath());
        return ;
    }
    req.setAbsolutePath(config.get_servs()[req.get_index()].get_root() + "www" + req.getPath());
}

bool resolve_index(Httprequest &req, config &config)
{
    Location loc = findMatchingLocation(req, config);
    if (loc.index != "")
    {
        if(fileExists(req.getAbsolutePath() + loc.index) == true)
        {
            req.setAbsolutePath(req.getAbsolutePath() + loc.index);
            req.setStatus(200, "OK");
            return true;
        }
        return false;
    }
    if (config.get_servs()[req.get_index()].get_index() != "")
    {
        if(fileExists(req.getAbsolutePath() + config.get_servs()[req.get_index()].get_index()) == true)
        {
            req.setAbsolutePath(req.getAbsolutePath() + config.get_servs()[req.get_index()].get_index());
            req.setStatus(200, "OK");
            return true;
        }
    }
    return false;
}

bool isAutoindexEnabled(Httprequest &req, config &config, bool found)
{
    if (found == true)
    {
        std::string dir = req.getPath().substr(0, req.getPath().find('/' , 1));
        std::vector<Location> loc = config.get_servs()[req.get_index()].get_location();
        for(unsigned int i = 0; i < loc.size(); i++)
        {
            if (loc[i].path.find(dir) != std::string::npos)
            {
                if (loc[i].path != "")
                {
                    if(fileExists(req.getAbsolutePath() + "/" + loc[i].index) == true)
                    {
                        req.setAbsolutePath(req.getAbsolutePath() + "/" + loc[i].index);

                        return true;
                    }
                    return false;
                }
                // break ;
                return false;
            }
        }
    }
    return true;
}

std::string AutoindexPage(Httprequest &req)
{
    DIR *dir;
    struct dirent *entry;
    std::string html;

    html = "<html>\n<head><title>Index of " + req.getPath() + "</title></head>\n<body>\n";
    html += "<h1>Index of " + req.getPath() + "</h1>\n<ul>\n";
    dir = opendir(req.getAbsolutePath().c_str());
    if (dir == NULL)
        return html += "<li>Cannot open directory</li>\n</ul>\n</body>\n</html>\n";
    while ((entry = readdir(dir)) != NULL) 
    {
        if ((std::string)entry->d_name == "." || (std::string)entry->d_name == "..")
            continue;
         struct stat st;
        // if (stat(full_path.c_str(), &st) != 0)
        //     continue; 
        std::string href = req.getPath();
        if (href.empty() || href[href.size() - 1] != '/')
            href += '/';
        href += entry->d_name;
        if (S_ISDIR(st.st_mode))
            href += '/';
        std::string visible = entry->d_name;
        if (S_ISDIR(st.st_mode))
            visible += '/';
        html += "<li><a href=\"" + href +  "\">" + visible + "</a></li>\n";
    }
    closedir(dir);
    html += "</ul>\n</body>\n</body>\n";
    req.set_check_autoindex(true);
    return html;
}

bool isMethodAllowed(Httprequest &req, config &config)
{
    Location loc = findMatchingLocation(req, config);
    std::cout << req.getPath()<< std::endl;
    std::cout << "5555555555\n";
    std::string meth = req.getMethod();
    if (loc.methods.empty() && config.get_servs()[req.get_index()].get_methods().empty())
        return true;
    for(unsigned int i = 0; i < loc.methods.size(); i++)
    {
        if (loc.methods[i] == meth)
            return true;
    }
    if (loc.methods.empty())
    {
        std::vector<std::string> s = config.get_servs()[req.get_index()].get_methods();
        if (!s.empty())
        {
            for (unsigned int i = 0; i < s.size(); i++)
            {
                if (s[i] == meth)
                    return true;
            }
        }
    }
    if (!req.getError_page_found())
        req.setStatus(405, "Method Not Allowed");
    return false;
}

bool isUriEndsWithSlash(std::string s, Httprequest &req)
{
    if (!s.empty() && s[s.size() - 1] != '/')
    {
        req.setStatus(301, "Moved Permanently");
        req.setRedirectLocation(s + '/');
        return false;
    }
    return true;
}

bool location_has_cgi(Httprequest &req, config &config)
{
    Location loc = findMatchingLocation(req, config);
    std::cout << "oneeeee \n";
    HTTPCGI cgi(req, loc);
    if (cgi.can_execute(config, req.get_index(), req))
        return false ;
    std::cout << "twoooo \n";
    
    std::string response = cgi.execute(req.getAbsolutePath() , "");
    if (response != "")
        req.setBody_cgi(response);
    std::cout <<  "new responce  : " << RED << response << std::endl;
    return true;
    
}

bool handelGET(Httprequest &req, config &config)
{
    char c = '\0';
    bool t_f = true;
    if (!isMethodAllowed(req, config))
        return false;
    if (pathExists(req.getAbsolutePath(), req, c) != true)
    {
        req.setError(true);
        req.setStatus(404, "Not Found");
        return false;
    }
    if (c == 'D')
    {
        if (!isUriEndsWithSlash(req.getPath(), req))
            return false;
        if (resolve_index(req, config) == false)
            t_f = findIndexFile(req);
        if (t_f == false)
        {
            if(config.get_servs()[req.get_index()].get_autoindex() == true)
            {
                //khesni nzid auto index ela kol location
                req.setStatus(200, "OK");
                req.set_check_autoindex(true);
                return true;
            }
            else
            {
                req.setStatus(403, "Forbidden");
                return false;
            }
        }
    }
    if (req.getError_page_found() == false && location_has_cgi(req, config))
        std::cout << "its true\n";
    if (req.getError_page_found() == false)
        req.setStatus(200, "OK");
    return true;
}

bool handelPOST(Httprequest &req, config &config)
{
    char c = '\0';
    if ((pathExists(req.getAbsolutePath(), req, c) && c == 'F' ) || !pathExists(req.getAbsolutePath(), req, c))
    {
        req.setError(true);///
        req.setStatus(404, "Not Found");
        return false;
    }
    if (isMethodAllowed(req, config))
    {
        req.setStatus(201, "Created");
        return true;
    }
    if (pathExists(req.getAbsolutePath(), req, c) != true)
    {
        req.setError(true);///
        req.setStatus(404, "Not Found");
        return false;
    }
    if (c == 'D')
    {  
        bool t_f = true;
        if (!isUriEndsWithSlash(req.getPath(), req))
            return false;
        if (resolve_index(req, config) == false)
            t_f = findIndexFile(req);
        if (t_f == false)
        {
            if(config.get_servs()[req.get_index()].get_autoindex() == true)
            {
                //khesni nzid auto index ela kol location
                req.setStatus(200, "OK");
                AutoindexPage(req);
                return true;
            }
            else
            {
                req.setStatus(403, "Forbidden");
                return false;
            }
        }
    }
    if (req.getError_page_found() == false && location_has_cgi(req, config))
    {
        return true;
        std::cout << "its true\n";
    }
    //run cgi on requested file with POST REQUEST_METHOD Returtn Code Depend on cgi
    req.setStatus(403, "Forbidden");
    return false;
}

bool handelDELETE(Httprequest &req, config &config)
{
    char c = '\0';
    // bool t_f = true;
    if (!isMethodAllowed(req, config))
        return false;
    if (pathExists(req.getAbsolutePath(), req, c) != true)
    {
        req.setError(true);
        req.setStatus(404, "Not Found");
        return false;
    }
    if (c == 'D')
    {
        if (!isUriEndsWithSlash(req.getPath(), req))
        {
            req.setStatus(409, "Conflict");
            return false;
        }
        if (location_has_cgi(req, config))///khesni nebQa hna ghir nchiof cgi wach kayen ola la maci nranih
        {
            if (findIndexFile(req) == false)
            {
                req.setStatus(403, "Forbidden");
                return false;
            }
            location_has_cgi(req, config);//hna yetcola cgi 
            return true;//
        }
        else
        {
            if (remove(req.getAbsolutePath().c_str()) == 0)
            {
                req.setStatus(204, "No Content");
                return true;
            }
            else
            {
                if (access(req.getAbsolutePath().c_str(), W_OK) != 0)
                    req.setStatus(403, "Forbidden");
                else
                    req.setStatus(500, "Internal Server Error");
                return false;
            }
        }
  
    }
    //if ()location has cgi
    //true run it
    //false remove(req.getAbsolutePath().c_str()) req.setStatus(204, "No Content");

    return true;
}

bool    handleMethod(Httprequest &req, config &config)
{
    std::string meth = req.getMethod();
    bool check = false;
    if (meth == "GET")
        check = handelGET(req, config);
    else if (meth == "POST")
    {
        check = handelPOST(req, config);
    }
    else if (meth == "DELETE")
        check = handelDELETE(req, config);
    return check;
}

std::string readLineFromVector(const std::vector<char> &request , int &pos)
{
    std::string line;
    while ((unsigned int)pos < request.size())
    {
        if (request[pos] == '\r' && (size_t)(pos + 1) < request.size() && request[pos + 1] == '\n')
        {
            pos += 2;
            break;
        }
        line += request[pos];
        pos++;
    }
    return line;
}

void parseChunkedBody(std::vector<char>& body, ClientData &client, int start)
{
    client.set_reqs_done(false);
    std::string s;
    for(int i = start ; (unsigned int)i < client.get_request().size(); i++)
    {
        s = readLineFromVector(client.get_request(), i);
        long chunk_size = strtol(s.c_str(), NULL, 16);
        if (chunk_size == 0) 
        {
            client.set_reqs_done(true);
            break;
        }
        for(int j = 0; j < chunk_size; j++)
            body.push_back(client.get_request()[i + j]);
        i += chunk_size - 1;
        if ((unsigned int)(i + 2) <= client.get_request().size() && client.get_request()[i] == '\r' && client.get_request()[i + 1] == '\n')
            i += 2;
    }
}

bool is_location_have_redirect(Httprequest &req, config &config)
{
    //Ensure the status is valid (commonly 301, 302, 303, 307, 308).
    std::cout << "ana hna\n";
    Location loc = findMatchingLocation(req, config);
    std::cout << req.getPath() <<  "  " << loc.path << std::endl; 
    std::cout << loc.type <<std::endl;
    if (loc.type == REDIRECT)
    {
        req.setStatus(301, "Moved Permanently");
        req.setRedirectLocation(loc.return_r.red_url);
        std::cout << "*****   " << req.getRedirectLocation()<< std::endl;
        return true;
    }
    return false;
}

bool isValidMethod(Httprequest &req)
{
    std::string method = req.getMethod();
    if (method == "POST" || method == "DELETE" || method == "GET")
        return true;
    req.setStatus(405, "Method Not Allowed");
    return false;
}

bool checkAndApplyErrorPage(config &config, Httprequest &req, ClientData &client)
{
    if (!is_req_well_formed(req) || findMatchingLocation(req, config).path.empty() || !isValidMethod(req))
    {
        std::cout << RED <<"found\n" << RED << std::endl;
        client.set_reqs_done(true);//
        if (req.getMethod() == "POST")
        {
            req.setMethod("GET");
            req.setForceGetOnError(true);
        }
        std::cout << req.getStatus_code() << "\n";
        for(size_t i = 0; i < config.get_servs()[req.get_index()].get_errpage().size(); i++)
        {
            if (req.getStatus_code() == (unsigned int)config.get_servs()[req.get_index()].get_errpage()[i].err)
            {
                std::cout << "hasab tawaQo3i dekhlat hna\n";
                req.setError_page_found(true);
                std::cout << req.getStatus_code() << "\n";
                req.setPath(config.get_servs()[req.get_index()].get_errpage()[i].red_page);
                resolvePath(config, req);
                std::cout <<  " 1 "<< req.getAbsolutePath() << std::endl;
                if (fileExists(req.getAbsolutePath()) == false)
                    return false;
                return true;
            }
        }
        if (req.getError_page_found() == false)
            return false;
    }
    return true;
}

bool check_Error_pages(Httprequest &req, config &config)
{
    for(size_t i = 0; i < config.get_servs()[req.get_index()].get_errpage().size(); i++)
    {
        if (req.getStatus_code() == (unsigned int)config.get_servs()[req.get_index()].get_errpage()[i].err)
        {
            req.setError_page_found(true);
            req.setPath(config.get_servs()[req.get_index()].get_errpage()[i].red_page);
            resolvePath(config, req);
            if (!fileExists(req.getAbsolutePath()) || !isMethodAllowed(req, config))
                req.setError_page_found(false);
            std::cout << "ach l path jdid :" << req.getPath() << std::endl;
        }
    }
    if (req.getError_page_found() == false)
    {
        req.setAbsolutePath(getPWDwithWWW() + "/defaults_errors/" +  uintToString(req.getStatus_code()) + ".html");
        std::cout << req.getStatus_code() << std::endl;
        return true;
    }
    if (findMatchingLocation(req, config).path.empty())
        return false;
    resolvePath(config, req);
    std::cout <<"absolute path:" <<req.getAbsolutePath() << std::endl;
    handleMethod(req, config);
    return true;
}

std::vector<char> removeExtraSpaces(const std::vector<char> &input) 
{
    std::vector<char> result;
    bool inSpace = false;

    for (size_t i = 0; i < input.size(); i++) 
    {
        if (i + 3 < input.size() && input[i] == '\r' && input[i + 1] == '\n' &&
            input[i + 2] == '\r' && input[i + 3] == '\n') 
        {
            for (size_t j = i; j < input.size(); j++)
                result.push_back(input[j]);
            break ;
        }
        if (input[i] == ' ') 
        {
            if (!inSpace) 
            {
                result.push_back(' ');
                inSpace = true;
            }
        } 
        else
        {
            result.push_back(input[i]);
            inSpace = false;
        }
    }
    return result;
}

int Httprequest::request_pars(ClientData &client , config &config)
{
    std::string tmp;
    set_servername(config);
    set_index(client.get_srv_index());
    int a = 0;
    client.set_request(removeExtraSpaces(client.get_request()));
    for(int i = 0; i < client.get_request().size(); i++)
        tmp.push_back(client.get_request()[i]);
    std::cout << tmp;
    if (client.get_request()[0] != 'P')
    {
        if (tmp.find("\r\n\r\n" , 0) != std::string::npos)
            client.set_reqs_done(true);
        else 
            return 0;
    }
    a = tmp.find("\r\n\r\n" , 0) + 2;
    std::string r;
    for(int i = 0; i < a; i++)
        r.push_back(tmp[i]);
    method = r.substr(0 , r.find(' ', 0));
    size_t path_start = r.find(' ', 0) + 1;
    size_t path_end = r.find(' ', path_start);
    std::string tar = r.substr(path_start, path_end - path_start);
    if (tar.find('?') != std::string::npos)
    {
        path = tar.substr(0, tar.find('?'));
        QUERY_STRING = tar.substr(tar.find('?') + 1);
    }
    else
        path = tar;
    version = r.substr(path_end + 1, r.find('\r', 0) - (path_end + 1));
    for(unsigned int i = r.find("\r\n", 0) + 2; i < r.size(); i++)
    {
        headers[r.substr(i, r.find(':', i) - i)] = r.substr(r.find(':', i) + 2, (r.find("\r\n", r.find(':', i) + 1)) - (r.find(':', i) + 2));
        i = r.find("\r\n", r.find(':', i)) + 1;
    }
    if (method == "POST" && headers.find("Content-Length") != headers.end())
        client.set_length(atoi(headers["Content-Length"].c_str()));
    for(unsigned int i = a + 2; i < client.get_request().size(); i++)
            body.push_back(client.get_request()[i]);
    if (method == "POST")
    {
        if (headers.find("Content-Type") != headers.end())
        {
            size_t found = headers["Content-Type"].find("----");
            if (found != std::string::npos)
            {
                client.get_body_struct().key = headers["Content-Type"].substr(found + 4, headers["Content-Type"].size() - found + 4);
                std::cout << "keeeeey ===== \'" << client.get_body_struct().key << "\'" << std::endl;
            }
        }
    }
    std::cout << "111111111\n";
    if (checkAndApplyErrorPage(config, *this, client) == false)
    {
        setAbsolutePath(getPWDwithWWW() + "/defaults_errors/" + uintToString(this->getStatus_code()) + ".html");  
        return 0;
    }
    if (headers.find("Connection") != headers.end())
    {
        std::string val = headers["Connection"];
        for (size_t i = 0; i < val.size(); i++) 
            val[i] = tolower(val[i]);
        if (val == "close")
            client.set_keep_alive(false);
        else if (val == "keep-alive")
            client.set_keep_alive(true);
    }
    std::cout << "2222222222\n";
    resolvePath(config, *this);
    if (is_location_have_redirect(*this, config))
        return 0;
    std::cout << "333333333\n";
    std::cout << "absolute path  2 : " << this->getAbsolutePath() << std::endl;
    if (handleMethod(*this, config) == false)
    {
        std::cout << "ha ana hna \n";
        std::cout << this->getStatus_code() <<std::endl;
        check_Error_pages(*this, config);
    }
    return 0;
}




// Multiple error codes to the same page
// server {
//     listen 8080;
//     root /var/www/html;

//     error_page 403 404 500 /error.html;
// }
//<blank line>


void Httprequest::ft_clean()
{
    this->method.clear();
    this->path.clear();
    this->version.clear();
    this->body.clear();
    this->headers.clear();
    this->absolutePath.clear();
    this->server_name.clear();
    this->status_code = 0;
    this->status_text.clear();
    this->QUERY_STRING.clear();
    this->check_autoindex = false;
    this->Error_page_found = false;
    this->body_cgi = "";
    this->redirectLocation = "";

}
  
