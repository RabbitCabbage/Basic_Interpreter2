/*
 * File: exp.cpp
 * -------------
 * This file implements the Expression class and its subclasses.
 */

#include <string>
#include <unordered_set>
#include "../StanfordCPPLib/error.h"
#include "evalstate.h"
#include "exp.h"

#include "../StanfordCPPLib/strlib.h"
unordered_set<string> command_issue__{"REM","INPUT","LET","PRINT","END","IF","RUN","CLEAR","QUIT","HELP","LIST","GOTO"};

using namespace std;

/*
 * Implementation notes: the Expression class
 * ------------------------------------------
 * The Expression class declares no instance variables and needs no code.
 */

Expression::Expression() {
    /* Empty */
}

Expression::~Expression() {
    /* Empty */
}

/*
 * Implementation notes: the ConstantExp subclass
 * ----------------------------------------------
 * The ConstantExp subclass declares a single instance variable that
 * stores the value of the constant.  The eval method doesn't use the
 * value of state but needs it to match the general prototype for eval.
 */

ConstantExp::ConstantExp(int value) {
    this->value = value;
}

int ConstantExp::eval(EvalState &state) {
    return value;
}

string ConstantExp::toString() {
    return integerToString(value);
}

ExpressionType ConstantExp::getType() {
    return CONSTANT;
}

int ConstantExp::getValue() {
    return value;
}

/*
 * Implementation notes: the IdentifierExp subclass
 * ------------------------------------------------
 * The IdentifierExp subclass declares a single instance variable that
 * stores the name of the variable.  The implementation of eval must
 * look this variable up in the evaluation state.
 */

IdentifierExp::IdentifierExp(string name) {
    if(command_issue__.count(name))cout<<"SYNTAX ERROR"<<endl;
    this->name = name;
}

int IdentifierExp::eval(EvalState &state) {
    if (!state.isDefined(name)){cout<<"VARIABLE NOT DEFINED"<<endl;return -19260817;}
    if(state.identifier_collision(name)){cout<<"SYNTAX ERROR"<<endl;return -19260817;}
    return state.getValue(name);
}

string IdentifierExp::toString() {
    return name;
}

ExpressionType IdentifierExp::getType() {
    return IDENTIFIER;
}

string IdentifierExp::getName() {
    return name;
}

/*
 * Implementation notes: the CompoundExp subclass
 * ----------------------------------------------
 * The CompoundExp subclass declares instance variables for the operator
 * and the left and right subexpressions.  The implementation of eval 
 * evaluates the subexpressions recursively and then applies the operator.
 */

CompoundExp::CompoundExp(string op, Expression *lhs, Expression *rhs) {
    this->op = op;
    this->lhs = lhs;
    this->rhs = rhs;
}

CompoundExp::~CompoundExp() {
    delete lhs;
    delete rhs;
}

/*
 * Implementation notes: eval
 * --------------------------
 * The eval method for the compound expression case must check for the
 * assignment operator as a special case.  Unlike the arithmetic operators
 * the assignment operator does not evaluate its left operand.
 */

int CompoundExp::eval(EvalState &state) {
    if (op == "=") {
        if (lhs->getType() != IDENTIFIER) {
            error("Illegal variable in assignment");
        }
        int val = rhs->eval(state);
        state.setValue(((IdentifierExp *) lhs)->getName(), val);
        return val;
    }
    int left = lhs->eval(state);
    int right = rhs->eval(state);
    if (op == "+") return left + right;
    if (op == "-") return left - right;
    if (op == "*") return left * right;
    if (op == "/") {
        if(right) return left / right;
        else cout<<"DIVIDE BY ZERO"<<endl;
    }
    error("Illegal operator in expression");
    return 0;
}

string CompoundExp::toString() {
    return '(' + lhs->toString() + ' ' + op + ' ' + rhs->toString() + ')';
}

ExpressionType CompoundExp::getType() {
    return COMPOUND;
}

string CompoundExp::getOp() {
    return op;
}

Expression *CompoundExp::getLHS() {
    return lhs;
}

Expression *CompoundExp::getRHS() {
    return rhs;
}
