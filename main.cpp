#include "llparser.h"

int main(int argc, char **argv)
{
    string filename;
    if (argc != 2)
    {
        cout << "Please give just 1 file! ex)./[executable_file] [filename]\n";
        return (1);
    }
    filename = argv[1];
    Lex lexemes(filename); // 파일 읽고 해당 파일에 있는 모든 lexeme들을 lexical analyzer로 쪼개서 token으로 구별해놓기.
    return (0);
}