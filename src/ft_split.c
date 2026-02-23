/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: firas <firas@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 12:59:35 by maguel            #+#    #+#             */
/*   Updated: 2026/02/24 00:17:46 by firas            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>

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
