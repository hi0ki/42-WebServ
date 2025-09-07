#include "request.hpp"
#include "../core_srv/include/Server.hpp"
#include "../config/server.hpp"

/*POST /submit-form HTTP/1.1
Host: localhost:8080
Content-Type: application/x-www-form-urlencoded
Content-Length: 27

name=Fatima&age=20&city=Rabat
*/

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

void check_methods(Httprequest &req)
{
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
}

void check_version(Httprequest &req)
{
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

}

int Httprequest::request_pars(std::vector<char> &request)
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
    absolutePath = "root" + this->path;//add root from hafssa
    return 0;
}

// int main()
// {
//     // Httprequest req;
//     // std::string s ="GET /home HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html\r\n\r\n";

//     // std::vector<char> r;
//     // for(int i = 0; s[i]; i++)
//     //     req.request.push_back(s[i]);
//     // req.request_pars(req);
//     std::cout << "["<< req.getMethod() << "]"<<  std::endl;
//     std::cout << "["<< req.getPath() << "]"<<  std::endl;
//     std::cout << "["<< req.getVersion() << "]"<<  std::endl;
//     std::cout << std::endl;
//     for(std::map<std::string, std::string>::iterator it = req.getHeaders().begin(); it != req.getHeaders().end(); it++)
//         std::cout << it->first << "  " << it->second << "  ";
//     return 0;
//     for(int i = 0; i < req.getBody().size(); i++)
//         std::cout << req.getBody()[i] ;
// }
