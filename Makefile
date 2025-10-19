CXX      = c++ -Wall -Wextra -Werror -std=c++17
SRC      = main.cpp \
	core_srv/source/Server.cpp core_srv/source/ClientData.cpp \
	config/config.cpp config/server.cpp \
	request/Request.cpp request/Request_utils.cpp request/Response.cpp request/post_utils.cpp \
	CGI/HTTPCGI.cpp

HEADERS  = core_srv/include/Server.hpp core_srv/include/ClientData.hpp \
	config/config.hpp config/server.hpp \
	request/Request.hpp CGI/HTTPCGI.hpp

NAME     = webserv
RM       = rm -f
OBJ      = $(SRC:.cpp=.o)

# Colors
BLUE     = \033[0;34m
GREEN    = \033[0;32m
YELLOW   = \033[0;33m
darkGREY = \033[1;30m
RED      = \033[0;31m
RESET    = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	@echo "$(BLUE)[+] Linking $(NAME)...$(RESET)"
	@$(CXX) $(OBJ) -o $(NAME)
	@echo "$(GREEN)[✔] Build Complete!$(RESET)"
	@echo "$(YELLOW)Run the server with :$(RESET)"
	@echo "$(darkGREY)  ./$(NAME) config_file.conf$(RESET)"
	@echo "$(darkGREY)  ./$(NAME) yourfileconfig.conf$(RESET)"

%.o: %.cpp $(HEADERS)
	@printf "$(BLUE)Compiling %-35s$(RESET)" "$<"
	@$(CXX) -c $< -o $@
	@echo " $(GREEN)[✔]$(RESET)"

clean:
	@$(RM) $(OBJ)
	@echo "$(RED)[✖] Object files cleaned.$(RESET)"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(RED)[✖] Executable removed.$(RESET)"

re: fclean all

.PHONY: all clean fclean re
