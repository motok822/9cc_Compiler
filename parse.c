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

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
  fprintf(stderr, "^ ");
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

bool consume_return(){
    if(token->kind != TK_RETURN) return false;
    token = token -> next;
    return true;
}

bool consume_number(){  //こいつは例外的にtokenを進めないので注意
  if(token->kind != TK_NUM) return false;
  return true;
}

bool consume(char *op)
{
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

bool check(char *op){
  if(token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    return false;
  return true;
}

void expect(char *op)
{
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len)){
    printf("次のtokenは%sです\n", token->next->str);
    error("'%s'ではありません", op);

  }
  token = token->next;
}

int expect_number()
{
  if (token->kind != TK_NUM){
    printf("%s\n", token->str);
    error("数ではありません");
  }
  int val = token->val;
  token = token->next;
  return val;
}

struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str)
{
  struct Token *new = calloc(1, sizeof(struct Token));
  new->kind = tk;
  new->str = calloc(1, sizeof(char)*20);
  strncpy(new->str, str, len);
  new->str[len] = '\0';
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

struct Func* find_func(char *str){
  for(struct Func* i = functions; i; i = i->next){
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

void new_func(struct Node *res, char *str){  //functionsの列に追加
  struct Func *new = calloc(1, sizeof(struct Func));
  new->node_func = res;
  new->str = str;
  if(!functions){
    functions = new;
  }else{
    struct Func *c = functions;
    while(c->next){
      c = c->next;
    }
    c->next = new;
  }
}

int is_alnum(char c){
    return ('a'<=c && c <= 'z') || ('A'<=c && c <= 'Z') || c == '_' || ('0' <= c && c <= '9');
}

struct Token *tokenize(char *p)
{
  struct Token head;
  head.next = NULL;
  struct Token *cur = &head;
  var_cnt = 0;
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
    if(strncmp(p, "return", 6) == 0){
        cur = new_token(TK_RETURN, cur, 6, p);
        p += 6;
        continue;
    }
    if(strncmp(p, "if", 2) == 0){
        cur = new_token(TK_RESERVED, cur, 2, p);
        p += 2;
        continue;
    }
    if(strncmp(p, "else", 4) == 0){
        cur = new_token(TK_RESERVED, cur, 4, p);
        p += 4;
        continue;
    }
    if(strncmp(p, "while", 5) == 0){
        cur = new_token(TK_RESERVED, cur, 5, p);
        p += 5;
        continue;
    }
    if(strncmp(p, "for", 3) == 0){
        cur = new_token(TK_RESERVED, cur, 3, p);
        p += 3;
        continue;
    }
    if (strchr("+-*/()<>;={},", *p))
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

    int cnt = 0;
    static char *name[20];
    while(is_alnum(*(p+cnt))){
        cnt++;
    }
    if(cnt != 0){
        name[var_cnt] = calloc(1, sizeof(char)*20);
        int exist_var = 0;
        for(int i = 0;i < var_cnt;i++){
          if(strlen(name[i]) == cnt && strncmp(name[i], p, cnt) == 0)exist_var = i+1;
        }
        if(exist_var != 0){   //既に変数が存在している
          cur = new_token(TK_IDENT, cur, cnt, name[exist_var-1]);
          p+=cnt;
          continue;
        }
        strncpy(name[var_cnt], p, cnt);//新規の変数
        cur = new_token(TK_IDENT, cur, cnt, name[var_cnt]);
        p+=cnt;
        if(!find_lvar(name[var_cnt])){
            new_lvar(name[var_cnt]);
            var_cnt++;
        }
        continue;
    }

    error("トークナイズできません");
  }
  cur = new_token(TK_EOF, cur, 0, p);
  return head.next;
}