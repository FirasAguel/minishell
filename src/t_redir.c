#include "shell.h"

void	lst_redir_add_back(t_redir **lst, t_redir *new)
{
	t_redir	*p;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	p = lst_redir_last(*lst);
	p->next = new;
	return ;
}

t_redir	*lst_redir_last(t_redir *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

t_redir	*lst_redir_new(enum e_token_type type, char *file)
{
	t_redir	*token;

	token = (t_redir *)malloc(sizeof(t_redir));
	if (!token)
		return (NULL);
	token->type = type;
	token->file = strdup(file);
	if (!token->file)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

int	lst_redir_size(t_redir *lst)
{
	int	i;

	i = 0;
	while (lst)
	{
		i++;
		lst = lst->next;
	}
	return (i);
}

void	lst_redir_delone(t_redir *lst)
{
	if (!lst)
		return ;
	free(lst->file);
	free(lst);
	return ;
}

void	lst_redir_clear(t_redir **lst)
{
	t_redir	*curr;
	t_redir	*next;

	if (!lst)
		return ;
	curr = *lst;
	while (curr)
	{
		next = curr->next;
		lst_redir_delone(curr);
		curr = next;
	}
	*lst = NULL;
	return ;
}
