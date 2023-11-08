#ifndef DERIVATION_H
# define DERIVATION_H

#define ERROR_CASE_MAX_COUNT 40 // 최대 탐지가능 에러 개수

#include <memory>
#include <cstdlib>

typedef struct idents{ //symbol_table에 저장될 ident에 대한 정보
    string name; // 해당 ident의 이름.
    int num; // ident 내부의 값.
    int is_unknown; // 1 -> Unknown, 0 -> known // 현재 값이 저장되어 있는지 여부.
} idents;

class Node { // 파스트리를 구성하는 node의 class
public:
    int num; // 값
    int is_unknown; // 1 -> Unknown , 0 -> known // 현재 값이 저장되어 있는지 여부.
    int pos_neg; // 1 -> div/sub, 0 -> mul, plus , -1 -> 입실론, 99-> 상관 x (어떤 연산자인지 여부)
    string name; // 해당 node의 이름.
    vector<shared_ptr<Node> > children; // 스마트 포인터를 이용해 파스트리를 구성.
    // 해당 노드의 자식노드의 포인터 값이 여기에 저장됨.

    void printTree(int level) const; // 파스 트리의 구성을 확인하기 위한 메소드. 파스트리의 레벨은 들여쓰기로 표현함.

    Node(const string& name) : is_unknown(1), name(name) {} // 노드의 생성자. 기본적으로 생성되는 모든 node는 
    // 아직 값을 모르기 때문에 unknown이라고 명시 & 이름을 name에 넣어줌.
};

class Derivation // lexeme을 하나씩 불러오면서 유도를 시행하고 파스 트리를 만들어주는 class
{
private:
    vector<pair<string, int> > lexeme_table; // Lex class로 부터 받은 lexeme들과 token_code의 집합.
    vector<pair<string, int> > &statement; // Lex class에서 statements를 lexeme으로 나누면서 발생한 예외처리 내용.
    unsigned int current_index = 0; // lexeme_table의 현재 index
    pair<string, int> next_token; // lexeme_table에서 받은 하나의 lexeme과 token_code.
    vector<idents> symbol_table; // ident의 값과 특징들을 저장한 symbol_table.
    int IS_KNOWN = 1; // 1이면 아직 LHS의 값이 안정해짐. 0이면 정해짐.
    int ID = 0; // 현재 statement의 ident 개수
    int OP = 0; // 현재 statement의 operator 개수
    int CONST = 0; // 현재 statement의 상수(decimal number) 개수
    int errorCase[ERROR_CASE_MAX_COUNT] = {0}; // error & warning 여부. error == 1, warning == 2, 
    //3일 때는 특수한 warning (상수와 ident가 연산자없이 연달아 왔을 때)
    string errorLexeme[ERROR_CASE_MAX_COUNT]; // error & warning 의 원인이 되는 lexeme 
    int errorCount = 0; // error & warning의 횟수
    int statementCount = 0; // 현재 몇 번째 statement인지를 나타내는 숫자 (경고문을 출력하는데 사용).

public:
    pair<string, int> lex(); // Lex() class에서 넘겨 받은 lexeme_table에서 lexeme과 token_code를 하나씩 가져오는 메소드.

    void errorMessage(); // 경고문을 출력하는 메소드. (매 statement의 유도가 끝나고 error & warning이 있으면 출력)
    
    shared_ptr<Node> programs(); // programs라는 이름(name값)을 가진 노드를 만들고,
    // programs의 RHS인 statements 노드를 자식으로 가지게 하는 메소드.

    shared_ptr<Node> statements(); // statements 라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.

    shared_ptr<Node> getstatement(); // getstatement 라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.

    shared_ptr<Node> expression(); //  expression 이라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.

    shared_ptr<Node> term_tail(); //  term_tail 이라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.

    shared_ptr<Node> term(); //  term 이라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.

    shared_ptr<Node> factor_tail(); // factor_tail 이라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.
    
    shared_ptr<Node> factor(); //  factor 라는 이름(name값)을 가진 노드를 만들고,
    // 주어진 문법의 RHS에 오는 것들을 자식 노드로 가지게 하는 메소드.

    shared_ptr<Node> int_lit(); // int_lit 라는 이름(name값)을 가진 노드를 만들고, 
    // const값을 자식 노드의 이름(name값)으로 가지게 하는 메소드.

    shared_ptr<Node> ident(); // ident 라는 이름(name값)을 가진 노드를 만들고, 
    // token_code가 ident인 lexeme을 자식노드로 가지게 하는 메소드.

    shared_ptr<Node> assignment_op(); //  assignment_op 라는 이름(name값)을 가진 노드를 만들고,
    // ":=" 를 자식노드의 name 값으로 가지게 하는 메소드.

    shared_ptr<Node> semi_colon(); //  semi_colon 이라는 이름(name값)을 가진 노드를 만들고,
    // ";" 를 자식노드의 name 값으로 가지게 하는 메소드. 

    shared_ptr<Node> add_operator(); //  add_operator 라는 이름(name값)을 가진 노드를 만들고,
    // token_code에 따라 "+"나 "-" 를 자식노드의 name 값으로 가지게 하는 메소드. 

    shared_ptr<Node> mult_operator(); //  mult_operator 라는 이름(name값)을 가진 노드를 만들고,
    // token_code에 따라 "*"나 "/" 를 자식노드의 name 값으로 가지게 하는 메소드. 

    shared_ptr<Node> left_paren(); //  left_paren 이라는 이름(name값)을 가진 노드를 만들고,
    // "(" 를 자식노드의 name 값으로 가지게 하는 메소드. 

    shared_ptr<Node> right_paren(); //  right_paren 이라는 이름(name값)을 가진 노드를 만들고,
    // ")" 를 자식노드의 name 값으로 가지게 하는 메소드. 

    void error(); // error나 warning이 발생했을 떄 실행되는 메소드. 오류의 원인이 되는 lexeme을 errorLexeme이라는
    // 배열에 저장하고, errorCount를 증가시킨다.

    Derivation(const vector<pair<string, int> >& lex_table, vector<pair<string, int> >& statement); // 생성자. Derivation 객체 생성.

    int getIDCount(); // statement의 ident의 개수를 return
    int getOPCount(); // statement의 operator의 개수를 return
    int getCONSTCount(); // statement의 const의 개수를 return
    void resetCounts(); // statement가 끝날때마다 ident, operator, const의 개수를 초기화 하는 메소드.

    void printTree(const std::shared_ptr<Node>& node, int depth = 0); // 트리의 구조를 출력하는데 사용하는 함수. 
    // 처음부터 모든 statement의 파스트리를 출력함. 트리의 깊이는 들여쓰기로 표시. (test용으로 사용. main함수에서 주석부분 제거시 확인가능)

    void printStatement(const shared_ptr<Node>& node); // 구현된 파스트리를 이동하면서 올바른 배정문의 전체를 출력. ex) operand1 := 3 * 2 ;
    void printSymbolTableAll(); // 마지막 결과 (Result ==> ... )를 출력.
};

#endif