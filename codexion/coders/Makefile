AR = ar -rcs
CC = cc
CFLAGS = -Wall -Werror -Wextra -pthread
RM = rm -rf
INCLUDE = -Iinclude
NAME = codexion
TSANFLAGS = -fsanitize=thread -g -O1 -pthread
ASANFLAGS = -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer -pthread

SRC_DIR = .
TEST_DIR = tests
OBJS_DIR = obj

CFILES = main.c \
utils/ft_atoi_strict.c \
utils/ft_calloc.c \
utils/print_log.c \
utils/colors.c \
monitor.c \
dongle.c \
dongle_both.c \
coder.c \
coder_handle.c \
parser.c \
queue.c \
binary_tree.c \
scheduler.c \
free.c \
init.c \
time.c \
error.c \

OFILES = $(CFILES:%.c=%.o)

SRCS = $(addprefix $(SRC_DIR)/, $(CFILES))

OBJS = $(addprefix $(OBJS_DIR)/, $(OFILES))

all: $(NAME)

tsanitize:
	$(CC) $(TSANFLAGS) $(INCLUDE) $(SRCS) -o /tmp/codexion_tsan
	TSAN_OPTIONS="halt_on_error=1:exitcode=66" \
	  /tmp/codexion_tsan 4 1000 50 20 20 2 10 edf

asanitize:
	$(CC) $(ASANFLAGS) $(INCLUDE) $(SRCS) -o /tmp/codexion_asan
	ASAN_OPTIONS="detect_leaks=1:halt_on_error=1" \
	UBSAN_OPTIONS="print_stacktrace=1" \
	  /tmp/codexion_asan 4 1000 50 20 20 2 10 edf

valgrind:
	valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=42 \
  	./codexion 4 1000 50 20 20 2 10 edf

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

.PHONY: all bonus clean fclean re libft asanitize tsanitize valgrind
