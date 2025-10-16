config::config(std::string nameFile) : File(nameFile)
{
    if(nameFile.size() < 5 || nameFile.compare(nameFile.size() - 5, 5, ".conf") != 0)
        throw std::runtime_error("config file invalid : " + nameFile);
    if (!File.is_open())
        throw std::runtime_error("Unable to open the file: " + nameFile);
    this->parse_configFile();
    this->print_servers();
}

void config::parse_configFile()
{
    std::vector<std::string> conf;
    std::string ligne;
    std::vector<std::string>::iterator it;
    
    while(getline(File, ligne))
    {
        if(ligne == "\n")
            continue;//je peut la suppr
        conf.push_back(ligne);
    }
    it = conf.begin();
    std::vector<std::string> tmp;
    while(it != conf.end())// apres split verif lesligne vide
    {
        tmp = split(*it, ' ');
        try
        {
            if(!tmp.empty() && tmp.size() == 2 && tmp[0] == "server" && tmp[1] == "{")//check server
            {
                    
                if(tmp.size() >= 3)
                {
                    throw std::runtime_error("content invalid");
                }
                else
                {
                    set_server(++it, conf);
                    // tmp.clear();
                    // tmp = split(*(it + 1), ' ');
                    // if(*it == "}" && tmp[0] == "server" && tmp[1] == "{")
                    //     throw std::runtime_error("content invalid");
                          
                }       
            }
            else if (!tmp.empty())
                throw std::runtime_error("content invalid");
        }
        catch (std::exception &e)
        {
            std::cerr << RED << e.what() << RESET << std::endl;
        }
        it++;
        tmp.clear();
    }
}