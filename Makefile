NAME = ft_ls

SRCS =	main.c \
		ft_ls.c \
		parsing.c \
		utils.c \
		long_form.c \
		get_dir.c

SRCS_DIR = srcs

OBJS_DIR = .objs

INCLUDE_DIR = includes

LIBS = libft/ft

MAKE_DEP = ./libft/libft.a

CFLAGS = -Wall -Werror -Wextra -g3

include makefile-template/template.mk