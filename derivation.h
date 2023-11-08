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
    int errorCase[ERROR_CASE_MAX_COUNT] = {0}; // error & warning 여부. error == 1, warning == 2
    string errorLexeme[ERROR_CASE_MAX_COUNT];
    int errorCount = 0;
    int statementCount = 0;

public:
    pair<string, int> lex();

    void errorMessage();
    
    shared_ptr<Node> programs();
    shared_ptr<Node> statements();
    shared_ptr<Node> getstatement();
    shared_ptr<Node> expression();
    shared_ptr<Node> term_tail();
    shared_ptr<Node> term();
    shared_ptr<Node> factor_tail();
    shared_ptr<Node> factor();
    shared_ptr<Node> int_lit();
    shared_ptr<Node> ident();
    shared_ptr<Node> assignment_op();
    shared_ptr<Node> semi_colon();
    shared_ptr<Node> add_operator();
    shared_ptr<Node> mult_operator();
    shared_ptr<Node> left_paren();
    shared_ptr<Node> right_paren();
    void error();

    Derivation(const vector<pair<string, int> >& lex_table, vector<pair<string, int> >& statement);

    int getIDCount();
    int getOPCount();
    int getCONSTCount();
    void resetCounts();

    void printTree(const std::shared_ptr<Node>& node, int depth = 0);

    void printStatement(const shared_ptr<Node>& node);
    void printSymbolTableAll();
};

#endif