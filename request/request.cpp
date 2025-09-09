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

// void check_methods(Httprequest &req)
// {
    // if (req.method == "GET")
    //     std::cout << "GET\n";
    // else if(req.method == "POST")
    //     std::cout << "POST\n";
    // else if (req.method == "DELETE")
    //     std::cout << "DELETE\n";
    // else
    // {
    //     req.status_code = 400;
    //     req.status_text = "Bad Request";
    // }
// }

// void check_version(Httprequest &req)
// {
    // if (req.version[req.version.size() - 1] != '1' && req.version[req.version.size() - 2] == '1')
    // {
    //     req.status_code = 505;
    //     req.status_text = "HTTP Version Not Supported";
    // }
    // std::map<std::string, std::string>::const_iterator it = req.headers.find("Host");
    // if (it == req.headers.end() || it->second.empty())
    // {
    //     req.status_code = 400;
    //     req.status_text = "Bad Request";
    // }

// }

bool pathExists(const std::string& path, Httprequest &req, char &c) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0) {
        // req.setStatus(200, "OK");
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

std::string findIndexFile(const std::string& absolutePath) {
    std::vector<std::string> indexFiles;
    indexFiles.push_back("index.html");
    indexFiles.push_back("index.htm");
    indexFiles.push_back("index.php");

    for (size_t i = 0; i < indexFiles.size(); ++i) 
    {
        std::string fullPath = absolutePath + "/" + indexFiles[i];
        if (fileExists(fullPath)) 
            return fullPath;
    }
    return ""; 
}

bool resolvePath(config &config, Httprequest &req)
{
    std::string dir;
    bool found = false;
    req.setPath("/api");
    dir = req.getPath().substr(0, req.getPath().find('/' , 1));
    std::cout << dir << std::endl;
    // exit(0);
    std::vector<Location> loc = config.get_servs()[0].get_location();
    for(int i = 0; i < loc.size();i++)
    {
        if (loc[i].path.find(dir) != -1)
        {
            found = true;
            if (loc[i].root != "")
            {
                dir = req.getPath().substr(req.getPath().find('/' , 1) + 1);
                req.setAbsolutePath(loc[i].root + "/" + dir);
                return found;
                // std::cout << req.getAbsolutePath() << std::endl;
            }
            else
            {
                dir = req.getPath().substr(req.getPath().find('/' , 1) + 1);
                req.setAbsolutePath(config.get_servs()[0].get_root() + "/" + dir);
                return found;
            }
        }
    }
    req.setAbsolutePath(config.get_servs()[0].get_root() + req.getPath());
    return found;
}


bool resolve_index(Httprequest &req, std::vector<Location> &loc, bool found_loc, config &config)
{
    if (found_loc == true)
    {
        std::string dir = req.getPath().substr(0, req.getPath().find('/' , 1));
        for(int i = 0; i < loc.size(); i++)
        {
            if (loc[i].path.find(dir) != -1)
            {
                if (loc[i].index != "")
                {
                    if(fileExists(req.getAbsolutePath() + "/" + loc[i].index) == true)
                    {
                        req.setfullPath(req.getAbsolutePath() + "/" + loc[i].index);
                        return true;
                    }
                    return false;
                }
                return false;
            }
        }
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
    bool found_loc = resolvePath(config, *this);
    std::cout << found_loc << std::endl;
    //check if path and root correct or not check if u should handel this GET /../../etc/passwd HTTP/1.1
    // if (config.get_servs()[0].get_root().back() == '/' && !path.empty() && path.front() == '/')
    //     absolutePath = config.get_servs()[0].get_root() + path.substr(1); // remove one slash
    // else
    //     absolutePath = config.get_servs()[0].get_root() + this->path;//add root from hafssa
    char c = '\0';
    if (pathExists(absolutePath, *this, c) == false)
        std::cout << "nice\n";
    std::vector<Location> loc = config.get_servs()[0].get_location();
    if (c == 'D')
    {
        if (resolve_index(*this, loc, found_loc, config) == true)
            std::cout << "oky";
        if (findIndexFile(absolutePath) != "")
            std::cout << "ok\n";
        else if (config.get_servs()[0].get_autoindexEnabled() == true)
        {
            std::cout << "ok\n";
        }
    }
    return 0;
}


