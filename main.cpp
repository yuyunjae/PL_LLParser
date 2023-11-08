#include "lex.h" // lexical analyzer
#include "derivation.h" // ll_parser


int main(int argc, char **argv)
{
    string filename; // 파일 이름 저장.
    if (argc != 2) // 파일이 1개만 들어온 게 아니라면,
    {
        cout << "\e[31m" << "Please give just 1 file! ex)./[executable_file] [filename]\e[37m\n"; // 파일이 없거나 2개 이상의 파일이 들어왔을 때
        return (1);
    }
    filename = argv[1]; // 입력 받은 파일 이름을 filename에 저장.
    
    Lex lexemes(filename); // Lex class의 object생성
    lexemes.file_read(); // 파일 읽고 해당 파일에 있는 모든 lexeme들을 lexical analyzer로 쪼개서 token으로 구별해놓기.
    vector<pair<string, int> > token_vector = lexemes.get_vector(); // lexical analyzer의 결과를 <lexeme, token_number>형태의 vector로 받아서 저장.
    /*for (const auto& token : token_vector) {
            cout << "Token Code: " << token.second << ", Lexeme: " << token.first << endl;
    }*/ // lexical analyzer의 결과 vector 출력. (test 용).

    // lex에서 생기는 error와 warning을 <해당 lexeme을 포함한 경고문, statement 번호>로 저장.
    vector<pair<string, int> > statement = lexemes.get_statement();
    /*for (const auto& token : statement) {
            cout << "statement: " << token.second << " Message: " << token.first << endl;
    }
    std::cout << "\n\n";
    */ // lex과정에서의 예외처리의 결과 vector 출력. (test 용).

    Derivation derivation(token_vector, statement); // 파싱 객체 생성
    shared_ptr<Node> root = derivation.programs(); // 구문 트리 생성 -> 스마트 포인터를 이용해 파스트리 구현.
    /*std::cout << "\n\n";
    root->printTree(0);
    */ // 파스 트리 구성 확인. (test 용.) 깊이는 들여쓰기 2칸으로 표현했습니다.
    std::cout << "\n";
    derivation.printSymbolTableAll(); //마지막 결과문 출력 ex) Result ==> operand1: 3; operand2: 6; target: 5
    std::cout << "\n";

    return (0);
}
