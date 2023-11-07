G++ = g++ -Wall -Wextra -Werror
NAME = parser.exe
SRCS = lex.cpp main.cpp derivation.cpp
OBJS = $(SRCS:.cpp=.o)

all:$(NAME)

$(NAME) : $(OBJS)
	$(G++) $(OBJS) -o $(NAME)

%.o : %.cpp  lex.h derivation.h
	$(G++) -c $< -o $@

clean :
	rm -rf $(OBJS) 

fclean :
	rm -rf $(OBJS) $(NAME)

re :
	make fclean
	make all

.PHONY: clean all re fclean