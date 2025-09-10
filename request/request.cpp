#include "request.hpp"

Httprequest::Httprequest()
{
    this->method = "";
    this->path = "";
    this->version = "";
    // this->body = ;
        // std::string query;
    // this->headers = "";
    this->status_code = 0;
    this->status_text = "";
}

void Httprequest::setMethod(const std::string &method)
{
    this->method = method;
}

std::string Httprequest::getMethod() const
{
    return this->method;
}

void Httprequest::setPath(const std::string &Path)
{
    this->path = Path;
}

std::string Httprequest::getPath() const
{
    return this->path;
}

void Httprequest::setVersion(const std::string &Version)
{
    this->version = Version;
}

std::string Httprequest::getVersion() const
{
    return this->version;
}

void Httprequest::setHeaders(const std::map<std::string, std::string> &Headers)
{
    this->headers = Headers;
}
    
std::map<std::string, std::string>& Httprequest::getHeaders() 
{
    return this->headers;
}
        
void Httprequest::setBody(const std::vector<char> &Body)
{
    this->body = Body;
}
        
std::vector<char> Httprequest::getBody() const
{
    return this->body;
}

void Httprequest::setAbsolutePath(const std::string &AbsolutePath)
{
    this->absolutePath = AbsolutePath;
}

std::string Httprequest::getAbsolutePath() const
{
    return this->absolutePath;
}

void    Httprequest::setStatus(int status_code, std::string status_text)
{
    this->status_code = status_code;
    this->status_text = status_text;
}

void Httprequest::setfullPath(const std::string &fullPath)
{
    this->fullPath = fullPath;
}

std::string Httprequest::getfullPath() const
{
    return (this->fullPath);
}

void Httprequest::setError(const bool &Error)
{
    this->error = Error;
}
            
bool Httprequest::getError() const
{
    return error;
}

void Httprequest::setRedirectLocation(const std::string &uri)
{
        redirectLocation = uri;
}

std::string Httprequest::getRedirectLocation() const 
{
        return redirectLocation;
}

bool pathExists(const std::string& path, Httprequest &req, char &c) {
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

bool fileExists(const std::string& path) {
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
        std::string fullPath = req.getAbsolutePath() + "/" + indexFiles[i];
        if (fileExists(fullPath))
        {
            req.setfullPath(fullPath);
            return true;
        } 
    }
    return false; 
}

Location& findMatchingLocation(Httprequest &req,config &config)
{
    std::vector<Location> l = config.get_servs()[0].get_location();
    for (size_t i = 0; i < l.size(); i++) 
    {
        std::string s = req.getPath().substr(0, req.getPath().find('/', 1));
        if (s.compare(0, l[i].path.size(), l[i].path) == 0) 
        {
            if (s.size() == l[i].path.size() || s[l[i].path.size()] == '/') 
                return l[i];
        }
    }
    // req.setStatus(404, "Not Found");
    static Location dummy;
    return dummy;
}

void resolvePath(config &config, Httprequest &req)
{
    std::string dir;

    req.setPath("/api/html");
    Location loc = findMatchingLocation(req, config);
    if (loc.root != "")
    {
        dir = req.getPath().substr(req.getPath().find('/' , 1) + 1);
        req.setAbsolutePath("/Users/felhafid/Desktop/hikii/api/"  + dir);
        // req.setAbsolutePath(loc.root + "/" + dir);
        return ;
    }
    dir = req.getPath().substr(req.getPath().find('/' , 1) + 1);
    req.setAbsolutePath(config.get_servs()[0].get_root() + "/" + dir);
}

bool resolve_index(Httprequest &req, config &config)
{
    Location loc = findMatchingLocation(req, config);
    if (loc.index != "")
    {
        if(fileExists(req.getAbsolutePath() + "/" + loc.index) == true)
        {
            req.setfullPath(req.getAbsolutePath() + "/" + loc.index);
            return true;
        }
        return false;
    }
    if (config.get_servs()[0].get_index() != "")
    {
        if(fileExists(req.getAbsolutePath() + "/" + config.get_servs()[0].get_index()) == true)
        {
            req.setfullPath(req.getAbsolutePath() + "/" + config.get_servs()[0].get_index());
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
        std::vector<Location> loc = config.get_servs()[0].get_location();
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
    // std::cout << html << std::endl;
    return html;
}

bool is_valid_uri(const std::string &uri) 
{
    for (size_t i = 0; i < uri.size(); ++i) 
    {
        char c = uri[i];
        if (c == ' ')
            return false;
        // 2️⃣ Control characters not allowed (ASCII < 32)
        if (c >= 0 && c < 32)
            return false;
        // 3️⃣ Non-ASCII characters not allowed (>= 127)
        if (static_cast<unsigned char>(c) >= 127)
            return false;
        // 4️⃣ Percent-encoding check
        if (c == '%') 
        {
            if (i + 2 >= uri.size())  // not enough chars after %
                return false;
            char h1 = uri[i + 1];
            char h2 = uri[i + 2];
            if (!isxdigit(h1) || !isxdigit(h2))  // must be hex digits
                return false;
            i += 2; // skip the two hex digits
        }
    }
    return true;
}

bool is_req_well_formed(Httprequest &req)
{
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
    if (is_valid_uri(req.getPath()) == false)
        req.setStatus(400, "Bad Request");
    if (req.getPath().size() > 2048)
    {
        req.setStatus(414, "Request-URI Too Long");
        return false;
    }
    //if =>Request body larger han client max body size in config file
    return true;
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

bool handelGET(Httprequest &req, config &config)
{
    char c = '\0';
    bool t_f = true;;
    if (pathExists(req.getAbsolutePath(), req, c) != true)
    {
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
            //had lpart mazal naQsa
            if(config.get_servs()[0].get_autoindex() == true)
            {
                //khesni nzid auto index ela kol location
                AutoindexPage(req);
            }
            else
            {
                req.setStatus(403, "Forbidden");
                return false;
            }
        }
        req.setStatus(200, "OK");
  
    }
    return true;
}

bool    handleMethod(Httprequest &req, config &config)
{
    std::string meth =  req.getMethod();
    if (meth == "GET")
        handelGET(req, config);
    // else if (meth == "POST")
// 
    // else if (meth == "DELETE")
// 
    else
    {
        req.setStatus(405, "Method Not Allowed");
    }
    return true;
}

int Httprequest::request_pars(std::vector<char> &request,config &config)
{
    std::string tmp;
    int a = 0;
    for(int i = 0; i < request.size(); i++)
        tmp.push_back(request[i]);
    a = tmp.find("\r\n\r\n" , 0) + 2;
    std::string r;
    for(int i = 0; i < a; i++)
        r.push_back(tmp[i]);
    for(int i = a + 2; i < request.size(); i++)
        body.push_back(request[i]);  
    method = r.substr(0 , r.find(' ', 0));
    path = r.substr(r.find(' ', 0) + 1, r.find(' ',r.find(' ', 0) + 1) - (r.find(' ', 0) + 1));
    version = r.substr(r.find(' ',r.find(' ', 0) + 1) + 1,(r.find('\r', 0) - 1) - r.find(' ',r.find(' ', 0) + 1));
    for(int i = r.find("\r\n", 0) + 2; i < r.size(); i++)
    {
        headers[r.substr(i, r.find(':', i) - i)] = r.substr(r.find(':', i) + 2, (r.find("\r\n", r.find(':', i) + 1)) - (r.find(':', i) + 2));
        i = r.find("\r\n", r.find(':', i)) + 1;
    }
    setPath("/api/html");
    error = is_req_well_formed(*this);
    if (error == false || findMatchingLocation(*this, config).path.empty())
    {
        std::cout << "Error found\n";
        return 0;//dont miss do something like return
    }

    //want from hafsa method allowed and return 301 /home/index.html and autoindex in every location

    /*Look at the location’s config.Check if it has a redirection rule (e.g. return 301 ... or redirect ...).
    Return true (or a redirect config object) if yes, otherwise false.*/


    resolvePath(config, *this);
    handleMethod(*this, config);
    //check if path and root correct or not check if u should handel this GET /../../etc/passwd HTTP/1.1
    // if (config.get_servs()[0].get_root().back() == '/' && !path.empty() && path.front() == '/')
    //     absolutePath = config.get_servs()[0].get_root() + path.substr(1); // remove one slash
    // else
    //     absolutePath = config.get_servs()[0].get_root() + this->path;//add root from hafssa
    /********************************************/
    // char c = '\0';
    // if (pathExists(absolutePath, *this, c) == false)
    // {
    //     setStatus(404, "Not found");
    //     error = true;
    // }
    // std::vector<Location> loc = config.get_servs()[0].get_location();
    // bool t_f = true;
    // if (c == 'D' && error == false)
    // {
    //     if (resolve_index(*this, loc, found_loc, config) == false)
    //         t_f = findIndexFile(*this);
    //     if (t_f == false)
    //     {
    //         if(config.get_servs()[0].get_autoindex() == true)
    //         {
    //             AutoindexPage(*this);
    //         }
    //         else
    //         {
    //             setStatus(403, "Forbidden");
    //             error = true;
    //         }
    //     }
    // }
    return 0;
}


