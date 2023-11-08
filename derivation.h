#ifndef DERIVATION_H
# define DERIVATION_H

#define ERROR_CASE_MAX_COUNT 40

#include <memory>
#include <cstdlib>

typedef struct idents{
    string name;
    int num;
    int is_unknown; // 1 -> Unknown, 0 -> known 
} idents;

class Node {
public:
    int num; // 값
    int is_unknown; // 1 -> Unknown , 0 -> known 
    int pos_neg; // 1 -> div/sub, 0 -> mul, plus , -1 -> 입실론, 99-> 상관 x
    string name;
    vector<shared_ptr<Node> > children;

    void printTree(int level) const;

    Node(const string& name) : is_unknown(1), name(name) {}
};

class Derivation
{
private:
    vector<pair<string, int> > lexeme_table;
    vector<pair<string, int> > &statement;
    unsigned int current_index = 0;
    pair<string, int> next_token;
    vector<idents> symbol_table;
    int IS_KNOWN = 1; // 1이면 아직 LHS의 값이 안정해짐. 0이면 정해짐.
    int ID = 0;
    int OP = 0;
    int CONST = 0;
    int errorCase[ERROR_CASE_MAX_COUNT] = {0};
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