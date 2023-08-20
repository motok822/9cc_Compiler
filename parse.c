#include "9cc.h"

struct Token *token;

void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

bool at_eof()
{
  if (token->kind == TK_EOF)
    return true;
  return false;
}

bool consume(char *op)
{
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

void expect(char *op)
{
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    error("'%s'ではありません", op);
  token = token->next;
}

int expect_number()
{
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str)
{
  struct Token *new = calloc(1, sizeof(struct Token));
  new->kind = tk;
  new->str = str;
  cur->next = new;
  new->len = len;
  return new;
}

struct Token *tokenize(char *p)
{
  struct Token head;
  head.next = NULL;
  struct Token *cur = &head;
  while (*p)
  {
    if (isspace(*p))
    {
      p++;
      continue;
    }
    if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0||
        strncmp(p, "<=", 2) == 0|| strncmp(p, ">=", 2) == 0)
    {
      cur = new_token(TK_RESERVED, cur, 2, p);
      p += 2;
      continue;
    }
    if (strchr("+-*/()<>", *p))
    {
      cur = new_token(TK_RESERVED, cur, 1, p++);
      continue;
    }
    if (isdigit(*p))
    {
      cur = new_token(TK_NUM, cur, 0, p);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }
    error("トークナイズできません");
  }
  cur = new_token(TK_EOF, cur, 0, p);
  return head.next;
}