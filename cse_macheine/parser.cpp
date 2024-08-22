#include "parser.hpp"
#include "lexicon.hpp"
#include <fstream>
#include <iostream>
#include <stack>

using namespace std;

const string FCN_FORM_LABEL = "function_form";

const string GAMMA_STD_LABEL = "Gamma";
const string LAMBDA_STD_LABEL = "Lambda";

stack<Node *> parserStack;

using namespace std;

void buildTree(string nodeLabel, int noOfTreesToPopAndMakeChildren)
{
    // cout << "\n# Going to build the node: '" << nodeLabel << "' in tree!";
    Node *treeNode = new Node;
    treeNode->value = nodeLabel;
    treeNode->rightSibling = NULL;
    Node *treeNodePtr = NULL; // which will point to the first child of children (if any) of the newly added node
    if (noOfTreesToPopAndMakeChildren > 0 && parserStack.empty())
    {
        cout << "\n\nERROR! Something went wrong in AST generation! Program will die now!\n\n";
        exit(0);
    }
    while (noOfTreesToPopAndMakeChildren > 0 && !parserStack.empty())
    {
        if (treeNodePtr != NULL)
        {
            // cout << "\n# Node '" << trees.top()->label << "' to be the child of tree: '" << nodeLabel <<
            //"', and left-sibling of '" << treeNodePtr->label;
            parserStack.top()->rightSibling = treeNodePtr;
            treeNodePtr = parserStack.top();
        }
        else
        {
            treeNodePtr = parserStack.top();
            // cout << "\n# Node '" << treeNodePtr->label << "' to be the child of tree: '" << nodeLabel << "'";
        }
        parserStack.pop();
        noOfTreesToPopAndMakeChildren--;
    }
    treeNode->leftChild = treeNodePtr;
    // cout << "\n# Addinde: '" << nodeLabel << "'";
    parserStack.push(treeNode);
    return;
}

void fn_E(ifstream &file);

void fn_D(ifstream &file);

void readNextToken(ifstream &file, string token);

/*
 * The procedure for the Vl non-terminal.
 */
int fn_Vl(ifstream &file, int identifiersReadBefore, bool isRecursiveCall)
{
    // cout << "\nVl!";
    buildTree("<ID:" + NT + ">", 0);
    readNextToken(file, IDENTIFIER_TOKEN);
    if (NT.compare(",") == 0)
    {
        readNextToken(file, ",");
        identifiersReadBefore += 1;
        identifiersReadBefore = fn_Vl(file, identifiersReadBefore, true);
    }
    int identifiersInVList = identifiersReadBefore + 1;
    if (!isRecursiveCall && identifiersInVList > 1)
    {
        // cout << "\nBefore calling buildTree in Vl\n";
        // cout << "\nidentifiersInVList= " << identifiersInVList << ", and trees are of number: " << trees.size();
        buildTree(",", identifiersInVList);
    }
    return identifiersReadBefore;
}

/*
 * The procedure for the Vb non-terminal.
 */
void fn_Vb(ifstream &file)
{
    // cout << "\nVb!";
    if (NT.compare("(") == 0)
    {
        readNextToken(file, "(");
        bool isVl = false;
        if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0)
        {
            fn_Vl(file, 0, false);
            isVl = true;
        }
        readNextToken(file, ")");
        if (!isVl)
        {
            // cout << "\nBefore calling buildTree in Vb\n";
            buildTree("()", 0);
        }
    }
    else if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0)
    {
        buildTree("<ID:" + NT + ">", 0);
        readNextToken(file, IDENTIFIER_TOKEN);
    }
}

/*
 * The procedure for the Db non-terminal.
 */
void fn_Db(ifstream &file)
{
    // cout << "\nDb!";
    if (NT.compare("(") == 0)
    {
        readNextToken(file, "(");
        fn_D(file);
        readNextToken(file, ")");
    }
    else if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0)
    {
        buildTree("<ID:" + NT + ">", 0);
        readNextToken(file, IDENTIFIER_TOKEN);
        if (NT.compare(",") == 0)
        {
            readNextToken(file, ",");
            fn_Vl(file, 1, false);
            readNextToken(file, "=");
            fn_E(file);
            // cout << "\nBefore calling buildTree in Db\n";
            buildTree("=", 2);
        }
        else if (NT.compare("=") == 0)
        {
            readNextToken(file, "=");
            fn_E(file);
            // cout << "\nBefore calling buildTree in Db1\n";
            buildTree("=", 2);
        }
        else
        {
            int n = 1;
            while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || NT.compare("(") == 0)
            {
                fn_Vb(file);
                n++;
            }
            readNextToken(file, "=");
            fn_E(file);
            // cout << "\nBefore calling buildTree in Db2\n";
            buildTree(FCN_FORM_LABEL, n + 1); // n + 'E'
        }
    }
}

/*
 * The procedure for the Dr non-terminal.
 */
void fn_Dr(ifstream &file)
{
    // cout << "\nDr!";
    int isRec = false;
    if (NT.compare("rec") == 0)
    {
        // cout << "\n Going to consume \"REC!\"";
        readNextToken(file, "rec");
        isRec = true;
    }
    fn_Db(file);
    if (isRec)
    {
        // cout << "\nBefore calling buildTree in Dr\n";
        buildTree("rec", 1);
    }
}

/**
 ** The procedure for the Da non-terminal.
 **/
void fn_Da(ifstream &file)
{
    // cout << "\nDa!";
    fn_Dr(file);
    int n = 1;
    while (NT.compare("and") == 0)
    {
        readNextToken(file, "and");
        fn_Dr(file);
        n++;
    }
    if (n > 1)
    {
        // cout << "\nBefore calling buildTree in Da\n";
        buildTree("and", n);
    }
}

/*
 * The procedure for the D non-terminal.
 */
void fn_D(ifstream &file)
{
    // cout << "\nD!";
    fn_Da(file);
    if (NT.compare("within") == 0)
    {

        readNextToken(file, "within");
        fn_D(file);

        buildTree("within", 2);
    }
}

/*
 * The procedure for the Rn non-terminal.
 */
void fn_Rn(ifstream &file)
{
    // cout << "\nRn!";
    if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0)
    {
        // cout << "\n\nbuildTreeNode ID:" + NT + "\n\n";
        buildTree("<ID:" + NT + ">", 0);
        readNextToken(file, IDENTIFIER_TOKEN);
    }
    else if (nextTokenType.compare(STRING_TOKEN) == 0)
    {
        // cout << "\n\nbuildTreeNode STR:" + NT + "\n\n";
        buildTree("<STR:" + NT + ">", 0);
        readNextToken(file, STRING_TOKEN);
    }
    else if (nextTokenType.compare(INTEGER_TOKEN) == 0)
    {
        // cout << "\n\nbuildTreeNode INT:" + NT + "\n\n";
        buildTree("<INT:" + NT + ">", 0);
        readNextToken(file, INTEGER_TOKEN);
    }
    else if (NT.compare("true") == 0 || NT.compare("false") == 0 ||
             NT.compare("nil") == 0 || NT.compare("dummy") == 0)
    {
        buildTree("<" + NT + ">", 0);
        readNextToken(file, NT);
    }
    else if (NT.compare("(") == 0)
    {
        readNextToken(file, "(");
        fn_E(file);
        readNextToken(file, ")");
    }
}

/*
 * The procedure for the R non-terminal.
 */
void fn_R(ifstream &file)
{
    // cout << "\nR!";
    fn_Rn(file);
    while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || nextTokenType.compare(INTEGER_TOKEN) == 0 ||
           nextTokenType.compare(STRING_TOKEN) == 0 || NT.compare("true") == 0 || NT.compare("false") == 0 ||
           NT.compare("nil") == 0 || NT.compare("dummy") == 0 || NT.compare("(") == 0)
    {
        fn_Rn(file);
        buildTree("gamma", 2);
    }
}

/*
 * The procedure for the Ap non-terminal.
 */
void fn_Ap(ifstream &file)
{
    // cout << "\nAp!";
    fn_R(file);
    while (NT.compare("@") ==
           0)
    {
        readNextToken(file, "@");
        buildTree("<ID:" + NT + ">", 0); // the operator which is being inFixed, for example 'Conc'.
        readNextToken(file, IDENTIFIER_TOKEN);
        fn_R(file);
        buildTree("@", 3);
    }
}

/*
 * The procedure for the Af non-terminal.
 */
void fn_Af(ifstream &file)
{
    // cout << "\nAf!";
    fn_Ap(file);
    if (NT.compare("**") == 0)
    {
        readNextToken(file, "**");
        fn_Af(file);
        buildTree("**", 2);
    }
}

/*
 * The procedure for the At non-terminal.
 */
void fn_At(ifstream &file)
{
    // cout << "\nAt!";
    fn_Af(file);
    while (NT.compare("*") == 0 || NT.compare("/") == 0)
    {
        string token = NT; // saving token since call to read will update NT with the next token.
        readNextToken(file, NT);
        fn_Af(file);
        buildTree(token, 2);
    }
}

/*
 * The procedure for the A non-terminal.
 */
void fn_A(ifstream &file)
{
    // cout << "\nA!";
    if (NT.compare("+") == 0)
    {
        readNextToken(file, "+");
        fn_At(file);
    }
    else if (NT.compare("-") == 0)
    {
        readNextToken(file, "-");
        fn_At(file);
        buildTree("neg", 1);
    }
    else
    {
        fn_At(file);
    }
    while (NT.compare("+") == 0 || NT.compare("-") == 0)
    {
        string token = NT; // saving token since call to read will update NT with the next token.
        readNextToken(file, NT);
        fn_At(file);
        buildTree(token, 2);
    }
}

/*
 * The procedure for the Bp non-terminal.
 */
void fn_Bp(ifstream &file)
{
    // cout << "\nBp!";
    fn_A(file);
    if (NT.compare("gr") == 0 || NT.compare(">") == 0)
    {
        readNextToken(file, NT);
        fn_A(file);
        buildTree("gr", 2);
    }
    else if (NT.compare("ge") == 0 || NT.compare(">=") == 0)
    {
        readNextToken(file, NT);
        fn_A(file);
        buildTree("ge", 2);
    }
    else if (NT.compare("ls") == 0 || NT.compare("<") == 0)
    {
        readNextToken(file, NT);
        fn_A(file);
        buildTree("ls", 2);
    }
    else if (NT.compare("le") == 0 || NT.compare("<=") == 0)
    {
        readNextToken(file, NT);
        fn_A(file);
        buildTree("le", 2);
    }
    else if (NT.compare("eq") == 0)
    {
        readNextToken(file, "eq");
        fn_A(file);
        buildTree("eq", 2);
    }
    else if (NT.compare("ne") == 0)
    {
        readNextToken(file, "ne");
        fn_A(file);
        buildTree("ne", 2);
    }
}

/*
 * The procedure for the Bs non-terminal.
 */
void fn_Bs(ifstream &file)
{
    // cout << "\nBs!";
    bool isNeg = false;
    if (NT.compare("not") == 0)
    {
        readNextToken(file, "not");
        isNeg = true;
    }
    fn_Bp(file);
    if (isNeg)
    {
        buildTree("not", 1);
    }
}

/*
 * The procedure for the Bt non-terminal.
 */
void fn_Bt(ifstream &file)
{
    // cout << "\nBt!";
    fn_Bs(file);
    int n = 1;
    while (NT.compare("&") == 0)
    {
        readNextToken(file, "&");
        fn_Bs(file);
        n++;
    }
    if (n > 1)
    {
        buildTree("&", n);
    }
}

/*
 * The procedure for the B non-terminal.
 */
void fn_B(ifstream &file)
{
    // cout << "\nB!";
    fn_Bt(file);
    int n = 1;
    while (NT.compare("or") == 0)
    {
        readNextToken(file, "or");
        fn_Bt(file);
        n++;
    }
    if (n > 1)
    {
        buildTree("or", n);
    }
}

/*
 * The procedure for the Tc non-terminal.
 */
void fn_Tc(ifstream &file)
{
    // cout << "\nTc!";
    fn_B(file);
    if (NT.compare("->") ==
        0)
    {
        readNextToken(file, "->");
        fn_Tc(file);
        readNextToken(file, "|");
        fn_Tc(file);
        buildTree("->", 3);
    }
}

/*
 * The procedure for the Ta non-terminal.
 */
void fn_Ta(ifstream &file)
{
    // cout << "\nTa!";
    fn_Tc(file);
    while (NT.compare("aug") == 0)
    { // left recursion
        readNextToken(file, "aug");
        fn_Tc(file);
        buildTree("aug", 2);
    }
}

/*
 * The procedure for the T non-terminal.
 */
void fn_T(ifstream &file)
{
    // cout << "\nT!";
    fn_Ta(file);
    int n = 1;
    while (NT.compare(",") == 0)
    { // combo of left recursion AND common prefix
        n++;
        readNextToken(file, ",");
        fn_Ta(file);
    }
    if (n != 1)
    {
        buildTree("tau", n);
    }
}

/*
 * The procedure for the Ew non-terminal.
 */
void fn_Ew(ifstream &file)
{
    // cout << "\nEw!";
    fn_T(file);
    if (NT.compare("where") == 0)
    { // common prefix
        // cout << "\n Going to consume \"WHERE!\"";
        readNextToken(file, "where");
        fn_Dr(file);
        buildTree("where", 2);
    }
}

void fn_E(ifstream &file)
{

    // cout << "\nE!";
    int N = 0;
    if (NT.compare("let") == 0)
    {
        readNextToken(file, "let");
        fn_D(file);
        readNextToken(file, "in");
        fn_E(file);
        buildTree("let", 2);
    }
    else if (NT.compare("fn") == 0)
    {
        readNextToken(file, "fn");
        do
        {
            fn_Vb(file);
            N++;
        } while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || NT.compare("(") == 0);
        readNextToken(file, ".");
        fn_E(file);
        buildTree("lambda", N + 1); // number of 'Vb's plus the 'E'
    }
    else
    {
        fn_Ew(file);
    }
}

void readNextToken(ifstream &file, string token)
{
    if (token.compare(NT) != 0 && token.compare(nextTokenType) != 0)
    {
        // cout << "\n\nError! Expected '" << token << "' , but found '" << NT << "' !\n\n";
        throw exception();
    }
    // cout << "\ntoken : '" << token << endl;
    scan(file);
}