#include "lex.h"

Lex::Lex(string file_name)
{
    filename = file_name;
    next_token = WHITE_SPACE;
    num_state = 0;
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
    }
    else if (ch == '-')
    {
        next_token = SUB_OP;
    }
    else if (ch == '*')
    {
        next_token = MULT_OP;
    }
    else if (ch == '/')
    {
        next_token = DIV_OP;
    }
    else if (ch == ':')
        next_token = COLON;
    else if (ch == ';')
    {
        next_token = SEMI_COLON;
        num_state += 1;
    }
    // white space 일떄
    else
        next_token = EOF; // 혹은 애러...
}

void Lex::lexical(ifstream& r_file)
{
    get_char(r_file);
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
            lexeme_table.push_back(make_pair(token_string, next_token));
            token_string.clear();
            break;
    
        case DIGIT:
            while (char_class == DIGIT)
            {
                token_string += ch;
                get_char(r_file);
            }
            next_token = INT_LIT;
            lexeme_table.push_back(make_pair(token_string, next_token));
            token_string.clear();
            break;

        case WHITE_SPACE:
            //
            break;

        case EOF:
            //string에 뭐 있으면 저장하고 비우기??
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
            }
            else if(next_token >= 21 && next_token <= 27)
            {
                token_string += ch;
                lexeme_table.push_back(make_pair(token_string, next_token));
                token_string.clear();
            }
            else
                cout << ch << " 에 해당하는 문자는 들어갈 수 없습니다.\n"; // just 표시. 나중에 수정해놓기
            break;
        default:
            break;
    }
    //b_char_class = char_class;
}

vector<pair<string, int>> Lex::get_vector() const {
    return lexeme_table;
}