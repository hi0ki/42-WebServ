#pragma once

    #include <iostream>
    #include <vector>
    #include <map>
    #include <string>
    #include <algorithm>

    class Httprequest {
    
            std::string method;
            std::string path;
            std::string version;
            std::vector<char> body;
            std::map<std::string, std::string> headers;
            std::string absolutePath;

            // std::string query;
            int status_code;
            std::string status_text;
        public:
            Httprequest();
            // Httprequest(std::vector<char>& request);
            int request_pars(std::vector<char>& request);
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


    };