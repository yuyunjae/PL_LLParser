#ifndef DERIVATION_H
# define DERIVATION_H

#include <memory>

class Node {
public:
    string name;
    vector<shared_ptr<Node>> children;


    Node(const string& name) : name(name) {}
};

class Derivation
{
private:
    vector<pair<string, int>> lexeme_table;
    int current_index = 0;
    pair<string, int> next_token;
    int ID = 0;
    int OP = 0;
    int CONST = 0;

public:
    pair<string, int> lex();
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
    shared_ptr<Node> error();

    Derivation(const vector<pair<string, int>> &lex_table);

    int getIDCount();
    int getOPCount();
    int getCONSTCount();
    void resetCounts();

    void printTree(const std::shared_ptr<Node>& node, int depth = 0);
};

#endif