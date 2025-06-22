#ifndef LL1_PARSER_HPP
#define LL1_PARSER_HPP
#include <string>
#include <set>
#include <vector>
#include <map>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
// #include "Quater.hpp"

class Quater
{
public:
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;

    Quater(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
        : op(op), arg1(arg1), arg2(arg2), result(result) {}

    std::string to_string() const
    {
        std::string formatted_arg1 = arg1.empty() ? "_" : arg1;
        std::string formatted_arg2 = arg2.empty() ? "_" : arg2;
        return "(" + op + ", " + formatted_arg1 + ", " + formatted_arg2 + ", " + result + ")";
    }

    bool is_jump() const { return op[0] == 'j'; }
};

enum class SymbolType
{
    Terminal,    // 终结符
    NonTerminal, // 非终结符
    Epsilon      // 空串
};

class Symbol
{
public:
    SymbolType type;        // 终结符/非终结符/空串
    std::string literal;    // 这个符号的字面量，比如一个非终结符名为 S,A,B e.g.一个终结符名为T_INT,T_xxxx
    std::string real_value; // 这个符号的真实值，比如一个变量名a，一个int数值3

    Symbol(const SymbolType &type = SymbolType::Epsilon,
           const std::string &literal = "",
           const std::string &real_value = "")
        : type(type), literal(literal), real_value(real_value) {}

    std::string to_string() const
    {
        return literal; // 或者任何合适的表示方式
    }

    friend bool operator==(const Symbol &lhs, const Symbol &rhs)
    {
        return lhs.type == rhs.type && lhs.literal == rhs.literal;
        // && lhs.real_value == rhs.real_value;
    }

    friend bool operator<(const Symbol &lhs, const Symbol &rhs)
    {
        if (lhs.type != rhs.type)
        {
            return lhs.type < rhs.type;
        }
        // if (lhs.literal != rhs.literal) {
        return lhs.literal < rhs.literal;
        // }
        // return lhs.real_value < rhs.real_value;
    }

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol)
    {
        if (symbol.type == SymbolType::Epsilon)
        {
            return os << static_cast<int>(symbol.type);
        }
        return os << static_cast<int>(symbol.type) << " " << symbol.literal << " "
                  << (symbol.real_value.empty() ? "NULL" : symbol.real_value);
    }
    friend std::istream &operator>>(std::istream &is, Symbol &symbol)
    {
        int temp;
        is >> temp;
        symbol.type = static_cast<SymbolType>(temp);
        if (symbol.type == SymbolType::Epsilon)
        {
            return is;
        }

        is >> symbol.literal >> symbol.real_value;
        if (symbol.real_value == "NULL")
            symbol.real_value = "";
        return is;
    }
};

struct Production
{
    Symbol lhs;              // 左手边，非终结符
    std::vector<Symbol> rhs; // 右手边，非终结符和终结符的 组合

    Production() {}
    Production(Symbol lhs, std::vector<Symbol> rhs) : lhs(lhs), rhs(rhs) {}
    std::string to_string() const
    {
        std::string res;
        res += lhs.literal + " -> ";
        for (auto &item : rhs)
        {
            res += item.literal;
        }
        return "[" + res + "]";
    }

    friend bool operator==(const Production &lhs, const Production &rhs)
    {
        return lhs.lhs == rhs.lhs && lhs.rhs == rhs.rhs;
    }
    friend bool operator<(const Production &lhs, const Production &rhs)
    {
        if (lhs.lhs < rhs.lhs)
            return true;
        if (rhs.lhs < lhs.lhs)
            return false;
        return lhs.rhs < rhs.rhs;
    }

    friend std::ostream &operator<<(std::ostream &os, Production &production)
    {
        os << production.lhs << " " << production.rhs.size();
        for (auto &symbol : production.rhs)
        {
            os << " " << symbol;
        }
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Production &production)
    {
        size_t size;
        is >> production.lhs >> size;
        production.rhs.clear();
        Symbol symbol;
        for (size_t i = 0; i < size; ++i)
        {
            is >> symbol;
            production.rhs.push_back(symbol);
        }
        return is;
    }
};

class SemanticTreeNode : public Symbol
{
public:
    SemanticTreeNode *parent = nullptr;

    SemanticTreeNode(const Symbol &sym) : Symbol(sym), next_quater_id(0) {}

    SemanticTreeNode(const Symbol &s, SemanticTreeNode *parent = nullptr)
        : Symbol(s), parent(parent), next_quater_id(0)  {}
    bool leaf() { return children.empty(); }

    // 返回添加四元式的ID
    size_t add_quater(const Quater &quater);
    size_t add_quater(const size_t &base_id, const Quater &quater);
    size_t add_quater(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result);
    size_t add_quater(const std::string &op, const std::string &arg1, const std::string &arg2, const size_t &result);

    // 追加一个quaters进来
    void append_quaters(const std::vector<std::pair<size_t, Quater>> &quaters);

public:
    std::vector<SemanticTreeNode *> children;
    std::vector<std::pair<size_t, Quater>> quater_list;

    // 新增：递归打印语法树
    static void printTree(SemanticTreeNode *node, int depth = 0)
    {
        if (!node)
            return;

        // 缩进显示层级
        for (int i = 0; i < depth; ++i)
            std::cout << "|   ";

        // 节点类型标识
        std::string typeMarker;
        if (node->type == SymbolType::Terminal)
        {
            typeMarker = "[T] ";
        }
        else if (node->type == SymbolType::Epsilon)
        {
            typeMarker = "[ε] ";
        }
        else
        {
            typeMarker = "<NT> ";
        }

        std::cout << typeMarker << node->literal << std::endl;

        // 递归子节点
        for (auto &child : node->children)
        {
            printTree(child, depth + 1);
        }
    }

private:
    size_t next_quater_id;
};

#endif