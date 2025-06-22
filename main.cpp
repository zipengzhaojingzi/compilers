#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iomanip>
#include "LexAnalysis.h"
#include "SynAnalysis.h"
#include "SemanticAnalyzer.hpp"
// #include "asmGenerator.h"

using namespace std;

int main()
{
    // �ʷ���������
    initKeyMapping();
    initOperMapping();
    initLimitMapping();
    initNode();
    scanner();
    BraMappingError();
    printNodeLink();

    printErrorLink();
    printIdentLink();

    // �﷨��������
    initGrammer();
    First();
    Follow();
    Select();
    MTable();

    SemanticTreeNode *root = nullptr;
    Analysis(root);
    // �����������ӡ�﷨��
    if (root)
    {
        std::cout << "=== �﷨���ṹ ===" << std::endl;
        SemanticTreeNode::printTree(root);
        std::cout << "=================" << std::endl;
    }

    SemanticAnalyzer analyzer(root);
    analyzer.semantic_analyze();
    analyzer.print_variable_table();
    analyzer.print_intermediate_code();
    // asmGenerator as;
    // as.parse("intermediateCode.txt", "final_mips.asm");
    // close();
    return 0;
}
