NAME = ft_ping
CC = gcc
CFLAGS = -g -Wall -Werror -Wextra -I ./include
SRC_DIR = src/
FIND = $(shell find $(SRC_DIR))
SRC = $(filter %.c, $(FIND)) main.c
OBJ = ${SRC:.c=.o}

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -lm -o $(NAME)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME).a
	rm -rf $(NAME).a

re: fclean all
