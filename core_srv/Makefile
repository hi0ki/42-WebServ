C++ = c++ -Wall -Wextra -Werror

Src = source/Server.cpp main.cpp

Headers = include/Server.hpp

Name = server

Rm = rm -rf

Obj = $(Src:.cpp=.o)


all: $(Name)

$(Name) : $(Headers) $(Obj)
	$(C++) $(Obj) -o $(Name)

clean:
	$(Rm) $(Obj)

fclean: clean
	$(Rm) $(Name);

re: fclean all
