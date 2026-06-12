NAME = ft_ping
CC = gcc
CLANG = clang
CFLAGS = -g -Wall -Werror -Wextra -I ./include
SRC_DIR = src/
LIBFTPRINTF = lib/libftprintf.a
FIND = $(shell find $(SRC_DIR))
SRC = $(filter %.c, $(FIND))
OBJ = ${SRC:.c=.o}

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(<:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
#		make -C lib
		ar -rcs ft_ping.a $(OBJ)
		$(CC) $(CFLAGS) main.c ft_ping.a -o $(NAME)
		clear

clean:
#		make -C lib clean
		rm -rf $(OBJ)
		clear

fclean: clean
#		make -C lib fclean
		rm -rf ft_ping.a
		rm -rf $(NAME)
		clear

re: fclean all
