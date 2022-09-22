NAME = ft_ls
SRCS =	main.c \
		ft_ls.c \
		parsing.c \
		utils.c \
		long_form.c \
		get_dir.c

INCLUDES = ft_ls.h

LIBS = libft/ft

MAKE_DEP = ./libft/libft.a

LIB_ARG = $(foreach path, $(LIBS), -L $(dir $(path)) -l $(notdir $(path)))

OBJS := $(SRCS:%.c=%.o)

CFLAGS = -Wall -Werror -Wextra -g3 -D VECTOR_STORAGE -fsanitize=undefined #-D USE_LOCALES

CC = cc

all: $(NAME)

$(NAME): $(OBJS) $(MAKE_DEP)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIB_ARG)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(MAKE_DEP):
	make -C $(dir $@) $(notdir $@)

malloc_test: $(OBJS) $(MAKE_DEP)
	        $(CC) $(CFLAGS) -fsanitize=undefined -rdynamic -o $@ ${OBJS} ${LIB_ARG} -L. -lmallocator

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean all fclean re
