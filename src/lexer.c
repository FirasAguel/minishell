#include "shell.h"

int update_mode(char c, t_lexer *lexer)
{
  if (!c || lexer->escape_flag)
    return (0);
  if (c == '\'' && lexer->mode == MODE_SINGLE_QUOTE)
    return (lexer->mode = MODE_NORMAL, 1);
  if (c == '\'' && lexer->mode == MODE_NORMAL)
    return (lexer->mode = MODE_SINGLE_QUOTE, 1);
  if (c == '"' && lexer->mode == MODE_DOUBLE_QUOTE)
    return (lexer->mode = MODE_NORMAL, 1);
  if (c == '"' && lexer->mode == MODE_NORMAL)
    return (lexer->mode = MODE_DOUBLE_QUOTE, 1);
  return (0);
}

int is_operator(char c, t_lexer *lexer)
{
  if (lexer->mode != MODE_NORMAL || lexer->escape_flag)
    return (0);
  return (c == '|' || c == '<' || c == '>');
}

int is_delimiter(char c, t_lexer *lexer)
{
  if (lexer->mode == MODE_NORMAL && !lexer->escape_flag)
  {
	if (c == ' ' || c == '|') // || c == '<' || c == '>')
	  return (1);
  }
  return (0);
}

static int is_escapable_in_double_quote(char c)
{
  return (c == '"' || c == '\\' || c == '$' || c == '`' || c == '\n');
}

void append_to_token(char c, t_lexer *lexer)
{
  lexer->token_started = 1;
  if (lexer->escape_flag)
  {
    if (lexer->mode == MODE_NORMAL || (lexer->mode == MODE_DOUBLE_QUOTE && is_escapable_in_double_quote(c)))
    {
      lexer->buff[lexer->buff_i++] = c;
      lexer->escape_flag = 0;
    }
    else
    {
      lexer->buff[lexer->buff_i++] = '\\';
      lexer->buff[lexer->buff_i++] = c;
    }
    lexer->escape_flag = 0;
  }
  else if ((lexer->mode == MODE_NORMAL || lexer->mode == MODE_DOUBLE_QUOTE) && c == '\\')
    lexer->escape_flag = 1;
  else
    lexer->buff[lexer->buff_i++] = c;
}

void flush_token(t_lexer *lexer)
{
  lexer->buff[lexer->buff_i] = '\0';
  ft_lstadd_back(&(lexer->tokens), ft_lstnew(WORD, lexer->buff));
  lexer->token_count++; // do i still need this
  lexer->buff_i = 0;
  lexer->token_started = 0;
}

void handle_operator(char c, t_lexer *lexer)
{
	if (lexer->token_started)
	flush_token(lexer);
	if (c == '|')
		ft_lstadd_back(&(lexer->tokens), ft_lstnew(PIPE, "|"));
	else if (c == '<')
		ft_lstadd_back(&(lexer->tokens), ft_lstnew(REDIR_IN, "<"));
	else if (c == '>')
		ft_lstadd_back(&(lexer->tokens), ft_lstnew(REDIR_OUT, ">"));
}

int init_lexer(t_lexer **lexer)
{
  *lexer = (t_lexer *)malloc(sizeof(t_lexer));
  if (!*lexer)
    return (ft_puterr("(*lexer) malloc failed\n"), EXIT_FAILURE);
  (*lexer)->tokens = NULL;
  (*lexer)->token_count = 0;
  (*lexer)->mode = MODE_NORMAL;
  (*lexer)->buff_i = 0;
  (*lexer)->escape_flag = 0;
  return (EXIT_SUCCESS);
}

void	cleanup_after_lexer(t_lexer *lexer)
{
  if (lexer->mode != MODE_NORMAL)
    ft_puterr("lexing error: unclosed quote\n");
  if (lexer->escape_flag)
    ft_puterr("lexing error: dangling escape\n");
  if (lexer->token_started)
    flush_token(lexer);
}

t_token *lex(char *line)
{
  t_lexer *lexer;
  t_token *tokens;
  int i;

  if (init_lexer(&lexer) != EXIT_SUCCESS)
    return (NULL);
  i = 0;
  while (line[i])
  {
    if (update_mode(line[i], lexer))
      lexer->token_started = 1;
    else if (is_operator(line[i], lexer))
		handle_operator(line[i], lexer);
    else if (!is_delimiter(line[i], lexer))
      append_to_token(line[i], lexer);
    else if (lexer->token_started)
      flush_token(lexer);
    i++;
  }
  cleanup_after_lexer(lexer);
  tokens = lexer->tokens;
  free(lexer);
  return (tokens);
}
