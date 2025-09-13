#pragma once

    #include <iostream>
    #include <vector>
    #include <map>
    #include <string>
    #include <algorithm>
    #include <sys/stat.h>
    #include <dirent.h>
    #include <sstream>

    #include "../core_srv/include/Server.hpp"
    #include "../config/server.hpp"
    #include "../config/server.hpp"
    class Httprequest {
    
            std::string method;
            std::string path;
            std::string version;
            std::vector<char> body;
            std::map<std::string, std::string> headers;
            std::string absolutePath;
            std::string fullPath;
            bool error;
            std::string redirectLocation;

            // std::string query;
            int status_code;
            std::string status_text;
        public:
            Httprequest();
            int request_pars(std::vector<char>& request, config &config);
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



            void setfullPath(const std::string &fullPath);
            std::string getfullPath() const;

            void setError(const bool &Error);
            bool getError() const;

            void setRedirectLocation(const std::string &uri);
            std::string getRedirectLocation() const;

    };

    //i should add querys :

    /*GET /search?q=shoes&page=2 HTTP/1.1
    Host: shop.com*/