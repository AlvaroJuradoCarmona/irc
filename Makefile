NAME	= ircserv
FLAGS	= c++ -Wall -Werror -Wextra -std=c++98
RM 		= rm -rf

PORT	?= 6667
PASS	?= 1111

# =================================================================================

INCLUDES_PATH		= includes
INCLUDES_SUBDIRS	= commands parser exceptions
INCLUDES_DIRS		= $(INCLUDES_PATH) $(addprefix $(INCLUDES_PATH)/, $(INCLUDES_SUBDIRS))

HEADERS				= $(addprefix -I , $(INCLUDES_DIRS))

# =================================================================================

SRC_DIR		= src
CMD_DIR		= $(SRC_DIR)/commands/
PARSER_DIR	= $(SRC_DIR)/parser/


CMD_PREFIXS		= A User Nick Pass Quit PrivateMessage Join Part Invite Mode Kick Topic Notice
CMD_FILES		= $(addsuffix Command, $(CMD_PREFIXS))
CMD_SRCS		= $(addprefix $(CMD_DIR), $(CMD_FILES))

PARSER_PREFIXS	= Command User Pass Nick Quit PrivateMessage Join Part Invite Mode Kick Topic Notice
PARSER_FILES	= $(addsuffix Parser, $(PARSER_PREFIXS))
PARSER_SRCS		= $(addprefix $(PARSER_DIR), $(PARSER_FILES))

FILES 			= main Server User Channel Utils Logger Responses

SRCS_PATHS	= $(addprefix $(SRC_DIR)/, $(FILES)) $(CMD_SRCS) $(PARSER_SRCS)
SRCS			  = $(addsuffix .cpp, $(SRCS_PATHS))

# =================================================================================

all:		$(NAME)

%.o:		%.cpp
	$(FLAGS) $(HEADERS) -c $< -o $@

$(NAME):	$(OBJS)
	$(FLAGS) $(HEADERS) $(SRCS) -o $(NAME)

clean: 		
	$(RM) $(OBJS) $(BOBJS) files

fclean:
	$(RM) $(NAME)

re: 		fclean all

e:			re
	clear
	./$(NAME) $(PORT) $(PASS)

a:			$(NAME)
	clear
	./$(NAME) $(PORT) $(PASS)

.PHONY: a all clean e fclean re