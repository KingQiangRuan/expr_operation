#ifndef EXPR_EVAL_H
#define EXPR_EVAL_H

#include <string>
#include <deque>
#include <stack>
#include <map>
#include <vector>

typedef unsigned int uint32;

typedef std::deque<char> exprDeque; 
typedef std::stack<char> operatorStack;

class ExprEval {
    public:
        ExprEval(){};
        ~ExprEval(){};
    
    public:
        uint32 calculate(exprDeque &fmtDeque);
        uint32 combByOperatorRules(std::string &s, exprDeque &fmtDeque, operatorStack &optStack);
        uint32 format(const std::string &s, std::string &expr);
        int getPriority(char c);    
};


#endif
