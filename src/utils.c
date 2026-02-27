#include "shell.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	j;

	i = 0;
	j = 0;
	while (src[i++])
		;
	if (size == 0)
		return (i - 1);
	while (j < (size - 1) && src[j])
	{
		dst[j] = src[j];
		j++;
	}
	dst[j] = '\0';
	return (i - 1);
}

char	*ft_strndup(char const *s, size_t n)
{
	size_t	l;
	char	*str;

	l = ft_strlen(s);
	if (l > n)
		l = n;
	str = (char *) malloc(l + 1);
	if (!str)
		return (NULL);
	ft_strlcpy(str, s, l + 1);
	return (str);
}

char	*ft_strncpy(char *dest, const char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n && src[i])
	{
		dest[i] = src[i];
		i++;
	}
	while (i < n)
	{
		dest[i] = '\0';
		i++;
	}
	return (dest);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	if (n == 0)
		return (0);
	i = 0;
	while (i < n - 1 && s1[i] && s2[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

int	str_in_arr(char *s, char *arr[])
{
	int	i;

	if (!arr)
		return (0);
	i = 0;
	while(arr[i])
	{
		if(!strcmp(s, arr[i]))
			return (1);
		i++;
	}
	return (0);
}
