#ifndef LEX_H
# define LEX_H

# include <iostream>
# include <string>
# include <fstream>
# include <vector>
# include <cctype>

using namespace std;

/* 문자유형들 */ // char_class
#define LETTER 0 // 문자
#define DIGIT 1 // 숫자
#define UNKNOWN 99 // 정해지지 않은 값
/* 토큰코드들 */ // next_token
#define INT_LIT 10 // const (상수)
#define IDENT 11 // 식별자(nonterminal)
#define COLON 19 // 등호 앞에 와서 유도할수있도록 해주는 콜론 :
#define ASSIGN_OP 20 // 등호 = (:= 가 되어야 유도할 수 있는 하나의 토큰이 됨.)
#define ADD_OP 21 // 더하기 기호 +
#define SUB_OP 22 // 빼기 기호 -
#define MULT_OP 23 // 곱하기 기호 *
#define DIV_OP 24 // 나누기 기호 /
#define LEFT_PAREN 25 // 괄호 열기 (
#define RIGHT_PAREN 26 // 괄호 닫기 )
#define SEMI_COLON 27 // 문장의 끝을 알려주는 세미콜론 ;
#define WHITE_SPACE 32 // ascii코드 32이하는 각 token을 구분하는 용도로 사용

class Lex
{
private:
    string filename; // 읽어야할 파일이름.
    vector<pair<string, int> > lexeme_table; // <lexeme, 토큰 코드>
    vector<pair<string, int> > statement; // 각 statement들의 lex class에서의 오류. 에러
    string token_string; // 각 lexeme의 문자열
    char ch; // 파일에서 읽어오는 char 1개(1byte).
    int next_token; // 읽어온 token의 타입(코드들).
    int char_class; // 하나의 ch를 읽었을 때 예상되는 lex의 그 타입.
    int num_state; // 몇 번째 statement인지.
    
    void get_char(ifstream& r_file); // 파일에서 한 문자씩 읽어오는 method.
    void lookup(); // 읽어온 문자의 유형 (char_class)가 unknown일 때 이를 토큰 코드별로 구분&처리 해주는 method.
public:
    Lex(string file_name); // 생성자. 읽어올 파일 이름을 매개변수로 받음.
    vector<pair<string, int> > get_vector() const; // lexeme_table을 return 함.
    vector<pair<string, int> > get_statement() const; // 각 statement들의 예외처리들을 return 함.
    void lexical(ifstream& r_file); // 읽은 문자(char 1개)를 char_class값을 이용해 추가적인 문자를 읽거나 
    //분석하여 하나의 lexeme을 만들어냄.
    int file_read(); // 파일을 열어서 한개의 문자를 읽고 lexical을 호출하는 코드.
};

#endif