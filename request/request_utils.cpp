#include "Request.hpp"
#include "../core_srv/include/ClientData.hpp" 

Httprequest::Httprequest()
{
    this->method = "";
    this->path = "";
    this->version = "";
    // this->body = ;
        // std::string query;
    this->error = false;
    this->absolutePath = "";
    this->status_code = 0;
    this->status_text = "";
    this->QUERY_STRING = "";
    this->check_autoindex = false;
    this->forceGetOnError = false;
    this->body_cgi = "";
    this->redirectLocation = "";
    this->is_deleted = false;
    // this->lenght = 0;
    this->file_opened = false;
    // this->cookie = "";
}


std::string uintToString(unsigned int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
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

void Httprequest::setError_page_found(const bool &Error)
{
    this->Error_page_found = Error;
}
            
bool Httprequest::getError_page_found() const
{
    return Error_page_found;
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

unsigned int Httprequest::getStatus_code() const
{
    return status_code;
}
            
std::string Httprequest::getStatus_text() const
{
    return status_text;
}

void Httprequest::set_servername(config &config)
{
    this->server_name = config.get_servs()[0].get_name();
}
            
std::string& Httprequest::get_servername()
{
    return this->server_name;
}


void Httprequest::set_check_autoindex(bool autoindex)
{
    this->check_autoindex = autoindex;
}
            
bool Httprequest::get_check_autoindex() const
{
    return this->check_autoindex;
}

void Httprequest::set_index(int nindex)
{
    this->index = nindex;
}
            
int Httprequest::get_index() const
{
    return this->index;
}

void Httprequest::setForceGetOnError(bool forceGetOnError)
{
    this->forceGetOnError = forceGetOnError;
}
bool Httprequest::getForceGetOnError()const
{
    return this->forceGetOnError;
}

void Httprequest::setBody_cgi(std::string body)
{
    body_cgi = body;
}
            
std::string Httprequest::getBody_cgi()const
{
    return body_cgi;
}

std::string Httprequest::get_query_string() const
{
    return (QUERY_STRING);
}

void Httprequest::set_query_string(std::string query)
{
    QUERY_STRING = query;
}

void Httprequest::set_is_deleted(bool is_deleted)
{
    this->is_deleted = is_deleted;
}
            
bool Httprequest::get_is_deleted() const
{
    return is_deleted;
}

void Httprequest::setCookie(const std::map<std::string, std::string> &Cookie)
{
    this->cookie = Cookie;
}
    
std::map<std::string, std::string>& Httprequest::getCookie() 
{
    return this->cookie;
}
     