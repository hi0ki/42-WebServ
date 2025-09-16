#include "request.hpp"
#include "../core_srv/include/ClientData.hpp" 

bool is_valid_url(const std::string &uri) 
{
    const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_-~/?#[]@!$()'*+,'=%.&";
    const std::vector<char> allowedChars(chars.begin(), chars.end());
  
    for(size_t i = 0; i < uri.size(); i++) 
    {
        if(uri[i] != '\0' && std::find(allowedChars.begin(), allowedChars.end(), uri[i]) == allowedChars.end())
            return false;
    }
    return true;
}

bool is_req_well_formed(Httprequest &req)
{
    if(req.getPath().find("//") != std::string::npos)
    {
        req.setStatus(404, "Not Found");//should remplate it
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
    if (req.getHeaders().find("Transfer-Encoding") == req.getHeaders().end() && req.getHeaders().find("Content-Length:") == req.getHeaders().end() \
       && req.getMethod() == "POST")
    {
        req.setStatus(400, "Bad Request");
        return false;
    } 
    if (is_valid_url(req.getPath()) == false)
    {
        req.setStatus(400, "Bad Request");
        return false;
    }
    if (req.getPath().size() > 2048)
    {
        req.setStatus(414, "Request-URI Too Long");
        return false;
    }
    //if =>Request body larger han client max body size in config file
    return true;
}

Location findMatchingLocation(Httprequest &req, config &config) 
{
   std::vector<Location> locations = config.get_servs()[req.get_index()].get_location();
    static Location dummy; 
    Location best_match;
    size_t longest_match = 0;

    for (size_t i = 0; i < locations.size(); ++i) 
    {
        if (req.getPath().compare(0, locations[i].path.size(), locations[i].path) == 0)
        {
            bool valid_match = false;
            if (locations[i].path == "/" || req.getPath().size() == locations[i].path.size())
                valid_match = true;
            else if (req.getPath().size() > locations[i].path.size() && req.getPath()[locations[i].path.size()] == '/')
                valid_match = true;
            if (valid_match && locations[i].path.size() > longest_match) 
            {
                best_match = locations[i];
                longest_match = locations[i].path.size();
            }
        }
    }
    if (best_match.path.size() && best_match.path == "/" && req.getPath().find('/', 1) != std::string::npos)
    {
        if (req.getError() == false)
            req.setStatus(404, "Not Found");
        return dummy;
    }
    if (best_match.path.size())
        return best_match;
    if (req.getError() == false)
        req.setStatus(404, "Not Found");
    return dummy;
}

// Location& findMatchingLocation(Httprequest &req, config &config)
// {
//     std::vector<Location> locations = config.get_servs()[req.get_index()].get_location();
//     static Location dummy; 
//     Location* best_match = nullptr;
//     size_t longest = 0;

//     for (size_t i = 0; i < locations.size(); i++) 
//     {
//         if (req.getPath().compare(0, locations[i].path.size(), locations[i].path) == 0) 
//         {
//             if (locations[i].path.size() > longest) 
//             {
//                 best_match = &locations[i];
//                 longest = locations[i].path.size();
//             }
//         }
//     }
//     if (best_match)
//         return *best_match;
//     req.setStatus(404, "Not Found");
//     return dummy;
// }

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

//Most websites use a default “homepage” inside directories.
//If the user requests /folder/, they usually want /folder/index.html.

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
            req.setfullPath(fullPath);
            return true;
        } 
    }
    return false; 
}

void resolvePath(config &config, Httprequest &req)
{
    Location loc = findMatchingLocation(req, config);
    if (loc.root != "")
    {
        req.setAbsolutePath(loc.root + req.getPath());
        return ;
    }
    req.setAbsolutePath(config.get_servs()[req.get_index()].get_root() + req.getPath());
}

bool resolve_index(Httprequest &req, config &config)
{
    Location loc = findMatchingLocation(req, config);
    if (loc.index != "")
    {
        if(fileExists(req.getAbsolutePath() + loc.index) == true)
        {
            req.setfullPath(req.getAbsolutePath() + loc.index);
            return true;
        }
        return false;
    }
    if (config.get_servs()[req.get_index()].get_index() != "")
    {
        if(fileExists(req.getAbsolutePath() + config.get_servs()[req.get_index()].get_index()) == true)
        {
            req.setfullPath(req.getAbsolutePath() + config.get_servs()[req.get_index()].get_index());
            return true;
        }
    }
    return false;
}

//Connects to IP 127.0.0.1 on port 8080.
//If your web server is running and listening on 8080, connection succeeds.
bool isAutoindexEnabled(Httprequest &req, config &config, bool found)
{
    if (found == true)
    {
        std::string dir = req.getPath().substr(0, req.getPath().find('/' , 1));
        std::vector<Location> loc = config.get_servs()[req.get_index()].get_location();
        for(int i = 0; i < loc.size(); i++)
        {
            if (loc[i].path.find(dir) != -1)
            {
                if (loc[i].path != "")
                {
                    if(fileExists(req.getAbsolutePath() + "/" + loc[i].index) == true)
                    {
                        req.setfullPath(req.getAbsolutePath() + "/" + loc[i].index);
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
        // std::string full_path = req.getAbsolutePath() + "/" + entry->d_name;
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
    if (loc.path.empty())
        return false;
    return true;
    // loc.me

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

std::string uintToString(unsigned int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string buildHeaders(Httprequest &req, size_t contentLength, bool keep_alive)
{
    std::string s;

    std::string contentType = "";
    std::string filePath = req.getfullPath();
    if (filePath.find(".html") != std::string::npos)
        contentType = "text/html";
    else if (filePath.find(".css") != std::string::npos)
        contentType = "text/css";
    else if (filePath.find(".js") != std::string::npos)
        contentType = "application/javascript";
    else if (filePath.find(".png") != std::string::npos)
        contentType = "image/png";
    else if (filePath.find(".jpg") != std::string::npos || filePath.find(".jpeg") != std::string::npos)
        contentType = "image/jpeg";
    s = "Server: " + req.get_servername() + "\r\n";
    s += "Content-Type: " + contentType + "\r\n";
    s += "Content-Length: " + uintToString(contentLength) + "\r\n";
    s += "Connection: " ;
    if (keep_alive == true)
        s+= "keep-alive\r\n";
    else 
        s+= "close\r\n";
    s += "\r\n"; 
    return s;
}

// std::string Httprequest::buildHttpResponse() 
// {
//     // std::cout << "wlh \n";
//     std::string response;
//     std::string body;
//     std::string statusLine;
//     std::ifstream file(this->getfullPath().c_str(), std::ios::binary);
//     // std::cout << this->getfullPath() << "   here\n";
//    //remember to test with www/
// //    if (this->get_check_autoindex() == true)
// //         body = AutoindexPage(*this);
//     if (!file.is_open()) {
//         statusLine = "HTTP/1.1 404 Not Found\r\n";
//         body = "<html><body><h1>404 Not Found</h1></body></html>";
//     }
//     else{
//         statusLine = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text() + "\r\n";
//         std::ostringstream bodyStream;
//         bodyStream << file.rdbuf();
//         body = bodyStream.str();
//     }
//     // std::cout << "im hereeeee\n";
//     // response = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text();
//     // response += "\r\n";
//     response = statusLine + buildHeaders(*this, body.size())+  body;
//     std::cout << "response :"<< response<< std::endl; // print response
//     return response;
// }

std::string Httprequest::buildHttpResponse(bool keep_alive) 
{
    // std::cout << "wlh mnf mal had lekhra\n";
    std::string response;
    std::string body;
    std::string statusLine;
    std::ifstream file(this->getfullPath().c_str(), std::ios::binary);
    // std::cout << this->getfullPath() << "   here\n"; 
    if (!file.is_open()) {
        statusLine = "HTTP/1.1 404 Not Found\r\n";
        body = "<html><body><h1>404 Not Found</h1></body></html>";
    }
    else{
        statusLine = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text() + "\r\n";
        std::ostringstream bodyStream;
        bodyStream << file.rdbuf();
        body = bodyStream.str();
    }
    // std::cout << "im hereeeee\n";
    // response = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text();
    // response += "\r\n";
    response = statusLine + buildHeaders(*this, body.size(), keep_alive)+  body;
    // std::cout << "response :"<< response<< std::endl; // print response
    return response;
}


bool handelGET(Httprequest &req, config &config)
{
    char c = '\0';
    bool t_f = true;
    if (pathExists(req.getAbsolutePath(), req, c) != true)
    {
        // std::cout << "Path not exist" << std::endl;
        req.setStatus(404, "Not Found");
        return false;
    }
    // std::cout << "ha ana wsalt lehna \n";
    if (c == 'D')
    {
        std::cout << "dkholt l directory\n";
        if (!isUriEndsWithSlash(req.getPath(), req))
            return false;
        // std::cout << "mazal haya\n";
        if (resolve_index(req, config) == false)
            t_f = findIndexFile(req);
        if (t_f == false)
        {
            std::cout << "la dekhlat hna anmoot\n";
            //had lpart mazal naQsa
            if(config.get_servs()[req.get_index()].get_autoindex() == true)
            {
                //  std::cout << "la dekhlat hna anmoot\n";
                //khesni nzid auto index ela kol location
                req.setStatus(200, "OK");
                AutoindexPage(req);
                std::cout << "hahiya hna\n";
                return true;
            }
            else
            {
                std::cout << "wtf\n";
                req.setStatus(403, "Forbidden");
                return false;
            }
        }
    }
    if (req.getError() == false)
        req.setStatus(200, "OK");
    if (c == 'F')
        req.setfullPath(req.getAbsolutePath());
    std::cout << req.getAbsolutePath() << std::endl;
    return true;
}

bool handelPOST(Httprequest &req, Location &loc, config &config)
{
    //khesni nzid   cgi_enabled on; allowed methods
    bool found = false;
    // for(int i = 0; i < loc.limit_except.size(); i++)
    // {
    //     if(loc.limit_except[i] == "POST")
    //         found = !found;
    // }
    if (found == true)
    {
        //If yes → read the body, validate size, store it.
        /*Read the Content-Length header from the request.
        Make sure it does not exceed client_max_body_size.
        Read the body from the socket into a buffer (or temporary file if large).
        Save the body to the appropriate location on the filesystem.*/
        req.setStatus(201, "Created");
        return true;
    }
    // else
    // {
        //possible nzewel had else
        char c = '\0';
        if (pathExists(req.getAbsolutePath(), req, c) != true)
        {
            // std::cout << "Path not exist" << std::endl;
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
                //had lpart mazal naQsa
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
        // if it a file or dirc i should check cgi

    // }
    return true;
}

bool    handleMethod(Httprequest &req, config &config)
{
    std::string meth = req.getMethod();
    bool check;
    if (meth == "GET")
        check = handelGET(req, config);
    else if (meth == "POST")
    {
        // handelPOST(req, findMatchingLocation(req, config), config);
    }
    // else if (meth == "DELETE")
// 
    else
    {
        req.setStatus(405, "Method Not Allowed");
    }
    return check;
}

std::string readLineFromVector(const std::vector<char> &request , int &pos)
{
    std::string line;
    while (pos < request.size())
    {
        if (request[pos] == '\r' && pos + 1 < request.size() && request[pos + 1] == '\n')
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
    // std::cout << "dekhlat\n";
    client.set_reqs_done(false);
    std::string s;
    for(int i = start ; i < client.get_request().size(); i++)
    {
        s = readLineFromVector(client.get_request(), i);
        long chunk_size = strtol(s.c_str(), NULL, 16);
        if (chunk_size == 0) 
        {
            client.set_reqs_done(true);
            break;
        }
        // if (i + chunk_size > client.get_request().size()) 
        //     break;
        for(int j = 0; j < chunk_size; j++)
            body.push_back(client.get_request()[i + j]);
        i += chunk_size - 1;
        if (i + 2 <= client.get_request().size() && client.get_request()[i] == '\r' && client.get_request()[i + 1] == '\n')
            i += 2;
    }
}

bool checkAndApplyErrorPage(config &config, Httprequest &req)
{
    if (!is_req_well_formed(req) || findMatchingLocation(req, config).path.empty())
    {
        for(int i = 0; i < config.get_servs()[req.get_index()].get_errpage().size(); i++)
        {
            if (req.getStatus_code() == config.get_servs()[req.get_index()].get_errpage()[i].err)
            {
                req.setError(true);
                req.setPath(config.get_servs()[req.get_index()].get_errpage()[i].red_page);
            }
        }
        if (req.getError() == false)
            return false;
    }
    return true;
}

bool check_Error_pages(Httprequest &req, config &config)
{
    for(int i = 0; i < config.get_servs()[req.get_index()].get_errpage().size(); i++)
    {
        if (req.getStatus_code() == config.get_servs()[req.get_index()].get_errpage()[i].err)
        {
            req.setError(true);
            req.setPath(config.get_servs()[req.get_index()].get_errpage()[i].red_page);
        }
    }
    if (req.getError() == false)
        return false;
    if (findMatchingLocation(req, config).path.empty())
        return false;
    resolvePath(config, req);
    handleMethod(req, config);
    return true;
}

int Httprequest::request_pars(ClientData &client , config &config)
{
    std::string tmp;
    set_index(client.get_srv_index());
    int a = 0;
    for(int i = 0; i < client.get_request().size(); i++)
        tmp.push_back(client.get_request()[i]);
    if (tmp.find("\r\n\r\n" , 0) != std::string::npos)
        client.set_reqs_done(true);
    a = tmp.find("\r\n\r\n" , 0) + 2;
    std::string r;
    for(int i = 0; i < a; i++)
        r.push_back(tmp[i]);
    method = r.substr(0 , r.find(' ', 0));
    path = r.substr(r.find(' ', 0) + 1, r.find(' ',r.find(' ', 0) + 1) - (r.find(' ', 0) + 1));
    version = r.substr(r.find(' ',r.find(' ', 0) + 1) + 1,(r.find('\r', 0) - 1) - r.find(' ',r.find(' ', 0) + 1));
    for(int i = r.find("\r\n", 0) + 2; i < r.size(); i++)
    {
        headers[r.substr(i, r.find(':', i) - i)] = r.substr(r.find(':', i) + 2, (r.find("\r\n", r.find(':', i) + 1)) - (r.find(':', i) + 2));
        i = r.find("\r\n", r.find(':', i)) + 1;
    }
    // std::cout << "size : "<<path.size() << std::endl;
    // std::cout << "path :" << path << std::endl;
    // for(int i = 0; i < client.get_request().size(); i++)
    // {
    //     std::cout << client.get_request()[i];
    // }
    // std::cout << "bara : "<<client.get_request()[a + 2] << std::endl;
    // if (!headers.empty() && headers["Transfer-Encoding"] == "chunked")
    //     parseChunkedBody(body ,client, a + 2);
    // else
    // {
        for(int i = a + 2; i < client.get_request().size(); i++)
            body.push_back(client.get_request()[i]);
    //     client.set_reqs_done(true);
    // }
    // std::cout << "method : "<< method << std::endl;
    // std::cout << "path : "<< path << std::endl;
    // std::cout << "version : "<< version << std::endl;
    // for(std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
    // {
    //  std::cout << "header : "<< it->first << " : " << it->second<<  std::endl;
    // }
    // for(int i = 0; i < body.size(); i++)
    // {
    //  std::cout << "body : "<< body[i] << std::endl;
    // }

    // std::cout << "path : [" << path << "]"<<  "   methos :"<< method<<std::endl; 


    if (checkAndApplyErrorPage(config, *this) == false)
        return 0;
    //dont miss do something like return
    //want from hafsa method allowed and return 301 /home/index.html and autoindex in every location

    //mazal maQditha 
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

    set_servername(config);

    /*Look at the location’s config.Check if it has a redirection rule (e.g. return 301 ... or redirect ...).
    Return true (or a redirect config object) if yes, otherwise false.*/

    resolvePath(config, *this);
    std::cout << "absolute path : " <<absolutePath << std::endl;
    if (handleMethod(*this, config) == false)
    {
        check_Error_pages(*this, config);
        // std::cout << this->getStatus_code() << std::endl;
    } 
    



    /********************************************/
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
    this->fullPath.clear();
    this->server_name.clear();
    this->status_code = 0;
    this->status_text.clear();
}

/*std::string Httprequest::buildHttpResponse(bool keep_alive) 
{
    std::cout << "wlh mnf mal had lekhra\n";
    std::string response;
    std::string body;
    std::string statusLine;
    std::ifstream file(this->getfullPath().c_str(), std::ios::binary);
    // std::cout << this->getfullPath() << "   here\n"; 
    if (!file.is_open()) {
        statusLine = "HTTP/1.1 404 Not Found\r\n";
        body = "<html><body><h1>404 Not Found</h1></body></html>";
    }
    else{
        statusLine = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text() + "\r\n";
        std::ostringstream bodyStream;
        bodyStream << file.rdbuf();
        body = bodyStream.str();
    }
    // std::cout << "im hereeeee\n";
    // response = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text();
    // response += "\r\n";
    response = statusLine + buildHeaders(*this, body.size(), keep_alive)+  body;
    // std::cout << "response :"<< response<< std::endl; // print response
    return response;
}*/


/*    error_page 404 /errors/404.html;
    error_page 500 502 503 504 /errors/50x.html;*/

    //possible to be eroor page in location