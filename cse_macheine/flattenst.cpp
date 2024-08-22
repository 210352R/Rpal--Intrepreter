#include "parser.hpp"
#include "lexicon.hpp"
#include "asttost.hpp"
#include "flattenst.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stack>
#include <list>

vector<list<CSEMachineNode>> controlStructures(150); // each controlStructure would be a list of CSEMachineNodes
int numberOfControlStructures = 1;

void recursivelyFlattenTree(Node *treeNode, list<CSEMachineNode> *controlStructure, int controlStructureIndex,
                            bool processKid, bool processSiblings)
{
    //    cout << "\n in recursivelyFlattenTree for node: " << treeNode->label << ", controlStructure: " <<
    //    controlStructureIndex << " and size=" << controlStructure->size();
    CSEMachineNode controlStructureNode = CSEMachineNode();

    controlStructureNode.defaultLabel = treeNode->value;
    if (treeNode->value == "gamma" || treeNode->value == GAMMA_STD_LABEL)
    {
        controlStructureNode.isGamma = true;
        controlStructureNode.defaultLabel = "gamma";
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n it's a gamma!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "Y")
    {
        controlStructureNode.isY = true;
        controlStructureNode.defaultLabel = "Y";
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n it's a Y!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
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
        //        cout << "\n it's a string!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
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
        //        cout << "\n it's an identifier!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value.compare(0, 5, "<INT:") == 0)
    {
        controlStructureNode.isInt = true;
        string intString = treeNode->value.substr(5);
        // cout<<"\n intString= "<<intString<<" length= "<<intString.length();
        intString = intString.substr(0,
                                     intString.length() -
                                         1);
        // cout<<"\n intString= "<<intString;
        controlStructureNode.intValue = atoi(intString.c_str());
        controlStructureNode.defaultLabel = intString;
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n it's an integer!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "<true>" || treeNode->value == "<false>")
    {
        controlStructureNode.isBoolean = true;
        controlStructureNode.defaultLabel = treeNode->value == "<true>" ? "true" : "false";
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n it's a truthValue!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "<nil>")
    {
        controlStructureNode.isTuple = true;
        controlStructureNode.defaultLabel = "nil";
        controlStructureNode.numberOfElementsInTauTuple = 0;
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n it's nil!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "<dummy>")
    {
        controlStructureNode.isDummy = true;
        controlStructureNode.defaultLabel = "dummy";
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n it's nil!";
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == LAMBDA_STD_LABEL || treeNode->value == "lambda")
    {
        //        cout << "\n it's a lambda!";
        processKid = false;
        controlStructureNode.isLambda = true;
        int numberOfBoundVariables = 0;
        if (treeNode->leftChild->value == ",")
        {
            //            cout << "\nIt's a comma node! bound variables!\n";
            Node *boundVariableNode = treeNode->leftChild->leftChild;
            while (boundVariableNode != NULL)
            {
                numberOfBoundVariables++;
                string variable = boundVariableNode->value.substr(
                    4); // bound variables will always start with <ID: and end with >
                variable = variable.substr(0, variable.length() - 1);
                controlStructureNode.boundVariables.push_back(variable);
                boundVariableNode = boundVariableNode->rightSibling;
            }
        }
        else
        { // only one bound variable, which is first child (leftChild)
            numberOfBoundVariables++;
            //            cout << "\nthe bound variable for this lambda= " << treeNode->firstKid->label << "\n";
            string variable = treeNode->leftChild->value.substr(
                4); // bound variables will always start with <ID: and end with >
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
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
        list<CSEMachineNode> *controlStructureOfLambda = new list<CSEMachineNode>;
        recursivelyFlattenTree(treeNode->leftChild->rightSibling, controlStructureOfLambda,
                               controlStructureNode.indexOfBodyOfLambda, true, true);
    }
    else if (treeNode->value == "->")
    {
        //        cout << "\n\n ****** Handle CONDITIONAL! ****** \n\n";
        processKid = false;
        CSEMachineNode trueNode = CSEMachineNode();
        CSEMachineNode falseNode = CSEMachineNode();
        CSEMachineNode betaNode = CSEMachineNode();
        betaNode.isConditional = true;
        trueNode.isConditional = true;
        falseNode.isConditional = true;
        betaNode.defaultLabel = "BetaNode";
        trueNode.defaultLabel = "trueNode";
        falseNode.defaultLabel = "falseNode";
        trueNode.indexOfBodyOfLambda = numberOfControlStructures++;
        falseNode.indexOfBodyOfLambda = numberOfControlStructures++;
        betaNode.indexOfBodyOfLambda = controlStructureIndex;
        list<CSEMachineNode> *controlStructureOfTrueNode = new list<CSEMachineNode>;
        recursivelyFlattenTree(treeNode->leftChild->rightSibling, controlStructureOfTrueNode,
                               trueNode.indexOfBodyOfLambda, true, false);
        list<CSEMachineNode> *controlStructureOfFalseNode = new list<CSEMachineNode>;
        recursivelyFlattenTree(treeNode->leftChild->rightSibling->rightSibling, controlStructureOfFalseNode,
                               falseNode.indexOfBodyOfLambda, true, false);
        controlStructure->push_back(trueNode);
        controlStructure->push_back(falseNode);
        controlStructure->push_back(betaNode);
        recursivelyFlattenTree(treeNode->leftChild, controlStructure,
                               controlStructureIndex, true, false);
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "not" || treeNode->value == "neg")
    { // convert unary operators to standardized form
        //        cout << "\n it's a " << treeNode->label;
        controlStructureNode.isUnaryOperator = true;
        controlStructureNode.operatorStringValue = treeNode->value;
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "aug" || treeNode->value == "or" || treeNode->value == "&" ||
             treeNode->value == "gr" ||
             treeNode->value == "ge" || treeNode->value == "ls" || treeNode->value == "le" ||
             treeNode->value == "eq" ||
             treeNode->value == "ne" || treeNode->value == "+" || treeNode->value == "-" ||
             treeNode->value == "*" ||
             treeNode->value == "/" || treeNode->value == "**")
    {
        //        cout << "\n it's a " << treeNode->label;
        controlStructureNode.isBinaryOperator = true;
        controlStructureNode.operatorStringValue = treeNode->value;
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "tau")
    {
        //        cout << "\n\n ****** Handle TAU! ****** \n\n";
        processKid = false;
        controlStructureNode.isTau = true;
        int numberOfElementsInTuple = 0;
        Node *tauElementNode = treeNode->leftChild;
        do
        {
            numberOfElementsInTuple++;
            tauElementNode = tauElementNode->rightSibling;
        } while (tauElementNode != NULL);
        controlStructureNode.numberOfElementsInTauTuple = numberOfElementsInTuple;
        controlStructureNode.defaultLabel =
            "TAU[" + std::to_string(controlStructureNode.numberOfElementsInTauTuple) + "]";
        controlStructure->push_back(controlStructureNode);
        tauElementNode = treeNode->leftChild;
        do
        {
            CSEMachineNode tupleElementNode = CSEMachineNode();
            if (tauElementNode->value.compare(0, 6, "<STR:'") == 0)
            {
                tupleElementNode.isString = true;
                tupleElementNode.stringValue = tauElementNode->value.substr(6);
                tupleElementNode.stringValue = tupleElementNode.stringValue.substr(0,
                                                                                   tupleElementNode.stringValue.length() -
                                                                                       2);
                tupleElementNode.defaultLabel = tupleElementNode.stringValue;
                //                cout << "\n it's a string!";
                controlStructure->push_back(tupleElementNode);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
            }
            else if (tauElementNode->value.compare(0, 4, "<ID:") == 0)
            {
                tupleElementNode.isName = true;
                tupleElementNode.nameValue = tauElementNode->value.substr(4);
                tupleElementNode.nameValue = tupleElementNode.nameValue.substr(0,
                                                                               tupleElementNode.nameValue.length() -
                                                                                   1);
                tupleElementNode.defaultLabel = tupleElementNode.nameValue;
                //                cout << "\n it's an identifier!";
                controlStructure->push_back(tupleElementNode);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
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
                //                cout << "\n it's an integer!";
                controlStructure->push_back(tupleElementNode);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
            }
            else if (tauElementNode->value == "<true>" || tauElementNode->value == "<false>")
            {
                tupleElementNode.isBoolean = true;
                tupleElementNode.defaultLabel = tauElementNode->value == "<true>" ? "true" : "false";
                //                cout << "\n it's a truthValue!";
                controlStructure->push_back(tupleElementNode);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
            }
            else if (tauElementNode->value == "gamma" || tauElementNode->value == GAMMA_STD_LABEL)
            {
                tupleElementNode.isGamma = true;
                //                cout << "\n it's a gamma!";
                tupleElementNode.defaultLabel = "gamma";
                controlStructure->push_back(tupleElementNode);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
                recursivelyFlattenTree(tauElementNode->leftChild, controlStructure, controlStructureIndex, true, true);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
            }
            else if (tauElementNode->value == "aug" || tauElementNode->value == "or" ||
                     tauElementNode->value == "&" ||
                     tauElementNode->value == "gr" ||
                     tauElementNode->value == "ge" || tauElementNode->value == "ls" ||
                     tauElementNode->value == "le" ||
                     tauElementNode->value == "eq" ||
                     tauElementNode->value == "ne" || tauElementNode->value == "+" || tauElementNode->value == "-" ||
                     tauElementNode->value == "*" ||
                     tauElementNode->value == "/" || tauElementNode->value == "**")
            {
                //                cout << "\n it's a " << tauElementNode->label;
                tupleElementNode.isBinaryOperator = true;
                tupleElementNode.operatorStringValue = tauElementNode->value;
                controlStructure->push_back(tupleElementNode);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
                recursivelyFlattenTree(tauElementNode->leftChild, controlStructure, controlStructureIndex, true, true);
                //                cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " <<
                // controlStructure->size();
            }
            else
            {
                //                cout << "\n it's a " << tauElementNode->label;
                recursivelyFlattenTree(tauElementNode, controlStructure, controlStructureIndex, true, false);
            }
            tauElementNode = tauElementNode->rightSibling;
        } while (tauElementNode != NULL);

        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == ",")
    {
        //        cout << "\n\n ****** Handle CommaNode! ****** \n\n";
        controlStructureNode.isComma = true;
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    else if (treeNode->value == "true" || treeNode->value == "false")
    {
        controlStructureNode.isBoolean = true;
        controlStructure->push_back(controlStructureNode);
        //        cout << "\n size of controlStructure '" << controlStructureIndex << "' is= " << controlStructure->size();
    }
    controlStructures[controlStructureIndex] = *controlStructure;

    if (processKid && treeNode->leftChild != NULL)
    {
        recursivelyFlattenTree(treeNode->leftChild, controlStructure, controlStructureIndex, true, true);
    }

    if (processSiblings && treeNode->rightSibling != NULL)
    {
        recursivelyFlattenTree(treeNode->rightSibling, controlStructure, controlStructureIndex, true, true);
    }
}

void flattenStandardizedTree()
{
    //    cout << "\n\nGoing to flattenStandardizedTree now!\n\n";
    if (!parserStack.empty())
    {
        Node *treeRoot = parserStack.top();
        // cout << "\n\nBefore pointer declare\n\n";
        list<CSEMachineNode> *controlStructure = new list<CSEMachineNode>;
        // cout << "\n\n after pointer declare\n\n";
        recursivelyFlattenTree(treeRoot, controlStructure, 0, true, true);
    }
}