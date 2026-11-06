AR = ar -rcs
CC = cc
CFLAGS = -Wall -Werror -Wextra -pthread
RM = rm -rf
INCLUDE = -Iinclude
NAME = codexion


SRC_DIR = .
TEST_DIR = tests
OBJS_DIR = obj

CFILES = main.c \
utils/ft_atol_or_exit.c \
utils/ft_calloc.c \
utils/ft_math.c \
utils/ft_memset.c \
utils/ft_strcpy.c \
utils/ft_strdup.c \
utils/ft_strlen.c \
utils/ft_strncmp.c \
utils/ft_toupper.c \
monitor.c \
coder.c \
parser.c \
time.c \
errors.c \

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
