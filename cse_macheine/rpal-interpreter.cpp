#include <iostream>
#include <fstream>
#include <cstring>
#include "lexicon.hpp"
#include "parser.hpp"
#include "asttost.hpp"
#include "flattenst.hpp"
#include "cse.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    try
    {
        // cout << "RPAL Interpreter\n";
        if (argc != 2 && argc != 3) // Expecting either one or two arguments
        {
            throw std::invalid_argument("Usage: " + std::string(argv[0]) + " [-ast] file_name");
        }

        bool printAST = false; // Flag to determine if printTree() should be executed
        string filename;

        if (argc == 2)
        {
            filename = argv[1];
        }
        else if (argc == 3 && std::string(argv[1]) == "-ast")
        {
            printAST = true;
            filename = argv[2];
        }
        else
        {
            throw std::invalid_argument("Usage: " + std::string(argv[0]) + " [-ast] file_name");
        }

        ifstream file(filename);

        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open the file.");
        }

        // cout << "File opened successfully!\n";
        scan(file); // Prepare the first token by placing it within 'NT'
        // cout << "Lexicon prepared!\n";
        fn_E(file); // Call the first non-terminal procedure to start parsing
        // cout << "Parsing complete!\n";

        if (checkIfEOF(file))
        {
            if (printAST)
            {
                printTree();
            }
            convertASTToStandardizedTree();
            flattenStandardizedTree();
            runCSEMachine();
            cout << "\n";
        }
        else
        {
            cout << "\n\nERROR! EOF not reached but went through the complete grammar! Will exit now!!\n\n";
        }
    }
    catch (const std::exception &e)
    {
        cerr << "Exception caught: " << e.what() << endl;
        return 1; // Exit with error code
    }

    return 0; // Exit normally
}
