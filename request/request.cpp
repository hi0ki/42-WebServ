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
    //check if path and root correct or not check if u should handel this GET /../../etc/passwd HTTP/1.1
    if (config.get_servs()[0].get_root().back() == '/' && !path.empty() && path.front() == '/')
        absolutePath = config.get_servs()[0].get_root() + path.substr(1); // remove one slash
    else
        absolutePath = config.get_servs()[0].get_root() + this->path;//add root from hafssa
    char c = '\0';
    pathExists(absolutePath, *this, c);
    if (c == 'D')
    {
        if (findIndexFile(absolutePath) != "")
            std::cout << "ok\n";
        else if (config.get_servs()[0].get_autoindex() == true)
        {
            std::cout << "ok\n";
        }
    }
    return 0;
}


