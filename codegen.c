#include "9cc.h"

struct Token *token;
struct Node *new_Node(NodeKind kind, struct Node *lhs, struct Node *rhs)
{
  struct Node *new = calloc(1, sizeof(struct Node));
  new->kind = kind;
  new->lhs = lhs;
  new->rhs = rhs;
  return new;
}

struct Node *new_Node_num(int val)
{
  struct Node *new = calloc(1, sizeof(struct Node));
  new->kind = ND_NUM;
  new->val = val;
  return new;
}

struct Node *expr()
{
  struct Node *cur = equality();
  return cur;
}

struct Node *equality()
{
  struct Node *cur = relational();
  while (1)
  {
    if (consume("=="))
    {
      cur = new_Node(ND_EQ, cur, relational());
    }
    if (consume("!="))
    {
      cur = new_Node(ND_NE, cur, relational());
    }
    break;
  }
  return cur;
}

struct Node *relational()
{
  struct Node *cur = add();
  while (1)
  {
    if (consume("<"))
    {
      cur = new_Node(ND_LT, cur, add());
    }
    if (consume("<="))
    {
      cur = new_Node(ND_LE, cur, add());
    }
    if (consume(">"))
    {
      cur = new_Node(ND_LT, add(), cur);
    }
    if (consume(">="))
    {
      cur = new_Node(ND_LE, add(), cur);
    }
    break;
  }
  return cur;
}

struct Node *add()
{
  struct Node *cur = mul();
  while (1)
  {
    if (consume("+"))
    {
      cur = new_Node(ND_ADD, cur, mul());
    }
    if (consume("-"))
    {
      cur = new_Node(ND_SUB, cur, mul());
    }
    break;
  }
  return cur;
}

struct Node *mul()
{
  struct Node *cur = unary();
  while (1)
  {
    if (consume("*"))
    {
      cur = new_Node(ND_MUL, cur, unary());
      continue;
    }
    if (consume("/"))
    {
      cur = new_Node(ND_DIV, cur, unary());
      continue;
    }
    break;
  }
  return cur;
}

struct Node *unary()
{
  if (consume("+"))
  {
    return primary();
  }
  if (consume("-"))
  {
    return new_Node(ND_SUB, new_Node_num(0), primary());
  }
  return primary();
}

struct Node *primary()
{
  if (consume("("))
  {
    struct Node *cur = expr();
    expect(")");
    return cur;
  }
  return new_Node_num(expect_number());
}

void gen(struct Node *cur)
{
  if (cur->kind == ND_NUM)
  {
    printf("push %d\n", cur->val);
    return;
  }
  gen(cur->lhs);
  gen(cur->rhs);
  printf("pop rdi\n");
  printf("pop rax\n");
  switch (cur->kind)
  {
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
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  }
  printf("push rax\n");
}