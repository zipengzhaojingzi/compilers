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
    // 词法分析部分
    initKeyMapping();
    initOperMapping();
    initLimitMapping();
    initNode();
    scanner();
    BraMappingError();
    printNodeLink();

    printErrorLink();
    printIdentLink();

    // 语法分析部分
    initGrammer();
    First();
    Follow();
    Select();
    MTable();

    SemanticTreeNode *root = nullptr;
    Analysis(root);
    // 调试输出：打印语法树
    if (root)
    {
        std::cout << "=== 语法树结构 ===" << std::endl;
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
