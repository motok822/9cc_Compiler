#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

typedef enum
{
  TK_RESERVED,
  TK_NUM,
  TK_EOF,
} TokenKind;

typedef enum
{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_NUM,
} NodeKind;

struct Node
{
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val;
};

struct Token
{
  TokenKind kind;
  struct Token *next;
  int val;
  char *str;
  int len;
};

struct Token *token;
static struct Token *tokenize(char *p);
static struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str);
static void error(char *fmt, ...);
static bool consume(char *op);
static void expect(char *op);
static int expect_number();
static bool at_eof();
static struct Node *new_Node(NodeKind kind, struct Node* lhs, struct Node* rhs);
static struct Node *new_Node_num(int val);
static struct Node *mul();
static struct Node *primary();
static struct Node *expr();
static struct Node *unary();
static void gen(struct Node *cur);

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");
  token = tokenize(argv[1]);
  struct Node *node = expr();
  gen(node);
  printf("pop rax\n");
  printf("ret\n");
  return 0;
}

static void error(char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

static bool at_eof()
{
  if (token->kind == TK_EOF)
    return true;
  return false;
}

static bool consume(char *op)
{
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

static void expect(char *op)
{
  if (token->kind != TK_RESERVED || token->len != strlen(op) || memcmp(token->str, op, token->len))
    error("'%s'ではありません", op);
  token = token->next;
}

static int expect_number()
{
  if (token->kind != TK_NUM)
    error("数ではありません");
  int val = token->val;
  token = token->next;
  return val;
}

static struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str)
{
  struct Token *new = calloc(1, sizeof(struct Token));
  new->kind = tk;
  new->str = str;
  cur->next = new;
  new->len = len;
  return new;
}

static struct Token *tokenize(char *p)
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
    if (strchr("+-*/()", *p)) {
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

static struct Node *new_Node(NodeKind kind, struct Node* lhs, struct Node* rhs)
{
  struct Node *new = calloc(1, sizeof(struct Node));
  new->kind = kind;
  new->lhs = lhs;
  new->rhs = rhs;
  return new;
}

static struct Node* new_Node_num(int val){
  struct Node *new = calloc(1, sizeof(struct Node));
  new->kind = ND_NUM;
  new->val = val;
  return new;
}

static struct Node* expr(){
  struct Node* cur = mul();
  while(1){
    if(consume("+")){
      cur = new_Node(ND_ADD, cur, mul());
      continue;
    }
    if(consume("-")){
      cur = new_Node(ND_SUB, cur, mul());
      continue;
    }
    break;
  }
  return cur;
}

static struct Node* mul(){
  struct Node* cur = unary();
  while(1){
    if(consume("*")){
      cur = new_Node(ND_MUL, cur, unary());
      continue;
    }
    if(consume("/")){
      cur = new_Node(ND_DIV, cur, unary());
      continue;
    }
    break;
  }
  return cur;
}

static struct Node* unary(){
  if(consume("+")){
    return primary();
  }
  if(consume("-")){
    return new_Node(ND_SUB, new_Node_num(0), primary());
  }
  return primary();
}

static struct Node* primary(){
  if(consume("(")){
    struct Node *cur = expr();
    expect(")");
    return cur;
  }
  return new_Node_num(expect_number());
}

static void gen(struct Node *cur){
  if(cur->kind == ND_NUM){
    printf("push %d\n", cur->val);
    return;
  }
  gen(cur->lhs);
  gen(cur->rhs);
  printf("pop rdi\n");
  printf("pop rax\n");
  switch(cur->kind){
    case ND_ADD:
      printf("add rax, rdi\n");
      break;
    case ND_SUB:
      printf("sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
  }
  printf("push rax\n");
}