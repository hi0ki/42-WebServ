C++ = c++ -Wall -Wextra -Werror

Src = main.cpp \
	core_srv/source/Server.cpp core_srv/source/ClientData.cpp\
	config/config.cpp config/server.cpp

Headers = core_srv/include/Server.hpp core_srv/include/ClientData.hpp\
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
