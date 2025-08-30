NAME = config
CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98
rm = rm -rf
HEADERS = config.hpp

SRCS = config.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS) $(HEADERS)
	$(CPP) $(CPPFLAGS) -o $(NAME) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	$(rm) $(OBJS)

fclean: clean
	$(rm) $(NAME)
re: fclean all

.PHONY: all clean fclean re