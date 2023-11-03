#include "lex.h"

Lex::Lex(string file_name)
{
    filename = file_name;
    next_token = WHITE_SPACE;
    num_state = 0;
    state.const_count= 0;
    state.id = 0;
    state.op = 0;
    state.parsing_output = 0;
    //token = 
}

int Lex::file_read()
{
    char next_char;
    ifstream r_file(filename);
    if (r_file.is_open())
    {
        while (next_token != EOF)
        {
            get_char(r_file);
            lexical(r_file);
        }
        r_file.close();
        return (0);
    }
    else cout << "Unable to open file: " << filename << "\n";
    return (1);
}

void Lex::get_char(ifstream& r_file)
{
    if (!r_file.get(ch))
    {
        next_token = EOF;
        char_class = EOF;
        return ;
    }
    if (isalpha(ch)) // || ch == '_'
        char_class = LETTER;
    else if (isdigit(ch))
        char_class = DIGIT;
    else if (ch >= 0 && ch <= 32)
        char_class = WHITE_SPACE; // 이거 뺄지말지 생각해보기
    else 
        char_class = UNKNOWN;
}

void Lex::lookup()
{
    if (ch == '(')
    {
        next_token = LEFT_PAREN;
    }
    else if (ch == ')')
    {
        next_token = RIGHT_PAREN;
    }
    else if (ch == '+')
    {
        next_token = ADD_OP;
        state.op += 1;
    }
    else if (ch == '-')
    {
        next_token = SUB_OP;
        state.op += 1;
    }
    else if (ch == '*')
    {
        next_token = MULT_OP;
        state.op += 1;
    }
    else if (ch == '/')
    {
        next_token = DIV_OP;
        state.op += 1;
    }
    else if (ch == ':')
        next_token = COLON;
    else if (ch == ';')
    {
        next_token = SEMI_COLON;
        statement.push_back(state);
        state.const_count= 0;
        state.id = 0;
        state.op = 0;
        state.parsing_output = 0;
        num_state += 1;
    }
    // white space 일떄
    else if (ch == EOF)
        next_token = EOF; // 혹은 애러...
}

void Lex::lexical(ifstream& r_file)
{
    switch (char_class)
    {
        case LETTER: // 문자나 숫자 바로 뒤에 특수기호나 이상하게 오면 warning 없이 그냥 씹어버림. 수정하기.
            while (char_class == LETTER || char_class == DIGIT)
            {
                token_string += ch;
                get_char(r_file);
            }
            // _________만 예외처리해주기 (언더바 오는거)
            next_token = IDENT;
            state.id += 1; // 1d개수 증가.
            lexeme_table.push_back(make_pair(token_string, next_token));
            token_string.clear();
            if (char_class == UNKNOWN) // 만약 괄호닫기 처럼 바로 붙어서 오는 어떤 lexeme일 경우, 수정할것!.unknown말고 가능성 있는 것만..
                return lexical(r_file);
            break;
    
        case DIGIT:
            while (char_class == DIGIT)
            {
                token_string += ch;
                get_char(r_file);
            }
            next_token = INT_LIT;
            state.const_count += 1; // const 개수 증가.
            lexeme_table.push_back(make_pair(token_string, next_token));
            token_string.clear();
            if (char_class == UNKNOWN) // 만약 괄호닫기 처럼 바로 붙어서 오는 어떤 lexeme일 경우
                return lexical(r_file);
            else if (char_class == LETTER) // 숫자 뒤에 바로 문자가 올 경우, 문자 삭제 
            {
                // 공백이나 연산자가 올때까지 get_char해서 다 지워버려야 할듯?
                // ex> 13421asad2s -> 13421 atoi함수처럼 처리해야 할듯.
            }
            break;

        case WHITE_SPACE:
            //
            break;

        case EOF:
            //string에 뭐 있으면 저장하고 비우기??
            //if (token_string.size())
            statement.push_back(state);
            state.const_count= 0;
            state.id = 0;
            state.op = 0;
            state.parsing_output = 0;
            break;

        case UNKNOWN:
            lookup();
            if (next_token == COLON)
            {
                token_string += ch;
                get_char(r_file);
                if(ch == '=')
                {
                    token_string += ch;
                    next_token = ASSIGN_OP;
                    lexeme_table.push_back(make_pair(token_string, next_token));
                    token_string.clear();
                }
                else
                {
                    token_string.clear();
                    state.parsing_output = 1;
                    // 에러메시지 저장하는 것도 생각해야 할 듯.
                    // warning! 등호가 오지 않았으므로 :는 올바른 토큰이 아님. -> 삭제
                }
            }
            else if(next_token >= 21 && next_token <= 27)
            {
                token_string += ch;
                lexeme_table.push_back(make_pair(token_string, next_token));
                token_string.clear();
            }
            else
            {
                cout << ch << " 에 해당하는 문자는 들어갈 수 없습니다.\n"; // just 표시. 나중에 수정해놓기
            }
            break;
        default:
            break;
    }
    //b_char_class = char_class;
}

vector<pair<string, int>> Lex::get_vector() const
{
    return lexeme_table;
}

vector<line_ans> Lex::get_statement() const
{
    return statement;
}