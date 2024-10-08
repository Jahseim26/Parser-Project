#include <iostream>
#include <string>
#include <vector>
#include <cctype>

enum TokenType {
    BEGIN, END, ID, NUMBER, ASSIGN, PLUS, SEMICOLON, INVALID, END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
};

class Lexer {
public:
    Lexer(const std::string &input) : input(input), pos(0) {}

    Token getNextToken() {
        while (pos < input.size() && isspace(input[pos])) pos++;  // Skip whitespace

        if (pos >= input.size()) return {END_OF_FILE, ""};

        char currentChar = input[pos];

        if (isalpha(currentChar) || currentChar == '_') {
            return identifier();
        }
        if (isdigit(currentChar)) {
            return number();
        }
        if (currentChar == '=') {
            pos++;
            return {ASSIGN, "="};
        }
        if (currentChar == '+') {
            pos++;
            return {PLUS, "+"};
        }
        if (currentChar == ';') {
            pos++;
            return {SEMICOLON, ";"};
        }
        if (input.substr(pos, 5) == "begin") {
            pos += 5;
            return {BEGIN, "begin"};
        }
        if (input.substr(pos, 3) == "end") {
            pos += 3;
            return {END, "end"};
        }

        // If no valid token is found
        pos++;
        return {INVALID, std::string(1, currentChar)};
    }

private:
    std::string input;
    size_t pos;

    Token identifier() {
        size_t start = pos;
        while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) pos++;
        return {ID, input.substr(start, pos - start)};
    }

    Token number() {
        size_t start = pos;
        while (pos < input.size() && isdigit(input[pos])) pos++;
        return {NUMBER, input.substr(start, pos - start)};
    }
};

// Recursive Descent Parser
class Parser {
public:
    Parser(Lexer &lexer) : lexer(lexer), currentToken(lexer.getNextToken()) {}

    void parse() {
        program();
    }

private:
    Lexer &lexer;
    Token currentToken;

    void program() {
        if (currentToken.type == BEGIN) {
            currentToken = lexer.getNextToken();
            stmt_list();
            if (currentToken.type == END) {
                std::cout << "The program is generated by the grammar\n";
            } else {
                error("Expected 'end'");
            }
        } else {
            error("Expected 'begin'");
        }
    }

    void stmt_list() {
        if (currentToken.type == ID) {
            stmt();
            if (currentToken.type == SEMICOLON) {
                currentToken = lexer.getNextToken();
                stmt_list(); // Continue parsing statement list
            } else {
                error("Expected ';'");
            }
        }
    }

    void stmt() {
        if (currentToken.type == ID) {
            currentToken = lexer.getNextToken();
            if (currentToken.type == ASSIGN) {
                currentToken = lexer.getNextToken();
                expr();
            } else {
                error("Expected '='");
            }
        } else {
            error("Expected identifier");
        }
    }

    void expr() {
        term();
        if (currentToken.type == PLUS) {
            currentToken = lexer.getNextToken();
            expr(); // Recursive expression parsing
        }
    }

    void term() {
        if (currentToken.type == ID || currentToken.type == NUMBER) {
            currentToken = lexer.getNextToken();
        } else {
            error("Expected identifier or number");
        }
    }

    void error(const std::string &message) {
        std::cout << "Syntax Error: " << message << ", found " << currentToken.lexeme << "\n";
    }
};

int main() {
    std::string inputProgram = "begin x = 5 + y; end";
    Lexer lexer(inputProgram);
    Parser parser(lexer);

    parser.parse();
    return 0;
}
