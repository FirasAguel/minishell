#ifndef SHELL_H
# define SHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <readline/readline.h>
# include <sys/wait.h>

# ifdef _WIN32
    #define ENV_PATH_SEP ';'
    #define PATH_SEP '\\'
# else
    #define ENV_PATH_SEP ':'
    #define PATH_SEP '/'
# endif

# define CMD_NOT_FOUND 127

char	**ft_split(char const *s, char c);
void	free_split(char **arr);
size_t	ft_strlen(const char *s);
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
char	*ft_strndup(char const *s, size_t n);
char	*ft_strncpy(char *dest, const char *src, size_t n);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

#endif // SHELL_H