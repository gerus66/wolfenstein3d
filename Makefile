# q
# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ehugh-be <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/10/29 15:05:00 by ehugh-be          #+#    #+#              #
#    Updated: 2018/10/29 17:52:30 by ehugh-be         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = wolf
SRC = $(wildcard *.c) 
OBJ = $(SRC:.c=.o)
HDR = wolf.h
LIB = libft/libft.a
LIBDIR = libft/
FLAGS = -Wall -Wextra -Werror
#MFL = -lmlx -framework OpenGL -framework AppKit
#MLX = /usr/local/lib/
#MLXH = /usr/local/include
SDLFL = -l SDL2 -l SDL2_MIXER
SDL = /Users/*/.brew/Cellar/sdl2/2.0.9_1/lib/
SDL_MIXER = /Users/*/.brew/Cellar/sdl2_mixer/2.0.4/lib/
SDLH = SDL/includes

all: lib $(NAME)

lib:
	make -C $(LIBDIR)

$(NAME): $(OBJ)
	gcc $(FLAGS) $(LIB) -I $(SDLH) -L $(SDL) -L $(SDL_MIXER) $(SDLFL) $(OBJ) -o $(NAME)

%.o: %.c $(HDR) $(LIB)
	gcc $(FLAGS) -c $< -o $@

clean: 
	rm -f $(OBJ)
	make clean -C $(LIBDIR)

fclean: clean
	rm -f $(NAME)
	make fclean -C $(LIBDIR)

re: fclean all
