// Location& findMatchingLocation(Httprequest &req, config &config)
// {
//     std::vector<Location> locations = config.get_servs()[req.get_index()].get_location();
//     static Location dummy; 
//     Location* best_match = nullptr;
//     size_t longest = 0;

//     for (size_t i = 0; i < locations.size(); i++) 
//     {
//         if (req.getPath().compare(0, locations[i].path.size(), locations[i].path) == 0) 
//         {
//             if (locations[i].path.size() > longest) 
//             {
//                 best_match = &locations[i];
//                 longest = locations[i].path.size();
//             }
//         }
//     }
//     if (best_match)
//         return *best_match;
//     req.setStatus(404, "Not Found");
//     return dummy;
// }



//Most websites use a default “homepage” inside directories.
//If the user requests /folder/, they usually want /folder/index.html.


//Connects to IP 127.0.0.1 on port 8080.
//If your web server is running and listening on 8080, connection succeeds.



// std::string Httprequest::buildHttpResponse() 
// {
//     // std::cout << "wlh \n";
//     std::string response;
//     std::string body;
//     std::string statusLine;
//     std::ifstream file(this->getfullPath().c_str(), std::ios::binary);
//     // std::cout << this->getfullPath() << "   here\n";
//    //remember to test with www/
// //    if (this->get_check_autoindex() == true)
// //         body = AutoindexPage(*this);
//     if (!file.is_open()) {
//         statusLine = "HTTP/1.1 404 Not Found\r\n";
//         body = "<html><body><h1>404 Not Found</h1></body></html>";
//     }
//     else{
//         statusLine = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text() + "\r\n";
//         std::ostringstream bodyStream;
//         bodyStream << file.rdbuf();
//         body = bodyStream.str();
//     }
//     // std::cout << "im hereeeee\n";
//     // response = "HTTP/1.1 " + uintToString(this->getStatus_code()) + " " + this->getStatus_text();
//     // response += "\r\n";
//     response = statusLine + buildHeaders(*this, body.size())+  body;
//     std::cout << "response :"<< response<< std::endl; // print response
//     return response;
// }