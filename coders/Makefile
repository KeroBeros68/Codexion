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

define BANNER
██ ▄█▀▓█████  ██▀███   ▒█████   ▄▄▄▄   ▓█████  ██▀███   ▒█████    ██████
 ██▄█▒ ▓█   ▀ ▓██ ▒ ██▒▒██▒  ██▒▓█████▄ ▓█   ▀ ▓██ ▒ ██▒▒██▒  ██▒▒██    ▒ 
▓███▄░ ▒███   ▓██ ░▄█ ▒▒██░  ██▒▒██▒ ▄██▒███   ▓██ ░▄█ ▒▒██░  ██▒░ ▓██▄   
▓██ █▄ ▒▓█  ▄ ▒██▀▀█▄  ▒██   ██░▒██░█▀  ▒▓█  ▄ ▒██▀▀█▄  ▒██   ██░  ▒   ██▒
▒██▒ █▄░▒████▒░██▓ ▒██▒░ ████▓▒░░▓█  ▀█▓░▒████▒░██▓ ▒██▒░ ████▓▒░▒██████▒▒
▒ ▒▒ ▓▒░░ ▒░ ░░ ▒▓ ░▒▓░░ ▒░▒░▒░ ░▒▓███▀▒░░ ▒░ ░░ ▒▓ ░▒▓░░ ▒░▒░▒░ ▒ ▒▓▒ ▒ ░
░ ░▒ ▒░ ░ ░  ░  ░▒ ░ ▒░  ░ ▒ ▒░ ▒░▒   ░  ░ ░  ░  ░▒ ░ ▒░  ░ ▒ ▒░ ░ ░▒  ░ ░
░ ░░ ░    ░     ░░   ░ ░ ░ ░ ▒   ░    ░    ░     ░░   ░ ░ ░ ░ ▒  ░  ░  ░  
░  ░      ░  ░   ░         ░ ░   ░         ░  ░   ░         ░ ░        ░  
                                      ░                                   
endef

# **************************************************************************** #
#                                  VARIABLES                                   #
# **************************************************************************** #

# Output Name
NAME = codexion

# Directories
OBJ_DIR = Obj

COD = \
		main.c \
		init.c \
		mem.c \
		logs.c \
		sim.c \
		monitoring.c \
		routine.c \
		setter.c \
		getter.c \
		dongles.c \
		heap.c \
		clean.c \
		error.c \
		utils.c \
		parsing.c

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

st:
	git status

add:
	echo "$(CYAN)--- Status ---$(RESET)"
	git status -s
	git add .
	echo "$(GREEN)✔ Files added.$(RESET)"
	git status -s
	echo "$(GREEN)✔ Done$(RESET)\n"

_commit: add
	if [ -z "$(M)" ]; then \
		echo "\033[0;31mError: Le message du commit (M) est vide !\033[0m"; \
		exit 1; \
	fi
	git commit -m "$(TYPE): $(M)"
	git push
	echo "$(GREEN)🚀 Successful push ($(TYPE)).$(RESET)\n"

push_feat:
	$(MAKE) _commit TYPE=feat M="$(M)"

push_fix:
	$(MAKE) _commit TYPE=fix M="$(M)"

push_refactor:
	$(MAKE) _commit TYPE=refactor M="$(M)"

push_docs:
	$(MAKE) _commit TYPE=docs M="$(M)"

push_style:
	$(MAKE) _commit TYPE=style M="$(M)"

push_chore:
	$(MAKE) _commit TYPE=chore M="$(M)"

re: fclean all

run:
	./codexion 10 2000 200 100 200 10 400 $(T)

run_hell:
	valgrind --tool=helgrind ./codexion 10 2000 200 100 200 10 400 $(T)

run_leak:
	valgrind --leak-check=full ./codexion 10 2000 200 100 200 10 400 $(T)

.PHONY: clean fclean re bonus \
		 push_chore push_doc \
		 push_factor push_feat \
		 push_fix push_style \
		 run_hell run_leak \
		 add st