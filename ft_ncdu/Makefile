# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: laghzal <laghzal@student.1337.ma>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/22 22:45:00 by laghzal           #+#    #+#              #
#    Updated: 2026/08/22 22:45:00 by laghzal          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME        = ft_ncdu

CC          = gcc
CFLAGS      = -Wall -Wextra -Werror -O3 -march=native -Iincludes
LDFLAGS     = -lncurses -pthread

SRC_DIR     = src
OBJ_DIR     = obj

SRCS        = $(SRC_DIR)/main.c \
              $(SRC_DIR)/core/scanner.c \
              $(SRC_DIR)/core/worker.c \
              $(SRC_DIR)/core/nav.c \
              $(SRC_DIR)/core/events.c \
              $(SRC_DIR)/core/dispatch.c \
              $(SRC_DIR)/actions/delete.c \
              $(SRC_DIR)/actions/goinfre.c \
              $(SRC_DIR)/actions/healer.c \
              $(SRC_DIR)/actions/presets.c \
              $(SRC_DIR)/actions/peek.c \
              $(SRC_DIR)/actions/tools.c \
              $(SRC_DIR)/actions/report.c \
              $(SRC_DIR)/actions/cli.c \
              $(SRC_DIR)/ui/colors.c \
              $(SRC_DIR)/ui/hud.c \
              $(SRC_DIR)/ui/table.c \
              $(SRC_DIR)/ui/inspector.c \
              $(SRC_DIR)/ui/draw.c \
              $(SRC_DIR)/ui/modals.c \
              $(SRC_DIR)/utils/presets_data.c \
              $(SRC_DIR)/utils/security.c \
              $(SRC_DIR)/utils/format.c \
              $(SRC_DIR)/utils/memory.c \
              $(SRC_DIR)/utils/filter.c

OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

install: $(NAME)
	@mkdir -p $(HOME)/.local/bin
	cp -f $(NAME) $(HOME)/.local/bin/$(NAME)
	cp -f $(NAME) $(HOME)/.local/bin/ntcl13
	cp -f $(NAME) $(HOME)/.local/bin/clean42
	@chmod +x $(HOME)/.local/bin/$(NAME) $(HOME)/.local/bin/ntcl13 $(HOME)/.local/bin/clean42
	@echo "======================================================"
	@echo "  Installed to $(HOME)/.local/bin:"
	@echo "  -> ft_ncdu  : Interactive Disk & Quota Manager"
	@echo "  -> ntcl13   : Native C Fast 42 Cluster Cleaner"
	@echo "  -> clean42  : Native C Fast 42 Cluster Cleaner"
	@echo "======================================================"

uninstall:
	rm -f $(HOME)/.local/bin/$(NAME)
	rm -f $(HOME)/.local/bin/ntcl13
	rm -f $(HOME)/.local/bin/clean42
	@echo "Uninstalled ft_ncdu, ntcl13, clean42 from $(HOME)/.local/bin"

norm:
	@norminette src includes || true


.PHONY: all clean fclean re install uninstall norm
