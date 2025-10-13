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

    #define BUFFER_SIZE 4096 

    class ClientData;

    class Httprequest {
    
            std::string method;
            std::string path;
            std::string version;
            std::vector<char> body;
            std::map<std::string, std::string> headers;
            std::string absolutePath;
            bool error; //fach kaykon path makeynech andlit biha wahed l cas odaba anzid f aya cas
            std::string redirectLocation;
            std::string server_name;
            int index;
            bool forceGetOnError; //mazal makhademto
            bool Error_page_found; //fach kanelQa fconfig erro page kayen

            // std::string query;
            int status_code;
            std::string status_text;
            bool check_autoindex;
            std::string QUERY_STRING;
            std::string body_cgi;
            bool is_deleted;
            bool file_opened;;
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

            // clean
            void ft_clean();

            void set_servername(config &config);
            std::string& get_servername();

            void set_check_autoindex(bool autoindex);
            bool get_check_autoindex() const;


            void set_index(int nindex);
            int get_index() const;


            void setForceGetOnError(bool forceGetOnError);
            bool getForceGetOnError()const;

            //add by hanebaro
            std::string get_query_string() const;
            void set_query_string(std::string query);


            void setBody_cgi(std::string body);
            std::string getBody_cgi()const;


            void set_is_deleted(bool is_delted);
            bool get_is_deleted() const;



            


    
    };

    Location findMatchingLocation(Httprequest &req, config &config);
    std::string uintToString(unsigned int value);
    std::string AutoindexPage(Httprequest &req);

   /*
    1/ is_location_have_redirection()

    location /old {
        return 301 /new;
    }
    HTTP/1.1 301 Moved Permanently

    2/  is_method_allowded_in_location()
    location /upload {
        methods POST, GET;
        root /var/www/uploads;
    }   
    server {
    listen 8080;
    server_name localhost;

    root /var/www/html;
    index index.html;

    methods_allowed GET, POST;   # default for entire server

    location / {
        # inherits: GET, POST
    }

    location /delete {
        methods_allowed DELETE;  # overrides: only DELETE
    }
}
    Request → / with GET ✅ allowed

    Request → / with POST ✅ allowed (from server default)

    Request → /delete with GET ❌ not allowed (location overrides)

    Request → /delete with DELETE ✅ allowed

    3/    //possible to be eroor page in location

    4/  //want from hafsa method allowed and return 301 /home/index.html and autoindex in every location
    5/ //khesni nzid   cgi_enabled on; allowed methods
    6// pwd
    7 //  errors pages for every location 
     note:  location /cgi-bin {
        root /var/www/cgi-bin;
        methods GET, POST;
        cgi_enabled on;
        cgi_path /usr/bin/python3;
        cgi_extension .py;
}
    //i should add querys :

    GET /search?q=shoes&page=2 HTTP/1.1
    Host: shop.com
    */
   