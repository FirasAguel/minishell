#include "shell.h"

void	free_char_arr(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

static int	count_words(char const *s, char c)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static int	ft_free_array(char ***arr, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		free((*arr)[i]);
		i++;
	}
	free(*arr);
	return (0);
}

static void	ft_add_words(char const *s, char c, char ***res, int *status)
{
	int	i;
	int	w_start;
	int	w_index;

	i = 0;
	w_index = 0;
	while (s[i] && *status)
	{
		if (s[i] != c)
		{
			w_start = i;
			while (s[i] && s[i] != c)
				i++;
			(*res)[w_index] = ft_strndup(s + w_start, i - w_start);
			if (!(*res)[w_index])
				*status = ft_free_array(res, w_index);
			else
				w_index++;
		}
		else
			i++;
	}
	if (*status)
		(*res)[w_index] = NULL;
}

char	**ft_split(char const *s, char c)
{
	char	**res;
	int		status;

	if (!s)
		return (NULL);
	res = (char **) malloc((count_words(s, c) + 1) * sizeof(char *));
	if (res == NULL)
		return (NULL);
	status = 1;
	ft_add_words(s, c, &res, &status);
	if (!status)
		return (NULL);
	return (res);
}

// #include<stdio.h>
// int	main()
// {
// 	char **arr = ft_split("   as  qwe 213 1 54 90 ",' ');
// 	for(int i = 0; arr[i]; i++)
// 		printf("%0d\t%s\n",i, arr[i]);
// 	return 0;
// }
