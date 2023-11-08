#include "lex.h"
#include "derivation.h" //추가


int main(int argc, char **argv)
{
    string filename;
    if (argc != 2)
    {
        cout << "\e[31m" << "Please give just 1 file! ex)./[executable_file] [filename]\e[37m\n";
        return (1);
    }
    filename = argv[1];
    
    Lex lexemes(filename); // 파일 읽고 해당 파일에 있는 모든 lexeme들을 lexical analyzer로 쪼개서 token으로 구별해놓기.
    lexemes.file_read();
    vector<pair<string, int> > token_vector = lexemes.get_vector();
    // for (const auto& token : token_vector) {
    //         cout << "Token Code: " << token.second << ", Lexeme: " << token.first << endl;
    // }

    //lex warning check
    vector<pair<string, int> > statement = lexemes.get_statement();
    // for (const auto& token : statement) {
    //         cout << "statement: " << token.second << " Message: " << token.first << endl;
    // }

    //std::cout << "\n\n";
    Derivation derivation(token_vector, statement); // 파싱 객체 생성 (추가됨) 오류 수정 필요
    shared_ptr<Node> root = derivation.programs(); // 구문 트리 생성 (추가됨)
    //std::cout << "\n\n";
    //root->printTree(0);
    std::cout << "\n";
    derivation.printSymbolTableAll();
    std::cout << "\n";

    return (0);
}
