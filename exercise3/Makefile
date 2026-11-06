AR = ar -rcs
CC = cc
CFLAGS = -Wall -Werror -Wextra
RM = rm -rf
INCLUDE = -Iinclude
NAME = codexion


SRC_DIR = .
TEST_DIR = tests
OBJS_DIR = obj

CFILES = main.c \
ft_strcpy.c \
ft_strdup.c \
ft_strncmp.c \
ft_toupper.c \
ft_atoi.c \
ft_calloc.c \
ft_memset.c \
ft_toupper.c \
thr.c \
utils.c \

OFILES = $(CFILES:%.c=%.o)

SRCS = $(addprefix $(SRC_DIR)/, $(CFILES))

OBJS = $(addprefix $(OBJS_DIR)/, $(OFILES))

all: $(NAME)

$(OBJS_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCLUDE) -c $^ -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $^ -o $@ 


clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all bonus clean fclean re libft
