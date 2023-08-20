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

struct Token
{
  TokenKind kind;
  struct Token *next;
  int val;
  char *str;
  int len;
};


typedef enum
{
  ND_ADD,
  ND_SUB,
  ND_MUL,
  ND_DIV,
  ND_EQ, // ==
  ND_NE, // !=
  ND_LT, // <
  ND_LE, // <=
  ND_NUM,
} NodeKind;

struct Node
{
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val;
};

extern struct Token *token;
struct Token *tokenize(char *p);
struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str);
void error(char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
struct Node *new_Node(NodeKind kind, struct Node *lhs, struct Node *rhs);
struct Node *new_Node_num(int val);
struct Node *mul();
struct Node *primary();
struct Node *expr();
struct Node *unary();
struct Node *add();
struct Node *relational();
struct Node *equality();
void gen(struct Node *cur);