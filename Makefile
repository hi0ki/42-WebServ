C++ = c++ -Wall -Wextra -Werror

Src = core_srv/source/Server.cpp main.cpp\
	config/config.cpp config/server.cpp

Headers = core_srv/include/Server.hpp\
	config/config.hpp config/server.hpp\


Name = webserv

Rm = rm -f

Obj = $(Src:.cpp=.o)


all: $(Name)

$(Name) : $(Headers) $(Obj)
	$(C++) $(Obj) -o $(Name)

clean:
	$(Rm) $(Obj)

fclean: clean
	$(Rm) $(Name);

re: fclean all
