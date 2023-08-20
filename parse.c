#include "9cc.h"

struct Token *token;
struct LVAR *locals;

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

struct Token* consume_ident(){
    if(token->kind != TK_IDENT) return NULL;
    struct Token* res = token;
    token = token->next;
    return res;
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
  new->str = calloc(1, sizeof(char)*20);
  new->str = str;
  cur->next = new;
  new->len = len;
  return new;
}


struct LVAR* find_lvar(char *str){
    for(struct LVAR* i = locals;i; i= i->next){
        if(memcmp(str, i->str, strlen(str)) == 0)return i;
    }
    return NULL;
}

void new_lvar(char *str){
    struct LVAR* new = calloc(1, sizeof(struct LVAR));
    new->str = str;
    new->len = strlen(str);
    if(!locals){
        locals = new;
        new->offset = 8;
    }
    else{
        struct LVAR* c = locals;
        int cnt = 2;
        while(c->next){
            c = c->next;
            cnt++;
        }
        new->offset = cnt*8;
        c->next = new;
    }
}

struct Token *tokenize(char *p)
{
  struct Token head;
  head.next = NULL;
  struct Token *cur = &head;
  int name_cnt = 0;
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
    int cnt = 0;
    static char name[20][20] = {'\n'};
    while('a' <= *(p+cnt) && *(p+cnt) <= 'z'){
        cnt++;
    }
    if(cnt != 0){
        strncpy(name[name_cnt], p, cnt);
        cur = new_token(TK_IDENT, cur, cnt, name[name_cnt]);
        p+=cnt;
        if(!find_lvar(name[name_cnt])){
            new_lvar(name[name_cnt]);
        }
        name_cnt++;
        continue;
    }

    if (strchr("+-*/()<>;=", *p))
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