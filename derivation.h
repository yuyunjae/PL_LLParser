#ifndef DERIVATION_H
# define DERIVATION_H

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
    vector<shared_ptr<Node>> children;

    void printTree(int level) const;

    Node(const string& name) : is_unknown(1), name(name) {}
};

class Derivation
{
private:
    vector<pair<string, int>> lexeme_table;
    unsigned int current_index = 0;
    pair<string, int> next_token;
    vector<idents> symbol_table;
    int IS_KNOWN = 1; // 1이면 아직 LHS의 값이 안정해짐. 0이면 정해짐.
    int ID = 0;
    int OP = 0;
    int CONST = 0;

public:
    pair<string, int> lex();

    pair<string, int> current_lex();
    
    shared_ptr<Node> programs();
    shared_ptr<Node> statements();
    shared_ptr<Node> statement();
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

    Derivation(const vector<pair<string, int>> &lex_table);

    int getIDCount();
    int getOPCount();
    int getCONSTCount();
    void resetCounts();

    void printTree(const std::shared_ptr<Node>& node, int depth = 0);

    void printStatement(const shared_ptr<Node>& node);
    void printSymbolTableAll();
};

#endif