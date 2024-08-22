#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // Include <cctype> for std::isdigit
#include <vector>
#include <queue>
#include <stdlib.h>
#include <stack>

using namespace std;

// For implementing parser Tree --------------------------
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
    string symbols = "+-<>&.@/:=~|$!#%^_[{}\"‘?";
    return symbols.find(c) != string::npos;
}

bool isPunctuation(char c)
{
    std::string punctuations = "().,";
    return punctuations.find(c) != std::string::npos;
}

void getNextToken(string input, vector<Token> &tokenList)
{

    string::iterator currentChar = input.begin();

    // Skip spaces
    while (currentChar != input.end() && (*currentChar == ' ' || *currentChar == '\t' || *currentChar == '\n'))
    {
        ++currentChar;
    }

    // Check for end of input
    if (currentChar == input.end())
    {
        Token token;
        token.type = SPACES;
        token.value = "";
        tokenList.push_back(token);
    }

    // Recognize different token types

    // Recognize Identifiers --------------------
    if (isalpha(*currentChar))
    {
        Token token;
        // Identifier
        token.type = IDENTIFIER;
        while (currentChar != input.end() && (isalnum(*currentChar) || *currentChar == '_'))
        {
            token.value += *currentChar;

            ++currentChar;
        }
        tokenList.push_back(token);
        if (!(isalnum(*currentChar) || *currentChar == '_'))
        {

            // make String again
            string tempToken;
            while (currentChar != input.end())
            {
                tempToken += *currentChar;
                ++currentChar;
            }

            getNextToken(tempToken, tokenList);
        }
    }
    else if (isdigit(*currentChar))
    {
        // Integer
        Token token;
        token.type = INTEGER;
        while (currentChar != input.end() && isdigit(*currentChar))
        {
            token.value += *currentChar;
            ++currentChar;
        }
        tokenList.push_back(token);
        if (!isdigit(*currentChar))
        {
            string tempToken;
            while (currentChar != input.end())
            {
                tempToken += *currentChar;
                ++currentChar;
            }
            if (!tempToken.empty())
            {
                getNextToken(tempToken, tokenList);
            }
        }
    }
    // Check is Operater ------------------------
    else if (isOperatorSymbol(*currentChar))
    {
        // Operator
        Token token;
        token.type = OPERATOR;
        while (currentChar != input.end() && isOperatorSymbol(*currentChar))
        {
            token.value += *currentChar;
            ++currentChar;
        }
        tokenList.push_back(token);
        if (!isOperatorSymbol(*currentChar))
        {
            string tempToken;
            while (currentChar != input.end())
            {
                tempToken += *currentChar;
                ++currentChar;
            }
            if (!tempToken.empty())
            {
                getNextToken(tempToken, tokenList);
            }
        }
    }
    // Identify String ------------------------
    else if (*currentChar == '\'')
    {
        // String
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

    // Identifying Punctuations ------------------------
    else if (isPunctuation(*currentChar))
    {
        // Punctuation
        Token token;
        token.type = PUNCTUATION;
        token.value = *currentChar;
        ++currentChar;
        tokenList.push_back(token);

        if (isPunctuation(*currentChar))
        {
            cout << "Error" << endl;
        }
        string tempToken;
        while (currentChar != input.end())
        {
            tempToken += *currentChar;
            ++currentChar;
        }
        if (!tempToken.empty())
        {
            getNextToken(tempToken, tokenList);
        }
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
        if (integ(root->value))
            cout << "<INT:" << root->value << ">" << endl;
        else if (str(root->value))
            cout << "<STR:" << root->value << ">" << endl;
        else if (ident(root->value))
            cout << "<ID:" << root->value << ">" << endl;
        else
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
    if (i == 0)
    {
        Node *temp = new Node;
        temp->value = s;
        parserStack.push(temp);
        // if(temp->value=="")
        //	myStack.pop();
    }
    else if (i == 1)
    {
        Node *temp = parserStack.top();
        parserStack.pop();
        Node *newHead = new Node;
        newHead->leftChild = temp;
        newHead->value = s;
        parserStack.push(newHead);
    }
    else if (i == 2)
    {
        Node *temp = parserStack.top();
        parserStack.pop();
        parserStack.top()->rightSibling = temp;
        temp = parserStack.top();
        parserStack.pop();
        Node *newRoot = new Node;
        newRoot->value = s;
        newRoot->leftChild = temp;
        parserStack.push(newRoot);
    }
    else
    {
        if (present_specs(s))
        {
            vector<Node *> allNodes;
            for (int p = 1; p < i; ++p)
            {
                allNodes.push_back(parserStack.top());
                parserStack.pop();
            }
            buildTree(s, 1);
            for (int p = 0; p < allNodes.size(); ++p)
            {
                Node *trv = parserStack.top();
                trv = trv->leftChild;
                for (int p = 0; p < allNodes.size(); ++p)
                {
                    trv->rightSibling = allNodes[allNodes.size() - p - 1];
                    trv = trv->rightSibling;
                }
            }
        }
        else if (present_specs(s))
        {
            vector<Node *> threeNodes;
            for (int p = 1; p < i; ++p)
            {
                threeNodes.push_back(parserStack.top());
                parserStack.pop();
            }
            buildTree(s, 1);
            for (int p = 0; p < 2; ++p)
            {
                Node *trv = parserStack.top();
                trv = trv->leftChild;
                for (int p = 0; p < 2; ++p)
                {
                    trv->rightSibling = threeNodes[threeNodes.size() - p - 1];
                    trv = trv->rightSibling;
                }
            }
        }
        else
        {
            vector<Node *> myNodes;
            for (int p = 2; p < i; ++p)
            {
                myNodes.push_back(parserStack.top());
                parserStack.pop();
            }
            buildTree(s, 2);
        }
    }
    // printTree(myStack.top());
    //  cout << "...................." << endl;
}

// scan next token from token Queue -----------------------------------------------------
string scanNextToken()
{
    if (tokenQueue.empty())
    {
        return "Token Queue is Empty !";
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
        cout << "Expected " << Token << " but found " << NextToken << endl;
        exit(1);
    }
    if (integ(Token) || ident(Token) || str(Token))
    {
        cout << "Read  Tokens ----------------------------------------------- " << Token << endl;
        buildTree(Token, 0);
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
        // cout << "LET" << endl;
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

        // cout << "GAMMA" << endl;
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
        // cout << "WITHIN" << endl;
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
        // cout << "AND" << endl;
    }
}

void fn_Dr()
{
    if (NextToken.compare("rec") == 0)
    {
        Read("rec");
        fn_Db();
        buildTree("rec", 1);
    }
    else
        fn_Db();
}

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
            // cout << "FUNCTION_FORM" << endl;
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

// ----------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " file_name -ast" << endl;
        return 1;
    }

    string filename = argv[1];
    string option = argv[2];

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

    string word;

    // Read the file word by word
    while (file >> word)
    {

        Token token;
        vector<Token> tokenList;
        // cout << "Word  :::: -> " << word << endl;
        getNextToken(word, tokenList);

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
    file.close(); // Close the fil
    printAST();

    return 0;
}
