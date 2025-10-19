#pragma once

    #include <iostream>
    #include <vector>
    #include <map>
    #include <string>
    #include <algorithm>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <sstream>

    #include "../config/server.hpp"
    #include "../config/config.hpp"

    #define BUFFER_SIZE 2048 

    class ClientData;

    class Httprequest {
    
            std::string method;
            std::string path;
            std::string version;
            std::vector<char> body;
            std::map<std::string, std::string> headers;
            std::string absolutePath;
            bool error;
            std::string redirectLocation;
            std::string server_name;
            int index;
            bool Error_page_found;
            int status_code;
            std::string status_text;
            bool check_autoindex;
            std::string QUERY_STRING;
            std::string body_cgi;
            bool file_opened;
            bool cgi_allowed;
        public:
            Httprequest();
            int request_pars(ClientData &client , config &config);
            ~Httprequest(){}

            void setMethod(const std::string &method);
            std::string getMethod() const;

            void setPath(const std::string &Path);
            std::string getPath() const;

            void setVersion(const std::string &Version);
            std::string getVersion() const;

            void setHeaders(const std::map<std::string, std::string> &Headers);
            std::map<std::string, std::string>& getHeaders();

            void setBody(const std::vector<char> &Body);
            std::vector<char> getBody() const;

            void setAbsolutePath(const std::string &AbsolutePath);
            std::string getAbsolutePath() const;

            void setStatus(int status_code, std::string status_text);
            unsigned int getStatus_code() const;
            std::string getStatus_text() const;

            void setError_page_found(const bool &Error);
            bool getError_page_found() const;

            void setError(const bool &Error);
            bool getError() const;

            void setRedirectLocation(const std::string &uri);
            std::string getRedirectLocation() const;

            
            std::string buildHttpResponse(bool keep_alive, ClientData &client);

            void ft_clean();

            void set_servername(config &config);
            std::string& get_servername();

            void set_check_autoindex(bool autoindex);
            bool get_check_autoindex() const;


            void set_index(int nindex);
            int get_index() const;

            std::string get_query_string() const;
            void set_query_string(std::string query);


            void setBody_cgi(std::string body);
            std::string getBody_cgi()const;

            void setcgi_allowed(bool is_cgi_allowed);
            bool getcgi_allowed();
    
    };

    Location findMatchingLocation(Httprequest &req, config &config);
    std::string uintToString(unsigned int value);
    std::string AutoindexPage(Httprequest &req);
    bool location_has_cgi(Httprequest &req, config &config, ClientData &client);
    bool    handleMethod(Httprequest &req, config &config, ClientData &client);
    bool check_Error_pages(Httprequest &req, config &config, ClientData &client);
