#include <iostream>
#include <fstream>
#include <string>
#include <cctype> // Include <cctype> for std::isdigit
#include <vector>
#include <queue>
using namespace std;

queue<string> tokenQueue2;

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

int main()
{
    ifstream file("testRec"); // Open the file

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
                cout << "<IDENTIFIER>: " << token.value << endl;
                tokenQueue2.push(token.value);
                break;
            case INTEGER:
                cout << "<INTEGER>: " << token.value << endl;
                tokenQueue2.push(token.value);
                break;
            case OPERATOR:
                cout << "<OPERATOR>: " << token.value << endl;
                tokenQueue2.push(token.value);
                break;
            case STRING:
                cout << "<STRING>: " << token.value << endl;
                tokenQueue2.push(token.value);
                break;
            case SPACES:
                // Ignore spaces
                break;
            case PUNCTUATION:
                cout << "<PUNCTUATION>:" << token.value << endl;
                tokenQueue2.push(token.value);
                break;
            case COMMENT:
                cout << "<PUNCTUATION>:" << token.value << endl;
                break;

            default:
                cout << "Unknown token type" << endl;
            }
        }
    }

    file.close(); // Close the file

    // print queue elements
    if (tokenQueue2.empty() == true)
    {
        cout << "Token Queue Is Empty" << endl;
    }
    else
    {
        while (!tokenQueue2.empty())
        {
            cout << tokenQueue2.front() << endl;
            tokenQueue2.pop();
        }
    }
    return 0;
}
