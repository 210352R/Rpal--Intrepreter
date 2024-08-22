#include "parser.hpp"
#include "lexicon.hpp"
#include "asttost.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <stack>
#include <list>

void convertFunctionForm(Node *functionFormNode)
{

    Node *fcnLambdaRightChildNodeHeader = new Node; // the "lambda" right child node header of the final standardized sub-tree
    fcnLambdaRightChildNodeHeader->value = LAMBDA_STD_LABEL;
    fcnLambdaRightChildNodeHeader->rightSibling = NULL;

    list<Node *> fcnVariableList;
    functionFormNode->value = "="; // the "=" header node of the final standardized sub-tree

    Node *temp = functionFormNode->leftChild; // the fcn label left child node of the final standardized sub-tree
    while (temp->rightSibling->rightSibling !=
           NULL)
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
}

void convertInfixOperator(Node *infixOperatorNode)
{

    Node *leftGammaChild = new Node;
    Node *leftLeftOperatorChild = new Node;
    leftLeftOperatorChild->value = infixOperatorNode->leftChild->rightSibling->value;
    leftLeftOperatorChild->rightSibling = infixOperatorNode->leftChild; // E1
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
    //   The standardized lambda expression ";
}

void convertRecExpression(Node *recNode)
{
    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";

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
    functionNameNode->value = recNode->leftChild->value;
    functionNameNode->leftChild = NULL;

    rightRightLambdaChild->leftChild = functionNameNode;
    functionNameNode->rightSibling = recNode->leftChild->rightSibling; // E

    recNode->leftChild->rightSibling = rightGammaChild;

    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";
    //    cout << "\nInside convertRecExpression conversion!\nrecNode ast form after standardizing is:\n";
    //    recursivelyPrintTreeNode(recNode, "");
    //    cout<< "\nThe recNode label is: "<<recNode->label<<"\n";
}

void convertWhereExpression(Node *whereNode)
{
    whereNode->value = GAMMA_STD_LABEL;

    Node *pNode = whereNode->leftChild;
    Node *leftChildLambdaNode = pNode->rightSibling;
    leftChildLambdaNode->value = LAMBDA_STD_LABEL;
    Node *eNode = leftChildLambdaNode->leftChild->rightSibling;

    whereNode->leftChild = leftChildLambdaNode;

    // switch the p and e nodes

    leftChildLambdaNode->rightSibling = eNode;
    leftChildLambdaNode->leftChild->rightSibling = pNode;

    pNode->rightSibling = NULL;
}

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
        // Do not standardize conditionals (optimizations for the CISE machine)
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
    else if (node->value == FCN_FORM_LABEL)
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