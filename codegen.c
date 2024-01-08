#include "9cc.h"

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

void program()
{
  int i = 0;
  while (!at_eof())
  {
    code[i++] = stmt();
  }
  code[i] = NULL;
}

struct Node *new_block()
{
  struct Node *res = calloc(1, sizeof(struct Node));
  res->blocks = calloc(1, sizeof(struct Node *) * 8);
  res->block_len = 0;
  res->cap = 8;
  res->kind = ND_BLOCK;
  return res;
}

struct Node *stmt()
{
  if (consume("{"))  //blockの場合の処理
  {
    struct Node *res = new_block();
    res->block_len = 0;
    while (!consume("}"))
    {
      if (res->cap > res->block_len)
      {
        struct Node *cur = stmt();
        res->blocks[res->block_len] = cur;
        res->block_len++;
      }
      else
      {
        res->cap *= 2;
        res->blocks = realloc(res->blocks, sizeof(struct Node *) * res->cap);
      }
    }
    return res;
  }
  if (consume("for"))
  {
    expect("(");
    struct Node *for1 = expr();
    expect(";");
    struct Node *for2 = expr();
    expect(";");
    struct Node *for3 = expr();
    expect(")");
    struct Node *cur = stmt();
    struct Node *res = calloc(1, sizeof(struct Node));
    res->kind = ND_FOR;
    res->lhs = for1;
    res->rhs = calloc(1, sizeof(struct Node));
    res->rhs->lhs = for2;
    res->rhs->rhs = calloc(1, sizeof(struct Node));
    res->rhs->rhs->lhs = for3;
    res->rhs->rhs->rhs = cur;
    return res;
  }
  if (consume("while"))
  {
    expect("(");
    struct Node *cur = expr();
    expect(")");
    struct Node *res = calloc(1, sizeof(struct Node));
    res->kind = ND_WHILE;
    res->lhs = cur;
    res->rhs = stmt();
    return res;
  }
  if (consume("if"))
  {
    expect("(");
    struct Node *cur = expr();
    expect(")");
    struct Node *res = calloc(1, sizeof(struct Node));
    res->kind = ND_IF;
    res->lhs = cur;
    struct Node *run = calloc(1, sizeof(struct Node));
    run->kind = ND_ELSE;
    run->lhs = stmt();
    if (consume("else"))
    {
      run->rhs = stmt();
    }
    res->rhs = run;
    return res;
  }
  if (consume_return())
  {
    struct Node *cur = calloc(1, sizeof(struct Node));
    cur->kind = ND_RETURN;
    cur->rhs = expr();
    expect(";");
    return cur;
  }
  struct Node *cur;
  cur = expr();
  if (cur->kind != ND_FUNC)
    expect(";");
  return cur;
}

struct Node *expr()
{
  struct Node *cur = assign();
  return cur;
}

struct Node *assign()
{
  struct Node *cur = equality();
  if (consume("="))
  {
    cur = new_Node(ND_ASSIGN, cur, assign());
  }
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
  if(consume("*")){
    return new_Node(ND_DEREF, new_Node_num(0), primary());
  }
  if(consume("&")){
    return new_Node(ND_ADDR, new_Node_num(0), primary());
  }
  return primary();
}

struct Func* define_argument(struct Node *cur, struct Token *tok)
{
  struct Func *new = calloc(1, sizeof(struct Func));
  int cnt = 0;
  new -> cap = 8;
  new -> args_len = 0;
  new -> args = calloc(1, sizeof(struct LVAR)*8);
  new -> node_func = cur;
  new -> str = tok -> str;
  if(check(")"))return new;
  while (tok)
  { 
    if(!consume("int"))error("引数の型がありません");
    tok = consume_ident();
    if (tok)
    {
      if(new->args_len >= new->cap){
        new->cap *= 2;
        new->args = realloc(new->args, sizeof(struct LVAR)*new->cap);
      }
      new_lvar(tok->str);
      new->args[new->args_len] = find_lvar(tok->str);
      new->args_len++;
      if(!consume(",")) break;
    }
  }
  return new;
}


struct Node *primary()
{
  
  int first_def = 0;
  if(consume("int")){
    first_def = 1;
  }
  struct Token *tok = consume_ident();
  if (tok)
  {
    if (find_func(tok->str))
    { // 関数が定義済み(呼び出し)
      struct Func *f = find_func(tok->str);
      struct Node *res = calloc(1, sizeof(struct Node));
      res->rhs = f->node_func->rhs; //関数の内部（実行するやつ）
      res->func = f;
      res->lvar = f->node_func->lvar;
      res->kind = ND_FUNCCALL;
      res->args_node = calloc(1, sizeof(struct Node)*res->func->args_len);
      if (consume("(")) 
      {
        int cnt = 0;
        do{
          if(check(")"))break;
          res -> args_node[cnt] = expr();
          cnt++;
        }while(consume(","));
        if(cnt != res->func->args_len){
          printf("cnt: %d\n", cnt);
          printf("args_len: %d\n", res->func->args_len);
          error("引数の数が違います");
        }
        expect(")");
        return res;
      }
      else
      {
        error("関数の呼び出し方が違います");
      }
    }
    if (consume("("))
    {
      if(first_def != 1)error("関数の宣言方法が違います");
      struct Node *res = calloc(1, sizeof(struct Node));
      res->lhs = calloc(1, sizeof(struct Node));
      res->kind = ND_FUNC; // ND_FUNCの左側には引数の変数
      // res->lvar = find_lvar(tok->str);
      struct Func* new = define_argument(res->lhs, tok);// 関数の引数の処理
      if(new->args_len > 6)error("サポートしてしていません");
      res -> func = new;
      if(!functions){   //functionsの列に追加
        functions = new;
      }else{
        struct Func *c = functions;
        while(c->next){
          c = c->next;
        }
        c = new;
      }
      expect(")");
      res->rhs = stmt();
      return res;
    }
    // ローカル変数
    struct Node *cur = calloc(1, sizeof(struct Node));
    cur->kind = ND_LVAR;
    if(!find_lvar(tok->str) && first_def == 0)error("変数の定義の仕方が違います");
    if(find_lvar(tok->str)){
      cur->lvar = find_lvar(tok->str);
      return cur;
    }
    new_lvar(tok->str);
    cur->lvar = find_lvar(tok->str);
    return cur;
  }
  if (consume("("))
  {
    struct Node *cur = expr();
    expect(")");
    return cur;
  }
  return new_Node_num(expect_number());
}

const char *arg_resister[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
void gen_lvar(struct Node *node)
{
  if (node->kind != ND_LVAR)
    return;
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->lvar->offset);
  printf("  push rax\n");
}

void gen_lvar2(struct LVAR *l){
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", l->offset);
  printf("  push rax\n");
}

void init_var(struct Node* cur){
  for(int i = 0;i < cur->func->args_len;i++){
    gen_lvar2(cur->func->args[i]);
    printf("  pop rax\n");
    printf("  mov [rax], %s\n", arg_resister[i]);
  }
}

void init_arg_resister(struct Node* cur){
  for(int i = 0;i < cur->func->args_len;i++){
    gen(cur->args_node[i]);
    printf("  pop rax\n");
    printf("  mov %s, rax\n",arg_resister[i]);
  }
}

void gen(struct Node *cur)
{
  int jmp_cnt = 0;
  int cnt = 0;
  if (!cur)
    return;
  switch (cur->kind)
  {
  case ND_NUM:
    printf("  push %d\n", cur->val);
    return;
  case ND_LVAR:
    gen_lvar(cur);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  case ND_ASSIGN:
    gen_lvar(cur->lhs);
    gen(cur->rhs);
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  case ND_RETURN:
    gen(cur->rhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_IF:
    gen(cur->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lelse%d\n", jmp_cnt);
    gen(cur->rhs->lhs);
    printf("  jmp .Lend%d\n", jmp_cnt);
    printf("  .Lelse%d:\n", jmp_cnt);
    if (cur->rhs->rhs)
    {
      gen(cur->rhs->rhs);
    }
    printf("  .Lend%d:\n", jmp_cnt);
    jmp_cnt++;
    return;
  case ND_WHILE:
    printf("  .Lbegin%d:\n", jmp_cnt);
    gen(cur->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", jmp_cnt);
    gen(cur->rhs);
    printf("  jmp .Lbegin%d\n", jmp_cnt);
    printf("  .Lend%d:\n", jmp_cnt);
    jmp_cnt++;
    return;
  case ND_FOR:
    gen(cur->lhs);
    printf("  .Lbegin%d:\n", jmp_cnt);
    gen(cur->rhs->lhs);
    printf("  pop rax\n");
    printf("  cmp rax, 0\n");
    printf("  je .Lend%d\n", jmp_cnt);
    gen(cur->rhs->rhs->rhs);
    gen(cur->rhs->rhs->lhs);
    printf("  jmp .Lbegin%d\n", jmp_cnt);
    printf("  .Lend%d:\n", jmp_cnt);
    jmp_cnt++;
    return;
  case ND_BLOCK:
    while (cur->blocks[cnt])
    {
      gen(cur->blocks[cnt]);
      cnt++;
      if (!cur->blocks[cnt])
        break;
      printf("  pop rax\n");
    }
    return;
  case ND_FUNC:
    printf("%s:\n", cur->func->str);
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", 8 * var_cnt);
    init_var(cur);
    gen(cur->rhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  case ND_FUNCCALL:
    init_arg_resister(cur);
    printf("  call %s\n", cur->func->str);
    printf("  push rax\n");
    return;
  case ND_ADDR:
    gen_lvar(cur->rhs);
    return;
  case ND_DEREF:
    gen(cur->rhs);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }
  gen(cur->lhs);
  gen(cur->rhs);
  printf("  pop rdi\n");
  printf("  pop rax\n");
  switch (cur->kind)
  {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
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
  printf("  push rax\n");
}
