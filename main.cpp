#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <cctype>
#include <cmath>

using namespace std;

enum TokenType { NUMBER, VARIABLE, OPERATOR, LPAREN, RPAREN };

struct Token {
    TokenType type;
    string    value;
};

bool isOpenBracket (const string& s) { return s=="(" || s=="[" || s=="{"; }
bool isCloseBracket(const string& s) { return s==")" || s=="]" || s=="}"; }
bool isMatchingPair(const string& open, const string& close) {
    return (open=="(" && close==")") ||
           (open=="[" && close=="]") ||
           (open=="{" && close=="}");
}
int precedence(const string& op) {
    if (op=="*" || op=="/") return 2;
    if (op=="+" || op=="-") return 1;
    return 0;
}
bool isOperand(const Token& t) { return t.type==NUMBER || t.type==VARIABLE; }

vector<Token> tokenize(const string& expr) {
    vector<Token> tokens;
    size_t i = 0;
    while (i < expr.size()) {
        if (isspace((unsigned char)expr[i])) { i++; continue; }
        if (isdigit((unsigned char)expr[i])) {
            string num;
            while (i < expr.size() && isdigit((unsigned char)expr[i])) num += expr[i++];
            tokens.push_back({NUMBER, num});
            continue;
        }
        if (isalpha((unsigned char)expr[i]) || expr[i]=='_') {
            string id;
            while (i < expr.size() && (isalnum((unsigned char)expr[i]) || expr[i]=='_'))
                id += expr[i++];
            tokens.push_back({VARIABLE, id});
            continue;
        }
        if (expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/') {
            tokens.push_back({OPERATOR, string(1, expr[i++])});
            continue;
        }
        string br(1, expr[i]);
        if (isOpenBracket(br))  { tokens.push_back({LPAREN, br}); i++; continue; }
        if (isCloseBracket(br)) { tokens.push_back({RPAREN, br}); i++; continue; }
        cerr << "Syntax error: unexpected character '" << expr[i] << "'\n";
        exit(1);
    }
    return tokens;
}

void validateSyntax(const vector<Token>& t) {
    if (t.empty()) { cerr << "Syntax error: empty expression\n"; exit(1); }

    if (t.front().type == OPERATOR) {
        cerr << "Syntax error: expression starts with operator '" << t.front().value << "'\n";
        exit(1);
    }
    if (t.front().type == RPAREN) {
        cerr << "Syntax error: expression starts with closing bracket '" << t.front().value << "'\n";
        exit(1);
    }
    if (t.back().type == OPERATOR) {
        cerr << "Syntax error: expression ends with operator '" << t.back().value << "'\n";
        exit(1);
    }
    if (t.back().type == LPAREN) {
        cerr << "Syntax error: expression ends with opening bracket '" << t.back().value << "'\n";
        exit(1);
    }

    for (size_t i = 0; i+1 < t.size(); i++) {
        const Token& cur = t[i];
        const Token& nxt = t[i+1];

        bool curIsOperand = isOperand(cur);
        bool nxtIsOperand = isOperand(nxt);
        bool curIsOp      = (cur.type == OPERATOR);
        bool nxtIsOp      = (nxt.type == OPERATOR);
        bool curIsOpen    = (cur.type == LPAREN);
        bool curIsClose   = (cur.type == RPAREN);
        bool nxtIsOpen    = (nxt.type == LPAREN);
        bool nxtIsClose   = (nxt.type == RPAREN);

        if (curIsOp && nxtIsOp) {
            cerr << "Syntax error: consecutive operators '" << cur.value << "' and '" << nxt.value << "'\n";
            exit(1);
        }
        if (curIsOp && nxtIsClose) {
            cerr << "Syntax error: operator '" << cur.value << "' before closing bracket '" << nxt.value << "'\n";
            exit(1);
        }
        if (curIsOpen && nxtIsOp) {
            cerr << "Syntax error: operator '" << nxt.value << "' after opening bracket '" << cur.value << "'\n";
            exit(1);
        }
        if (curIsOperand && nxtIsOperand) {
            cerr << "Syntax error: missing operator between '" << cur.value << "' and '" << nxt.value << "'\n";
            exit(1);
        }
        if (curIsOperand && nxtIsOpen) {
            cerr << "Syntax error: missing operator before '" << nxt.value << "'\n";
            exit(1);
        }
        if (curIsClose && nxtIsOperand) {
            cerr << "Syntax error: missing operator after '" << cur.value << "'\n";
            exit(1);
        }
        if (curIsClose && nxtIsOpen) {
            cerr << "Syntax error: missing operator between '" << cur.value << "' and '" << nxt.value << "'\n";
            exit(1);
        }
        if (curIsOpen && nxtIsClose) {
            cerr << "Syntax error: empty brackets '" << cur.value << nxt.value << "'\n";
            exit(1);
        }
    }
}

vector<Token> toPostfix(const vector<Token>& tokens) {
    vector<Token> output;
    stack<Token>  ops;
    for (const Token& tok : tokens) {
        if (isOperand(tok)) {
            output.push_back(tok);
        } else if (tok.type == OPERATOR) {
            while (!ops.empty() && ops.top().type==OPERATOR &&
                   precedence(ops.top().value) >= precedence(tok.value)) {
                output.push_back(ops.top()); ops.pop();
            }
            ops.push(tok);
        } else if (tok.type == LPAREN) {
            ops.push(tok);
        } else if (tok.type == RPAREN) {
            bool matched = false;
            while (!ops.empty()) {
                Token top = ops.top(); ops.pop();
                if (top.type == LPAREN) {
                    if (!isMatchingPair(top.value, tok.value)) {
                        cerr << "Syntax error: mismatched brackets '" << top.value << "' and '" << tok.value << "'\n";
                        exit(1);
                    }
                    matched = true; break;
                }
                output.push_back(top);
            }
            if (!matched) {
                cerr << "Syntax error: unmatched closing bracket '" << tok.value << "'\n";
                exit(1);
            }
        }
    }
    while (!ops.empty()) {
        Token top = ops.top(); ops.pop();
        if (top.type == LPAREN) {
            cerr << "Syntax error: unmatched opening bracket '" << top.value << "'\n";
            exit(1);
        }
        output.push_back(top);
    }
    return output;
}

vector<string> collectVariables(const vector<Token>& tokens) {
    vector<string> vars;
    for (const Token& t : tokens) {
        if (t.type == VARIABLE) {
            bool found = false;
            for (const string& v : vars) if (v==t.value) { found=true; break; }
            if (!found) vars.push_back(t.value);
        }
    }
    return vars;
}

double evaluate(const vector<Token>& postfix, const map<string,double>& varValues) {
    stack<double> stk;
    for (const Token& tok : postfix) {
        if (tok.type == NUMBER) {
            stk.push(stod(tok.value));
        } else if (tok.type == VARIABLE) {
            auto it = varValues.find(tok.value);
            if (it == varValues.end()) {
                cerr << "Runtime error: undefined variable '" << tok.value << "'\n";
                exit(2);
            }
            stk.push(it->second);
        } else if (tok.type == OPERATOR) {
            if (stk.size() < 2) {
                cerr << "Runtime error: insufficient operands for '" << tok.value << "'\n";
                exit(2);
            }
            double b = stk.top(); stk.pop();
            double a = stk.top(); stk.pop();
            if      (tok.value=="+") stk.push(a+b);
            else if (tok.value=="-") stk.push(a-b);
            else if (tok.value=="*") stk.push(a*b);
            else if (tok.value=="/") {
                if (abs(b) < 1e-9) { cerr << "Logical error: division by zero\n"; exit(3); }
                stk.push(a/b);
            }
        }
    }
    if (stk.size() != 1) { cerr << "Runtime error: malformed expression\n"; exit(2); }
    return stk.top();
}

int main() {
    string expr;
    if (!getline(cin, expr)) {
        cerr << "Syntax error: no input provided\n";
        return 1;
    }
    size_t first = expr.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        cerr << "Syntax error: empty expression\n";
        return 1;
    }
    expr = expr.substr(first, expr.find_last_not_of(" \t\r\n") - first + 1);

    vector<Token> tokens  = tokenize(expr);
    validateSyntax(tokens);
    vector<Token> postfix = toPostfix(tokens);

    for (size_t i = 0; i < postfix.size(); i++) {
        if (i) cerr << " ";
        cerr << postfix[i].value;
    }
    cerr << "\n";

    vector<string>     vars = collectVariables(postfix);
    map<string,double> varValues;
    for (const string& v : vars) {
        double val;
        cerr << "Enter value for " << v << ": ";
        if (!(cin >> val)) {
            cerr << "Runtime error: invalid value for variable '" << v << "'\n";
            return 2;
        }
        varValues[v] = val;
    }

    double result = evaluate(postfix, varValues);

    if (result == floor(result) && abs(result) < 1e15)
        cout << (long long)result << "\n";
    else
        cout << result << "\n";

    return 0;
}
