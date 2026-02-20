.SILENT:


# **************************************************************************** #
#                                   COLORS                                     #
# **************************************************************************** #

GREEN   = \033[0;32m
RED     = \033[0;31m
YELLOW  = \033[0;33m
BLUE    = \033[0;34m
MAGENTA = \033[0;35m
CYAN    = \033[0;36m
RESET   = \033[0m

# **************************************************************************** #
#                                  VARIABLES                                   #
# **************************************************************************** #

# Output Name
NAME = codexion

# Directories
OBJ_DIR = Obj

COD = \
		coders.c \
		parsing.c \
		init.c \
		clean.c \
		error.c

SRCS =	$(COD)

INCLUDES = \
			coders.h

# Compilation
CC = cc
CFLAGS = -Wall -Werror -Wextra -pthread -I.

# Object name
OBJS = $(addprefix $(OBJ_DIR)/, $(SRCS:.c=.o))

#
TOTAL := $(words $(OBJS))
CURRENT := 0

# **************************************************************************** #
#									Rules									   #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS)
	printf "$(CYAN)Création du programme $(NAME)...$(RESET) "
	if $(CC) $(CFLAGS) -o $@ $(OBJS) 2>/dev/null; then \
		echo "$(GREEN)✓$(RESET)"; \
	else \
		echo "$(RED)✗$(RESET)"; \
		$(CC) $(CFLAGS) -o $@ $(OBJS); \
	fi

$(OBJ_DIR)/%.o: %.c $(INCLUDES) | $(OBJ_DIR)
	$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))
	printf "$(CYAN)[%2d/%2d]$(RESET) %-30s " $(CURRENT) $(TOTAL) $(notdir $<)
	if $(CC) $(CFLAGS) -c $< -o $@ 2>/dev/null; then \
		echo "$(GREEN)✓$(RESET)"; \
	else \
		echo "$(RED)✗$(RESET)"; \
		$(CC) $(CFLAGS) -c $< -o $@; \
	fi

$(OBJ_DIR):
	printf "$(CYAN)Création du répertoire $(OBJ_DIR)...$(RESET) "
	if mkdir -p $(OBJ_DIR) 2>/dev/null; then \
		echo "$(GREEN)✓$(RESET)"; \
	else \
		echo "$(RED)✗$(RESET)"; \
		mkdir -p $(OBJ_DIR); \
	fi

bonus: re

clean:
	printf "$(CYAN)Suppression de $(OBJ_DIR)...$(RESET) "
	if [ -d "$(OBJ_DIR)" ]; then \
		$(RM) -rf $(OBJ_DIR) && \
		echo "$(GREEN)✓ Fichiers objets supprimés$(RESET)"; \
	else \
		echo "$(YELLOW)⚠ Rien à nettoyer$(RESET)"; \
	fi

fclean: clean
	printf "$(CYAN)Suppression de $(NAME)...$(RESET) "
	if rm $(NAME) 2>/dev/null; then \
		echo "$(GREEN)✓$(RESET)"; \
	else \
		echo "$(YELLOW)⚠ Déjà supprimée$(RESET)"; \
	fi

push:
	cd .. && git add . && git commit -m "$(M)" && git push

re: fclean all

.PHONY: clean fclean re bonus push