#include "9cc.h"
struct Token *token;
struct Node *code[100];
struct LVAR *locals;
struct Func *functions;
char *user_input;
int var_cnt = 0;
void all_var(){
    int cnt = 0;
    for(struct LVAR* i = locals; i; i = i -> next){
        printf("var%d %s\n", cnt, i->str);
        cnt++;
    }
}
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
  user_input = argv[1];
  token = tokenize(argv[1]);
  program(); //構文木の作成
  printf("push rbp\n");
  printf("mov rbp, rsp\n");
  printf("sub rsp, %d\n", 8*var_cnt);
  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]);
    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("pop rax\n");
  }
  printf("mov rsp, rbp\n");
  printf("pop rbp\n");
  printf("ret\n");
  return 0;
}
