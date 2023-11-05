#include "lex.h"
#include "derivation.h"



Derivation::Derivation(const vector<pair<string, int>>& lex_table) : lexeme_table(lex_table){
    next_token = lex(); // 첫 번째 토큰을 가져옵니다.
    resetCounts();
}

std::pair<std::string, int> Derivation::lex()
{
    if (current_index >= lexeme_table.size()) {
        // 더 이상 읽을 토큰이 없을 경우, EOF를 의미하는 토큰을 반환합니다.
        return make_pair("", EOF);
    } else {
        return lexeme_table[current_index++];
    }
}

std::shared_ptr<Node> Derivation::programs() 
{
    shared_ptr<Node> node = make_shared<Node>("programs");
    node->children.push_back(statements());
    // ID, OP, CONST의 개수를 출력
    std::cout << "ID count: " << getIDCount() << std::endl;
    std::cout << "CONST count: " << getCONSTCount() << std::endl;
    std::cout << "OP count: " << getOPCount() << std::endl;
    return node;
}

std::shared_ptr<Node> Derivation::statements()
{
    shared_ptr<Node> node = make_shared<Node>("statements");
    if(next_token.second != SEMI_COLON && next_token.second != EOF) {
        node->children.push_back(statement());
    }
    if (next_token.second == SEMI_COLON) {
        node->children.push_back(semi_colon());
        node->children.push_back(statements());
    }
    return node;
}

std::shared_ptr<Node> Derivation::statement() 
{
    shared_ptr<Node> node = make_shared<Node>("statement");
    if(next_token.second == IDENT){
        node->children.push_back(ident());
        if(next_token.second == ASSIGN_OP){
            node->children.push_back(assignment_op());
            node->children.push_back(expression());
        }
    }
    return node;
}

std::shared_ptr<Node> Derivation::expression()
{
    shared_ptr<Node>node = make_shared<Node>("expression");
    node->children.push_back(term());
    node->children.push_back(term_tail());
    return node;
}

std::shared_ptr<Node> Derivation::term_tail()  //입실론 처리할 것
{
    shared_ptr<Node> node = make_shared<Node>("term_tail");
    if(next_token.second == ADD_OP){
        node->children.push_back(add_operator());
        node->children.push_back(term());
        if (next_token.second != EOF) { // EOF 조건 추가
            node->children.push_back(term_tail());
        }
    }
    else{
        node->children.push_back(make_shared<Node>("UNKNOWN"));
    }
    return node;
}

std::shared_ptr<Node> Derivation::term()
{
    shared_ptr<Node> node = make_shared<Node>("term");
    node->children.push_back(factor());
    node->children.push_back(factor_tail());
    return node;
}

std::shared_ptr<Node> Derivation::factor_tail() //입실론 처리할 것
{
    shared_ptr<Node> node = make_shared<Node>("factor_tail");
    if(next_token.second == MULT_OP){
        node->children.push_back(mult_operator());
        node->children.push_back(factor());
        if (next_token.second != EOF) { // EOF 조건 추가
            node->children.push_back(factor_tail());
        }
    }
    else{
        node->children.push_back(make_shared<Node>("UNKNOWN"));
        return node;
    }
    return node;
}

std::shared_ptr<Node> Derivation::factor()
{
    shared_ptr<Node> node = make_shared<Node>("factor");
    
    if (next_token.second == IDENT){
        node->children.push_back(ident());
    }
    else if(next_token.second == INT_LIT){
        node->children.push_back(int_lit());
        
    }    
    else if (next_token.second == LEFT_PAREN){
        node->children.push_back(left_paren());
        node->children.push_back(expression());
        
        if(next_token.second == RIGHT_PAREN){
            node->children.push_back(right_paren());
            
        }
        else{
            error();
        }
    }
    else{
        error();
    }
    return node;
}

std::shared_ptr<Node> Derivation::int_lit()
{
    std::shared_ptr<Node> int_lit_node = std::make_shared<Node>("int_lit");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    int_lit_node->children.push_back(std::make_shared<Node>(token.first));
    CONST++;
    current_index--;
    next_token = lex();
    return int_lit_node;
}

std::shared_ptr<Node> Derivation::ident()
{
    std::shared_ptr<Node> ident_node = std::make_shared<Node>("ident");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    ident_node->children.push_back(std::make_shared<Node>(token.first));
    ID++;
    current_index--;
    next_token = lex();
    return ident_node;
}

std::shared_ptr<Node> Derivation::assignment_op()
{
    std::shared_ptr<Node> assignment_op_node = std::make_shared<Node>("assignment_op");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    assignment_op_node->children.push_back(std::make_shared<Node>(":="));
    //이것만 불러와서 저장하는 게 아니라 직접 저장하는 방식으로 수정(:, =이 다른 lexeme이라서)
    current_index--;
    next_token = lex();
    return assignment_op_node;
}


std::shared_ptr<Node> Derivation::semi_colon()
{
    std::shared_ptr<Node> semi_colon_node = std::make_shared<Node>("semi_colon");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    semi_colon_node->children.push_back(std::make_shared<Node>(token.first));
    
    // ID, OP, CONST의 개수를 출력
    std::cout << "ID count: " << getIDCount() << std::endl;
    std::cout << "CONST count: " << getCONSTCount() << std::endl;
    std::cout << "OP count: " << getOPCount() << std::endl;
    resetCounts();

    current_index--;
    next_token = lex();
    return semi_colon_node;
}

std::shared_ptr<Node> Derivation::add_operator()
{
    std::shared_ptr<Node> add_operator_node = std::make_shared<Node>("add_operator");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    add_operator_node->children.push_back(std::make_shared<Node>(token.first));
    OP++;
    current_index--;
    next_token = lex();
    return add_operator_node;
}

std::shared_ptr<Node> Derivation::mult_operator()
{
    std::shared_ptr<Node> mult_operator_node = std::make_shared<Node>("mult_operator");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    mult_operator_node->children.push_back(std::make_shared<Node>(token.first));
    OP++;
    current_index--;
    next_token = lex();
    return mult_operator_node;
}

std::shared_ptr<Node> Derivation::left_paren()
{
    std::shared_ptr<Node> left_paren_node = std::make_shared<Node>("left_paren");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    left_paren_node->children.push_back(std::make_shared<Node>(token.first));
    current_index--;
    next_token = lex();
    return left_paren_node;
}

std::shared_ptr<Node> Derivation::right_paren()
{
    std::shared_ptr<Node> right_paren_node = std::make_shared<Node>("right_paren");
    std::pair<std::string, int> token = lex(); // lex() 함수에서 현재 토큰을 가져옴
    // 가져온 토큰으로 새로운 노드 생성해서 위에서 생성된 노드의 자식노드로 추가
    right_paren_node->children.push_back(std::make_shared<Node>(token.first));
    current_index--;
    next_token = lex();
    return right_paren_node;
}

std::shared_ptr<Node> Derivation::error()
{
    std::cerr << "Parsing Error: Unexpected token " << next_token.first << std::endl;
    std::cerr << "Number of IDENT tokens processed: " << getIDCount() << std::endl;
    std::cerr << "Number of CONST tokens processed: " << getCONSTCount() << std::endl;
    std::cerr << "Number of OP tokens processed: " << getOPCount() << std::endl;
    return nullptr; // 이 부분은 실행되지 않지만, 함수의 반환 타입에 맞추기 위해 추가했습니다.
}

int Derivation::getIDCount(){
    return ID;
}

int Derivation::getOPCount(){
    return OP;
}

int Derivation::getCONSTCount(){
    return CONST;
}

void Derivation::resetCounts() {
    ID = 0;
    OP = 0;
    CONST = 0;
}