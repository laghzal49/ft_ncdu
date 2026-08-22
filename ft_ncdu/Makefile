NAME        = ft_ncdu

CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -O3 -march=native -Iincludes
LDFLAGS     = -lncurses -pthread

SRC_DIR     = src
OBJ_DIR     = obj

SRCS        = $(SRC_DIR)/main.c \
              $(SRC_DIR)/scanner.c \
              $(SRC_DIR)/actions.c \
              $(SRC_DIR)/ui.c \
              $(SRC_DIR)/utils.c

OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
