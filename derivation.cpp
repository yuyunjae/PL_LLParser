#include "lex.h"
#include "derivation.h"

void Node::printTree(int level) const
{
    std::string indent(level * 2, ' '); // 각 레벨에 대해 2칸씩 들여쓰기를 합니다.
    std::cout << indent << this->name << std::endl;
    // if (!this->is_unknown)
    //     std::cout << indent << this->num << " " <<this->pos_neg << "\n";

    for (auto& child : this->children)
    {
        child->printTree(level + 1); // 자식 노드에 대해 재귀적으로 함수를 호출합니다.
    }
}

void Derivation::printStatement(const shared_ptr<Node>& node)
{
    if (node->name != "statements" && node->name != "statement" && node->name != "expression" &&
    node->name != "term_tail" && node->name != "term" && node->name != "factor_tail"
    && node->name != "factor" && node->name != "assignment_op" && node->name != "ident" && node->name != "int_lit" 
    && node->name != "semi_colon" && node->name != "add_operator" && node->name != "mult_operator" 
    && node->name != "left_paren" && node->name != "right_paren")
        std::cout << "\e[37m" << node->name << " ";
    for (auto& child : node->children)
    {
        printStatement(child); // 자식 노드에 대해 재귀적으로 함수를 호출합니다.
    }
    if (node->name == "statements")
        cout << "\e[37m\n";
}

Derivation::Derivation(const vector<pair<string, int>>& lex_table, vector<pair<string, int>>& statement) : lexeme_table(lex_table), statement(statement){
    next_token = lex(); // 첫 번째 토큰을 가져옵니다.
    resetCounts();
}

void Derivation::errorMessage(){
    statementCount++;
    for (const auto& token : statement){
        if(token.second == statementCount){
            std::cout << token.first << endl;
        }
    }
    for(int i=0; i<ERROR_CASE_MAX_COUNT;i++){
        if(errorCase[i] != 0){
            int error = errorCase[i];
            switch(error){
                case 1:
                    std::cout << "\e[31;1m(ERROR) \"There are undefined operand (" << errorLexeme[i] << error << ") was referenced\"\e[31;0m\e[37m" << std::endl;
                    break;
                case 2:
                    std::cout << "\e[31m(Warning) \"There are duplicated operator, so Remove(" << errorLexeme[i]  << ") operator \"\e[37m" << std::endl;
                    break;
            }   
        }
        errorCase[i] = 0;
        
    }
    if(errorCount == 0){
        std::cout << "\e[34m(OK)\e[37m"<< std::endl;
    }
    errorCount = 0;
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
    if (getIDCount() || getCONSTCount() || getOPCount())
    {
        std::cout << "ID: " << getIDCount() << "; ";
        std::cout << "CONST: " << getCONSTCount() << "; ";
        std::cout << "OP: " << getOPCount() << std::endl;
        errorMessage();
    }
    return node;
}

std::shared_ptr<Node> Derivation::statements()
{
    shared_ptr<Node> node = make_shared<Node>("statements");
    if(next_token.second != SEMI_COLON && next_token.second != EOF) {
        node->children.push_back(getstatement());
    }
    
    // symbol table에서 LHS값 계산해놓기.
    if (next_token.second == SEMI_COLON) {
        node->children.push_back(semi_colon());
        if (node->children.size() == 2)
        {
            // 세미콜론 출력 o
            cout << "\n";
            printStatement(node);
            // ID, OP, CONST의 개수를 출력
           if (getIDCount() || getCONSTCount() || getOPCount())
            {
                std::cout << "ID: " << getIDCount() << "; ";
                std::cout << "CONST: " << getCONSTCount() << "; ";
                std::cout << "OP: " << getOPCount() << std::endl;
                errorMessage();
            }
            resetCounts();
        }
        node->children.push_back(statements());
    }
    else if(node->children.size() == 1)
    {
        // 세미콜론 출력 x
        cout << "\n";
        printStatement(node);
    }
    
    return node;
}

std::shared_ptr<Node> Derivation::getstatement() 
{
    shared_ptr<Node> node = make_shared<Node>("statement");
    if(next_token.second == IDENT){
        node->children.push_back(ident());
        if(next_token.second == ASSIGN_OP){
            node->children.push_back(assignment_op());
            node->children.push_back(expression());
        }
        else{
            // := 가 없는 배정문? ㅋㅋㅋㅋ 이걸 어떻게 에러처리해..
        }

        // 잘 들어 왔다면,
        for (auto iter= symbol_table.begin(); iter < symbol_table.end(); iter++)
        {
            if (iter->name == node->children[0]->children[0]->name) // LHS와 이름이 같은 symbol 찾기
            {
                if (node->children.size() == 3 && !node->children[1]->is_unknown && !node->children[2]->is_unknown)
                {
                    node->is_unknown = 0;
                    node->num = node->children[2]->num;
                    iter->is_unknown = 0;
                    iter->num = node->children[2]->num;
                }
                else
                {
                    node->is_unknown = 1;
                    node->num = 0;
                    iter->is_unknown = 1;
                    iter->num = 0;
                }
                break ;
            }
        }
    }
    return node;
}

std::shared_ptr<Node> Derivation::expression()
{
    shared_ptr<Node>node = make_shared<Node>("expression");
    node->children.push_back(term());
    node->children.push_back(term_tail());

    // 파싱이 잘 되었다면..
    if (!node->children[0]->is_unknown && !node->children[1]->is_unknown) // term과 term_tall에 Unknown이 없음.
    {
        node->is_unknown = 0;
        if (node->children[0]->name == "term") // if문은 그저 확인용
            node->num = node->children[0]->num;
        if (node->children[1]->name == "term_tail") // if문은 그저 확인용
        { // 입실론일 경우 무시
            if (node->children[1]->pos_neg == 0) // 덧셈
                node->num += node->children[1]->num;
            else if (node->children[1]->pos_neg == 1) // 뺼셈
                node->num -= node->children[1]->num;
        }
    }

    return node;
}

std::shared_ptr<Node> Derivation::term_tail()  //입실론 처리할 것
{
    shared_ptr<Node> node = make_shared<Node>("term_tail");
    if(next_token.second == ADD_OP || next_token.second == SUB_OP){ // 덧셈 or 뺄셈
        node->children.push_back(add_operator());
        while (next_token.second == ADD_OP || next_token.second == SUB_OP || next_token.second == MULT_OP || next_token.second == DIV_OP){
            errorCase[errorCount] = 2;
            error();
        }
        if(next_token.second != ADD_OP && next_token.second != SUB_OP && next_token.second != MULT_OP && next_token.second != DIV_OP){
            node->children.push_back(term());
            if (next_token.second != EOF) { // EOF 조건 추가
                node->children.push_back(term_tail());
            }

            //node->pos_neg mult_operator가 -인지 +인지 저장.
            if (!node->children[0]->is_unknown)
                node->pos_neg = node->children[0]->pos_neg;

            for (unsigned int i=1; i < node->children.size(); i++)
            {
                if (!node->children[i]->is_unknown)
                {
                    if (node->children[i]->name == "term"){
                        node->is_unknown = 0;
                        node->num = node->children[i]->num;
                    }
                    else if (node->children[i]->name == "term_tail")
                    {// 입실론의 경우 아무것도 곱/나누지 않음.
                        if (node->children[i]->pos_neg == 0) // 덧셈
                            node->num += node->children[i]->num;
                        else if (node->children[i]->pos_neg == 1) // 뺄셈
                            node->num -= node->children[i]->num;
                    }
                }
                else
                {
                    node->is_unknown = 1; // children node에 unknown이 존재.
                    break ;
                }
            }
        }
    }
    else{ //입실론일떄 ? 아님 예외처리?
        node->is_unknown = 0; // 입실론 일 때
        node->pos_neg = -1;
        node->num = 0;
    }
    return node;
}

std::shared_ptr<Node> Derivation::term()
{
    shared_ptr<Node> node = make_shared<Node>("term");
    node->children.push_back(factor());
    node->children.push_back(factor_tail());

    // 파싱이 잘 되었다면..
    if (!node->children[0]->is_unknown && !node->children[1]->is_unknown) // factor와 factor_tall에 Unknown이 없음.
    {
        node->is_unknown = 0;
        if (node->children[0]->name == "factor") // if문은 그저 확인용
            node->num = node->children[0]->num;
        if (node->children[1]->name == "factor_tail") // if문은 그저 확인용
        { // 입실론일 경우 무시
            if (node->children[1]->pos_neg == 0) //곱하기
                node->num *= node->children[1]->num;
            else if (node->children[1]->pos_neg == 1) //나누기
                node->num /= node->children[1]->num;
        }
    }

    return node;
}

std::shared_ptr<Node> Derivation::factor_tail() //입실론 처리할 것
{
    shared_ptr<Node> node = make_shared<Node>("factor_tail");
    if(next_token.second == MULT_OP || next_token.second == DIV_OP){ //나눗셈 or 곱셈
        node->children.push_back(mult_operator());
        while (next_token.second == ADD_OP || next_token.second == SUB_OP || next_token.second == MULT_OP || next_token.second == DIV_OP){
            errorCase[errorCount] = 2;
            error();
        }
        if (next_token.second != ADD_OP && next_token.second != SUB_OP && next_token.second != MULT_OP && next_token.second != DIV_OP)
            node->children.push_back(factor());
        if (next_token.second != EOF) { // EOF 조건 추가
            node->children.push_back(factor_tail());
        }

        //node->pos_neg mult_operator가 /인지 *인지 저장.
        if (!node->children[0]->is_unknown)
            node->pos_neg = node->children[0]->pos_neg;

        for (unsigned int i=1; i < node->children.size(); i++)
        {
            if (!node->children[i]->is_unknown)
            {
                if (node->children[i]->name == "factor"){
                    node->is_unknown = 0;
                    node->num = node->children[i]->num;
                }
                else if (node->children[i]->name == "factor_tail")
                {// 입실론의 경우 아무것도 곱/나누지 않음.
                    if (node->children[i]->pos_neg == 0) // 곱셈
                        node->num *= node->children[i]->num;
                    else if (node->children[i]->pos_neg == 1) // 나눗셈
                        node->num /= node->children[i]->num;
                }
            }
            else
            {
                node->is_unknown = 1; // children node에 unknown이 존재.
                break ;
            }
        }

    }
    else{ //입실론 일 때? 아님 예외처리?
        node->is_unknown = 0; // 입실론 일 때
        node->pos_neg = -1;
        node->num = 0;
        return node;
    }
    return node;
}

std::shared_ptr<Node> Derivation::factor()
{
    shared_ptr<Node> node = make_shared<Node>("factor");
    
    if (next_token.second == IDENT){
        node->children.push_back(ident());

        if (!node->children[0]->is_unknown) // ident가 unknown이 아니면 factor의 값도 같게 해줌.
        {
            node->num = node->children[0]->num;
            node->is_unknown = node->children[0]->is_unknown;
            //node->pos_neg = 99;
        }
    }
    else if(next_token.second == INT_LIT){
        node->children.push_back(int_lit());

        if (!node->children[0]->is_unknown) // int_lit가 unknown이 아니면 factor의 값도 같게 해줌.
        {
            node->num = node->children[0]->num;
            node->is_unknown = node->children[0]->is_unknown;
            //node->pos_neg = 99;
        }
    }    
    else if (next_token.second == LEFT_PAREN){
        node->children.push_back(left_paren());
        node->children.push_back(expression());      
        node->children.push_back(right_paren());

        if (!node->children[1]->is_unknown) // expreesion이 unknown이 아니면 factor의 값도 같게 해줌.
        {
            node->num = node->children[1]->num; // '(' 가 당연히 에러없이 들어온다는 가정하에 1로 함.
            node->is_unknown = node->children[1]->is_unknown;
            //node->pos_neg = 99;
        }
    }

    return node;
}

std::shared_ptr<Node> Derivation::int_lit()
{
    std::shared_ptr<Node> int_lit_node = std::make_shared<Node>("int_lit");
    int_lit_node->children.push_back(std::make_shared<Node>(next_token.first));
    CONST++;
    // next_token.first에 대한 예외처리가 끝나면
    int_lit_node->num = atoi(next_token.first.c_str());
    int_lit_node->is_unknown = 0;

    next_token = lex();
    return int_lit_node;
}

std::shared_ptr<Node> Derivation::ident()
{
    std::shared_ptr<Node> ident_node = std::make_shared<Node>("ident");
    ident_node->children.push_back(std::make_shared<Node>(next_token.first));
    // 예외 처리가 끝나면,
    if (ID == 0) //LHS의 ident
    {
        int check = 0;
        for (auto iter = symbol_table.begin(); iter!= symbol_table.end(); iter++)
        {
            if(iter->name == next_token.first) // 이미 존재하는 ident
                check = 1;
        }
        if (!check) // 존재하지 않는 ident이므로 symbol table에 추가
        {
            idents id; //-> symbol table에 새로운 ident생성.

            id.is_unknown = 1;
            id.name = next_token.first;
            id.num = 0;
            symbol_table.push_back(id);
        }
    }
    else // RHS에 나오는 ident
    {
        int check_error = 1;
        for (auto iter = symbol_table.begin(); iter!= symbol_table.end(); iter++) {
            if(iter->name == next_token.first)
            {
                if (!(iter->is_unknown))
                {
                    check_error = 0;
                    ident_node->num = (*iter).num;
                    ident_node->is_unknown = 0;
                }
                else
                {   // 전 배정문에서 unknown인 것이 이번 배정문에도 사용됨.
                    check_error = 0;
                    ident_node->is_unknown = 1;
                    ident_node->num = 0;
                }
            }
        }
        if (check_error) // RHS임에도 symbol table에 없는 ident
        {
            ident_node->is_unknown = 1;
            // 값이 정의가 되어 있지 않는 ident 에러
            errorCase[errorCount] = 1;
            errorLexeme[errorCount] = next_token.first;
            errorCount++;

            idents id; //-> symbol table에 새로운 ident생성.

            id.is_unknown = 1;
            id.name = next_token.first;
            id.num = 0;
            symbol_table.push_back(id); // 새로 만들어는 주기.. 
        }
    }

    ID++;
    next_token = lex();
    return ident_node;
}

std::shared_ptr<Node> Derivation::assignment_op()
{
    std::shared_ptr<Node> assignment_op_node = std::make_shared<Node>("assignment_op");
    assignment_op_node->children.push_back(std::make_shared<Node>(":="));
    //이것만 불러와서 저장하는 게 아니라 직접 저장하는 방식으로 수정(:, =이 다른 lexeme으로 해석되지 않게)
    // := 가 잘 들어왔다면,
    assignment_op_node->is_unknown = 0;
    assignment_op_node->num = 0;
    next_token = lex();
    return assignment_op_node;
}


std::shared_ptr<Node> Derivation::semi_colon()
{
    std::shared_ptr<Node> semi_colon_node = std::make_shared<Node>("semi_colon");
    semi_colon_node->children.push_back(std::make_shared<Node>(next_token.first));

    next_token = lex();
    return semi_colon_node;
}

std::shared_ptr<Node> Derivation::add_operator()
{
    std::shared_ptr<Node> add_operator_node = std::make_shared<Node>("add_operator");
    add_operator_node->children.push_back(std::make_shared<Node>(next_token.first));

    // 예외처리 가정.
    add_operator_node->is_unknown = 0;
    add_operator_node->num = 0;
    if (next_token.second == 21) // 더하기
        add_operator_node->pos_neg = 0;
    else if (next_token.second == 22) // 빼기
        add_operator_node->pos_neg = 1;

    OP++;
    next_token = lex();

    return add_operator_node;
}

std::shared_ptr<Node> Derivation::mult_operator()
{
    std::shared_ptr<Node> mult_operator_node = std::make_shared<Node>("mult_operator");
    mult_operator_node->children.push_back(std::make_shared<Node>(next_token.first));

    // 예외처리 가정.
    mult_operator_node->is_unknown = 0;
    mult_operator_node->num = 0;
    if (next_token.second == 23) // 곱하기
        mult_operator_node->pos_neg = 0;
    else if (next_token.second == 24) // 나누기
        mult_operator_node->pos_neg = 1;

    OP++;
    next_token = lex();

    return mult_operator_node;
}

std::shared_ptr<Node> Derivation::left_paren()
{
    std::shared_ptr<Node> left_paren_node = std::make_shared<Node>("left_paren");
    left_paren_node->children.push_back(std::make_shared<Node>(next_token.first));
    next_token = lex();
    return left_paren_node;
}

std::shared_ptr<Node> Derivation::right_paren()
{
    std::shared_ptr<Node> right_paren_node = std::make_shared<Node>("right_paren");
    right_paren_node->children.push_back(std::make_shared<Node>(next_token.first));
    next_token = lex();
    return right_paren_node;
}

void Derivation::error()
{
    errorLexeme[errorCount] = next_token.first;
    next_token = lex();
    errorCount++;
    
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
    IS_KNOWN = 1;
}

void Derivation::printSymbolTableAll()
{
    std::cout << "\e[37;1mResult ==> ";
    for (auto iter = symbol_table.begin(); iter < symbol_table.end() - 1; iter++)
    {
        std::cout << iter->name << ": ";
        if (!iter->is_unknown)
            std::cout << iter->num << "; ";
        else
            std::cout << "Unknown; ";
    }
    std::cout << (symbol_table.end() - 1)->name << ": ";
        if (!(symbol_table.end() - 1)->is_unknown)
            std::cout << (symbol_table.end() - 1)->num;
        else
            std::cout << "UnKnown\e[37;0m";
}