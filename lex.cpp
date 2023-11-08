#include "lex.h"

Lex::Lex(string file_name)
{
    filename = file_name;
    next_token = WHITE_SPACE;
    num_state = 1;
    char_class = WHITE_SPACE;
    ch = 0;
}

int Lex::file_read()
{
    ifstream r_file(filename);
    if (r_file.is_open())
    {
        while (next_token != EOF && char_class != EOF)
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
    if (isalpha(ch))
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
    else if (ch == '=') //등호만 올 때
        next_token = ASSIGN_OP;
    else if (ch == ';')
    {
        next_token = SEMI_COLON;
        num_state += 1;
    }
    else if (ch == EOF)
    {
        next_token = EOF;
        char_class = EOF;
    }
    else
    {
        next_token = UNKNOWN;
    }
}

void Lex::lexical(ifstream& r_file)
{
    switch (char_class)
    {
        case LETTER: // ident
            while (char_class == LETTER || char_class == DIGIT || ch == '_')
            {
                token_string += ch;
                get_char(r_file);
            }
            next_token = IDENT;
            lexeme_table.push_back(make_pair(token_string, next_token));
            token_string.clear();
            if (char_class == UNKNOWN) // 예외처리 만약 괄호닫기 처럼 문자 바로 뒤에 바로 붙어서 오는 어떤 연산자일 경우
                return lexical(r_file);
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
            if (char_class == UNKNOWN) // 만약 괄호닫기 처럼 바로 붙어서 오는 어떤 lexeme일 경우
                return lexical(r_file);
            else if (char_class == LETTER) // 숫자 뒤에 바로 문자가 띄어쓰기없이 딱 붙어서 올 경우..
            {
                // 이런 예외도 처리 해야할까..
            }
            break;

        case WHITE_SPACE:
            // 그냥 건너뛰기
            break;

        case EOF:
            num_state += 1;
            // 아직 딱히 처리할 게 없으.
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
                {   // : 만 올떄
                    // warning! 등호가 오지 않았으므로 :는 올바른 토큰이 아님.
                    string s = "(Warning) ':' should always be followed by '='. add '='";
                    token_string += "=";
                    next_token = ASSIGN_OP;
                    lexeme_table.push_back(make_pair(token_string, next_token));
                    statement.push_back(make_pair(s, num_state));
                    token_string.clear();
                    lexical(r_file);
                }
            }
            else if (ch == '=')
            {
                get_char(r_file);
                // warning! 콜론이 오지 않았으므로 '='는 올바른 토큰이 아님.
                string s = "(Warning) '=' must always be preceded by ':'. add ':'";
                token_string += ":=";
                next_token = ASSIGN_OP;
                lexeme_table.push_back(make_pair(token_string, next_token));
                statement.push_back(make_pair(s, num_state));
                token_string.clear();
                lexical(r_file);
            }
            else if(next_token == 22)
            {  
                token_string += ch;
                get_char(r_file);
                if (char_class == DIGIT)
                    return lexical(r_file);
                else
                {
                    lexeme_table.push_back(make_pair(token_string, next_token));
                    token_string.clear();
                    lexical(r_file);
                }
            }
            else if(next_token >= 21 && next_token <= 27)
            {
                token_string += ch;
                lexeme_table.push_back(make_pair(token_string, next_token));
                token_string.clear();
            }
            else if(next_token == UNKNOWN)
            {   // 정의되지 않은 연산자가 들어올때
                string s = "(Warning) \"remove undefined operator: '";
                s += ch;
                s += "'\"";
                statement.push_back(make_pair(s, num_state));
            }
            break;
        default:
            break;
    }
}

vector<pair<string, int>> Lex::get_vector() const
{
    return lexeme_table;
}

vector<pair<string, int>> Lex::get_statement() const
{
    return statement;
}