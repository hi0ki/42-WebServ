#include "Request.hpp"
#include "../core_srv/include/ClientData.hpp" 
#include "../CGI/HTTPCGI.hpp"

std::string buildHeaders(Httprequest &req, size_t contentLength, bool keep_alive)
{
    std::string s;

    std::string contentType = "";
    std::string filePath = req.getAbsolutePath();
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
    else
        contentType = "text/plain";
    s = "Server: " + req.get_servername() + "\r\n";
    if (req.getStatus_code() == 301)
        s += "Location: " + req.getRedirectLocation() + "\r\n" ;
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

std::string Httprequest::buildHttpResponse(bool keep_alive) 
{
    std::cout << "wlh mnf mal had lekhra\n";
    std::cout << this->getStatus_code() << std::endl;
    std::string response;
    std::string body;
    std::string statusLine;
    std::ifstream file(this->getAbsolutePath().c_str(), std::ios::binary);
    std::cout << this->getAbsolutePath() << "   here\n"; 
    if (!this->get_check_autoindex() && !file.is_open()) {
        statusLine = "HTTP/1.1 404 Not Found\r\n";
        body = "<html><body><h1>404 Not Found</h1></body></html>";
    }
    else{
        statusLine = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text() + "\r\n";
        if (this->get_check_autoindex())
            body = AutoindexPage(*this);
        else
        {
            if (this->getBody_cgi() != "")
                body = this->getBody_cgi(); 
            else
            {
                std::ostringstream bodyStream;  
                bodyStream << file.rdbuf();
                body = bodyStream.str();
                if (this->method ==  "POST" && this->getStatus_code() == 201)
                {
                    body = "Upload Success\nFile uploaded successfully!";
                }

            }
        }
    }
    // response = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text();
    // response += "\r\n";
    response = statusLine + buildHeaders(*this, body.size(), keep_alive)+  body;
    // std::cout << "response :"<< response<< std::endl; // print response
    return response;
}