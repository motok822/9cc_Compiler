#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>


typedef enum
{
  TK_RESERVED,
  TK_RETURN,
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
  ND_RETURN,
  ND_IF,
  ND_ELSE,
  ND_WHILE,
  ND_FOR,
  ND_BLOCK,
  ND_FUNC,
  ND_FUNCCALL,
  ND_NUM,
} NodeKind;

struct Node
{
  NodeKind kind;
  struct Node *lhs;
  struct Node *rhs;
  struct LVAR *lvar;
  struct Node **blocks;
  struct Func *func;
  int val;
  int block_len;
  int cap;
};

struct LVAR{
    struct LVAR* next;
    int val;
    int offset;
    char *str;
    int len;
};

struct Func{
    char *str;
    int cap;
    int args_len;
    struct Node *node_func;
    struct LVAR **args;
    struct Func *next;
};

extern struct Token *token;
extern struct Node* code[100];
extern struct LVAR* locals;
extern struct Func *functions;
extern char *user_input;
extern int var_cnt;
struct Token *tokenize(char *p);
struct Token *new_token(TokenKind tk, struct Token *cur, int len, char *str);
void error(char *fmt, ...);
bool consume(char *op);
struct Token* consume_ident();
bool consume_number();
void expect(char *op);
int expect_number();
bool at_eof();
bool consume_return();
struct LVAR* find_lvar(char *str);
struct Func* find_func(char *str);
void new_lvar(char *str);
void new_func(struct Node *res, char *str);
void error_at(char *loc, char *fmt, ...);
int is_alnum(char c);
struct Node *new_Node(NodeKind kind, struct Node *lhs, struct Node *rhs);
struct Node *new_Node_num(int val);
struct Node *block(struct Node *cur);
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