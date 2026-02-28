#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <sys/wait.h>
# include <limits.h>
# include <readline/history.h>
# include <errno.h>
# include <fcntl.h>

# ifdef _WIN32
    #define ENV_PATH_SEP ';'
    #define PATH_SEP '\\'
# else
    #define ENV_PATH_SEP ':'
    #define PATH_SEP '/'
# endif

# define CMD_NOT_FOUND 127

enum e_lexing_modes
{
  MODE_NORMAL,
  MODE_SINGLE_QUOTE,
  MODE_DOUBLE_QUOTE
};

enum e_token_type
{
  WORD,
  PIPE,
  REDIR_IN,
  REDIR_OUT,
  REDIR_ERR,
  HEREDOC,
  APPEND,
  APPEND_ERR
};

typedef struct s_token
{
  enum e_token_type type;
  char *value;
  struct s_token *next;
} t_token;

typedef struct s_lexer
{
  enum e_lexing_modes mode;
  t_token *tokens;
  int token_count;
  // TODO use append or ft_realloc
  char buff[1024];
  int buff_i;
  int token_started;
  int escape_flag;
}	t_lexer;

typedef struct s_redir
{
  enum e_token_type type;
  char *file;
  void* next;
} t_redir;
typedef struct s_cmd
{
    t_redir  *redirs;
    char **argv;
    int in;
    int out;
    int err;
    void* next;
} t_cmd;
typedef struct s_std_fds
{
	int	in;
	int	out;
	int	err;
} t_std_fds;

int		ft_printf(const char *format, ...);
int		ft_printf_fd(int fd, const char *format, ...);

char	**ft_split(char const *s, char c);
void	free_char_arr(char **arr);
size_t	ft_strlen(const char *s);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
char	*ft_strndup(char const *s, size_t n);
char	*ft_strncpy(char *dest, const char *src, size_t n);
int		ft_strncmp(const char *s1, const char *s2, size_t n);
void	ft_puterr(char *s);
int		update_mode(char c, t_lexer *lexer);
int		is_delimiter(char c, t_lexer *lexer);
void	append_to_token(char c, t_lexer *lexer);
void	flush_token(t_lexer *lexer);
int		init_lexer(t_lexer **lexer);
t_token	*lex(char *line);
char	*ft_strjoin_path(char const *dir, char const *cmd);
char	**get_path_split_arr(char *envp[]);
char	*resolve_path(const char *cmd, char *envp[]);
void	exec_cmd(char **cmd_str_split, char **envp);
char	*get_home_dir(char *envp[]);
void	ft_lstadd_front(t_token **lst, t_token *new);
void	ft_lstadd_back(t_token **lst, t_token *new);
t_token	*ft_lstlast(t_token *lst);
t_token	*ft_lstnew(enum e_token_type type, char *value);
int		ft_lstsize(t_token *lst);
void	ft_lstdelone(t_token *lst);
void	ft_lstclear(t_token **lst);
int		str_in_arr(char *s, char *arr[]);
void	handle_echo(t_cmd *cmds);
void	handle_cd(char **argv, char **envp);
void	handle_pwd();
void	handle_type(t_cmd *cmds, char *builtin_cmds[], char **envp);
void	handle_history(t_cmd *cmds);
int		is_number(const char *nptr);
int		handle_exit(char **argv, int *exit_code);
int		handle_builtins(t_cmd *cmds, char *builtin_cmds[], char **envp);
int		handle_special_builtins(t_cmd *cmds, int *exit_code, char **envp);
void	t_cmd_clear(t_cmd **lst);
void	lst_redir_add_back(t_redir **lst, t_redir *new);
t_redir	*lst_redir_last(t_redir *lst);
t_redir	*lst_redir_new(enum e_token_type type, char *file);
int	lst_redir_size(t_redir *lst);
void	lst_redir_delone(t_redir *lst);
void	lst_redir_clear(t_redir **lst);

#endif // SHELL_H
