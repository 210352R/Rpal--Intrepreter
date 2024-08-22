#ifndef PARSER_HPP
#define PARSER_HPP
#include <cstring>
#include <fstream>
#include <iostream>
#include <stack>

using namespace std;

struct Node
{ // For the first child next sibling binary tree representation of nary trees.
    string value;
    struct Node *leftChild;
    struct Node *rightSibling;
};

extern const string FCN_FORM_LABEL;

extern const string GAMMA_STD_LABEL;
extern const string LAMBDA_STD_LABEL;

extern stack<Node *> parserStack;

void buildTree(string nodeLabel, int noOfTreesToPopAndMakeChildren);

void readNextToken(ifstream &file, string token);
int fn_Vl(ifstream &file, int identifiersReadBefore, bool isRecursiveCall);
void fn_Vb(ifstream &file);
void fn_Db(ifstream &file);
void fn_Dr(ifstream &file);
void fn_Da(ifstream &file);
void fn_D(ifstream &file);
void fn_Rn(ifstream &file);
void fn_R(ifstream &file);
void fn_Ap(ifstream &file);
void fn_Af(ifstream &file);
void fn_At(ifstream &file);
void fn_A(ifstream &file);
void fn_Bp(ifstream &file);
void fn_Bs(ifstream &file);
void fn_Bt(ifstream &file);
void fn_B(ifstream &file);
void fn_Tc(ifstream &file);
void fn_Ta(ifstream &file);
void fn_T(ifstream &file);
void fn_Ew(ifstream &file);
void fn_E(ifstream &file);

void fn_E(ifstream &file);

#endif // RPAL_INTERPRETER_PSG_H
