#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <limits>
#include <memory>
#include <ctime>
#include <cstdlib>

struct VarInfo { bool isMutable; };

enum TokenType {
    TOKEN_ID, TOKEN_NUMBER, TOKEN_STRING, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_EQ, TOKEN_NEQ, TOKEN_GT, TOKEN_LT, TOKEN_AND, TOKEN_OR,
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_TO,
    TOKEN_SWITCH, TOKEN_CASE, TOKEN_COLON,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_COMMA, TOKEN_DOT,
    TOKEN_FN, TOKEN_STRUCT, TOKEN_VAL, TOKEN_VAR, TOKEN_NULL,
    TOKEN_RETURN, TOKEN_BREAK, TOKEN_CONTINUE, TOKEN_THIS, TOKEN_EOF
};

struct Token { TokenType type; std::string value; };

struct ASTNode {
    std::string type; std::string name; std::string stringValue; int numberValue = 0;
    bool isMutable = false;
    ASTNode *left = nullptr; ASTNode *right = nullptr; ASTNode *elseBranch = nullptr;
    std::vector<ASTNode*> statements;
    std::vector<ASTNode*> functions;
    std::vector<ASTNode*> args;
    std::vector<ASTNode*> arrayElements;
    std::vector<std::string> params;
    std::vector<std::pair<std::string, ASTNode*>> mapEntries;
    std::vector<std::string> structFields;
    std::vector<ASTNode*> cases;
    std::vector<ASTNode*> caseBlocks;
    std::string structName;
};

class Lexer {
    std::string source;
    int pos = 0;
public:
    Lexer(std::string src) : source(src) {}
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < source.length()) {
            char c = source[pos];
            if (c == '/' && pos+1 < source.length() && source[pos+1] == '/') {
                while (pos < source.length() && source[pos] != '\n') pos++;
                continue;
            }
            if (isspace(c)) { pos++; continue; }
            if (c == '"') {
                pos++;
                std::string s;
                while (pos < source.length()) {
                    if (source[pos] == '"') break;
                    if (source[pos] == '\\' && pos+1 < source.length()) { s += source[pos++]; s += source[pos++]; continue; }
                    s += source[pos++];
                }
                pos++;
                tokens.push_back({TOKEN_STRING, s});
                continue;
            }
            if (isdigit(c)) {
                std::string n;
                while (pos < source.length() && isdigit(source[pos])) n += source[pos++];
                tokens.push_back({TOKEN_NUMBER, n});
                continue;
            }
            if (isalpha(c) || c == '_') {
                std::string s;
                while (pos < source.length() && (isalnum(source[pos]) || source[pos] == '_')) s += source[pos++];
                if (s == "fn") tokens.push_back({TOKEN_FN, "fn"});
                else if (s == "val") tokens.push_back({TOKEN_VAL, "val"});
                else if (s == "var") tokens.push_back({TOKEN_VAR, "var"});
                else if (s == "return") tokens.push_back({TOKEN_RETURN, "return"});
                else if (s == "break") tokens.push_back({TOKEN_BREAK, "break"});
                else if (s == "continue") tokens.push_back({TOKEN_CONTINUE, "continue"});
                else if (s == "if") tokens.push_back({TOKEN_IF, "if"});
                else if (s == "else") tokens.push_back({TOKEN_ELSE, "else"});
                else if (s == "struct") tokens.push_back({TOKEN_STRUCT, "struct"});
                else if (s == "while") tokens.push_back({TOKEN_WHILE, "while"});
                else if (s == "for") tokens.push_back({TOKEN_FOR, "for"});
                else if (s == "to") tokens.push_back({TOKEN_TO, "to"});
                else if (s == "switch") tokens.push_back({TOKEN_SWITCH, "switch"});
                else if (s == "case") tokens.push_back({TOKEN_CASE, "case"});
                else if (s == "null") tokens.push_back({TOKEN_NULL, "null"});
                else if (s == "this") tokens.push_back({TOKEN_THIS, "this"});
                else tokens.push_back({TOKEN_ID, s});
                continue;
            }
            if (c == ':' && pos+1 < source.length() && source[pos+1] == '=') { tokens.push_back({TOKEN_ASSIGN, ":="}); pos += 2; continue; }
            if (c == ':') { tokens.push_back({TOKEN_COLON, ":"}); pos++; continue; }
            if (c == '=' && pos+1 < source.length() && source[pos+1] == '=') { tokens.push_back({TOKEN_EQ, "=="}); pos += 2; continue; }
            if (c == '!' && pos+1 < source.length() && source[pos+1] == '=') { tokens.push_back({TOKEN_NEQ, "!="}); pos += 2; continue; }
            if (c == '&' && pos+1 < source.length() && source[pos+1] == '&') { tokens.push_back({TOKEN_AND, "&&"}); pos += 2; continue; }
            if (c == '|' && pos+1 < source.length() && source[pos+1] == '|') { tokens.push_back({TOKEN_OR, "||"}); pos += 2; continue; }
            if (c == '<') { tokens.push_back({TOKEN_LT, "<"}); pos++; continue; }
            if (c == '>') { tokens.push_back({TOKEN_GT, ">"}); pos++; continue; }
            if (c == '(') { tokens.push_back({TOKEN_LPAREN, "("}); pos++; continue; }
            if (c == ')') { tokens.push_back({TOKEN_RPAREN, ")"}); pos++; continue; }
            if (c == '[') tokens.push_back({TOKEN_LBRACKET, "["});
            else if (c == ']') tokens.push_back({TOKEN_RBRACKET, "]"});
            else if (c == '{') tokens.push_back({TOKEN_LBRACE, "{"});
            else if (c == '}') tokens.push_back({TOKEN_RBRACE, "}"});
            else if (c == '+') tokens.push_back({TOKEN_PLUS, "+"});
            else if (c == ',') tokens.push_back({TOKEN_COMMA, ","});
            else if (c == '-') tokens.push_back({TOKEN_MINUS, "-"});
            else if (c == '*') tokens.push_back({TOKEN_MUL, "*"});
            else if (c == '/') tokens.push_back({TOKEN_DIV, "/"});
            else if (c == '.') tokens.push_back({TOKEN_DOT, "."});
            pos++;
        }
        tokens.push_back({TOKEN_EOF, ""});
        return tokens;
    }
};

class Parser {
    std::vector<Token> tokens;
    int pos = 0;
public:
    Parser(std::vector<Token> t) : tokens(t) {}
    Token peek() { if (pos >= tokens.size()) return {TOKEN_EOF, ""}; return tokens[pos]; }
    Token consume() { if (pos >= tokens.size()) return {TOKEN_EOF, ""}; return tokens[pos++]; }
    ASTNode* parseExpression();
    ASTNode* parseStatement();
    ASTNode* parseBlock();
    ASTNode* parsePrimary() {
        Token t = consume();
        ASTNode* n = new ASTNode();
        if (t.type == TOKEN_NULL) { n->type = "Null"; return n; }
        if (t.type == TOKEN_NUMBER) { n->type = "Number"; n->numberValue = std::stoi(t.value); return n; }
        if (t.type == TOKEN_STRING) { n->type = "String"; n->stringValue = t.value; return n; }
        if (t.type == TOKEN_LBRACKET) {
            n->type = "Array";
            if (peek().type != TOKEN_RBRACKET) {
                n->arrayElements.push_back(parseExpression());
                while (peek().type == TOKEN_COMMA) { consume(); n->arrayElements.push_back(parseExpression()); }
            }
            consume();
            return n;
        }
        if (t.type == TOKEN_LBRACE) {
            n->type = "Map";
            if (peek().type != TOKEN_RBRACE) {
                while (true) {
                    std::string key = consume().value;
                    consume(); // :
                    ASTNode* val = parseExpression();
                    n->mapEntries.push_back({key, val});
                    if (peek().type == TOKEN_COMMA) consume();
                    else break;
                }
            }
            consume();
            return n;
        }
        if (t.type == TOKEN_LPAREN) { delete n; n = parseExpression(); consume(); return n; }
        if (t.type == TOKEN_ID || t.type == TOKEN_THIS) {
            n->type = "Variable";
            n->name = (t.type == TOKEN_THIS) ? "this" : t.value;
            while (peek().type == TOKEN_DOT) {
                consume();
                Token field = consume();
                if (peek().type == TOKEN_LPAREN) {
                    ASTNode* method = new ASTNode();
                    method->type = "MethodCall";
                    method->name = field.value;
                    method->left = n;
                    consume();
                    if (peek().type != TOKEN_RPAREN) {
                        method->args.push_back(parseExpression());
                        while (peek().type == TOKEN_COMMA) { consume(); method->args.push_back(parseExpression()); }
                    }
                    consume();
                    n = method;
                } else {
                    ASTNode* access = new ASTNode();
                    access->type = "MemberAccess";
                    access->name = field.value;
                    access->left = n;
                    n = access;
                }
            }
            if (peek().type == TOKEN_LBRACKET) {
                consume();
                ASTNode* idx = parseExpression();
                consume();
                ASTNode* access = new ASTNode();
                access->type = "IndexAccess";
                access->left = n;
                access->right = idx;
                n = access;
            }
            if (peek().type == TOKEN_LPAREN && t.type != TOKEN_THIS) {
                consume();
                n->type = "Call";
                n->name = t.value;
                if (peek().type != TOKEN_RPAREN) {
                    n->args.push_back(parseExpression());
                    while (peek().type == TOKEN_COMMA) { consume(); n->args.push_back(parseExpression()); }
                }
                consume();
                return n;
            }
            return n;
        }
        return n;
    }
    ASTNode* parseMultiplicative() {
        ASTNode* l = parsePrimary();
        while (peek().type == TOKEN_MUL || peek().type == TOKEN_DIV) {
            Token op = consume();
            ASTNode* n = new ASTNode();
            n->type = "BinaryOp";
            n->name = op.value;
            n->left = l;
            n->right = parsePrimary();
            l = n;
        }
        return l;
    }
    ASTNode* parseAdditive() {
        ASTNode* l = parseMultiplicative();
        while (peek().type == TOKEN_PLUS || peek().type == TOKEN_MINUS) {
            Token op = consume();
            ASTNode* n = new ASTNode();
            n->type = "BinaryOp";
            n->name = op.value;
            n->left = l;
            n->right = parseMultiplicative();
            l = n;
        }
        return l;
    }
    ASTNode* parseRelational() {
        ASTNode* l = parseAdditive();
        while (peek().type == TOKEN_LT || peek().type == TOKEN_GT) {
            Token op = consume();
            ASTNode* n = new ASTNode();
            n->type = "BinaryOp";
            n->name = op.value;
            n->left = l;
            n->right = parseAdditive();
            l = n;
        }
        return l;
    }
    ASTNode* parseEquality() {
        ASTNode* l = parseRelational();
        while (peek().type == TOKEN_EQ || peek().type == TOKEN_NEQ) {
            Token op = consume();
            ASTNode* n = new ASTNode();
            n->type = "BinaryOp";
            n->name = op.value;
            n->left = l;
            n->right = parseRelational();
            l = n;
        }
        return l;
    }
    ASTNode* parseProgram() {
        ASTNode* p = new ASTNode();
        p->type = "Program";
        while (peek().type != TOKEN_EOF) {
            if (peek().type == TOKEN_STRUCT) {
                consume();
                Token n = consume();
                consume();
                ASTNode* s = new ASTNode();
                s->type = "Struct";
                s->name = n.value;
                while (peek().type != TOKEN_RBRACE) {
                    s->structFields.push_back(consume().value);
                    if (peek().type == TOKEN_COMMA) consume();
                }
                consume();
                p->functions.push_back(s);
            } else if (peek().type == TOKEN_FN) {
                consume();
                Token n = consume();
                ASTNode* f = new ASTNode();
                f->type = "Function";
                if (peek().type == TOKEN_DOT) {
                    consume();
                    Token methodName = consume();
                    f->type = "Method";
                    f->structName = n.value;
                    f->name = methodName.value;
                } else {
                    f->name = n.value;
                }
                consume();
                if (peek().type != TOKEN_RPAREN) {
                    f->params.push_back(consume().value);
                    while (peek().type == TOKEN_COMMA) {
                        consume();
                        f->params.push_back(consume().value);
                    }
                }
                consume();
                f->right = parseBlock();
                p->functions.push_back(f);
            } else pos++;
        }
        return p;
    }
};

ASTNode* Parser::parseExpression() {
    ASTNode* l = parseEquality();
    while (peek().type == TOKEN_AND || peek().type == TOKEN_OR) {
        Token op = consume();
        ASTNode* n = new ASTNode();
        n->type = "BinaryOp";
        n->name = op.value;
        n->left = l;
        n->right = parseEquality();
        l = n;
    }
    return l;
}

ASTNode* Parser::parseStatement() {
    if (peek().type == TOKEN_BREAK) { consume(); ASTNode* n = new ASTNode(); n->type = "Break"; return n; }
    if (peek().type == TOKEN_CONTINUE) { consume(); ASTNode* n = new ASTNode(); n->type = "Continue"; return n; }
    if (peek().type == TOKEN_RETURN) { consume(); ASTNode* n = new ASTNode(); n->type = "Return"; n->left = parseExpression(); return n; }
    if (peek().type == TOKEN_VAL || peek().type == TOKEN_VAR) {
        bool isMut = (consume().type == TOKEN_VAR);
        Token name = consume();
        ASTNode* n = new ASTNode();
        n->type = "Declaration";
        n->name = name.value;
        n->isMutable = isMut;
        if (peek().type != TOKEN_ASSIGN) {
            std::cout << "[MHS ERROR] Expected ':=' after '" << name.value << "'" << std::endl;
            exit(1);
        }
        consume();
        n->left = parseExpression();
        return n;
    }
    if (peek().type == TOKEN_ID && tokens[pos + 1].type == TOKEN_ASSIGN) {
        Token n = consume();
        consume();
        ASTNode* nd = new ASTNode();
        nd->type = "Assignment";
        nd->name = n.value;
        nd->left = parseExpression();
        return nd;
    }
    if (peek().type == TOKEN_ID && tokens[pos + 1].type == TOKEN_LBRACKET) {
        Token name = consume();
        consume();
        ASTNode* index = parseExpression();
        consume();
        if (peek().type == TOKEN_ASSIGN) {
            consume();
            ASTNode* val = parseExpression();
            ASTNode* nd = new ASTNode();
            nd->type = "IndexAssignment";
            nd->name = name.value;
            nd->left = index;
            nd->right = val;
            return nd;
        }
    }
    if (peek().type == TOKEN_IF) {
        consume();
        ASTNode* n = new ASTNode();
        n->type = "If";
        n->left = parseExpression();
        n->right = parseBlock();
        if (peek().type == TOKEN_ELSE) {
            consume();
            n->elseBranch = parseBlock();
        }
        return n;
    }
    if (peek().type == TOKEN_WHILE) {
        consume();
        ASTNode* nd = new ASTNode();
        nd->type = "While";
        nd->left = parseExpression();
        nd->right = parseBlock();
        return nd;
    }
    if (peek().type == TOKEN_FOR) {
        consume();
        Token varName = consume();
        consume(); // :=
        ASTNode* start = parseExpression();
        consume(); // to
        ASTNode* end = parseExpression();
        ASTNode* body = parseBlock();
        ASTNode* n = new ASTNode();
        n->type = "For";
        n->name = varName.value;
        n->left = start;
        n->right = end;
        n->elseBranch = body;
        return n;
    }
    if (peek().type == TOKEN_SWITCH) {
        consume();
        consume(); // (
        ASTNode* cond = parseExpression();
        consume(); // )
        consume(); // {
        ASTNode* sw = new ASTNode();
        sw->type = "Switch";
        sw->left = cond;
        while (peek().type == TOKEN_CASE) {
            consume();
            sw->cases.push_back(parseExpression());
            consume(); // :
            sw->caseBlocks.push_back(parseBlock());
        }
        consume(); // }
        return sw;
    }
    return parseExpression();
}

ASTNode* Parser::parseBlock() {
    consume(); // {
    ASTNode* b = new ASTNode();
    b->type = "Block";
    while (peek().type != TOKEN_RBRACE) b->statements.push_back(parseStatement());
    consume(); // }
    return b;
}

class Compiler {
    std::set<std::string> structNames;
    std::map<std::string, std::vector<std::string>> structDefs;
    std::vector<std::string> methodDispatchers;
    std::string escape_cpp(std::string s) {
        std::string out;
        for (char c : s) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else out += c;
        }
        return out;
    }
public:
    std::string generate(ASTNode* node, std::map<std::string, VarInfo>& scope) {
        if (!node) return "";
        if (node->type == "Break") return "break;";
        if (node->type == "Continue") return "continue;";
        if (node->type == "Null") return "Value()";
        if (node->type == "Number") return "Value(" + std::to_string(node->numberValue) + ")";
        if (node->type == "String") return "Value(std::string(\"" + escape_cpp(node->stringValue) + "\"))";
        if (node->type == "Variable") {
            if (node->name == "this") return "var_this";
            return "var_" + node->name;
        }
        if (node->type == "Array") {
            std::string s = "Value::make_array({";
            for (size_t i = 0; i < node->arrayElements.size(); i++) {
                s += generate(node->arrayElements[i], scope);
                if (i < node->arrayElements.size() - 1) s += ", ";
            }
            s += "})";
            return s;
        }
        if (node->type == "Map") {
            std::string s = "Value::make_map({";
            for (size_t i = 0; i < node->mapEntries.size(); i++) {
                s += "{\"" + node->mapEntries[i].first + "\", " + generate(node->mapEntries[i].second, scope) + "}";
                if (i < node->mapEntries.size() - 1) s += ", ";
            }
            s += "})";
            return s;
        }
        if (node->type == "IndexAccess") return generate(node->left, scope) + ".at(" + generate(node->right, scope) + ")";
        if (node->type == "MemberAccess") return generate(node->left, scope) + ".get_safe(\"" + node->name + "\")";
        if (node->type == "MethodCall") {
            std::string obj = generate(node->left, scope);
            std::string args = "std::vector<Value>{";
            for (size_t i = 0; i < node->args.size(); i++) {
                args += generate(node->args[i], scope);
                if (i < node->args.size() - 1) args += ", ";
            }
            args += "}";
            return "mhs_dispatch_method(" + obj + ", \"" + node->name + "\", " + args + ")";
        }
        if (node->type == "Call") {
            if (node->name == "print") return "std::cout << " + generate(node->args[0], scope) + " << std::endl";
            if (node->name == "read_file") return "Value(std_read_file(" + generate(node->args[0], scope) + ".sVal))";
            if (node->name == "write_file") return "Value(std_write_file(" + generate(node->args[0], scope) + ".sVal, " + generate(node->args[1], scope) + ".sVal))";
            if (node->name == "len") return "Value((int)" + generate(node->args[0], scope) + ".len())";
            if (node->name == "push") {
                return generate(node->args[0], scope) + ".array_push(" + generate(node->args[1], scope) + ")";
            }
            if (node->name == "at") return generate(node->args[0], scope) + ".at(" + generate(node->args[1], scope) + ".iVal)";
            if (node->name == "str_len") return "Value((int)" + generate(node->args[0], scope) + ".sVal.length())";
            if (node->name == "str_at") return "Value(std::string(1, " + generate(node->args[0], scope) + ".sVal[" + generate(node->args[1], scope) + ".iVal]))";
            if (node->name == "random_int") {
                return "Value(std_random(0, " + generate(node->args[0], scope) + ".iVal - 1))";
            }
            if (node->name == "input") {
                if (node->args.empty()) {
                    return "Value(std_input())";
                } else {
                    return "Value(std_input(" + generate(node->args[0], scope) + ".sVal))";
                }
            }
            if (node->name == "to_int") {
                return "Value(std_to_int(" + generate(node->args[0], scope) + ".sVal))";
            }
            if (structNames.count(node->name)) {
                std::string s = "Value::make_struct(\"" + node->name + "\", {";
                for (size_t i = 0; i < node->args.size(); i++) {
                    s += generate(node->args[i], scope);
                    if (i < node->args.size() - 1) s += ", ";
                }
                s += "})";
                return s;
            }
            std::string s = node->name + "(";
            for (size_t i = 0; i < node->args.size(); i++) {
                s += generate(node->args[i], scope);
                if (i < node->args.size() - 1) s += ", ";
            }
            s += ")";
            return s;
        }
        if (node->type == "BinaryOp") {
            std::string l = generate(node->left, scope);
            std::string r = generate(node->right, scope);
            if (node->name == "+") return "(" + l + " + " + r + ")";
            if (node->name == "*") return "(" + l + " * " + r + ")";
            if (node->name == "==") return "(" + l + " == " + r + ")";
            if (node->name == ">") return "(" + l + " > " + r + ")";
            if (node->name == "<") return "(" + l + " < " + r + ")";
            if (node->name == "&&") return "(" + l + " && " + r + ")";
            if (node->name == "||") return "(" + l + " || " + r + ")";
            return "(" + l + " " + node->name + " " + r + ")";
        }
        if (node->type == "If") {
            std::string s = "if ((" + generate(node->left, scope) + ").is_true()) {\n" + generate(node->right, scope) + "}\n";
            if (node->elseBranch) s += "else {\n" + generate(node->elseBranch, scope) + "}\n";
            return s;
        }
        if (node->type == "While") return "while ((" + generate(node->left, scope) + ").is_true()) {\n" + generate(node->right, scope) + "}\n";
        if (node->type == "For") {
            std::string var = "var_" + node->name;
            scope[node->name] = { true };
            return "for (Value " + var + " = " + generate(node->left, scope) + "; (" + var + " < " + generate(node->right, scope) + ").is_true() || (" + var + " == " + generate(node->right, scope) + ").is_true(); " + var + " = " + var + " + Value(1)) {\n" + generate(node->elseBranch, scope) + "}\n";
        }
        if (node->type == "Switch") {
            std::string cond = generate(node->left, scope);
            std::string s = "";
            for (size_t i = 0; i < node->cases.size(); i++) {
                std::string check = "(" + cond + " == " + generate(node->cases[i], scope) + ").is_true()";
                if (i == 0) s += "if (" + check + ") {\n" + generate(node->caseBlocks[i], scope) + "}\n";
                else s += "else if (" + check + ") {\n" + generate(node->caseBlocks[i], scope) + "}\n";
            }
            return s;
        }
        if (node->type == "Return") return "return " + generate(node->left, scope) + ";";
        if (node->type == "Declaration") {
            scope[node->name] = { node->isMutable };
            std::string qualifier = node->isMutable ? "" : "const ";
            return qualifier + "Value var_" + node->name + " = " + generate(node->left, scope) + ";";
        }
        if (node->type == "Assignment") return "var_" + node->name + " = " + generate(node->left, scope) + ";";
        if (node->type == "IndexAssignment") return "var_" + node->name + ".set(" + generate(node->left, scope) + ", " + generate(node->right, scope) + ")";
        if (node->type == "Block") {
            std::string s = "";
            std::map<std::string, VarInfo> bs = scope;
            for (auto st : node->statements) s += generate(st, bs) + ";\n";
            return s;
        }
        if (node->type == "Struct") {
            structNames.insert(node->name);
            structDefs[node->name] = node->structFields;
            return "";
        }
        if (node->type == "Function" || node->type == "Method") {
            std::map<std::string, VarInfo> fs;
            std::string cppName;
            if (node->type == "Method") {
                cppName = node->structName + "_" + node->name;
                methodDispatchers.push_back("if (obj.sVal == \"" + node->structName + "\" && method == \"" + node->name + "\") return " + cppName + "(obj" + (node->params.empty() ? "" : ", ") + "args);");
            } else {
                if (node->name == "main") return "Value mhs_main() {\n" + generate(node->right, fs) + "return Value(0);\n}\n";
                cppName = node->name;
            }
            std::string args = "";
            if (node->type == "Method") {
                args += "Value var_this";
                fs["this"] = { true };
                if (!node->params.empty()) args += ", ";
            }
            for (size_t i = 0; i < node->params.size(); i++) {
                args += "Value var_" + node->params[i];
                fs[node->params[i]] = { true };
                if (i < node->params.size() - 1) args += ", ";
            }
            return "\nValue " + cppName + "(" + args + ") {\n" + generate(node->right, fs) + "}\n";
        }
        if (node->type == "Program") {
            std::string s = "";
            std::map<std::string, VarInfo> e;
            for (auto f : node->functions) {
                if (f->type == "Function" && f->name != "main") {
                    s += "Value " + f->name + "(";
                    for (size_t i = 0; i < f->params.size(); i++) {
                        s += "Value";
                        if (i < f->params.size() - 1) s += ", ";
                    }
                    s += ");\n";
                }
            }
            for (auto f : node->functions) s += generate(f, e) + "\n";
            s += "Value mhs_dispatch_method(Value obj, std::string method, std::vector<Value> args) {\n";
            for (auto& disp : methodDispatchers) s += disp + "\n";
            s += "std::cerr << \"[PANIC] Method not found\" << std::endl; exit(1);\n";
            s += "return Value();\n}\n";
            s += "Value Value::make_struct(std::string name, std::vector<Value> args) {\n";
            s += "Value v; v.type = 3; v.sVal = name; v.members = std::make_shared<std::map<std::string, Value>>();\n";
            for (auto const& [name, fields] : structDefs) {
                s += "if (name == \"" + name + "\") { ";
                for (size_t i = 0; i < fields.size(); i++) s += "(*v.members)[\"" + fields[i] + "\"] = args[" + std::to_string(i) + "]; ";
                s += "}\n";
            }
            s += "return v;\n}\n";
            return s;
        }
        return "";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: mhs_compiler <file.mhs>\n";
        return 1;
    }
    std::ifstream f(argv[1]);
    std::stringstream buffer;
    buffer << f.rdbuf();
    Lexer l(buffer.str());
    Parser p(l.tokenize());
    Compiler c;
    std::ofstream out("output.cpp");
    out << "#include <iostream>\n#include <fstream>\n#include <sstream>\n#include <string>\n#include <vector>\n#include <map>\n#include <memory>\n#include <limits>\n#include <ctime>\n#include <cstdlib>\n";
    out << "struct Value;\n";
    out << "Value mhs_dispatch_method(Value, std::string, std::vector<Value>);\n";
    out << "struct Value {\n";
    out << " int type = 0; long long iVal = 0; std::string sVal;\n";
    out << " std::shared_ptr<std::map<std::string, Value>> members;\n";
    out << " std::shared_ptr<std::vector<Value>> arrayVals;\n";
    out << " Value() : type(0) {}\n";
    out << " Value(int i) : type(1), iVal(i) {}\n";
    out << " Value(long long i) : type(1), iVal(i) {}\n";
    out << " Value(std::string s) : type(2), sVal(s) {}\n";
    out << " static Value make_array(std::vector<Value> elems) { Value v; v.type = 4; v.arrayVals = std::make_shared<std::vector<Value>>(elems); return v; }\n";
    out << " static Value make_map(std::map<std::string, Value> elems) { Value v; v.type = 5; v.members = std::make_shared<std::map<std::string, Value>>(elems); return v; }\n";
    out << " int len() const { if(type==4) return arrayVals->size(); if(type==5) return members->size(); return 0; }\n";
    out << " void array_push(Value v) const { if(type==4) arrayVals->push_back(v); }\n";
    out << " Value at(Value idx) const {\n";
    out << " if(type==4) { int i = idx.iVal; if(i < 0 || i >= arrayVals->size()) { std::cerr << \"[PANIC] Index out of bounds\" << std::endl; exit(1); } return arrayVals->at(i); }\n";
    out << " if(type==5) { std::string k = idx.sVal; if(members->find(k) == members->end()) return Value(); return members->at(k); }\n";
    out << " return Value();\n";
    out << " }\n";
    out << " void set(Value idx, Value val) const { if(type==4) (*arrayVals)[idx.iVal] = val; if(type==5) (*members)[idx.sVal] = val; }\n";
    out << " static Value make_struct(std::string name, std::vector<Value> args);\n";
    out << " Value get_safe(std::string name) const { if (members && members->count(name)) return members->at(name); return Value(); }\n";
    out << " bool is_true() const { return (type==1 && iVal!=0) || (type==2 && !sVal.empty()); }\n";
    out << " friend std::ostream& operator<<(std::ostream& os, const Value& v) {\n";
    out << " if(v.type==0) os << \"null\";\n";
    out << " else if(v.type==1) os << v.iVal;\n";
    out << " else if(v.type==2) os << v.sVal;\n";
    out << " else if(v.type==4) { os << \"[\"; for(size_t i=0; i<v.arrayVals->size(); i++) { os << (*v.arrayVals)[i]; if(i<v.arrayVals->size()-1) os << \", \"; } os << \"]\"; }\n";
    out << " else if(v.type==5) { os << \"{\"; int c=0; for(auto const& p : *v.members) { os << p.first << \": \" << p.second; if(c++ < v.members->size()-1) os << \", \"; } os << \"}\"; }\n";
    out << " else os << \"[Object]\";\n";
    out << " return os;\n";
    out << " }\n";
    out << " Value operator+(const Value& o) const { return Value(to_string() + o.to_string()); }\n";
    out << " Value operator*(const Value& o) const { long long res = iVal * o.iVal; if (res > std::numeric_limits<int>::max() || res < std::numeric_limits<int>::min()) { std::cerr << \"[PANIC] Overflow\" << std::endl; exit(1); } return Value((int)res); }\n";
    out << " Value operator-(const Value& o) const { return Value((int)(iVal - o.iVal)); }\n";
    out << " Value operator>(const Value& o) const { return Value((int)(iVal > o.iVal)); }\n";
    out << " Value operator<(const Value& o) const { return Value((int)(iVal < o.iVal)); }\n";
    out << " Value operator==(const Value& o) const { if(type!=o.type) return Value(0); if(type==1) return Value((int)(iVal==o.iVal)); return Value((int)(sVal==o.sVal)); }\n";
    out << " Value operator!=(const Value& o) const { return Value((int)!((*this == o).is_true())); }\n";  // ← FINAL WORKING FIX
    out << " Value operator&&(const Value& o) const { return Value((int)(iVal && o.iVal)); }\n";
    out << " std::string to_string() const { std::stringstream ss; ss << *this; return ss.str(); }\n";
    out << "};\n";
    out << "Value mhs_main();\n";
    out << "int main() { mhs_main(); return 0; }\n";

    // RUNTIME FUNCTIONS
    out << "int std_random(int min, int max) { static bool init = false; if(!init){srand(time(0)); init=true;} return min + rand() % (max - min + 1); }\n";
    out << "std::string std_input() { std::string s; std::getline(std::cin, s); return s; }\n";
    out << "std::string std_input(std::string prompt) { std::cout << prompt; std::string s; std::getline(std::cin, s); return s; }\n";
    out << "long long std_to_int(std::string s) { try { return std::stoll(s); } catch (...) { std::cerr << \"[PANIC] Invalid number: \\\"\" << s << \"\\\"\" << std::endl; exit(1); } }\n";

    std::map<std::string, VarInfo> empty;
    out << c.generate(p.parseProgram(), empty);
    out.close();
    return 0;
}
