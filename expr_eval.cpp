/* 
* Copyright: Copyright(c) 2022-2023, all rights reversed
* Date : 2023-01-03
* Author: qiang
* Description: evalate string expression
*/

#include "expr_eval.h"

#include <iostream>

#define SEPERATOR     ('#')
#define IS_SPACE(c)   ((c) == ' ')
#define IS_DIGIT(c)   ((c) >= '0' && (c) <= '9')

/* operator '<' means "<<", '>' means: ">>" */
#define IS_OPERATOR(c) (c == '*' || c == '/' || c == '+' || c == '-' || \
                        c == '|' || c == '&' || c == '<' || c == '>') 

#define JUMP_SPACE(c, i) { \
    if (IS_SPACE(c)) { \
        i++; continue; \
    } \
}

#define GET_AN_INTEGER(c, num, q) { \
    while (c != SEPERATOR) { \
        num = num * 10 + c - '0'; \
        q.pop_back(); c=q.back(); \
    } \
    /* jump the seperator*/ \
    fmtDeque.pop_back(); \
}

#define GET_OPT_DATA(x1, x2, sk) { \
    x1 = sk.top(); sk.pop(); \
    x2 = sk.top(); sk.pop(); \
}

using namespace std;

uint32 ExprEval::calculate(exprDeque &fmtDeque)
{
    int x1;
    int x2;
    int y;
    int num = 0;
    char c;
    stack<int> dataStack;
    while (!fmtDeque.empty()) {
        c = fmtDeque.back();
        /* convert integer string to an integer */
        if (IS_DIGIT(c)) {
            GET_AN_INTEGER(c, num, fmtDeque);
            dataStack.push(num);
            num = 0;
            continue;
        }
        switch (c) {
            case '*' :
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 * x1;
                dataStack.push(y);
            } break;
            
            case '/':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 / x1;
                dataStack.push(y);
            } break;
            
            case '+':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 + x1;
                dataStack.push(y);
            } break;

            case '-':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 - x1;
                dataStack.push(y);
            } break;
            
            case '>':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 >> x1;
                dataStack.push(y);
            } break;
            
            case '<':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 << x1;
                dataStack.push(y);
            } break;
            
            case '|':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 | x1;
                dataStack.push(y);
            } break;
            
            case '&':
            {
                GET_OPT_DATA(x1, x2, dataStack);
                y = x2 & x1;
                dataStack.push(y);
            } break;
            
            default:
                std::cerr << "can’t recognise operator: " << c << std::endl;
                break;  
        }
        fmtDeque.pop_back();
    }
    // when caculation is over, the dataStack top element is the resalut of expression
    // and the dataStack must be only one element.
    uint32 rst = dataStack.top();
    dataStack.pop();    
    if (!dataStack.empty()) {
    std::cerr << "there is something wrong for caculation!!!\n";
         return 0;
    }
    return rst;
}


/*
 * 1. when the element in string s is a digit, push front to the fmtdeque directly.
 * 2. when the element in string is a '(', push to optStack directly.
 * 3. when the element is string is a ')', pop the operator form optStack,
 *    and then, push front to fmtDeque, until encountered to ')' in optStack. 
 * 4. low priority operator push statck firstly.
 * 5. when the current operator in sting s, is lower than the operator of optstack top,
 * 6. pop the operator from optStack, and push pront to fmtDeque.
 */

uint32 ExprEval::combByOperatorRules(std::string &s, 
                            exprDeque &fmtDeque, operatorStack &optStack)
                            
{
    std::cout << "expr : " << s << ", " << "lenght: " << s.size()<< std::endl;
    char c;
    char b;
    uint32 i = 0;
    while (i < s.size()) {
      c = s[i];
      if (IS_DIGIT(c)) {
            while (IS_DIGIT(c)) {
                fmtDeque.push_front(c);
                i++;
                c = s[i];
            }
            fmtDeque.push_front(SEPERATOR);
            continue;
        }
        
        if (c == '(') {
            optStack.push(c);
            i++;
        } else if (c == ')') {
            b = optStack.top();
            while (b != '(') {
                fmtDeque.push_front(b);
                optStack.pop();
                b = optStack.top();
            }
            i++;
            optStack.pop();
        } else if (IS_OPERATOR(c)) {
            if (optStack.empty()) {
                optStack.push(c);
                i++;
                continue;
            }

            if (getPriority(c) > getPriority(optStack.top())) {
                optStack.push(c);
                i++;
                continue;
            }
            //  optStack top operator is bigger than current operator
            //  pop this to fmtDeque,until optStack top operator is not bigger than current operator
            while (!optStack.empty() && getPriority(c) <= getPriority(optStack.top())) {
                    fmtDeque.push_front(optStack.top());
                    optStack.pop();
            }
            optStack.push(c);
            i++;       
        }
    }
    while (!optStack.empty()) {
        fmtDeque.push_front(optStack.top());
        optStack.pop();
    }
    return 0;
}

/* define operator priority */
int ExprEval::getPriority(char c)
{
    switch (c) {
        case '*':
        case '/':
            return 4;
        case '+':
        case '-':
            return 3;
        case '<':
        case '>':
            return 2;
        case '&':
        case '|':
            return 1;
        case '(':
        case ')':
            return 0;
        default:
            std::cerr << "unsupport operator :" << c << std::endl;
            return -1;        
    }
    return -1;
}

/*
 * check expression is compliance whit specifications 
 */
uint32 ExprEval::format(const std::string &s, std::string &expr)
{
    uint32 i = 0;
    char c;
    while (i < s.size()) {
        c = s[i];
        JUMP_SPACE(c, i);
      
        if (c == '<' && i + 1 < s.size()) {
            if (s[i + 1] != '<') {
                std::cout << " << operator is incrrect" << std::endl;
                return 1;
            }
            i++;
        } else if (c == '>' && i + 1 < s.size()) {
            if (s[i + 1] != '>') {
                std::cout << ">> operator is incrrect" << std::endl;
                return 1;
            }
            i++;
        }
        expr += c;
        i++;
    }
    return 0;
}

/* test string expression evalution */
int testCase()
{
    ExprEval eval;
    stack<char> optStack;
    deque<char> fmtQue;
    
    string expr[] = { 
                      "1/2",
                      "1 * 2",
                      "2-1",
                      "1+2",
                      "1+3 * 5 - 6 + (3 + 256) * 7 + 8",
                      "1 + 23 *15 -622 /23 + (22-1) /113 + (6 >> 2) + (3 << 1)",
                      "122 + (3 << 1) + 55 * 501",
                      "10000 | 10",
                      "10000 & 10"
    };
    int i;
    const int exprSize = 9;
    for (i = 0; i < exprSize; i++) {
        string fmtExpr;
        string s = expr[i];
        int ret = eval.format(s, fmtExpr);
        if (ret != 0) {
            std::cerr << "expression is incrrection\n";
            continue;
        }

        std::cout<< "\n====== fmt str: " << fmtExpr << "\n";
        eval.combByOperatorRules(fmtExpr, fmtQue, optStack);   
        uint32 v = eval.calculate(fmtQue);
        
        std::cout << s << " = " << v << std::endl;
    }    
    return 0;
}

int main(int argc, char *argv[])
{
    testCase();
    
    return 0;
}   
