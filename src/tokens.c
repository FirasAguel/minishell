#include "shell.h"

void	ft_lstadd_front(t_token **lst, t_token *new)
{
	if (!lst || !new)
		return ;
	new->next = *lst;
	*lst = new;
	return ;
}

void	ft_lstadd_back(t_token **lst, t_token *new)
{
	t_token	*p;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	p = ft_lstlast(*lst);
	p->next = new;
	return ;
}

t_token	*ft_lstlast(t_token *lst)
{
	if (!lst)
		return (NULL);
	while (lst->next)
		lst = lst->next;
	return (lst);
}

t_token	*ft_lstnew(enum e_token_type type, char *value)
{
	t_token	*token;

	token = (t_token *)malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->value = strdup(value);
	if (!token->value)
	{
		free(token);
		return (NULL);
	}
	token->next = NULL;
	return (token);
}

int	ft_lstsize(t_token *lst)
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

void	ft_lstdelone(t_token *lst)
{
	if (!lst)
		return ;
	free(lst->value);
	free(lst);
	return ;
}

void	ft_lstclear(t_token **lst)
{
	t_token	*curr;
	t_token	*next;

	if (!lst)
		return ;
	curr = *lst;
	while (curr)
	{
		next = curr->next;
		ft_lstdelone(curr);
		curr = next;
	}
	*lst = NULL;
	return ;
}
