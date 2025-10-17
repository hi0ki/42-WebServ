#include "Request.hpp"
#include "../core_srv/include/ClientData.hpp" 
#include "../CGI/HTTPCGI.hpp"

std::string buildHeaders(Httprequest &req, size_t contentLength, bool keep_alive, ClientData &client)
{
    std::string s;

    std::string contentType = "";
    std::string filePath = req.getAbsolutePath();
    if (filePath.find(".html") != std::string::npos || req.get_check_autoindex() || filePath.find(".htm") != std::string::npos)
        contentType = "text/html";
    else if (filePath.find(".css") != std::string::npos)
        contentType = "text/css";
    else if (filePath.find(".js") != std::string::npos)
        contentType = "application/javascript";
    else if (filePath.find(".png") != std::string::npos)
        contentType = "image/png";
    else if (filePath.find(".jpg") != std::string::npos || filePath.find(".jpeg") != std::string::npos)
        contentType = "image/jpeg";
    else if (filePath.find(".gif") != std::string::npos)
        contentType = "image/gif";
    else if (filePath.find(".svg") != std::string::npos)
        contentType = "image/svg+xml";
    else if (filePath.find(".json") != std::string::npos)
        contentType = "application/json";
    else if (filePath.find(".pdf") != std::string::npos)
        contentType = "application/pdf";
    else if (filePath.find(".txt") != std::string::npos)
        contentType = "text/plain";
    else if (filePath.find(".mp3") != std::string::npos)
        contentType = "audio/mpeg";
    else if (filePath.find(".wav") != std::string::npos)
        contentType = "audio/wav";
    else if (filePath.find(".ogg") != std::string::npos)
        contentType = "audio/ogg";
    else if (filePath.find(".mp4") != std::string::npos)
        contentType = "video/mp4";
    else if (filePath.find(".mov") != std::string::npos)
        contentType = "video/quicktime";
    else if (filePath.find(".webm") != std::string::npos)
        contentType = "video/webm";
    else if (filePath.find(".avi") != std::string::npos)
        contentType = "video/x-msvideo";
    else
        contentType = "text/plain";
    s = "Server: " + req.get_servername() + "\r\n";
    if (req.getStatus_code() == 301)
        s += "Location: " + req.getRedirectLocation() + "\r\n" ;
    if (!client.getSession_data().empty())
    {
        s += "Set-Cookie: ";
        size_t size = 0;
        for (std::map<std::string, std::string>::iterator it = client.getSession_data().begin(); it != client.getSession_data().end(); ++it)
        {
            if (it->first != "sessionId")
            {
                s += it->first + "=" + it->second;
                if (size + 1 != client.getSession_data().size())
                    s += "; ";
                else
                    s += "\r\n";
            }
            size++;
        }
    }
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

std::string Httprequest::buildHttpResponse(bool keep_alive, ClientData &client) 
{
    // std::cout <<  "fresponse status code " << this->getStatus_code() << getAbsolutePath() << std::endl;
    std::string response;
    std::string body;
    std::string statusLine;
    long long body_size = 0;

    if (method == "GET" && !this->get_check_autoindex() && this->getBody_cgi() == "")
    {
        if (!this->file_opened)
        {
            client.getFile().open(this->getAbsolutePath().c_str());
            // client.getFile().open(this->getAbsolutePath().c_str(), std::ios::binary);
            if (!client.getFile().is_open()) {
                response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\nContent-Length: 70\r\n\r\n \
                <html><body><h1>404 Not Found</h1><p>The requested file could not be found on this server.</p></body></html>";
                return response;
            }
            this->file_opened = true;
            if (client.get_resp_length() == 0)
            {
                client.getFile().seekg(0, std::ios::end);
                std::ifstream::pos_type size = client.getFile().tellg();
                client.getFile().close();
                client.getFile().open(this->getAbsolutePath().c_str());
                // client.getFile().open(this->getAbsolutePath().c_str(), std::ios::binary);
                body_size = static_cast<long long>(size);
                client.set_header_length(body_size);
            }
        }
        if (client.get_ftime_resp())
        {
            char buffer[BUFFER_SIZE];
            client.getFile().read(buffer, BUFFER_SIZE);
            body = std::string(buffer, client.getFile().gcount());
            client.set_resp_length(body.length());
            std::cout << "lenght = " << client.get_resp_length() << " | length li khso ikon = " << client.get_header_length() << std::endl;
            if (client.getFile().eof() || client.get_resp_length() >= client.get_header_length())
            {
                std::cout << RED << "End of file, closing\n" << RESET;
                client.getFile().close();
                client.set_header_length(-1);
            }
        }
    }
    if (this->get_check_autoindex())
        body = AutoindexPage(*this);
    else if (this->getBody_cgi() != "")
        body = this->getBody_cgi(); 
    else if (this->method ==  "POST" && this->getStatus_code() == 201)
        body = "Upload Success\nFile uploaded successfully!";
    if (this->get_check_autoindex() || this->getBody_cgi() != "" || this->getStatus_code() == 201)
        body_size = body.size();
    statusLine = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text() + "\r\n";
    if (!client.get_ftime_resp())
    {
        response = statusLine + buildHeaders(*this, body_size, keep_alive, client) + body;
        client.set_ftime_resp(true);
    }
    else
        response = body;
    return response;
}


