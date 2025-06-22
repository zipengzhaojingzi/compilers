#pragma once
#include "LL1Parser.hpp"
#include <iomanip>
#include <regex>

struct VaribleMeta
{
	std::string type;  // �������ͣ�int��float��char��
	std::string value; // ������ֵ��Ϊ�˼򻯲�����ȫ����Ϊstring������Ҫʹ��ʱ����ת��
};
struct SymbolTableEntry
{
	std::string type;
	std::string value;
	std::vector<std::pair<std::string, SymbolTableEntry>> members; // �� vector ��� map
};

class SemanticAnalyzer
{
public:
	SemanticAnalyzer(SemanticTreeNode *&root) : root(root), next_temp_varible_id(0) {}

	void semantic_analyze();
	void print_intermediate_code();
	void print_variable_table();

private:
	void handle_defalt(SemanticTreeNode *&node);
	void handle_var_declaration(SemanticTreeNode *&node);
	void handle_opt_init(SemanticTreeNode *&node);
	void handle_expression(SemanticTreeNode *&node);
	void handle_simple_expression(SemanticTreeNode *&node);
	void handle_additive_expression(SemanticTreeNode *&node);
	void handle_term(SemanticTreeNode *&node);
	void handle_postfix_expression(SemanticTreeNode *&node);
	void handle_var(SemanticTreeNode *&node);
	void handle_factor(SemanticTreeNode *&node);
	void handle_prefix_expression(SemanticTreeNode *&node);
	void handle_selection_stmt(SemanticTreeNode *&node);
	void handle_iteration_stmt(SemanticTreeNode *&node);
	void handle_opt_expression_stmt(SemanticTreeNode *&node);
	void handle_return_stmt(SemanticTreeNode *&node);
	void handle_default(SemanticTreeNode *&node);
	void handle_struct_definition(SemanticTreeNode *&node);
	void handle_declaration_statement(SemanticTreeNode *&node);
	void handle_declaration(SemanticTreeNode *&node);
	void handle_assignment_or_call(SemanticTreeNode *&node);
	void handle_factor_component(SemanticTreeNode *&node);
	void handle_function_call(SemanticTreeNode *&funcNode, SemanticTreeNode *&callNode);
	void handle_struct_member_list(SemanticTreeNode *node, SymbolTableEntry &structEntry);
	void handle_array_or_struct(SemanticTreeNode *&node);
	void parse_struct_array_initializer(const std::string &initStr,
										std::vector<std::vector<std::string>> &result,
										int expectedArraySize,
										int expectedMemberCount);
	void export_quaternions_to_file(const std::string &filename);

private:
	std::map<std::string, SymbolTableEntry> struct_definitions;

	std::string
	new_temp_varible()
	{
		return "t" + std::to_string(next_temp_varible_id++);
	}

private:
	bool exists_var_declaration(std::string var)
	{
		return varible_table.find(var) != varible_table.end();
	}

private:
	SemanticTreeNode *root;

	std::map<std::string, VaribleMeta> varible_table; // �������洢�Ѿ��������ı���

	std::vector<std::pair<size_t, Quater>> intermediate_code() { return root->quater_list; } // �м���룬һ�����ֱ�ţ�һ����Ԫʽ

	size_t next_temp_varible_id;
};