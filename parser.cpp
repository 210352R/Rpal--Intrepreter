#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // Include <cctype> for std::isdigit
#include <vector>
#include <queue>
#include <stdlib.h>
#include <stack>
#include <list>
#include <cmath>

// Define AST node labels
#define GAMMA_STD_LABEL "gamma"
#define LAMBDA_STD_LABEL "lambda"
#define FCN_FORM_LABEL = "function_form"

using namespace std;

struct Node
{
    string value;
    Node *leftChild = NULL;
    Node *rightSibling = NULL;
};

stack<Node *> parserStack;
int counter = 0;
// fstream fs;
string NextToken;

// Keywords / Identifiers and Operartors
string keywords[] = {"let", "in", "fn", "where", "aug", "or", "not", "gr", "ge", "ls", "le", "eq", "ne",
                     "true", "false", "dummy", "within", "and", "rec", "neg", "tau", "function_form", "lambda", "gamma", "nil", "", "<true>", "<false>", "<dummy>", "<nil>"};

string operators[] = {"->", ">=", "<=", "**", ".", ",", "|", ">", "<", "+", "-", "*", "/", "@", "(", ")", "=", "'", "&", "()"};

string specNodes[] = {"tau", "function_form", ",", "and", "lambda"};

string threeOps[] = {"@", "->"};

// token Queue --------------
std::queue<string> tokenQueue;

// Define token types
enum TokenType
{
    IDENTIFIER,
    INTEGER,
    OPERATOR,
    STRING,
    SPACES,
    COMMENT,
    PUNCTUATION
};

// Recursive Descent Functions  ------------------------------------------------------------------------------------------------------------
void fn_E();
void fn_Ew();
void fn_T();
void fn_Ta();
void fn_Tc();
void fn_B();
void fn_Bt();
void fn_Bs();
void fn_Bp();
void fn_A();
void fn_At();
void fn_Af();
void fn_Ap();
void fn_R();
void fn_Rn();
void fn_D();
void fn_Da();
void fn_Dr();
void fn_Db();
void fn_Vb();
void fn_Vl();

// Token structure
struct Token
{
    TokenType type;
    string value;
};

// Function to check if a character is an operator symbol
bool isOperatorSymbol(char c)
{
    string symbols = "+-*<>&.@/:=~|$!#%^_[{}\"‘?";
    return symbols.find(c) != string::npos;
}

bool isPunctuation(char c)
{
    std::string punctuations = "().,";
    return punctuations.find(c) != std::string::npos;
}

// Check if a character is whitespace (including tab)
bool isWhitespace(char c)
{
    return c == ' ' || c == '\t';
}

// Function to get the next token ---------------------
void getNextToken(string input, vector<Token> &tokenList)
{
    string::iterator currentChar = input.begin();

    // Skip leading whitespace
    while (currentChar != input.end() && isWhitespace(*currentChar))
    {
        ++currentChar;
    }

    bool inComment = false; // State variable for comment handling

    while (currentChar != input.end())
    {
        char c = *currentChar;

        // Check for comment start (if not already in comment)
        if (!inComment && c == '/' && *(currentChar + 1) == '/')
        {
            inComment = true;
            ++currentChar; // Skip the first slash
            ++currentChar; // Skip the second slash
            continue;      // Move to the next character
        }

        // Handle characters inside a comment
        if (inComment)
        {
            if (c == '\n')
            { // End of line reached within comment
                inComment = false;
            }
            ++currentChar; // Skip characters within the comment
            continue;
        }

        // Handle non-comment characters

        // Identify identifiers
        if (isalpha(*currentChar))
        {
            Token token;
            token.type = IDENTIFIER;
            while (currentChar != input.end() && (isalnum(*currentChar) || *currentChar == '_'))
            {
                token.value += *currentChar;
                ++currentChar;
            }
            tokenList.push_back(token);
        }
        else if (isdigit(*currentChar))
        { // Identify integers
            Token token;
            token.type = INTEGER;
            while (currentChar != input.end() && isdigit(*currentChar))
            {
                token.value += *currentChar;
                ++currentChar;
            }
            tokenList.push_back(token);
        }
        else if (isOperatorSymbol(*currentChar))
        { // Identify operators
            Token token;
            token.type = OPERATOR;
            while (currentChar != input.end() && isOperatorSymbol(*currentChar))
            {
                token.value += *currentChar;
                ++currentChar;
            }
            tokenList.push_back(token);
        }
        else if (c == '\'')
        { // Identify strings
            Token token;
            token.type = STRING;
            ++currentChar; // Skip the opening '
            while (currentChar != input.end() && *currentChar != '\'')
            {
                token.value += *currentChar;
                ++currentChar;
            }
            if (currentChar != input.end())
            {
                ++currentChar; // Skip the closing '
            }
            tokenList.push_back(token);
        }
        else if (isPunctuation(*currentChar))
        { // Identify punctuations
            Token token;
            token.type = PUNCTUATION;
            token.value = *currentChar;
            ++currentChar;
            tokenList.push_back(token);
        }
        else
        { // Handle unsupported characters (optional: you can add error handling)
            ++currentChar;
        }
    }

    // Add a SPACES token at the end if needed
    if (!inComment && !input.empty())
    { // Check for non-empty line after comments
        Token token;
        token.type = SPACES;
        token.value = "";
        tokenList.push_back(token);
    }
}

// Implement Parser  process  ------------------------------------------------------------------

// check wether my token is in string threeOps[] = {"@", "->"};
bool present_threeOps(string Token)
{
    for (int i = 0; i < (sizeof(threeOps) / sizeof(threeOps[0])); ++i)
    {
        if (Token.compare(threeOps[i]) == 0)
            return 1;
    }
    return 0;
}

// check wether my token is in string specNodes[] = {"tau", "function_form", ",", "and", "lambda"};
bool present_specs(string Token)
{
    for (int i = 0; i < (sizeof(specNodes) / sizeof(specNodes[0])); ++i)
    {
        if (Token.compare(specNodes[i]) == 0)
            return 1;
    }
    return 0;
}

bool present(char c, int pl)
{
    if (pl == 0)
    {
        for (int i = 0; i < 20; ++i)
        {
            if (operators[i].at(pl) == c)
                return 1;
        }
        return 0;
    }
    else if (pl == 1)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (operators[i].at(pl) == c)
                return 1;
        }
        return 0;
    }
}

// cheking Tokens
bool isoper(string Token)
{
    for (int i = 0; i < 20; ++i)
    {
        if (Token.compare(operators[i]) == 0)
            return 1;
    }
    return 0;
}

bool isKeyword(string Token)
{
    for (int i = 0; i < 30; ++i)
    {
        if (Token.compare(keywords[i]) == 0)
            return 1;
    }
    return 0;
}

bool integ(string Token)
{
    if (Token.length() == 0)
        return 0;
    for (size_t i = 0; i < Token.length(); ++i)
    {
        if (!isdigit(Token.at(i)))
            return 0;
    }
    return 1;
}

bool ident(string Token)
{
    if (Token.length() == 0)
        return 0;
    for (size_t i = 0; i < Token.length(); ++i)
    {
        if (!(isalnum(Token.at(i)) || Token.at(i) == '_'))
            return 0;
    }
    if (isKeyword(Token) || isoper(Token))
        return 0;
    return 1;
}

bool str(string Token)
{
    if (Token.length() == 0)
        return 0;
    if (!(Token.at(0) == '\'' && Token.at(Token.length() - 1) == '\''))
        return 0;
    else
        return 1;
}

bool strEq(const char *arg0, const char *arg1)
{                               // compare two arguments
    string var0 = (string)arg0; // converting both the arguments to strings and then using compare
    string var1 = (string)arg1;
    if (var0 == var1)
        return true;
    else
        return false;
}

void printTree(Node *root)
{

    if (root != NULL)
    {
        for (int j = 0; j < counter; ++j)
        {
            cout << ".";
        }

        cout << root->value << endl;
        // go to deep level
        counter++;
        printTree(root->leftChild);
        // come to upper level again
        counter--;
        printTree(root->rightSibling);
    }
}

void buildTree(string s, int i)
{
    Node *treeNode = new Node;
    treeNode->value = s;
    treeNode->rightSibling = NULL;
    Node *treeNodePtr = NULL;
    if (parserStack.empty() && i > 0)
    {
        cout << "\n\nERROR! Something went wrong in AST generation! Program will die now!\n\n";
        exit(0);
    }
    while (i > 0 && !parserStack.empty())
    {
        if (treeNodePtr != NULL)
        {
            parserStack.top()->rightSibling = treeNodePtr;
            treeNodePtr = parserStack.top();
        }
        else
        {
            treeNodePtr = parserStack.top();
            // cout << "\n# Node '" << treeNodePtr->label << "' to be the child of tree: '" << nodeLabel << "'";
        }
        parserStack.pop();
        i--;
    }
    treeNode->leftChild = treeNodePtr;
    // cout << "\n# Addinde: '" << nodeLabel << "'";
    parserStack.push(treeNode);
    return;
}

// scan next token from token Queue -----------------------------------------------------
string scanNextToken()
{
    if (tokenQueue.empty())
    {
        return "Token Queue is Empty !!!!! ";
    }
    else
    {
        string token = tokenQueue.front();
        tokenQueue.pop();
        return token;
    }
}

void Read(string Token)
{
    if (Token.compare(NextToken))
    {

        exit(1);
    }

    if (integ(Token))
    {

        buildTree("<INT:" + Token + ">", 0);
    }
    else if (ident(Token))
    {

        buildTree("<ID:" + Token + ">", 0);
    }
    else if (str(Token))
    {

        buildTree("<STR:" + Token + ">", 0);
    }

    NextToken = scanNextToken();
}

void printAST()
{
    // Get Token from token Queue -------
    NextToken = scanNextToken();
    // call starting function ------
    fn_E();
    // After calling all tokens Print Tree -----
    printTree(parserStack.top());
}

// E ---------------------------
void fn_E()
{
    // cout << "fn_E";
    if (NextToken.compare("let") == 0)
    {
        Read("let");
        fn_D();
        Read("in");
        fn_E();
        buildTree("let", 2);
    }
    else if (NextToken.compare("fn") == 0)
    {
        Read("fn");
        // Interpritation of Vb+
        int N = 0;
        do
        {
            fn_Vb();
            ++N;
        } while (NextToken.compare("(") == 0 || ident(NextToken));
        Read("."); //  read dot
        fn_E();
        buildTree("lambda", N + 1);
    }
    else
        fn_Ew(); //  call function fn_Ew
}

void fn_Ew()
{
    // In 2 cases fn_T() always calling
    fn_T();
    if (NextToken.compare("where") == 0)
    {
        Read("where");
        fn_Dr();
        buildTree("where", 2);
        // cout << "WHERE" << endl;
    }
}

void fn_T()
{
    // both cases fn_Ta()  calling

    fn_Ta();
    if (NextToken.compare(",") == 0)
    {
        int N = 0;
        do
        {
            Read(",");
            fn_Ta();
            ++N;
        } while (NextToken.compare(",") == 0);
        buildTree("tau", N + 1);
        // cout << "TAU" << endl;
    }
}

// test point 01
void fn_Ta()
{
    fn_Tc();
    if (NextToken.compare("aug") == 0)
    {
        while (NextToken.compare("aug") == 0)
        {
            Read("aug");
            fn_Tc();
            buildTree("aug", 2);
        }

        // cout << "AUG" << endl;
    }
}

void fn_Tc()
{

    fn_B();
    if (NextToken.compare("->") == 0)
    {
        Read("->");
        fn_Tc();
        Read("|");
        fn_Tc();
        buildTree("->", 3);
        // cout << "->" << endl;
    }
}

void fn_B()
{
    fn_Bt();
    if (NextToken.compare("or") == 0)
    {
        while (NextToken.compare("or") == 0)
        {
            Read("or");
            fn_Bt();
            buildTree("or", 2);
        }

        // cout << "OR" << endl;
    }
}

void fn_Bt()
{
    fn_Bs();
    if (NextToken.compare("&") == 0)
    {
        while (NextToken.compare("&") == 0)
        {
            Read("&");
            fn_Bs();
            buildTree("&", 2);
        }

        // cout << "&" << endl;
    }
}

void fn_Bs()
{
    if (NextToken.compare("not") == 0)
    {
        Read("not");
        fn_Bp();
        buildTree("not", 1);
        // cout << "NOT" << endl;
    }
    else
        fn_Bp();
}

void fn_Bp()
{
    fn_A();
    if (NextToken.compare("gr") == 0 || NextToken.compare(">") == 0)
    {
        Read(NextToken);
        fn_A();
        buildTree("gr", 2);
        // cout << "GR" << endl;
    }
    else if (NextToken.compare("ge") == 0 || NextToken.compare(">=") == 0)
    {
        Read(NextToken);
        fn_A();
        buildTree("ge", 2);
        // cout << "GE" << endl;
    }
    else if (NextToken.compare("ls") == 0 || NextToken.compare("<") == 0)
    {
        Read(NextToken);
        fn_A();
        buildTree("ls", 2);
        // cout << "LS" << endl;
    }
    else if (NextToken.compare("le") == 0 || NextToken.compare("<=") == 0)
    {
        Read(NextToken);
        fn_A();
        buildTree("le", 2);
    }
    else if (NextToken.compare("eq") == 0)
    {
        Read(NextToken);
        fn_A();
        buildTree("eq", 2);
    }
    else if (NextToken.compare("ne") == 0)
    {
        Read(NextToken);
        fn_A();
        buildTree("ne", 2);
    }
}

void fn_A()
{
    if (NextToken.compare("-") == 0)
    {
        Read("-");
        fn_At();
        buildTree("neg", 1);
        // cout << "NEG" << endl;
    }
    else if (NextToken.compare("+") == 0)
    {
        Read("+");
        fn_At();
    }
    else
        fn_At();
    if (NextToken.compare("+") == 0 || NextToken.compare("-") == 0)
    {
        while (NextToken.compare("+") == 0 || NextToken.compare("-") == 0)
        {
            string Temp = NextToken;
            Read(NextToken);
            fn_At();
            buildTree(Temp, 2);
            // cout << Temp << endl;
        }
    }
}

void fn_At()
{
    fn_Af();
    if (NextToken.compare("*") == 0 || NextToken.compare("/") == 0)
    {

        while (NextToken.compare("*") == 0 || NextToken.compare("/") == 0)
        {
            string Temp = NextToken;
            Read(Temp);
            fn_Af();
            buildTree(Temp, 2);
        }
    }
}

void fn_Af()
{
    fn_Ap();
    if (NextToken.compare("**") == 0)
    {
        Read("**");
        fn_Af();
        buildTree("**", 2);
    }
}

void fn_Ap()
{
    fn_R();
    if (NextToken.compare("@") == 0)
    {
        while (NextToken.compare("@") == 0)
        {
            Read("@");
            if (ident(NextToken))
            {
                Read(NextToken);
            }
            fn_R();
            buildTree("@", 3);
            // cout << "@" << endl;
        }
    }
}

void fn_R()
{
    fn_Rn();
    if (NextToken.compare("true") == 0 || NextToken.compare("false") == 0 || NextToken.compare("nil") == 0 ||
        NextToken.compare("(") == 0 || NextToken.compare("dummy") == 0 || ident(NextToken) || integ(NextToken) || str(NextToken))
    {

        while (NextToken.compare("true") == 0 || NextToken.compare("false") == 0 || NextToken.compare("nil") == 0 ||
               NextToken.compare("(") == 0 || NextToken.compare("dummy") == 0 || ident(NextToken) || integ(NextToken) || str(NextToken))
        {
            fn_Rn();
            buildTree("gamma", 2);
        }
    }
}

void fn_Rn()
{
    if (ident(NextToken) || integ(NextToken) || str(NextToken))
        Read(NextToken);
    else if (NextToken.compare("") == 0)
    {
    }
    else if (NextToken.compare("true") == 0)
    {
        Read(NextToken);
        buildTree("<true>", 0);
    }
    else if (NextToken.compare("false") == 0)
    {
        Read(NextToken);
        buildTree("<false>", 0);
    }
    else if (NextToken.compare("nil") == 0)
    {
        Read(NextToken);
        buildTree("<nil>", 0);
    }
    else if (NextToken.compare("dummy") == 0)
    {
        Read(NextToken);
        buildTree("<dummy>", 0);
    }
    else if (NextToken.compare("(") == 0)
    {
        Read("(");
        fn_E();
        Read(")");
    }
    else
    {
        cout << "Error\n"
             << endl;
        exit(1);
    }
}

void fn_D()
{
    fn_Da();
    if (NextToken.compare("within") == 0)
    {
        Read("within");
        fn_D();
        buildTree("within", 2);
    }
}

void fn_Da()
{
    fn_Dr();
    if (NextToken.compare("and") == 0)
    {
        int N = 0;
        do
        {
            Read("and");
            fn_Dr();
            ++N;
        } while (NextToken.compare("and") == 0);
        buildTree("and", N + 1);
    }
}

void fn_Dr()
{
    int isRec = false;

    if (NextToken.compare("rec") == 0)
    {
        Read("rec");
        isRec = true;
    }
    fn_Db();
    if (isRec)
    {
        buildTree("rec", 1);
    }
}

// function _ form ---------------------------------------------
void fn_Db()
{
    if (NextToken.compare("(") == 0)
    {
        Read("(");
        fn_D();
        Read(")");
    }
    else if (ident(NextToken))
    {

        Read(NextToken);
        if (ident(NextToken) || NextToken.compare("(") == 0)
        {
            int N = 0;
            while (ident(NextToken) || NextToken.compare("(") == 0)
            {
                fn_Vb();
                ++N;
            }
            Read("=");
            fn_E();
            buildTree("function_form", N + 2);
        }
        else
        {
            if (NextToken.compare(",") == 0)
            {
                int N = 0;
                while (NextToken.compare(",") == 0)
                {
                    Read(",");
                    if (ident(NextToken))
                        Read(NextToken);
                    else
                    {
                        cout << "Error: Expected " << NextToken << endl;
                        exit(1);
                    }
                    ++N;
                }
                buildTree(",", N + 1);
            }
            Read("=");
            fn_E();
            buildTree("=", 2);
            // cout << "=" << endl;
        }
    }
}

void fn_Vb()
{
    if (ident(NextToken))
    {
        Read(NextToken);
    }
    else if (NextToken.compare("(") == 0)
    {
        Read("(");
        if (NextToken.compare(")") == 0)
        {
            Read(")");
            buildTree("()", 0);
        }
        else if (ident(NextToken))
        {
            // Read(NextToken);
            fn_Vl();
            Read(")");
        }
    }
}

void fn_Vl()
{
    if (ident(NextToken))
        Read(NextToken);
    if (NextToken.compare(",") == 0)
    {
        int N = 0;
        while (NextToken.compare(",") == 0)
        {
            Read(",");
            if (ident(NextToken))
                Read(NextToken);
            ++N;
        }
        buildTree(",", N + 1);
        // cout << "," << endl;
    }
}

// pre-order travese of top of stack
void preOrder(Node *root)
{
    if (root != NULL)
    {
        cout << root->value << " ";
        preOrder(root->leftChild);
        preOrder(root->rightSibling);
    }
}

// --------------------------------------------------- Convert AST to Starndard Tree --------------------------------------------------------------------------------

// Convert Let Expression
void convertLetExpression(Node *letNode)
{
    //    cout << "\nInside convertLetExpression conversion!\nletNode ast form before standardizing is:\n";
    //    recursivelyPrintTreeNode(letNode, "");
    letNode->value = GAMMA_STD_LABEL;

    letNode->leftChild->value = LAMBDA_STD_LABEL;

    Node *pNode = letNode->leftChild->rightSibling;
    Node *eNode = letNode->leftChild->leftChild->rightSibling;

    // switch the p and e nodes

    letNode->leftChild->rightSibling = eNode;
    letNode->leftChild->leftChild->rightSibling = pNode;

    //    cout << "\nInside convertLetExpression conversion!\nletNode ast form after standardizing is:\n";
    //    recursivelyPrintTreeNode(letNode, "");
}

void convertWhereExpression(Node *whereNode)
{
    //    cout << "\nInside convertWhereExpression conversion!\nwhereNode ast form before standardizing is:\n";
    //    recursivelyPrintTreeNode(whereNode, "");
    whereNode->value = GAMMA_STD_LABEL;

    Node *pNode = whereNode->leftChild;
    Node *eqlNode = whereNode->leftChild->rightSibling;
    Node *eNode = eqlNode->leftChild->rightSibling;
    Node *xNode = eqlNode->leftChild;

    whereNode->leftChild->value = LAMBDA_STD_LABEL;

    // Node *pNode = whereNode->leftChild->rightSibling;
    // Node *eNode = whereNode->leftChild->leftChild->rightSibling;

    // switch the p and e nodes

    whereNode->leftChild->rightSibling = eNode;
    whereNode->leftChild->leftChild = xNode;
    whereNode->leftChild->leftChild->rightSibling = pNode;

    //    cout << "\nInside convertWhereExpression conversion!\nwhereNode ast form after standardizing is:\n";
    //    recursivelyPrintTreeNode(whereNode, "");
}

// convert fcn_form
void convertFunctionForm(Node *functionFormNode)
{
    //    cout<<"\nInside function form conversion!\nFunction form is:\n";
    //    if(functionFormNode->nextSibling != NULL) {
    //        cout<<"functionFormNode's sibling is: "<<functionFormNode->nextSibling->label<<"\n";
    //    }
    //    recursivelyPrintTreeNode(functionFormNode, "");
    Node *fcnLambdaRightChildNodeHeader = new Node; // the "lambda" right child node header of the final standardized sub-tree
    fcnLambdaRightChildNodeHeader->value = LAMBDA_STD_LABEL;
    fcnLambdaRightChildNodeHeader->rightSibling = NULL;

    list<Node *> fcnVariableList;
    functionFormNode->value = "="; // the "=" header node of the final standardized sub-tree

    Node *temp = functionFormNode->leftChild; // the fcn label left child node of the final standardized sub-tree
    while (temp->rightSibling->rightSibling != NULL)
    { // temp->nextSibling->nextSibling == NULL implies temp->nextSibling is the "Expression" part of the fcnForm
        temp = temp->rightSibling;
        fcnVariableList.push_back(temp);
    }
    // temp = temp->nextSibling; //this would be the expression part of the fcn form //the final expression node which is the rightMost child of the right sub-tree

    functionFormNode->leftChild->rightSibling = fcnLambdaRightChildNodeHeader;
    fcnLambdaRightChildNodeHeader->rightSibling = NULL;
    fcnLambdaRightChildNodeHeader->leftChild = fcnVariableList.front();
    Node *lambdaTemp = fcnLambdaRightChildNodeHeader;
    fcnVariableList.pop_front();
    while (fcnVariableList.size() > 0)
    {
        Node *newLambdaRightNode = new Node;
        lambdaTemp->leftChild->rightSibling = newLambdaRightNode;
        newLambdaRightNode->value = LAMBDA_STD_LABEL;
        newLambdaRightNode->rightSibling = NULL;
        lambdaTemp = newLambdaRightNode;
        lambdaTemp->leftChild = fcnVariableList.front();
        fcnVariableList.pop_front();
    }
    // lambdaTemp->firstKid->nextSibling = temp;
    //    cout<<"\nInside function form conversion!\nThe standardized Function form is:\n";
    //    recursivelyPrintTreeNode(functionFormNode, "");
    //
    //    if(functionFormNode->nextSibling != NULL) {
    //        cout<<"functionFormNode's sibling is: "<<functionFormNode->nextSibling->label<<"\n";
    //    }
}

// Infix Operater
void convertInfixOperator(Node *infixOperatorNode)
{
    // cout<<"\nInside Infix Operator conversion!\nInfix ast form before standardizing is:\n";
    // recursivelyPrintTree(opNode, "");
    Node *leftGammaChild = new Node;
    Node *leftLeftOperatorChild = new Node;
    leftLeftOperatorChild->value = infixOperatorNode->leftChild->rightSibling->value; // N
    leftLeftOperatorChild->rightSibling = infixOperatorNode->leftChild;               // E1
    leftLeftOperatorChild->leftChild = NULL;
    leftGammaChild->value = GAMMA_STD_LABEL;
    leftGammaChild->leftChild = leftLeftOperatorChild;
    leftGammaChild->rightSibling = infixOperatorNode->leftChild->rightSibling->rightSibling; // E2
    infixOperatorNode->leftChild->rightSibling = NULL;
    infixOperatorNode->leftChild = leftGammaChild;
    infixOperatorNode->value = GAMMA_STD_LABEL;
    // cout<<"\nThe standardized Infix operator is:\n";
    // recursivelyPrintTree(opNode, "");
}

void convertRecExpression(Node *recNode)
{
    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";
    //    cout << "\nInside convertRecExpression conversion!\nrecNode ast form before standardizing is:\n";
    //    recursivelyPrintTreeNode(recNode, "");
    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";
    //
    //    if(recNode->nextSibling != NULL) {
    //        cout<<"recNode's sibling is: "<<recNode->nextSibling->label<<"\n";
    //    }
    //
    //    if(recNode->firstKid->nextSibling != NULL) {
    //        cout<<"recNode's firstKid's sibling is: "<<recNode->firstKid->nextSibling->label<<"\n";
    //    }

    Node *recNodeOriginalEqualsChild = recNode->leftChild;

    recNode->value = recNodeOriginalEqualsChild->value;
    recNode->leftChild = recNodeOriginalEqualsChild->leftChild;

    Node *rightGammaChild = new Node;
    rightGammaChild->value = GAMMA_STD_LABEL;
    rightGammaChild->rightSibling = NULL;
    Node *rightRightLambdaChild = new Node;
    rightRightLambdaChild->value = LAMBDA_STD_LABEL;
    rightRightLambdaChild->rightSibling = NULL;

    Node *leftChildYNode = new Node;
    leftChildYNode->value = "Y";
    leftChildYNode->leftChild = NULL;

    rightGammaChild->leftChild = leftChildYNode;
    leftChildYNode->rightSibling = rightRightLambdaChild;

    Node *functionNameNode = new Node;
    functionNameNode->value = recNode->leftChild->value; // X
    functionNameNode->leftChild = NULL;

    rightRightLambdaChild->leftChild = functionNameNode;
    functionNameNode->rightSibling = recNode->leftChild->rightSibling; // E

    recNode->leftChild->rightSibling = rightGammaChild;

    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";
    //    cout << "\nInside convertRecExpression conversion!\nrecNode ast form after standardizing is:\n";
    //    recursivelyPrintTreeNode(recNode, "");
    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";
}

// convert within Expression
void convertWithinExpression(Node *withinNode)
{
    withinNode->value = "=";

    Node *withinOne = withinNode->leftChild;
    Node *withinTwo = withinOne->rightSibling;

    Node *rightGammaChild = new Node;
    Node *rightLeftLambdaChild = new Node;
    rightGammaChild->value = GAMMA_STD_LABEL;
    rightGammaChild->rightSibling = NULL;
    rightLeftLambdaChild->value = LAMBDA_STD_LABEL;

    rightGammaChild->leftChild = rightLeftLambdaChild;
    rightLeftLambdaChild->rightSibling = withinOne->leftChild->rightSibling;            // E1
    rightLeftLambdaChild->leftChild = withinOne->leftChild;                             // X1
    rightLeftLambdaChild->leftChild->rightSibling = withinTwo->leftChild->rightSibling; // E2

    withinNode->leftChild = withinTwo->leftChild; // X2
    withinNode->leftChild->rightSibling = rightGammaChild;
}

void convertAndExpression(Node *andHeaderNode)
{
    andHeaderNode->value = "=";
    Node *tempEqualsChildHeaderNode = andHeaderNode->leftChild;
    list<Node *> variableNodesList;
    list<Node *> expressionNodesList;
    while (tempEqualsChildHeaderNode != NULL)
    {
        variableNodesList.push_back(tempEqualsChildHeaderNode->leftChild);
        expressionNodesList.push_back(tempEqualsChildHeaderNode->leftChild->rightSibling);
        tempEqualsChildHeaderNode = tempEqualsChildHeaderNode->rightSibling;
    }

    Node *commaHeaderNode = new Node;
    Node *tauHeaderNode = new Node;

    commaHeaderNode->value = ",";
    tauHeaderNode->value = "tau";
    tauHeaderNode->rightSibling = NULL;
    commaHeaderNode->rightSibling = tauHeaderNode;

    andHeaderNode->leftChild = commaHeaderNode;

    Node *commaVariableTempNode, *tauExpressionTempNode;

    commaVariableTempNode = variableNodesList.front();
    variableNodesList.pop_front();
    tauExpressionTempNode = expressionNodesList.front();
    expressionNodesList.pop_front();

    commaHeaderNode->leftChild = commaVariableTempNode;

    tauHeaderNode->leftChild = tauExpressionTempNode;

    while (!variableNodesList.empty())
    {
        commaVariableTempNode->rightSibling = variableNodesList.front();
        variableNodesList.pop_front();
        tauExpressionTempNode->rightSibling = expressionNodesList.front();
        expressionNodesList.pop_front();
        commaVariableTempNode = commaVariableTempNode->rightSibling;
        tauExpressionTempNode = tauExpressionTempNode->rightSibling;
    }

    commaVariableTempNode->rightSibling = NULL;
    tauExpressionTempNode->rightSibling = NULL;
}

void convertLambdaExpression(Node *lambdaNode)
{
    //    cout << "\nInside lambda expression conversion!\n lambda expression ast form before standardizing is:\n";
    //    recursivelyPrintTreeNode(lambdaNode, "");
    lambdaNode->value = LAMBDA_STD_LABEL;

    list<Node *> fcnVariableList;

    Node *temp = lambdaNode->leftChild; // the top left child node of the final standardized lambda sub-tree
    while (temp->rightSibling->rightSibling !=
           NULL)
    { // temp->nextSibling->nextSibling == NULL implies temp->nextSibling is the "Expression" part of the fcnForm
        temp = temp->rightSibling;
        fcnVariableList.push_back(temp);
    }
    temp = temp->rightSibling; // this would be the expression part of the fcn form //the final expression node which is the rightMost child of the right sub-tree

    Node *lambdaTemp = lambdaNode;
    while (fcnVariableList.size() > 0)
    {
        Node *newLambdaRightNode = new Node;
        lambdaTemp->leftChild->rightSibling = newLambdaRightNode;
        newLambdaRightNode->rightSibling = NULL;
        newLambdaRightNode->value = LAMBDA_STD_LABEL;
        lambdaTemp = newLambdaRightNode;
        lambdaTemp->leftChild = fcnVariableList.front();
        fcnVariableList.pop_front();
    }
    lambdaTemp->leftChild->rightSibling = temp; // E
    //    cout << "\nThe standardized lambda expression is:\n";
    //    recursivelyPrintTreeNode(lambdaNode, "");
}

// ------------------------- Functions to Convert tree to Stanadarize tree-------

void recursivelyStandardizeTree(Node *node)
{
    if (node->leftChild != NULL)
    {
        recursivelyStandardizeTree(node->leftChild);
    }
    if (node->rightSibling != NULL)
    {
        recursivelyStandardizeTree(node->rightSibling);
    }
    if (node->value == "->")
    {
        // Do not standardize conditionals (optimizations for the CSE machine)-- It is operation Action ---------------
    }
    else if (node->value == "not" || node->value == "neg")
    { // convert unary operators to standardized form
      // Do not standardize unary operators (optimizations for the CISE machine) //convertUop(node);
    }
    else if (node->value == "aug" || node->value == "or" || node->value == "&" || node->value == "gr" ||
             node->value == "ge" || node->value == "ls" || node->value == "le" || node->value == "eq" ||
             node->value == "ne" || node->value == "+" || node->value == "-" || node->value == "*" ||
             node->value == "/" || node->value == "**")
    {
        // Do not standardize binary operators (optimizations for the CISE machine) //convertOperator(node);
    }
    else if (node->value == "tau")
    {
        // Do not standardize tau (optimizations for the CISE machine)
    }
    else if (node->value == "lambda")
    { // convert lambda expression to standardized form
        if (node->leftChild->value == ",")
        { // lambda expression with a tuple of variables
          // Do not standardize lambda with a tuple of variables (optimizations for the CISE machine)
        }
        else
        { // lambda expression with a list(?) of variable(s)
            //            cout << "\nGoing to convertLambdaExpression\n";
            convertLambdaExpression(node);
        }
    }
    else if (node->value == "function_form")
    { // convert function_form to standardized form
        //        cout << "\nGoing to convertFunctionForm\n";
        convertFunctionForm(node);
    }
    else if (node->value == "@")
    { // convert infix operator to standardized form
        //        cout << "\nGoing to convertInfixOperator\n";
        convertInfixOperator(node);
    }
    else if (node->value == "and")
    {
        //        cout << "\nGoing to convertAndExpression\n";
        convertAndExpression(node);
    }
    else if (node->value == "within")
    {
        //        cout << "\nGoing to convertWithinExpression\n";
        convertWithinExpression(node);
    }
    else if (node->value == "rec")
    {
        //        cout << "\nGoing to convertRecExpression for nodeLabel= " << node->label << "\n";
        convertRecExpression(node);
        //        cout << "\nAfter convertRecExpression for nodeLabel= " << node->label << "\n";
    }
    else if (node->value == "let")
    {
        //        cout << "\nGoing to convertLetExpression\n";
        convertLetExpression(node);
    }
    else if (node->value == "where")
    {
        //        cout << "\nGoing to convertWhereExpression\n";
        convertWhereExpression(node);
    }
}

void convertASTToStandardizedTree()
{
    // cout << "\n\nGoing to standardize the tree now!\n\n";
    if (parserStack.empty())
    {
        return;
    }
    else
    {
        // cout << "\n\nThis is supposed to be the only tree below!\n";
        Node *treeRootOfAST = parserStack.top();
        recursivelyStandardizeTree(treeRootOfAST);
    }
}

// --------------- Flatten Starndarized AST for make Control Stack ---------------------------------------------------------------

struct MachineNode
{ // Node abstraction for the CSE machine for both the control and stack
    string nameValue;
    bool isName; // whether it's an identifier
    bool isString;
    string stringValue;
    bool isGamma;
    bool isLambda;
    std::vector<string> boundVariables;
    int indexOfBodyOfLambda; // index of the controlStructure of this lambda expression
    bool isTau;              // refers to the control stack variable which will convert stack elements to tuple
    int numberOfElementsInTauTuple;
    bool isTuple;                           // refers to the CSE stack structure variable containing variables
    std::vector<MachineNode> tupleElements; // can be either int/bool/string
    bool isComma;
    bool isEnvironmentMarker;
    int environmentMarkerIndex; // for a lambda, it means the environment in which it was placed on the stack.
    bool isInt;
    int intValue;
    bool isConditional;
    bool isUnaryOperator;
    bool isBinaryOperator;
    string operatorStringValue;
    string defaultLabel;
    bool isBoolean;
    bool isBuiltInFunction;
    bool isY;
    bool isYF;
    bool isDummy;

    MachineNode()
    {
        isName = false;
        isString = false;
        isGamma = false;
        isLambda = false;
        isTau = false;
        isEnvironmentMarker = false;
        isInt = false;
        isConditional = false;
        isUnaryOperator = false;
        isBinaryOperator = false;
        isComma = false;
        isBoolean = false;
        isBuiltInFunction = false;
        isTuple = false;
        isY = false;
        isYF = false;
        isDummy = false;
    }
};

struct EnvironmentNode
{ // Node abstraction for an environment marker in the CSE machine
    EnvironmentNode *parentEnvironment;
    EnvironmentNode *previousEnvironment;
    MachineNode boundedValuesNode;
    // the bounded values node will have the bounded variable mappings from the boundedVariables string vector to the tupleElements MachineNode vector.
    //  boundedVariables (string) -> tupleElements (MachineNode) [the tupleElement could be int/string/Lambda]
    int environmentIndex;
};

// List of Control Stacks
vector<list<MachineNode>> controlStructures(150); // each controlStructure would be a list of machineNodes
int numberOfControlStructures = 1;

// Generate Control Structure form Standarized tree
void recursivelyFlattenTree(Node *treeNode, list<MachineNode> *controlStructure, int controlStructureIndex,
                            bool processKid, bool processSiblings)
{
    MachineNode controlStructureNode;

    controlStructureNode.defaultLabel = treeNode->value;
    if (treeNode->value == "gamma" || treeNode->value == GAMMA_STD_LABEL)
    {
        controlStructureNode.isGamma = true;
        controlStructureNode.defaultLabel = "gamma";
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "Y")
    {
        controlStructureNode.isY = true;
        controlStructureNode.defaultLabel = "Y";
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value.compare(0, 6, "<STR:'") == 0)
    {
        controlStructureNode.isString = true;
        controlStructureNode.stringValue = treeNode->value.substr(6);
        controlStructureNode.stringValue = controlStructureNode.stringValue.substr(0,
                                                                                   controlStructureNode.stringValue.length() -
                                                                                       2);
        controlStructureNode.defaultLabel = controlStructureNode.stringValue;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value.compare(0, 4, "<ID:") == 0)
    {
        controlStructureNode.isName = true;
        controlStructureNode.nameValue = treeNode->value.substr(4);
        controlStructureNode.nameValue = controlStructureNode.nameValue.substr(0,
                                                                               controlStructureNode.nameValue.length() -
                                                                                   1);
        controlStructureNode.defaultLabel = controlStructureNode.nameValue;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value.compare(0, 5, "<INT:") == 0)
    {
        controlStructureNode.isInt = true;
        string intString = treeNode->value.substr(5);
        intString = intString.substr(0,
                                     intString.length() -
                                         1);
        controlStructureNode.intValue = atoi(intString.c_str());
        controlStructureNode.defaultLabel = intString;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "<true>" || treeNode->value == "<false>")
    {
        controlStructureNode.isBoolean = true;
        controlStructureNode.defaultLabel = treeNode->value == "<true>" ? "true" : "false";
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "<nil>")
    {
        controlStructureNode.isTuple = true;
        controlStructureNode.defaultLabel = "nil";
        controlStructureNode.numberOfElementsInTauTuple = 0;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "<dummy>")
    {
        controlStructureNode.isDummy = true;
        controlStructureNode.defaultLabel = "dummy";
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == LAMBDA_STD_LABEL || treeNode->value == "lambda")
    {
        processKid = false;
        controlStructureNode.isLambda = true;
        int numberOfBoundVariables = 0;
        if (treeNode->leftChild->value == ",")
        {
            Node *boundVariableNode = treeNode->leftChild->leftChild;
            while (boundVariableNode != nullptr)
            {
                numberOfBoundVariables++;
                string variable = boundVariableNode->value.substr(4);
                variable = variable.substr(0, variable.length() - 1);
                controlStructureNode.boundVariables.push_back(variable);
                boundVariableNode = boundVariableNode->rightSibling;
            }
        }
        else
        {
            numberOfBoundVariables++;
            string variable = treeNode->leftChild->value.substr(4);
            variable = variable.substr(0, variable.length() - 1);
            controlStructureNode.boundVariables.push_back(variable);
        }
        controlStructureNode.indexOfBodyOfLambda = numberOfControlStructures++;
        controlStructureNode.numberOfElementsInTauTuple = numberOfBoundVariables;
        string boundVariables;
        for (int i = 0; i < numberOfBoundVariables; i++)
        {
            boundVariables += controlStructureNode.boundVariables[i] + ", ";
        }
        controlStructureNode.defaultLabel =
            "Lambda with bound variables(" + boundVariables + ") and body(" +
            std::to_string(controlStructureNode.indexOfBodyOfLambda) + ")";
        controlStructure->push_back(controlStructureNode);
        list<MachineNode> *controlStructureOfLambda = new list<MachineNode>;
        recursivelyFlattenTree(treeNode->leftChild->rightSibling, controlStructureOfLambda,
                               controlStructureNode.indexOfBodyOfLambda, true, true);
    }
    else if (treeNode->value == "->")
    {
        processKid = false;
        MachineNode trueNode;
        MachineNode falseNode;
        MachineNode betaNode;
        betaNode.isConditional = true;
        trueNode.isConditional = true;
        falseNode.isConditional = true;
        betaNode.defaultLabel = "BetaNode";
        trueNode.defaultLabel = "trueNode";
        falseNode.defaultLabel = "falseNode";
        trueNode.indexOfBodyOfLambda = numberOfControlStructures++;
        falseNode.indexOfBodyOfLambda = numberOfControlStructures++;
        betaNode.indexOfBodyOfLambda = controlStructureIndex;
        list<MachineNode> *controlStructureOfTrueNode = new list<MachineNode>;
        recursivelyFlattenTree(treeNode->leftChild->rightSibling, controlStructureOfTrueNode,
                               trueNode.indexOfBodyOfLambda, true, false);
        list<MachineNode> *controlStructureOfFalseNode = new list<MachineNode>;
        recursivelyFlattenTree(treeNode->leftChild->rightSibling->rightSibling, controlStructureOfFalseNode,
                               falseNode.indexOfBodyOfLambda, true, false);
        controlStructure->push_back(trueNode);
        controlStructure->push_back(falseNode);
        controlStructure->push_back(betaNode);
        recursivelyFlattenTree(treeNode->leftChild, controlStructure,
                               controlStructureIndex, true, false);
    }
    else if (treeNode->value == "not" || treeNode->value == "neg")
    {
        controlStructureNode.isUnaryOperator = true;
        controlStructureNode.operatorStringValue = treeNode->value;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "aug" || treeNode->value == "or" || treeNode->value == "&" ||
             treeNode->value == "gr" || treeNode->value == "ge" || treeNode->value == "ls" ||
             treeNode->value == "le" || treeNode->value == "eq" || treeNode->value == "ne" ||
             treeNode->value == "+" || treeNode->value == "-" || treeNode->value == "*" ||
             treeNode->value == "/" || treeNode->value == "**")
    {
        controlStructureNode.isBinaryOperator = true;
        controlStructureNode.operatorStringValue = treeNode->value;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "tau")
    {
        processKid = false;
        controlStructureNode.isTau = true;
        int numberOfElementsInTuple = 0;
        Node *tauElementNode = treeNode->leftChild;
        do
        {
            numberOfElementsInTuple++;
            tauElementNode = tauElementNode->rightSibling;
        } while (tauElementNode != nullptr);
        controlStructureNode.numberOfElementsInTauTuple = numberOfElementsInTuple;
        controlStructureNode.defaultLabel =
            "TAU[" + std::to_string(controlStructureNode.numberOfElementsInTauTuple) + "]";
        controlStructure->push_back(controlStructureNode);
        tauElementNode = treeNode->leftChild;
        do
        {
            MachineNode tupleElementNode;
            if (tauElementNode->value.compare(0, 6, "<STR:'") == 0)
            {
                tupleElementNode.isString = true;
                tupleElementNode.stringValue = tauElementNode->value.substr(6);
                tupleElementNode.stringValue = tupleElementNode.stringValue.substr(0,
                                                                                   tupleElementNode.stringValue.length() -
                                                                                       2);
                tupleElementNode.defaultLabel = tupleElementNode.stringValue;
                controlStructure->push_back(tupleElementNode);
            }
            else if (tauElementNode->value.compare(0, 4, "<ID:") == 0)
            {
                tupleElementNode.isName = true;
                tupleElementNode.nameValue = tauElementNode->value.substr(4);
                tupleElementNode.nameValue = tupleElementNode.nameValue.substr(0,
                                                                               tupleElementNode.nameValue.length() -
                                                                                   1);
                tupleElementNode.defaultLabel = tupleElementNode.nameValue;
                controlStructure->push_back(tupleElementNode);
            }
            else if (tauElementNode->value.compare(0, 5, "<INT:") == 0)
            {
                tupleElementNode.isInt = true;
                string intString = tauElementNode->value.substr(5);
                intString = intString.substr(0,
                                             intString.length() -
                                                 1);
                tupleElementNode.intValue = atoi(intString.c_str());
                tupleElementNode.defaultLabel = intString;
                controlStructure->push_back(tupleElementNode);
            }
            else if (tauElementNode->value == "<true>" || tauElementNode->value == "<false>")
            {
                tupleElementNode.isBoolean = true;
                tupleElementNode.defaultLabel = tauElementNode->value == "<true>" ? "true" : "false";
                controlStructure->push_back(tupleElementNode);
            }
            else if (tauElementNode->value == "gamma" || tauElementNode->value == GAMMA_STD_LABEL)
            {
                tupleElementNode.isGamma = true;
                tupleElementNode.defaultLabel = "gamma";
                controlStructure->push_back(tupleElementNode);
                recursivelyFlattenTree(tauElementNode->leftChild, controlStructure, controlStructureIndex, true, true);
            }
            else if (tauElementNode->value == "aug" || tauElementNode->value == "or" ||
                     tauElementNode->value == "&" || tauElementNode->value == "gr" ||
                     tauElementNode->value == "ge" || tauElementNode->value == "ls" ||
                     tauElementNode->value == "le" || tauElementNode->value == "eq" ||
                     tauElementNode->value == "ne" || tauElementNode->value == "+" ||
                     tauElementNode->value == "-" || tauElementNode->value == "*" ||
                     tauElementNode->value == "/" || tauElementNode->value == "**")
            {
                tupleElementNode.isBinaryOperator = true;
                tupleElementNode.operatorStringValue = tauElementNode->value;
                controlStructure->push_back(tupleElementNode);
                recursivelyFlattenTree(tauElementNode->leftChild, controlStructure, controlStructureIndex, true, true);
            }
            else
            {
                recursivelyFlattenTree(tauElementNode, controlStructure, controlStructureIndex, true, false);
            }
            tauElementNode = tauElementNode->rightSibling;
        } while (tauElementNode != nullptr);
    }
    else if (treeNode->value == ",")
    {
        controlStructureNode.isComma = true;
        controlStructure->push_back(controlStructureNode);
    }
    else if (treeNode->value == "true" || treeNode->value == "false")
    {
        controlStructureNode.isBoolean = true;
        controlStructure->push_back(controlStructureNode);
    }

    if (processKid && treeNode->leftChild != nullptr)
    {
        recursivelyFlattenTree(treeNode->leftChild, controlStructure, controlStructureIndex, true, true);
    }

    if (processSiblings && treeNode->rightSibling != nullptr)
    {
        recursivelyFlattenTree(treeNode->rightSibling, controlStructure, controlStructureIndex, true, true);
    }
}

// Make Control Structures -------------------------------------------------------------------
void flattenStandardizedTree()
{
    //    cout << "\n\nGoing to flattenStandardizedTree now!\n\n";
    if (!parserStack.empty())
    {
        Node *treeRoot = parserStack.top();
        // cout << "\n\nBefore pointer declare\n\n";
        list<MachineNode> *controlStructure = new list<MachineNode>;
        // cout << "\n\n after pointer declare\n\n";
        recursivelyFlattenTree(treeRoot, controlStructure, 0, true, true);
    }
}

// Build CSE Machiene ------------------------------------------------------------------------------------

EnvironmentNode *environments[1200];

stack<MachineNode> cseMachineControl; // the Control stack of the CSE machine
stack<MachineNode> cseMachineStack;   // the "Stack" stack of values of the CSE machine

EnvironmentNode *currentEnvironment = new EnvironmentNode;

int environmentCounter = 0;

void initializeCSEMachine()
{
    // initialize environment with the primitive environment (PE / e0)
    currentEnvironment->environmentIndex = 0;
    currentEnvironment->parentEnvironment = NULL;
    currentEnvironment->previousEnvironment = NULL;
    environments[environmentCounter++] = currentEnvironment;

    // initialize control.
    // push the first token as the e0 environment variable
    MachineNode e0 = MachineNode();
    e0.isEnvironmentMarker = true;
    e0.environmentMarkerIndex = 0;
    e0.defaultLabel = "e0";
    cseMachineControl.push(e0);
    // push the 0th control structure's elements ------
    std::list<MachineNode>::const_iterator iterator;
    for (iterator = controlStructures[0].begin(); iterator != controlStructures[0].end(); ++iterator)
    {
        // add all nodes in first control structure to CSe machiene control stack
        MachineNode controlStructureToken = *iterator;
        cseMachineControl.push(controlStructureToken);
    }

    // initialize stack with e0 as well (push e0 to stack)
    cseMachineStack.push(e0);
}

void recursivelyPrintTree(Node *node, string indentDots)
{
    // cout<<"\nPrinting tree for: "<<node->label<<"\n";
    cout << indentDots + node->value << "\n";
    if (node->leftChild != NULL)
    {
        // cout<<"\nPrinting firstKid tree for: "<<node->label<<"\n";
        recursivelyPrintTree(node->leftChild, indentDots + ".");
    }
    if (node->rightSibling != NULL)
    {
        // cout<<"\nPrinting nextSibling tree for: "<<node->label<<"\n";
        recursivelyPrintTree(node->rightSibling, indentDots);
    }
    // cout<<"\nDONE! Printing tree for: "<<node->label<<"\n";
}

void recursivelyPrintTreeNode(Node *node, string indentDots)
{
    // cout<<"\nPrinting tree for: "<<node->label<<"\n";
    cout << indentDots + node->value << "\n";
    if (node->leftChild != NULL)
    {
        // cout<<"\nPrinting firstKid tree for: "<<node->label<<"\n";
        recursivelyPrintTree(node->leftChild, indentDots + "(-.#.-)");
    }
    // cout<<"\nDONE! Printing tree for: "<<node->label<<"\n";
}

void printTree()
{
    // cout << "\n\nGoing to print the tree now!\n\n";
    if (!parserStack.empty())
    {
        // cout << "\n\nThis is supposed to be the only tree below!\n";
        Node *treeRoot = parserStack.top();
        recursivelyPrintTree(treeRoot, "");
    }
}

void printString(std::string stringToPrint)
{
    for (size_t i = 0; i < stringToPrint.length(); i++)
    {
        if (stringToPrint.at(i) == '\\' && stringToPrint.at(i + 1) == 'n')
        {
            cout << "\n";
            i++;
        }
        else if (stringToPrint.at(i) == '\\' && stringToPrint.at(i + 1) == 't')
        {
            cout << "\t";
            i++;
        }
        else
        {
            cout << stringToPrint.at(i);
        }
    }
}

void processCSEMachine()
{
    cout << "Process Cse Machiene ---------" << endl;
    // get first element in control stack -----
    MachineNode controlTop = cseMachineControl.top();
    cseMachineControl.pop();

    //    cout << "\n\n Control's top is: " << controlTop.defaultLabel;

    if (controlTop.isInt || controlTop.isString || controlTop.isBoolean ||
        controlTop.isDummy)
    { // CSE rule 1 for ints, booleans, dummy and strings -- add exactly what given
        cseMachineStack.push(controlTop);
    }
    else if (controlTop.isY)
    {
        cseMachineStack.push(controlTop);
    }
    else if (controlTop.isTuple)
    { // CSE rule 1 for 'nil', which can be the only tuple in a control structure
        cseMachineStack.push(controlTop);
    }
    else if (controlTop.isName)
    { // CSE rule 1 for variables
        controlTop.isName = false;
        EnvironmentNode *environmentWithVariableValue = currentEnvironment;
        MachineNode boundedValuesNode;
        bool variableValueFound = false;
        int indexOfBoundVariable = 0;

        while (environmentWithVariableValue != NULL)
        {
            //            cout << "\n\nlooking for " << controlTop.nameValue << " in environment " <<
            //            environmentWithVariableValue->environmentIndex;
            boundedValuesNode = environmentWithVariableValue->boundedValuesNode;
            for (int i = 0; i < boundedValuesNode.boundVariables.size(); i++)
            {
                if (boundedValuesNode.boundVariables[i] == controlTop.nameValue)
                {
                    indexOfBoundVariable = i;
                    variableValueFound = true;
                    break;
                }
            }
            if (variableValueFound)
            {
                break;
            }
            else
            {
                environmentWithVariableValue = environmentWithVariableValue->parentEnvironment;
            }
        }

        if (!variableValueFound)
        {
            // it could be a built-in function defined in the PE [e0]
            if (controlTop.nameValue == "Print" || controlTop.nameValue == "Conc" ||
                controlTop.nameValue == "Istuple" || controlTop.nameValue == "Isinteger" ||
                controlTop.nameValue == "Istruthvalue" || controlTop.nameValue == "Isstring" ||
                controlTop.nameValue == "Isfunction" || controlTop.nameValue == "Isdummy" ||
                controlTop.nameValue == "Stem" || controlTop.nameValue == "Stern" || controlTop.nameValue == "Order" ||
                controlTop.nameValue == "ItoS")
            {
                controlTop.isBuiltInFunction = true;
                controlTop.defaultLabel = controlTop.nameValue;
                cseMachineStack.push(controlTop);
            }
            else
            {
                cout << "\n\nERROR! Value for bound variable '" << controlTop.nameValue << "' not found in environment tree! DIE!\n\n";
                exit(0);
            }
        }
        else
        {
            //            cout << "\n\n Value of " << controlTop.nameValue << " is= ";
            controlTop = environmentWithVariableValue->boundedValuesNode.tupleElements[indexOfBoundVariable];
            //            cout << controlTop.defaultLabel << "\n\n";
            cseMachineStack.push(controlTop);
        }
    }
    else if (controlTop.isEnvironmentMarker)
    { // CSE rule 5
        MachineNode stackTop = cseMachineStack.top();
        cseMachineStack.pop();
        if (!stackTop.isEnvironmentMarker)
        {
            MachineNode stackTopEnvironmentVariable = cseMachineStack.top();
            cseMachineStack.pop();
            if (!stackTopEnvironmentVariable.isEnvironmentMarker ||
                (controlTop.environmentMarkerIndex != stackTopEnvironmentVariable.environmentMarkerIndex))
            {
                cout << "\n ERROR in resolving environment variables on control and stack! Die now! \n";
                exit(0);
            }
            cseMachineStack.push(stackTop);
        }
        else
        {
            if (controlTop.environmentMarkerIndex != stackTop.environmentMarkerIndex)
            {
                cout << "\n ERROR in resolving environment variables on control and stack! Die now! \n";
                exit(0);
            }
        }
        currentEnvironment = environments[controlTop.environmentMarkerIndex]->previousEnvironment;
    }
    else if (controlTop.isLambda)
    {                                                                             // CSE rule 2
        controlTop.environmentMarkerIndex = currentEnvironment->environmentIndex; // index of environment in which this lambda holds
        cseMachineStack.push(controlTop);
    }
    else if (controlTop.isGamma)
    { // CSE rule 3 & 4
        MachineNode result = MachineNode();
        MachineNode operatorNode = cseMachineStack.top();
        cseMachineStack.pop();
        MachineNode firstOperand = cseMachineStack.top();
        cseMachineStack.pop();
        if (operatorNode.isUnaryOperator || operatorNode.isUnaryOperator)
        { // CSE rule 3
            if (operatorNode.isUnaryOperator)
            {
                if (operatorNode.operatorStringValue == "neg")
                {
                    if (!firstOperand.isInt)
                    {
                        cout << "\n Operand is not int to apply 'neg', EXIT! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = true;
                        result.intValue = -firstOperand.intValue;
                        result.defaultLabel = std::to_string(result.intValue);
                    }
                }
                else if (operatorNode.operatorStringValue == "not")
                {
                    if (!firstOperand.isBoolean)
                    {
                        cout << "\n Operand is not boolean to apply 'not', EXIT! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isBoolean = true;
                        if (firstOperand.defaultLabel == "true")
                        {
                            result.defaultLabel = "false";
                        }
                        else if (firstOperand.defaultLabel == "false")
                        {
                            result.defaultLabel = "true";
                        }
                    }
                }
                cseMachineStack.push(result);
            }
            else if (operatorNode.isBinaryOperator)
            {
                MachineNode secondOperand = cseMachineStack.top();
                cseMachineStack.pop();
                if (operatorNode.operatorStringValue == "**")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for ** operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = true;
                        result.intValue = pow(firstOperand.intValue, secondOperand.intValue);
                        result.defaultLabel = std::to_string(result.intValue);
                    }
                }
                else if (operatorNode.operatorStringValue == "*")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for * operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = true;
                        result.intValue = firstOperand.intValue * secondOperand.intValue;
                        result.defaultLabel = std::to_string(result.intValue);
                    }
                }
                else if (operatorNode.operatorStringValue == "aug")
                {
                    if (!firstOperand.isTuple)
                    {
                        cout << "\n first Operand is not a tuple for 'aug' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isTuple = true;
                        result.numberOfElementsInTauTuple = firstOperand.numberOfElementsInTauTuple + 1;

                        if (firstOperand.numberOfElementsInTauTuple == 0)
                        { // if the first operand is nil
                            result.tupleElements.push_back(secondOperand);
                        }
                        else
                        {
                            result.tupleElements = firstOperand.tupleElements;
                            result.tupleElements.push_back(secondOperand);
                        }
                        result.defaultLabel = "TupleOfSize=" + std::to_string(result.numberOfElementsInTauTuple);
                    }
                }
                else if (operatorNode.operatorStringValue == "-")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for - operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = true;
                        result.intValue = firstOperand.intValue - secondOperand.intValue;
                        result.defaultLabel = std::to_string(result.intValue);
                    }
                }
                else if (operatorNode.operatorStringValue == "+")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for + operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = true;
                        result.intValue = firstOperand.intValue + secondOperand.intValue;
                        result.defaultLabel = std::to_string(result.intValue);
                    }
                }
                else if (operatorNode.operatorStringValue == "/")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for '/' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = true;
                        result.intValue = firstOperand.intValue / secondOperand.intValue;
                        result.defaultLabel = std::to_string(result.intValue);
                    }
                }
                else if (operatorNode.operatorStringValue == "gr")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for 'gr' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        result.defaultLabel = firstOperand.intValue > secondOperand.intValue ? "true" : "false";
                    }
                }
                else if (operatorNode.operatorStringValue == "ge")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for 'ge' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        result.defaultLabel = firstOperand.intValue >= secondOperand.intValue ? "true" : "false";
                    }
                }
                else if (operatorNode.operatorStringValue == "ls")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for 'ls' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        result.defaultLabel = firstOperand.intValue < secondOperand.intValue ? "true" : "false";
                    }
                }
                else if (operatorNode.operatorStringValue == "le")
                {
                    if (!firstOperand.isInt || !secondOperand.isInt)
                    {
                        cout << "\n operands not int for 'le' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        result.defaultLabel = firstOperand.intValue <= secondOperand.intValue ? "true" : "false";
                    }
                }
                else if (operatorNode.operatorStringValue == "eq")
                {
                    if (!((!firstOperand.isInt || !secondOperand.isInt) ||
                          (!firstOperand.isBoolean || !secondOperand.isBoolean) ||
                          (!firstOperand.isString || !secondOperand.isString)))
                    {
                        cout << "\n operands not of same type for 'eq' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        if (firstOperand.isInt)
                        {
                            result.defaultLabel = firstOperand.intValue == secondOperand.intValue ? "true" : "false";
                        }
                        else if (firstOperand.isBoolean)
                        {
                            result.defaultLabel =
                                firstOperand.defaultLabel == secondOperand.defaultLabel ? "true" : "false";
                        }
                        else if (firstOperand.isString)
                        {
                            result.defaultLabel =
                                firstOperand.stringValue == secondOperand.stringValue ? "true" : "false";
                        }
                    }
                }
                else if (operatorNode.operatorStringValue == "ne")
                {
                    if (!((!firstOperand.isInt || !secondOperand.isInt) ||
                          (!firstOperand.isBoolean || !secondOperand.isBoolean) ||
                          (!firstOperand.isString || !secondOperand.isString)))
                    {
                        cout << "\n operands not of same type for 'ne' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        if (firstOperand.isInt)
                        {
                            result.defaultLabel = firstOperand.intValue != secondOperand.intValue ? "true" : "false";
                        }
                        else if (firstOperand.isBoolean)
                        {
                            result.defaultLabel =
                                firstOperand.defaultLabel != secondOperand.defaultLabel ? "true" : "false";
                        }
                        else if (firstOperand.isString)
                        {
                            result.defaultLabel =
                                firstOperand.stringValue != secondOperand.stringValue ? "true" : "false";
                        }
                    }
                }
                else if (operatorNode.operatorStringValue == "or")
                {
                    if (!firstOperand.isBoolean || !secondOperand.isBoolean)
                    {
                        cout << "\n operands are not boolean for 'or' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        result.defaultLabel = (firstOperand.defaultLabel == "true" ||
                                               secondOperand.defaultLabel == "true")
                                                  ? "true"
                                                  : "false";
                    }
                }
                else if (operatorNode.operatorStringValue == "&")
                {
                    if (!firstOperand.isBoolean || !secondOperand.isBoolean)
                    {
                        cout << "\n operands are not boolean for '&' operation! exiting! \n";
                        exit(0);
                    }
                    else
                    {
                        result.isInt = false;
                        result.isBoolean = true;
                        result.defaultLabel = (firstOperand.defaultLabel == "true" &&
                                               secondOperand.defaultLabel == "true")
                                                  ? "true"
                                                  : "false";
                    }
                }
                cseMachineStack.push(result);
            }
        }
        else if (operatorNode.isLambda)
        { // CSE rule 4

            // cout << "\n Lambda2 \n";
            // add new lambda's environment variable to control
            MachineNode newEnvironmentVariableForCurrentLambda = MachineNode();
            newEnvironmentVariableForCurrentLambda.isEnvironmentMarker = true;
            newEnvironmentVariableForCurrentLambda.environmentMarkerIndex = environmentCounter++;
            newEnvironmentVariableForCurrentLambda.defaultLabel =
                "e" + std::to_string(newEnvironmentVariableForCurrentLambda.environmentMarkerIndex);
            cseMachineControl.push(newEnvironmentVariableForCurrentLambda);
            // cout << "\n Lambda3 \n";

            // update currentEnvironment
            EnvironmentNode *newEnvironmentForCurrentLambda = new EnvironmentNode();
            newEnvironmentForCurrentLambda->parentEnvironment = environments[operatorNode.environmentMarkerIndex];
            newEnvironmentForCurrentLambda->previousEnvironment = currentEnvironment;
            currentEnvironment = newEnvironmentForCurrentLambda;
            newEnvironmentForCurrentLambda->environmentIndex = newEnvironmentVariableForCurrentLambda.environmentMarkerIndex;
            newEnvironmentForCurrentLambda->boundedValuesNode = MachineNode();
            newEnvironmentForCurrentLambda->boundedValuesNode.boundVariables = operatorNode.boundVariables;
            environments[newEnvironmentForCurrentLambda->environmentIndex] = newEnvironmentForCurrentLambda;

            // We have separate cases here instead of just assigning
            // newEnvironmentForCurrentLambda->boundedValuesNode = firstOperand
            // because we need to have the boundVariables and the tupleElements in the same boundedValuesNode.

            if (operatorNode.boundVariables.size() ==
                1)
            { // only one bound variable, then the firstOperand is stored as it is in the tupleElements
                // first operand could be int/string/tuple
                newEnvironmentForCurrentLambda->boundedValuesNode.tupleElements.push_back(firstOperand);
            }
            else
            { // there are multiple variable bindings, so the firstOperand must be a tuple and that is what we assign
                // CSE Rule 11 (n-ary function)
                newEnvironmentForCurrentLambda->boundedValuesNode.tupleElements = firstOperand.tupleElements;
            }

            //            cout << "\n\nNew environment[" + std::to_string(newEnvironmentForCurrentLambda->environmentIndex) +
            //                    "] created with parent environment[" +
            //                    std::to_string(operatorNode.environmentMarkerIndex) + "], bound variables are:\n";
            //            for (int i = 0; i < newEnvironmentForCurrentLambda->boundedValuesNode.boundVariables.size(); i++) {
            //                cout << "\n" << newEnvironmentForCurrentLambda->boundedValuesNode.boundVariables[i] << "= " <<
            //                newEnvironmentForCurrentLambda->boundedValuesNode.tupleElements[i].defaultLabel <<
            //                "\n\n";
            //            }

            // cout << "\n Lambda4 \n";

            // add new lambda environment variable to stack
            cseMachineStack.push(newEnvironmentVariableForCurrentLambda);

            // cout << "\n Lambda5 \n";
            // add lambda's control structure to control
            std::list<MachineNode>::const_iterator iterator;
            for (iterator = controlStructures[operatorNode.indexOfBodyOfLambda].begin();
                 iterator != controlStructures[operatorNode.indexOfBodyOfLambda].end(); ++iterator)
            {
                MachineNode controlStructureToken = *iterator;
                cseMachineControl.push(controlStructureToken);
            }
            // cout << "\n Lambda6 \n";
        }
        else if (operatorNode.isY)
        { // CSE rule 12 (applying Y)
            firstOperand.isYF = true;
            firstOperand.isLambda = false;
            cseMachineStack.push(firstOperand);
        }
        else if (operatorNode.isYF)
        { // CSE rule 13 (applying f.p.)
            cseMachineStack.push(firstOperand);
            cseMachineStack.push(operatorNode);
            MachineNode lambdaNode = operatorNode;
            lambdaNode.isYF = false;
            lambdaNode.isLambda = true;
            cseMachineStack.push(lambdaNode);
            MachineNode gammaNode = MachineNode();
            gammaNode.isGamma = true;
            gammaNode.defaultLabel = "gamma";
            cseMachineControl.push(gammaNode);
            cseMachineControl.push(gammaNode);
        }
        else if (operatorNode.isBuiltInFunction)
        {
            if (operatorNode.defaultLabel == "Print")
            {
                if (firstOperand.isBoolean)
                {
                    cout << "print Boolean ----------- ";
                    cout << firstOperand.defaultLabel;
                }
                else if (firstOperand.isInt)
                {
                    cout << "print Integer ----------- ";
                    cout << firstOperand.intValue;
                }
                else if (firstOperand.isString)
                {
                    cout << "print String ----------- ";
                    printString(firstOperand.stringValue);
                }
                else if (firstOperand.isDummy)
                {
                    // Do nothing
                }
                else if (firstOperand.isTuple)
                {
                    if (firstOperand.tupleElements.size() == 0)
                    {
                        cout << "nil"; // empty tuple
                    }
                    else
                    {
                        cout << "(";
                        for (int i = 0; i < firstOperand.tupleElements.size(); i++)
                        {
                            if (firstOperand.tupleElements[i].isBoolean)
                            {
                                cout << firstOperand.tupleElements[i].defaultLabel;
                            }
                            else if (firstOperand.tupleElements[i].isInt)
                            {
                                cout << firstOperand.tupleElements[i].intValue;
                            }
                            else if (firstOperand.tupleElements[i].isString)
                            {
                                printString(firstOperand.tupleElements[i].stringValue);
                            }
                            if (i + 1 != firstOperand.tupleElements.size())
                            {
                                cout << ", ";
                            }
                        }
                        cout << ")";
                    }
                }
                else if (firstOperand.isLambda)
                {
                    cout << "[lambda closure: " + firstOperand.boundVariables[0] + ": " +
                                std::to_string(firstOperand.indexOfBodyOfLambda) + "]";
                }
                else
                {
                    cout << "\n\n ERROR! I don't know how to PRINT the value on stack= " + firstOperand.defaultLabel + "\n\n";
                    exit(0);
                }
            }
            else if (operatorNode.defaultLabel == "Conc")
            {
                cseMachineControl.pop(); // to pop out the second gamma node
                MachineNode secondOperand = cseMachineStack.top();
                cseMachineStack.pop();
                result.isString = true;
                result.stringValue = firstOperand.stringValue + secondOperand.stringValue;
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "Order")
            {
                if (!firstOperand.isTuple)
                {
                    cout << "\n\n Error! can't apply 'Order' to a datatype other than tuple! DIE NO! \n\n ";
                    exit(0);
                }
                else
                {
                    result.isInt = true;
                    result.intValue = firstOperand.numberOfElementsInTauTuple;
                    result.defaultLabel = std::to_string(result.intValue);
                    cseMachineStack.push(result);
                }
            }
            else if (operatorNode.defaultLabel == "Stem")
            {
                result.isString = true;
                result.stringValue = firstOperand.stringValue[0];
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "Stern")
            {
                result.isString = true;
                result.stringValue = firstOperand.stringValue.substr(1);
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "Isstring")
            {
                result.isBoolean = true;
                result.defaultLabel = firstOperand.isString ? "true" : "false";
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "Istuple")
            {
                result.isBoolean = true;
                result.defaultLabel = firstOperand.isTuple ? "true" : "false";
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "Isinteger")
            {
                result.isBoolean = true;
                result.defaultLabel = firstOperand.isInt ? "true" : "false";
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "ItoS")
            {
                if (!firstOperand.isInt)
                {
                    cout << "ERROR! operand to ItoS is not Int! DIE NOW!";
                    exit(0);
                }
                result.isString = true;
                result.defaultLabel = std::to_string(firstOperand.intValue);
                result.stringValue = std::to_string(firstOperand.intValue);
                cseMachineStack.push(result);
            }
            else if (operatorNode.defaultLabel == "Istruthvalue")
            {
                result.isBoolean = true;
                result.defaultLabel = firstOperand.isBoolean ? "true" : "false";
                cseMachineStack.push(result);
            }
            else
            {
                cout << "\n\n AYO!! I haven't defined the behavior of the function= " + operatorNode.defaultLabel + "\n\n";
                exit(0);
            }
        }
        else if (operatorNode.isTuple)
        { //  CSE rule 10 for Tuple selection
            result = operatorNode.tupleElements[firstOperand.intValue - 1];
            cseMachineStack.push(result);
        }
    }
    else if (controlTop.isBinaryOperator)
    { // CSE rule 6
        MachineNode result = MachineNode();
        MachineNode operatorNode = controlTop;
        MachineNode firstOperand = cseMachineStack.top();
        cseMachineStack.pop();
        MachineNode secondOperand = cseMachineStack.top();
        cseMachineStack.pop();
        if (operatorNode.operatorStringValue == "**")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for '**' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = true;
                result.intValue = pow(firstOperand.intValue, secondOperand.intValue);
                result.defaultLabel = std::to_string(result.intValue);
            }
        }
        else if (operatorNode.operatorStringValue == "*")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for '*' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = true;
                result.intValue = firstOperand.intValue * secondOperand.intValue;
                result.defaultLabel = std::to_string(result.intValue);
            }
        }
        else if (operatorNode.operatorStringValue == "aug")
        {
            if (!firstOperand.isTuple)
            {
                cout << "\n first Operand is not a tuple for 'aug' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isTuple = true;
                result.numberOfElementsInTauTuple = firstOperand.numberOfElementsInTauTuple + 1;

                if (firstOperand.numberOfElementsInTauTuple == 0)
                { // if the first operand is nil
                    result.tupleElements.push_back(secondOperand);
                }
                else
                {
                    result.tupleElements = firstOperand.tupleElements;
                    result.tupleElements.push_back(secondOperand);
                }
                result.defaultLabel = "TupleOfSize=" + std::to_string(result.numberOfElementsInTauTuple);
            }
        }
        else if (operatorNode.operatorStringValue == "-")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for '-' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = true;
                result.intValue = firstOperand.intValue - secondOperand.intValue;
                result.defaultLabel = std::to_string(result.intValue);
            }
        }
        else if (operatorNode.operatorStringValue == "+")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for '+' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = true;
                result.intValue = firstOperand.intValue + secondOperand.intValue;
                result.defaultLabel = std::to_string(result.intValue);
            }
        }
        else if (operatorNode.operatorStringValue == "/")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for '/' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = true;
                result.intValue = firstOperand.intValue / secondOperand.intValue;
                result.defaultLabel = std::to_string(result.intValue);
            }
        }
        else if (operatorNode.operatorStringValue == "gr")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for 'gr' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                result.defaultLabel = firstOperand.intValue > secondOperand.intValue ? "true" : "false";
            }
        }
        else if (operatorNode.operatorStringValue == "ge")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for 'ge' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                result.defaultLabel = firstOperand.intValue >= secondOperand.intValue ? "true" : "false";
            }
        }
        else if (operatorNode.operatorStringValue == "ls")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for 'ls' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                result.defaultLabel = firstOperand.intValue < secondOperand.intValue ? "true" : "false";
            }
        }
        else if (operatorNode.operatorStringValue == "le")
        {
            if (!firstOperand.isInt || !secondOperand.isInt)
            {
                cout << "\n operands not int for 'le' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                result.defaultLabel = firstOperand.intValue <= secondOperand.intValue ? "true" : "false";
            }
        }
        else if (operatorNode.operatorStringValue == "eq")
        {
            if (!((!firstOperand.isInt || !secondOperand.isInt) ||
                  (!firstOperand.isBoolean || !secondOperand.isBoolean) ||
                  (!firstOperand.isString || !secondOperand.isString)))
            {
                cout << "\n operands not of same type for 'eq' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                if (firstOperand.isInt)
                {
                    result.defaultLabel = firstOperand.intValue == secondOperand.intValue ? "true" : "false";
                }
                else if (firstOperand.isBoolean)
                {
                    result.defaultLabel =
                        firstOperand.defaultLabel == secondOperand.defaultLabel ? "true" : "false";
                }
                else if (firstOperand.isString)
                {
                    result.defaultLabel =
                        firstOperand.stringValue == secondOperand.stringValue ? "true" : "false";
                }
            }
        }
        else if (operatorNode.operatorStringValue == "ne")
        {
            if (!((!firstOperand.isInt || !secondOperand.isInt) ||
                  (!firstOperand.isBoolean || !secondOperand.isBoolean) ||
                  (!firstOperand.isString || !secondOperand.isString)))
            {
                cout << "\n operands not of same type for 'ne' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                if (firstOperand.isInt)
                {
                    result.defaultLabel = firstOperand.intValue != secondOperand.intValue ? "true" : "false";
                }
                else if (firstOperand.isBoolean)
                {
                    result.defaultLabel =
                        firstOperand.defaultLabel != secondOperand.defaultLabel ? "true" : "false";
                }
                else if (firstOperand.isString)
                {
                    result.defaultLabel =
                        firstOperand.stringValue != secondOperand.stringValue ? "true" : "false";
                }
            }
        }
        else if (operatorNode.operatorStringValue == "or")
        {
            if (!firstOperand.isBoolean || !secondOperand.isBoolean)
            {
                cout << "\n operands are not boolean for 'or' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                result.defaultLabel = (firstOperand.defaultLabel == "true" || secondOperand.defaultLabel == "true")
                                          ? "true"
                                          : "false";
            }
        }
        else if (operatorNode.operatorStringValue == "&")
        {
            if (!firstOperand.isBoolean || !secondOperand.isBoolean)
            {
                cout << "\n operands are not boolean for '&' operation! exiting! \n";
                exit(0);
            }
            else
            {
                result.isInt = false;
                result.isBoolean = true;
                result.defaultLabel = (firstOperand.defaultLabel == "true" && secondOperand.defaultLabel == "true")
                                          ? "true"
                                          : "false";
            }
        }
        cseMachineStack.push(result);
    }
    else if (controlTop.isUnaryOperator)
    { // CSE rule 7
        MachineNode result = MachineNode();
        MachineNode operatorNode = controlTop;
        MachineNode firstOperand = cseMachineStack.top();
        cseMachineStack.pop();
        if (operatorNode.operatorStringValue == "neg")
        {
            if (!firstOperand.isInt)
            {
                cout << "\n Operand is not int to apply 'neg', EXIT! \n";
                exit(0);
            }
            else
            {
                result.isInt = true;
                result.intValue = -firstOperand.intValue;
                result.defaultLabel = std::to_string(result.intValue);
            }
        }
        else if (operatorNode.operatorStringValue == "not")
        {
            if (!firstOperand.isBoolean)
            {
                cout << "\n Operand is not boolean to apply not, EXIT! \n";
                exit(0);
            }
            else
            {
                result.isBoolean = true;
                if (firstOperand.defaultLabel == "true")
                {
                    result.defaultLabel = "false";
                }
                else if (firstOperand.defaultLabel == "false")
                {
                    result.defaultLabel = "true";
                }
            }
        }
        cseMachineStack.push(result);
    }
    else if (controlTop.isConditional)
    { // CSE rule 8
        MachineNode booleanNode = cseMachineStack.top();
        cseMachineStack.pop();
        MachineNode falseNode = cseMachineControl.top();
        cseMachineControl.pop();
        MachineNode trueNode = cseMachineControl.top();
        cseMachineControl.pop();
        int controlStructureIndexOfChosenConditional;
        if (booleanNode.defaultLabel == "true")
        {
            // choose the true control structure
            controlStructureIndexOfChosenConditional = trueNode.indexOfBodyOfLambda;
        }
        else if (booleanNode.defaultLabel == "false")
        {
            // choose the true control structure
            controlStructureIndexOfChosenConditional = falseNode.indexOfBodyOfLambda;
        }
        // push the 0th control structure's elements
        std::list<MachineNode>::const_iterator iterator;
        for (iterator = controlStructures[controlStructureIndexOfChosenConditional].begin();
             iterator != controlStructures[controlStructureIndexOfChosenConditional].end(); ++iterator)
        {
            MachineNode controlStructureToken = *iterator;
            cseMachineControl.push(controlStructureToken);
        }
    }
    else if (controlTop.isTau)
    { // CSE rule 9 for Tau tuple formation on CSE's stack structure
        int numberOfTupleElements = controlTop.numberOfElementsInTauTuple;
        // TODO: This checking for if the popped elements are environmentMarkers and working around it was added to handle the 'recurs.1'
        // program. In that program, the tau selection didn't have enough elements for it on the stack. Is this the actual way to do it?
        stack<MachineNode> environmentVariablesToBePushedBackToStack;
        while (numberOfTupleElements > 0 && !cseMachineStack.empty())
        {
            MachineNode poppedStackElement = cseMachineStack.top();
            cseMachineStack.pop();
            if (!poppedStackElement.isEnvironmentMarker)
            {
                numberOfTupleElements--;
                controlTop.tupleElements.push_back(poppedStackElement);
            }
            else
            {
                environmentVariablesToBePushedBackToStack.push(poppedStackElement);
            }
        }
        controlTop.isTau = false;
        controlTop.isTuple = true;
        controlTop.defaultLabel = "TupleOfSize=" + std::to_string(controlTop.tupleElements.size());
        controlTop.numberOfElementsInTauTuple = controlTop.tupleElements.size();
        while (!environmentVariablesToBePushedBackToStack.empty())
        {
            cseMachineStack.push(environmentVariablesToBePushedBackToStack.top());
            environmentVariablesToBePushedBackToStack.pop();
        }
        cseMachineStack.push(controlTop);
    }
}

void runCSEMachine()
{
    initializeCSEMachine();
    cout << "Intialized CSE Machiene Sucess  ---------- " << endl;
    while (!cseMachineControl.empty())
    {
        processCSEMachine(); // process the value on top of the control stack one by one
        // according to the rules of the CSE machine
    }
}

void runAllProcess()
{
    // Combine all process and build cse machiene
}

// ----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << "-ast file_name" << endl;
        return 1;
    }

    string option = argv[1];
    string filename = argv[2];

    if (option != "-ast")
    {
        cerr << "Error: Unknown option '" << option << "'. Use '-ast' to print AST." << endl;
        return 1;
    }
    ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    string line;

    // Read the file line by line
    while (getline(file, line))
    {
        Token token;
        vector<Token> tokenList;
        getNextToken(line, tokenList);

        for (size_t i = 0; i < tokenList.size(); ++i)
        {
            const Token &token = tokenList[i];
            // Process token
            switch (token.type)
            {
            case IDENTIFIER:
                // cout << "<IDENTIFIER>: " << token.value << endl;
                tokenQueue.push(token.value);
                break;
            case INTEGER:
                // cout << "<INTEGER>: " << token.value << endl;
                tokenQueue.push(token.value);
                break;
            case OPERATOR:
                // cout << "<OPERATOR>: " << token.value << endl;
                tokenQueue.push(token.value);
                break;
            case STRING:
                // cout << "<STRING>: " << token.value << endl;
                tokenQueue.push(token.value);
                break;
            case SPACES:
                // Ignore spaces
                break;
            case PUNCTUATION:
                // cout << "<PUNCTUATION>:" << token.value << endl;
                tokenQueue.push(token.value);
                break;
            default:
                cout << "Unknown token type" << endl;
            }
        }
    }

    // Close the file // Close the fil
    printAST();
    convertASTToStandardizedTree();
    flattenStandardizedTree();
    runCSEMachine();
    cout << cseMachineStack.top().intValue << endl;

    cout << "\n";

    file.close();

    return 0;
}
