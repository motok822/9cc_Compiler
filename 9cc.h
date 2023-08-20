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
  TK_IDENT,
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
  ND_ASSIGN,
  ND_EQ, // ==
  ND_NE, // !=
  ND_LT, // <
  ND_LE, // <=
  ND_LVAR,
  ND_NUM,
} NodeKind;

struct Node
{
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  int val;
  struct LVAR *lvar;
};

struct LVAR{
    struct LVAR* next;
    int offset;
    char *str;
    int len;
};

extern struct Token *token;
extern struct Node* code[100];
extern struct LVAR* locals;
struct Token *tokenize(char *p);
struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str);
void error(char *fmt, ...);
bool consume(char *op);
struct Token* consume_ident();
void expect(char *op);
int expect_number();
bool at_eof();
struct LVAR* find_lvar(char *str);
void new_lvar(char *str);
struct Node *new_Node(NodeKind kind, struct Node *lhs, struct Node *rhs);
struct Node *new_Node_num(int val);
void program();
struct Node *stmt();
struct Node *assign();
struct Node *mul();
struct Node *primary();
struct Node *expr();
struct Node *unary();
struct Node *add();
struct Node *relational();
struct Node *equality();
void gen(struct Node *cur);