#include "9cc.h"
struct Token *token;
struct Node *code[100];
struct LVAR *locals;
int var_cnt;

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
  program();
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
