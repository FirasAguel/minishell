#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <sys/wait.h>
# include <limits.h>

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

typedef struct s_lexer
{
  enum e_lexing_modes mode;
  // TODO: use a linked list instead
  char **tokens;
  int token_count;
  // TODO use append or ft_realloc
  char buff[1024];
  int buff_i;
  int token_started;
  int escape_flag;
}	t_lexer;

char	**ft_split(char const *s, char c);
void	free_split(char **arr);
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
char	**lex(char *line);
char	*ft_strjoin_path(char const *dir, char const *cmd);
char	**get_path_split_arr(char *envp[]);
char	*resolve_path(const char *cmd, char *envp[]);
void	exec_cmd(char **cmd_str_split, char **envp);
char	*get_home_dir(char *envp[]);

#endif // SHELL_H
