#ifndef LEX_H
# define LEX_H

# include <iostream>
# include <string>
# include <fstream>

using namespace std;

/* 문자유형들 */
#define LETTER 0 // 문자
#define DIGIT 1 // 숫자
#define UNKNOWN 99 // 정해지지 않은 값
/* 토큰코드들 */
#define INT_LIT 10 // 
#define IDENT 11 // ID(nonterminal)
#define ASSIGN_OP 20 // 등호 =
#define ADD_OP 21 // 더하기 기호 +
#define SUB_OP 22 // 빼기 기호 -
#define MULT_OP 23 // 곱하기 기호 *
#define DIV_OP 24 // 나누기 기호 /
#define LEFT_PAREN 25 // 괄호 열기 (
#define RIGHT_PAREN 26 // 괄호 닫기 )

class Lex
{
private:
    string filename;
    //string 
    int file_read();
public:
    Lex(string file_name);
};

#endif