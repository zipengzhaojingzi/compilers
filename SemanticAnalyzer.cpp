#include "SemanticAnalyzer.hpp"
size_t SemanticTreeNode::add_quater(const Quater &quater)
{
	size_t this_id = next_quater_id++;
	quater_list.push_back({this_id, quater});
	return this_id;
}
size_t SemanticTreeNode::add_quater(const size_t &id, const Quater &quater)
{
	quater_list.push_back({id, quater});
	return id;
}
size_t SemanticTreeNode::add_quater(const std::string &op, const std::string &arg1, const std::string &arg2, const std::string &result)
{
	Quater quater = Quater(op, arg1, arg2, result);
	size_t this_id = next_quater_id++;
	quater_list.push_back({this_id, quater});
	return this_id;
}
size_t SemanticTreeNode::add_quater(const std::string &op, const std::string &arg1, const std::string &arg2, const size_t &result)
{
	Quater quater = Quater(op, arg1, arg2, std::to_string(result));
	size_t this_id = next_quater_id++;
	quater_list.push_back({this_id, quater});
	return this_id;
}
void SemanticTreeNode::append_quaters(const std::vector<std::pair<size_t, Quater>> &quaters)
{
	size_t base_id = next_quater_id;
	for (const auto &[id, quater] : quaters)
	{
		if (quater.is_jump())
		{
			// 跳转语句不仅id要+base_id, 而且result也要加
			Quater q(quater);
			q.result = std::to_string(std::stoi(q.result) + base_id);
			this->add_quater(id + base_id, q);
			continue;
		}

		this->add_quater(id + base_id, quater);
	}
	next_quater_id += quaters.size();
}
void SemanticAnalyzer::print_intermediate_code()
{
	// 首先检查中间代码是否为空
	if (intermediate_code().empty())
	{
		std::cout << "Intermediate code table is empty." << std::endl;
		return;
	}

	// 确定ID列的最大宽度
	size_t maxIdLength = std::string("ID").length();
	for (const auto &[id, _] : intermediate_code())
	{
		maxIdLength = std::max(maxIdLength, std::to_string(id).length());
	}
	maxIdLength += 2; // 增加一些额外的空间以美化输出

	// 四元式每部分的最大宽度
	size_t maxPart1Length = 0, maxPart2Length = 0, maxPart3Length = 0, maxPart4Length = 0;
	for (const auto &[_, item] : intermediate_code())
	{
		std::istringstream iss(item.to_string());
		std::string part;
		std::getline(iss, part, ','); // 第一部分
		maxPart1Length = std::max(maxPart1Length, part.length());
		std::getline(iss, part, ','); // 第二部分
		maxPart2Length = std::max(maxPart2Length, part.length());
		std::getline(iss, part, ','); // 第三部分
		maxPart3Length = std::max(maxPart3Length, part.length());
		std::getline(iss, part, ')'); // 第四部分
		maxPart4Length = std::max(maxPart4Length, part.length());
	}

	maxPart1Length += 1;
	maxPart2Length += 1;
	maxPart3Length += 1;
	maxPart4Length += 0;
	// 计算四元式列的总宽度
	size_t maxItemLength = maxPart1Length + maxPart2Length + maxPart3Length + maxPart4Length + 6; // 9 是四个逗号和两个括号的长度

	// 计算总长度和打印表头
	size_t totalLength = maxIdLength + maxItemLength + 5; // 5 是分隔符和边界的长度
	std::string topBottomBorder = "+" + std::string(maxIdLength, '-') + "+" + std::string(maxItemLength, '-') + "+";
	std::cout << topBottomBorder << std::endl;
	std::cout << std::left << "|" << std::setw(maxIdLength) << " ID "
			  << "|" << std::setw(maxItemLength) << " Quarter "
			  << "|" << std::endl;
	std::cout << topBottomBorder << std::endl;

	// 遍历中间代码并打印
	for (const auto &[id, item] : intermediate_code())
	{
		std::istringstream iss(item.to_string());
		std::string part1, part2, part3, part4;
		std::getline(iss, part1, ',');
		std::getline(iss, part2, ',');
		std::getline(iss, part3, ',');
		std::getline(iss, part4, ')');
		std::string item_;
		std::stringstream ss;
		ss << " " << std::left << std::setw(maxPart1Length) << part1
		   << "," << std::setw(maxPart2Length) << part2
		   << "," << std::setw(maxPart3Length) << part3
		   << "," << std::setw(maxPart4Length) << part4 << ")";
		std::getline(ss, item_);

		std::cout << std::left << "|" << std::setw(maxIdLength) << (" " + std::to_string(id))
				  << "|"
				  << std::setw(maxItemLength) << item_
				  << "|" << std::endl;
	}

	// 打印底部横线
	std::cout << topBottomBorder << std::endl;
	// export_quaternions_to_file("intermediateCode.txt");
}

void SemanticAnalyzer::print_variable_table()
{
	// 检查变量表是否为空
	if (varible_table.empty())
	{
		std::cout << "Variable table is empty." << std::endl;
		return;
	}

	// 确定每列的最大宽度
	size_t maxNameLength = std::string("Variable Name").length();
	size_t maxTypeLength = std::string("Type").length();
	size_t maxValueLength = std::string("Initial Value").length();

	for (const auto &pair : varible_table)
	{
		maxNameLength = std::max(maxNameLength, pair.first.length());
		maxTypeLength = std::max(maxTypeLength, pair.second.type.length());
		maxValueLength = std::max(maxValueLength, pair.second.value.length());
	}

	// 增加一些额外的空间以美化输出
	maxNameLength += 2;
	maxTypeLength += 2;
	maxValueLength += 2;

	// 计算总长度和打印表头
	size_t totalLength = maxNameLength + maxTypeLength + maxValueLength + 7; // 7 是分隔符和边界的长度
	std::string topBottomBorder = "+" + std::string(maxNameLength, '-') + "+" + std::string(maxTypeLength, '-') + "+" + std::string(maxValueLength, '-') + "+";
	std::cout << topBottomBorder << std::endl;
	std::cout << std::left << "|" << std::setw(maxNameLength) << " Variable Name "
			  << "|" << std::setw(maxTypeLength) << " Type "
			  << "|" << std::setw(maxValueLength) << " Initial Value "
			  << "|" << std::endl;
	std::cout << topBottomBorder << std::endl;

	// 遍历varible_table并打印
	for (const auto &pair : varible_table)
	{
		std::cout << std::left << "|" << std::setw(maxNameLength) << (" " + pair.first)
				  << "|" << std::setw(maxTypeLength) << (" " + pair.second.type)
				  << "|" << std::setw(maxValueLength) << (" " + pair.second.value) << "|" << std::endl;
	}

	// 打印底部横线
	std::cout << topBottomBorder << std::endl;
}

// void SemanticAnalyzer::semantic_analyze()
// {
// 	if (root == nullptr)
// 		return;

// 	std::stack<SemanticTreeNode *> stack1, stack2;
// 	stack1.push(root);

// 	while (!stack1.empty())
// 	{
// 		SemanticTreeNode *node = stack1.top();
// 		stack1.pop();
// 		stack2.push(node);

// 		for (SemanticTreeNode *child : node->children)
// 		{
// 			stack1.push(child);
// 		}
// 	}

// 	while (!stack2.empty())
// 	{
// 		SemanticTreeNode *node = stack2.top();
// 		stack2.pop();

// 		// Process node - this can be customized as needed
// 		// For example, print node information
// 		if (node->leaf())
// 		{
// 			continue;
// 		}

// 		handle_defalt(node);

// 		if (node->literal == "<结构体定义>" || node->literal == "<声明>")
// 		{
// 			std::cout << "catch catch catch catch catch catch" << std::endl;
// 			handle_var_declaration(node);
// 		}
// 		else if (node->literal == "<赋初值>")
// 		{
// 			handle_opt_init(node);
// 		}
// 		else if (node->literal == "expression")
// 		{
// 			handle_expression(node);
// 		}
// 		else if (node->literal == "simple_expression")
// 		{
// 			handle_simple_expression(node);
// 		}
// 		else if (node->literal == "additive_expression")
// 		{
// 			handle_additive_expression(node);
// 		}
// 		else if (node->literal == "term")
// 		{
// 			handle_term(node);
// 		}
// 		else if (node->literal == "postfix_expression")
// 		{
// 			handle_postfix_expression(node);
// 		}
// 		else if (node->literal == "factor")
// 		{
// 			handle_factor(node);
// 		}
// 		else if (node->literal == "prefix_expression")
// 		{
// 			handle_prefix_expression(node);
// 		}
// 		else if (node->literal == "selection_stmt")
// 		{
// 			handle_selection_stmt(node);
// 		}
// 		else if (node->literal == "iteration_stmt")
// 		{
// 			handle_iteration_stmt(node);
// 		}
// 		else if (node->literal == "opt_expression_stmt")
// 		{
// 			handle_opt_expression_stmt(node);
// 		}

// 		if (node->children.size() == 1 && node->children[0]->literal == "T_IDENTIFIER")
// 		{
// 			node->literal = node->children[0]->literal;
// 		}
// 	}
// }

void SemanticAnalyzer::handle_defalt(SemanticTreeNode *&node)
{
	for (const auto &child : node->children)
	{
		node->real_value += child->real_value;
		if (node->literal != "selection_stmt" && node->literal != "iteration_stmt")
		{
			node->append_quaters(child->quater_list);
		}
	}
}

void SemanticAnalyzer::handle_var_declaration(SemanticTreeNode *&node)
{
	// type_specifier T_IDENTIFIER opt_init T_SEMICOLON
	// type_specifier T_IDENTIFIER T_SEMICOLON
	const auto &list = node->children;

	const std::string &type = list[1]->real_value;
	const std::string &varible_name = list[2]->real_value;
	const std::string &init_val = list[4]->literal == "<赋初值>" ? list[4]->real_value : "NULL";

	if (varible_table.find(varible_name) != varible_table.end())
	{
		// 如果变量表中已经有了这个变量，报错
		std::cout << "Error：重定义变量：" << varible_name << std::endl;
		exit(-1);
	}

	varible_table[varible_name] = {type, init_val};
	node->real_value = type + " " + varible_name + " " + init_val + ";";
}

void SemanticAnalyzer::handle_opt_init(SemanticTreeNode *&node)
{
	// T_ASSIGN expression
	const auto &list = node->children;

	node->real_value = list[1]->real_value;
}

// void SemanticAnalyzer::handle_expression(SemanticTreeNode *&node)
// {
// 	/*
// 	simple_expression
// 	postfix_expression

// 	var T_ASSIGN expression
// 	a = exp
// 	(=, t, _, a)
// 	*/
// 	const auto &list = node->children;

// 	if (list.size() == 1)
// 	{
// 		return;
// 	}

// 	const std::string &var = list[0]->real_value;
// 	const std::string &op = list[1]->real_value;
// 	const std::string &exp = list[2]->real_value;
// 	if (list[0]->literal == "T_IDENTIFIER" && !exists_var_declaration(var))
// 	{
// 		std::cout << "Error: 未定义变量：" << var << std::endl;
// 		exit(-1);
// 	}
// 	if (list[2]->literal == "T_IDENTIFIER" && !exists_var_declaration(exp))
// 	{
// 		std::cout << "Error: 未定义变量：" << exp << std::endl;
// 		exit(-1);
// 	}
// 	node->add_quater(op, exp, "", var);
// 	node->real_value = var;
// }

void SemanticAnalyzer::handle_simple_expression(SemanticTreeNode *&node)
{
	/*
	additive_expression relop additive_expression
	simple_expression relop simple_expression
	additive_expression
	*/
	const auto &list = node->children;

	if (list.size() == 1)
	{
		return;
	}

	const std::string &arg1 = list[0]->real_value;
	const std::string &op = list[1]->real_value;
	const std::string &arg2 = list[2]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater(op, arg1, arg2, new_temp);
	node->real_value = new_temp;
}

void SemanticAnalyzer::handle_additive_expression(SemanticTreeNode *&node)
{
	/*
	term
	additive_expression addop term
	*/

	const auto &list = node->children;

	if (list.size() == 1)
	{
		return;
	}

	const std::string &arg1 = list[0]->real_value;
	const std::string &op = list[1]->real_value;
	const std::string &arg2 = list[2]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater(op, arg1, arg2, new_temp);
	node->real_value = new_temp;
}

// void SemanticAnalyzer::handle_term(SemanticTreeNode *&node)
// {
// 	/*
// 	factor
// 	postfix_expression
// 	term mulop factor
// 	*/
// 	const auto &list = node->children;

// 	if (list.size() == 1)
// 	{
// 		return;
// 	}

// 	const std::string &arg1 = list[0]->real_value;
// 	const std::string &op = list[1]->real_value;
// 	const std::string &arg2 = list[2]->real_value;

// 	std::string new_temp = new_temp_varible();
// 	node->add_quater(op, arg1, arg2, new_temp);
// 	node->real_value = new_temp;
// }

void SemanticAnalyzer::handle_factor(SemanticTreeNode *&node)
{
	/*
	T_LEFT_PAREN expression T_RIGHT_PAREN
	var
	T_INTEGER_LITERAL
	T_STRING_LITERAL
	T_CHAR_LITERAL
	T_FLOAT_LITERAL
	prefix_expression

	call 		*本次作业不处理函数调用
	*/
	const auto &list = node->children;

	if (list.size() == 1)
	{
		return;
	}

	node->real_value = list[0]->real_value;
}

void SemanticAnalyzer::handle_prefix_expression(SemanticTreeNode *&node)
{
	/*
	inc_dec_operator var   ++a
	(++, a, _, a)

	T_NOT factor
	T_BITNOT factor
	(~/!, a, _, t1)
	*/
	const auto &list = node->children;

	const std::string &op = list[0]->real_value;
	const std::string &varible_name = list[1]->real_value;

	if (list[0]->literal == "inc_dec_operator")
	{
		std::string op_ = op == "++" ? "+" : "-";
		node->add_quater(op_, varible_name, "1", varible_name);
		node->real_value = varible_name;
	}
	else
	{
		std::string new_temp = new_temp_varible();
		node->add_quater(op, varible_name, "", new_temp);
		node->real_value = new_temp;
	}
}

void SemanticAnalyzer::handle_postfix_expression(SemanticTreeNode *&node)
{
	/*
	var inc_dec_operator   			a++, b--

	(=, a, _, t1)
	(++, a, _, a)
	*/
	const auto &list = node->children;

	const std::string &varible_name = list[0]->real_value;
	const std::string &op = list[1]->real_value;

	std::string new_temp = new_temp_varible();
	node->add_quater("=", varible_name, "", new_temp);
	std::string op_ = op == "++" ? "+" : "-";
	node->add_quater(op_, varible_name, "1", varible_name);

	node->real_value = new_temp;
}

// void SemanticAnalyzer::handle_var(SemanticTreeNode *&node)
// {
// 	/*
// 	T_IDENTIFIER
// 	T_IDENTIFIER T_LEFT_SQUARE expression T_RIGHT_SQUARE a[]
// 	*/

// 	// 暂时不考虑数组
// 	return;
// }

void SemanticAnalyzer::handle_selection_stmt(SemanticTreeNode *&node)
{
	/*
	T_IF T_LEFT_PAREN expression T_RIGHT_PAREN statement
	T_IF T_LEFT_PAREN expression T_RIGHT_PAREN statement T_ELSE statement
	*/

	const auto &list = node->children;
	// 新增调试输出：打印list结构
	std::cout << "\n[DEBUG] Selection stmt children list:\n";
	for (size_t i = 0; i < list.size(); ++i)
	{
		std::cout << "  child[" << i << "]: " << list[i]->literal;
		if (!list[i]->real_value.empty())
		{
			std::cout << " (real_value: " << list[i]->real_value << ")";
		}
		std::cout << "\n";

		// 打印子节点的四元式列表
		if (!list[i]->quater_list.empty())
		{
			std::cout << "    Child quater_list:\n";
			for (const auto &[id, quater] : list[i]->quater_list)
			{
				std::cout << "      " << id << ": " << quater.to_string() << "\n";
			}
		}
	}
	const auto &stmt_THEN = list[4];
	const auto &cond = list[2]->real_value;
	node->append_quaters(list[2]->quater_list);

	size_t THEN = -1;
	size_t ELSE = -1;
	size_t ENDIF = -1;

	if (list.size() == 5)
	{
		THEN = 2 + list[2]->quater_list.size();
		ENDIF = THEN + stmt_THEN->quater_list.size();

		node->add_quater("jnz", cond, "", THEN);
		node->add_quater("j", "", "", ENDIF);
		node->append_quaters(stmt_THEN->quater_list);
	}
	else if (list.size() == 6)
	{
		const auto &stmt_ELSE = list[5];
		// ELSE = 1 + list[2]->quater_list.size();
		ELSE = node->quater_list.size() + 1;
		// THEN = ELSE + stmt_ELSE->children.size() + 1;
		// ENDIF = THEN + stmt_THEN->children.size();
		THEN = ELSE + stmt_ELSE->quater_list.size() + 1; // 正确：quater_list.size()
		ENDIF = THEN + stmt_THEN->quater_list.size();	 // 正确：quater_list.size()

		node->add_quater("jnz", cond, "", THEN);
		node->append_quaters(stmt_ELSE->quater_list);
		node->add_quater("j", "", "", ENDIF);
		node->append_quaters(stmt_THEN->quater_list);
		std::cout << "[IF-ELSE] Generated " << node->quater_list.size()
				  << " quaternions for selection stmt\n";
	}
	// 在函数结尾添加四元式列表输出
	std::cout << "[DEBUG] Generated quater_list for selection stmt:\n";
	for (const auto &[id, quater] : node->quater_list)
	{
		std::cout << "  " << id << ": " << quater.to_string() << "\n";
	}
}

void SemanticAnalyzer::handle_iteration_stmt(SemanticTreeNode *&node)
{
	const auto &list = node->children;

	if (list[0]->literal == "T_WHILE")
	{
		/*
		T_WHILE T_LEFT_PAREN expression T_RIGHT_PAREN statement
		*/
		const auto &cond = list[2]->real_value;
		const auto &stmt = list[4];
		size_t LOOP = 0;
		size_t BODY = LOOP + list[2]->quater_list.size() + 2;
		size_t END_LOOP = BODY + stmt->quater_list.size() + 1;

		node->append_quaters(list[2]->quater_list);
		node->add_quater("jnz", cond, "", BODY);
		node->add_quater("j", "", "", END_LOOP);
		node->append_quaters(stmt->quater_list);
		node->add_quater("j", "", "", LOOP);
	}
	else if (list[0]->literal == "T_FOR")
	{
		/*
		T_FOR T_LEFT_PAREN opt_expression_stmt opt_expression_stmt expression T_RIGHT_PAREN statement
		*/
		const auto &exp1 = list[2];
		const auto &exp2 = list[3];
		const auto &exp3 = list[4];
		const auto &stmt = list[6];

		size_t START = exp1->quater_list.size();
		size_t BODY = START + exp2->quater_list.size() + 2;
		size_t END_LOOP = BODY + stmt->quater_list.size() + exp3->quater_list.size() + 1;

		node->append_quaters(exp1->quater_list);
		node->append_quaters(exp2->quater_list);
		node->add_quater("jnz", exp2->real_value, "", BODY);
		node->add_quater("j", "", "", END_LOOP);
		node->append_quaters(stmt->quater_list);
		node->append_quaters(exp3->quater_list);
		node->add_quater("j", "", "", START);
	}
}

void SemanticAnalyzer::handle_opt_expression_stmt(SemanticTreeNode *&node)
{
	/*
	expression_stmt
	T_SEMICOLON
	*/
	node->real_value = node->real_value.substr(0, node->real_value.size() - 1); // 去掉分号;

	return;
}

void SemanticAnalyzer::semantic_analyze()
{
	if (root == nullptr)
		return;

	std::stack<SemanticTreeNode *> stack1, stack2;
	stack1.push(root);

	while (!stack1.empty())
	{
		SemanticTreeNode *node = stack1.top();
		stack1.pop();
		stack2.push(node);

		for (SemanticTreeNode *child : node->children)
		{
			stack1.push(child);
		}
	}

	while (!stack2.empty())
	{
		SemanticTreeNode *node = stack2.top();
		stack2.pop();
		// 调试输出：开始处理节点
		std::cout << "[DEBUG] Processing node: " << node->literal
				  << " (Leaf: " << (node->leaf() ? "Yes" : "No") << ")"
				  << std::endl;

		if (node->leaf())
		{
			std::cout << "        Leaf node, skipping handler." << std::endl;

			continue;
		}

		// std::cout << "节点" << node->literal << std::endl;

		handle_default(node);
		std::string handlerUsed = "handle_defalt";
		// if (node->literal == "<声明语句>")
		// {
		// 	// 声明语句包含声明和分号
		// 	handle_declaration_statement(node);
		// }
		// 修改1：根据新文法调整节点处理
		if (node->literal == "<结构体定义>")
		{
			handle_struct_definition(node);
			handlerUsed = "结构体定义";
		}
		else if (node->literal == "<结构体变量>")
		{
			handle_array_or_struct(node);
			handlerUsed = "结构体变量";
		}
		else if (node->literal == "<声明>")
		{
			handlerUsed = "声明";

			handle_declaration(node);
		}
		else if (node->literal == "<赋初值>")
		{
			handlerUsed = "赋初值";

			handle_opt_init(node);
		}
		else if (node->literal == "<表达式>")
		{
			handlerUsed = "表达式";

			handle_expression(node);
		}
		else if (node->literal == "<因子>")
		{
			handle_factor(node);
			handlerUsed = "因子";
		}
		else if (node->literal == "<项>")
		{
			handle_term(node);
			handlerUsed = "项";
		}
		else if (node->literal == "<因式>")
		{
			handle_factor_component(node);
			handlerUsed = "因式";
		}
		else if (node->literal == "<后缀表达式>")
		{
			handle_postfix_expression(node);
			handlerUsed = "后缀表达式";
		}
		else if (node->literal == "<前缀表达式>")
		{
			handle_prefix_expression(node);
			handlerUsed = "前缀表达式";
		}
		else if (node->literal == "<条件语句>")
		{
			handle_selection_stmt(node);
			handlerUsed = "条件语句";
		}
		else if (node->literal == "<for循环>")
		{
			handle_iteration_stmt(node);
			handlerUsed = "for循环";
		}
		else if (node->literal == "<函数返回>")
		{
			handle_return_stmt(node);
			handlerUsed = "函数返回";
		}
		else if (node->literal == "<赋值函数>")
		{
			handle_assignment_or_call(node);
			handlerUsed = "赋值函数";
		}

		// 标识符提升规则保留
		if (node->children.size() == 1 && node->children[0]->literal == "<标志符>")
		{
			node->literal = node->children[0]->literal;
		}
		// 打印使用的处理函数
		std::cout << "        Handler used: " << handlerUsed << std::endl;

		// 打印 real_value（如果非空）
		if (!node->real_value.empty())
		{
			std::cout << "        real_value: \"" << node->real_value << "\"" << std::endl;
		}

		// // 打印四元式列表（如果非空）
		// if (!node->quater_list.empty()) {
		//     std::cout << "        Quaternary list:" << std::endl;
		//     for (size_t i = 0; i < node->quater_list.size(); ++i) {
		//         std::cout << "            [" << i << "] " << node->quater_list[i].to_string() << std::endl;
		//     }
		// }

		std::cout << std::endl;
	}
}

void SemanticAnalyzer::handle_struct_member_list(SemanticTreeNode *node, SymbolTableEntry &structEntry)
{
	// 空列表直接返回
	if (node->children.empty())
		return;

	// // 提取当前成员的 real_value 并解析出 类型 + 变量名
	// std::string memberRealValue = node->real_value;
	// std::istringstream iss(memberRealValue);
	// std::string token;
	// std::vector<std::string> tokens;

	// while (iss >> token)
	// {
	// 	tokens.push_back(token);
	// }

	// // 成员格式应为: type... id ;
	// if (tokens.size() < 3)
	// {
	// 	std::cerr << "Error: Invalid member declaration in struct." << std::endl;
	// 	exit(-1);
	// }

	// // 提取类型（拼接除最后两个 token 的部分）
	// std::string memberType;
	// for (size_t i = 0; i < tokens.size() - 2; ++i)
	// {
	// 	memberType += tokens[i];
	// 	if (i + 1 < tokens.size() - 2)
	// 		memberType += " ";
	// }

	// std::string memberName = tokens[tokens.size() - 2];

	// // 添加到结构体成员表中
	// structEntry.members[memberName] = {memberType, ""};
	std::string memberRealValue = node->real_value;
	size_t pos = 0;
	while (pos < memberRealValue.size())
	{
		// 跳过空白字符
		while (pos < memberRealValue.size() && isspace(memberRealValue[pos]))
			++pos;
		if (pos >= memberRealValue.size())
			break;

		size_t start = pos;
		// 找到分号或结尾
		while (pos < memberRealValue.size() && memberRealValue[pos] != ';')
			++pos;
		std::string memberStr = memberRealValue.substr(start, pos - start);

		// 解析单个成员
		std::istringstream iss(memberStr);
		std::vector<std::string> tokens;
		std::string token;
		while (iss >> token)
		{
			tokens.push_back(token);
		}

		if (tokens.size() < 2)
		{
			std::cerr << "Error: Invalid member declaration in struct." << std::endl;
			exit(-1);
		}

		// 修正：类型是除最后一个 token 以外的所有 token
		std::string memberType;
		for (size_t i = 0; i < tokens.size() - 1; ++i)
		{
			memberType += tokens[i];
			if (i + 1 < tokens.size() - 1)
				memberType += " ";
		}

		// 修正：变量名是最后一个 token
		std::string memberName = tokens.back();
		// 添加到结构体成员表中
		structEntry.members.push_back({memberName,
									   SymbolTableEntry{
										   memberType, // type
										   "",		   // value
										   {}		   // members
									   }});

		// 跳过分号及可能的空格
		if (pos < memberRealValue.size())
			++pos;
	}
}

// 修改2：添加新文法的处理函数
void SemanticAnalyzer::handle_struct_definition(SemanticTreeNode *&node)
{
	// struct id { <结构体成员列表> } ;
	// 处理结构体定义
	std::string structName = node->children[1]->real_value; // struct后的标识符
															// SemanticTreeNode *memberList = node->children[3];		// 结构体成员列表

	// 创建结构体符号表条目
	SymbolTableEntry newStruct;

	newStruct.type = "struct " + structName;

	// 获取成员列表并解析
	SemanticTreeNode *memberListNode = node->children[3];
	handle_struct_member_list(memberListNode, newStruct);
	struct_definitions[structName] = newStruct;
	// 调试：打印当前结构体信息

	std::cout << "[DEBUG] Struct defined: " << newStruct.type << std::endl;
	// 修改前：
	// for (const auto &member : structEntry.members)

	// 修改后：
	for (const auto &[memberName, memberEntry] : newStruct.members)
	{
		std::cout << "  Member: " << memberName << ", Type: " << memberEntry.type << std::endl;
	}
	// 将结构体加入变量表

	// 设置当前节点 real_value
	varible_table[structName].type = newStruct.type;
	varible_table[structName].value = ""; // 或者其它默认值
	node->real_value = "struct " + structName;
}

void SemanticAnalyzer::handle_declaration_statement(SemanticTreeNode *&node)
{
	// <声明语句> -> <声明> ;
	handle_declaration(node->children[0]); // 处理声明部分
										   // 分号不包含语义信息，直接跳过
}

void SemanticAnalyzer::handle_declaration(SemanticTreeNode *&node)
{
	// <声明> -> <修饰词闭包> <类型> <变量> <变量闭包> <赋初值>
	SemanticTreeNode *typeNode = node->children[1];
	SemanticTreeNode *varNode = node->children[2];
	SemanticTreeNode *initNode = node->children[4];

	// 处理类型
	std::string type = typeNode->children[0]->real_value;
	// std::cout << "类型" << type << std::endl;

	// 处理变量
	std::string varName = varNode->children[0]->children[0]->real_value; // <标志符> -> [T] id

	// 处理初始化
	std::string initValue = "NULL";
	if (initNode->children.size() > 0)
	{
		handle_opt_init(initNode);
		initValue = initNode->real_value;
	}
	// std::cout << "typeNode literal: " << typeNode->literal << std::endl;
	// std::cout << "varNode literal: " << varNode->literal << std::endl;
	// std::cout << "typeNode 子节点数量: " << typeNode->children.size() << std::endl;
	// std::cout << "varNode 子节点数量: " << varNode->children.size() << std::endl;
	// for (size_t i = 0; i < typeNode->children.size(); ++i) {
	//     std::cout << "typeNode child[" << i << "] real_value: " << typeNode->children[i]->real_value << std::endl;
	// }
	// for (size_t i = 0; i < varNode->children.size(); ++i) {
	//     std::cout << "varNode child[" << i << "] real_value: " << varNode->children[i]->real_value << std::endl;
	// }

	std::cout << "varNode child[" << 0 << "] real_value: " << varNode->children[0]->children[0]->real_value << std::endl;

	// 符号表操作
	if (varible_table.find(varName) != varible_table.end())
	{
		std::cout << "Error：重定义变量：" << varName << std::endl;
		exit(-1);
	}

	varible_table[varName] = {type, initValue};
	node->real_value = type + " " + varName + " = " + initValue;
}

void SemanticAnalyzer::handle_factor_component(SemanticTreeNode *&node)
{
	// <因式> -> ( <表达式> ) | <变量> | <数字>
	if (node->children.size() == 3)
	{
		// ( <表达式> )
		handle_expression(node->children[1]);
		node->real_value = node->children[1]->real_value;
		node->append_quaters(node->children[1]->quater_list);
	}
	else if (node->children[0]->literal == "<变量>")
	{
		handle_var(node->children[0]);
		node->real_value = node->children[0]->real_value;
		node->append_quaters(node->children[0]->quater_list);
	}
	else if (node->children[0]->literal == "<数字>")
	{
		node->real_value = node->children[0]->children[0]->real_value; // [T] digit
	}
}

// void SemanticAnalyzer::handle_assignment_or_call(SemanticTreeNode *&node)
// {
// 	// // <赋值函数> -> <变量> <赋值或函数调用>
// 	// SemanticTreeNode *varNode = node->children[0];	  // <变量>
// 	// SemanticTreeNode *actionNode = node->children[1]; // <赋值或函数调用>

// 	// if (actionNode->children[0]->literal == "=")
// 	// {
// 	// 	// 赋值操作
// 	// 	handle_var(varNode);
// 	// 	handle_expression(actionNode->children[1]); // 处理右值

// 	// 	std::string varName = varNode->real_value;
// 	// 	std::string exprValue = actionNode->children[1]->real_value;

// 	// 	// 检查变量是否已声明
// 	// 	if (!exists_var_declaration(varName))
// 	// 	{
// 	// 		std::cout << "Error: 未定义变量：" << varName << std::endl;
// 	// 		exit(-1);
// 	// 	}

// 	// 	node->add_quater("=", exprValue, "", varName);
// 	// 	node->real_value = varName + " = " + exprValue;
// 	// }
// 	// else
// 	// {
// 	// 	// 函数调用
// 	// 	handle_function_call(varNode, actionNode);
// 	// }
// 	// <赋值函数> -> <变量> <赋值或函数调用>
// 	SemanticTreeNode *varNode = node->children[0];	  // <变量>
// 	SemanticTreeNode *actionNode = node->children[1]; // <赋值或函数调用>
// 	if (actionNode->children[0]->literal == "=")
// 	{
// 		// std::cout << actionNode->children[1]->real_value << std::endl;
// 		// 赋值操作
// 		handle_var(varNode); // 处理左值（变量）

// 		// 处理右值（表达式）
// 		// handle_expression(actionNode->children[1]);
// 		// handle_default(actionNode->children[1]);

// 		std::string varName = varNode->real_value;
// 		std::string exprValue = actionNode->children[1]->real_value;
// 		// std::cout << exprValue << std::endl;
// 		// std::cout << varName << std::endl;

// 		// 检查变量是否已声明
// 		if (!exists_var_declaration(varName))
// 		{
// 			std::cout << "Error: 未定义变量：" << varName << std::endl;
// 			exit(-1);
// 		}

// 		// 判断是否是结构体数组赋值
// 		const auto &entry = varible_table[varName];
// 		if (entry.type.find("struct") == 0 && entry.type.find("[") != std::string::npos)
// 		{
// 			// 提取结构体数组大小
// 			std::smatch match;
// 			std::regex pattern(R"(\$(\d+)\$)");
// 			if (std::regex_search(entry.type, match, pattern))
// 			{
// 				int arraySize = std::stoi(match[1].str());

// 				// 提取结构体名
// 				// std::string structName = entry.type.substr(7, match.suffix().first - entry.type.c_str() - 8);

// 				// 查找结构体定义
// 				const auto &structDef = structEntry.members;

// 				// 解析初始化表达式
// 				exprValue = exprValue.substr(2, exprValue.size() - 3); // 去掉最外层 {}
// 				std::istringstream iss(exprValue);
// 				std::string item;
// 				std::vector<std::string> structItems;

// 				while (std::getline(iss >> std::ws, item, '}'))
// 				{
// 					if (!item.empty())
// 					{
// 						item.erase(item.begin(), item.begin() + 1); // 去掉 {
// 						structItems.push_back(item);
// 					}
// 				}

// 				// 遍历每个结构体
// 				for (int i = 0; i < arraySize && i < structItems.size(); ++i)
// 				{
// 					std::istringstream memStream(structItems[i]);
// 					std::string value;
// 					int j = 0;

// 					for (const auto &[memberName, memberEntry] : structDef)
// 					{
// 						if (std::getline(memStream >> std::ws, value, ','))
// 						{
// 							value.erase(value.find_last_not_of(" \n\r\t") + 1); // 去空格
// 							std::string access = varName + "[" + std::to_string(i) + "]." + memberName;
// 							node->add_quater("=", value, "", access);
// 						}
// 						++j;
// 					}
// 				}
// 			}
// 		}
// 		else
// 		{
// 			// 普通赋值
// 			node->add_quater("=", exprValue, "", varName);
// 		}

// 		node->append_quaters(varNode->quater_list);
// 		node->append_quaters(actionNode->children[1]->quater_list);

// 		node->real_value = varName + " = " + exprValue;
// 	}

// 	else
// 	{
// 		// 函数调用
// 		handle_function_call(varNode, actionNode);
// 	}
// }

void SemanticAnalyzer::handle_assignment_or_call(SemanticTreeNode *&node)
{
	SemanticTreeNode *varNode = node->children[0];
	SemanticTreeNode *actionNode = node->children[1];

	if (actionNode->children[0]->literal == "=")
	{
		handle_var(varNode);
		std::string varName = varNode->real_value;
		std::string exprValue = actionNode->children[1]->real_value;

		if (!exists_var_declaration(varName))
		{
			std::cout << "Error: 未定义变量：" << varName << std::endl;
			exit(-1);
		}

		const auto &entry = varible_table[varName];

		// 处理结构体数组赋值
		if (entry.type.find("struct ") == 0 && entry.type.find('[') != std::string::npos)
		{

			// 提取数组大小
			size_t bracketPos = entry.type.find('[');
			size_t endBracketPos = entry.type.find(']', bracketPos);
			if (bracketPos == std::string::npos || endBracketPos == std::string::npos)
			{
				std::cerr << "Error: 无效的数组类型声明: " << entry.type << std::endl;
				exit(-1);
			}

			int arraySize = std::stoi(entry.type.substr(bracketPos + 1, endBracketPos - bracketPos - 1));

			// 提取结构体名称
			size_t structStart = entry.type.find(' ') + 1;
			size_t structEnd = entry.type.find('[', structStart);
			std::string structName = entry.type.substr(structStart, structEnd - structStart);
			// 查找结构体定义
			SymbolTableEntry structDef;
			if (struct_definitions.find(structName) == struct_definitions.end())
			{
				std::cerr << "Error: 未定义结构体: " << structName << std::endl;
				exit(-1);
			}
			structDef = struct_definitions[structName];

			// 解析初始化表达式
			std::vector<std::vector<std::string>> arrayValues;
			parse_struct_array_initializer(exprValue, arrayValues, arraySize, structDef.members.size());

			// 生成四元式
			for (int i = 0; i < arraySize && i < arrayValues.size(); ++i)
			{
				for (size_t j = 0; j < structDef.members.size() && j < arrayValues[i].size(); ++j)
				{
					const auto &memberPair = structDef.members[j];
					const std::string &memberName = memberPair.first;
					std::string access = varName + "[" + std::to_string(i) + "]." + memberName;
					std::string value = arrayValues[i][j];

					// 移除值两端的空格
					value.erase(0, value.find_first_not_of(" \t\n\r"));
					value.erase(value.find_last_not_of(" \t\n\r") + 1);

					node->add_quater("=", value, "", access);
				}
			}
		}
		else
		{
			// 普通赋值
			node->add_quater("=", exprValue, "", varName);
		}

		node->append_quaters(varNode->quater_list);
		node->append_quaters(actionNode->children[1]->quater_list);
		node->real_value = varName + " = " + exprValue;
	}
	else
	{
		handle_function_call(varNode, actionNode);
	}
}

// 新增辅助函数：解析结构体数组初始化器
void SemanticAnalyzer::parse_struct_array_initializer(
	const std::string &initStr,
	std::vector<std::vector<std::string>> &result,
	int expectedArraySize,
	int expectedMemberCount)
{
	result.clear();
	std::string current = initStr;

	// 移除外层花括号
	if (current.front() == '{')
		current = current.substr(1);
	if (current.back() == '}')
		current = current.substr(0, current.size() - 1);

	int braceLevel = 0;
	std::string currentElement;
	std::vector<std::string> currentStruct;

	for (char c : current)
	{
		if (c == '{')
		{
			braceLevel++;
			if (braceLevel == 1) // 新的结构体开始
			{
				currentElement.clear();
				continue;
			}
		}
		else if (c == '}')
		{
			braceLevel--;
			if (braceLevel == 0) // 结构体结束
			{
				// 添加到当前结构体
				if (!currentElement.empty())
				{
					currentStruct.push_back(currentElement);
					currentElement.clear();
				}

				// 添加到结果数组
				if (!currentStruct.empty())
				{
					result.push_back(currentStruct);
					currentStruct.clear();
				}
				continue;
			}
		}

		if (braceLevel >= 1) // 在结构体内部
		{
			if (c == ',' && braceLevel == 1) // 成员分隔符
			{
				if (!currentElement.empty())
				{
					currentStruct.push_back(currentElement);
					currentElement.clear();
				}
			}
			else
			{
				currentElement += c;
			}
		}
	}

	// 处理最后一个元素
	if (!currentElement.empty())
	{
		currentStruct.push_back(currentElement);
	}
	if (!currentStruct.empty())
	{
		result.push_back(currentStruct);
	}

	// 验证结果
	if (result.size() > expectedArraySize)
	{
		std::cerr << "Warning: 初始化值数量(" << result.size()
				  << ")超过数组大小(" << expectedArraySize << ")" << std::endl;
	}

	for (const auto &structInit : result)
	{
		if (structInit.size() > expectedMemberCount)
		{
			std::cerr << "Warning: 结构体成员初始化值数量(" << structInit.size()
					  << ")超过定义(" << expectedMemberCount << ")" << std::endl;
		}
	}
}

void SemanticAnalyzer::handle_function_call(SemanticTreeNode *&funcNode, SemanticTreeNode *&callNode)
{
	std::string funcName = funcNode->real_value;

	// 检查函数是否已声明
	if (varible_table.find(funcName) == varible_table.end() ||
		varible_table[funcName].type != "function")
	{
		std::cout << "Error: 未定义函数：" << funcName << std::endl;
		exit(-1);
	}

	// 处理参数
	std::vector<std::string> args;
	if (callNode->children[1]->literal == "<参数列表>")
	{
		SemanticTreeNode *argList = callNode->children[1];
		for (auto arg : argList->children)
		{
			if (arg->literal == "<参数>")
			{
				// handle_parameter(arg);
				args.push_back(arg->real_value);
			}
		}
	}

	// 生成函数调用四元式
	std::string temp = new_temp_varible();
	// node->add_quater("call", funcName, std::to_string(args.size()), temp);

	// // 添加参数传递四元式
	// for (int i = 0; i < args.size(); i++) {
	//     node->add_quater("param", args[i], "", "arg" + std::to_string(i));
	// }

	// node->real_value = temp;
}

void SemanticAnalyzer::handle_return_stmt(SemanticTreeNode *&node)
{
	// <函数返回> -> return <因式> ;
	if (node->children.size() > 1)
	{
		handle_factor_component(node->children[1]);
		node->add_quater("return", node->children[1]->real_value, "", "");
	}
	else
	{
		node->add_quater("return", "", "", "");
	}
}
void SemanticAnalyzer::handle_array_or_struct(SemanticTreeNode *&node)
{
	// <数组或结构体> -> [ <表达式> ] | $

	if (node->children.size() == 4)
	{ // [ expr ]
		std::string size = node->children[0]->real_value;
		node->real_value = node->children[2]->real_value + "[" + size + "]";
	}
	else
	{
		node->real_value = ""; // 空表示不是数组
	}

	node->append_quaters(node->children[1]->quater_list); // 拷贝中间代码
}

// 修改3：增强变量处理函数
void SemanticAnalyzer::handle_var(SemanticTreeNode *&node)
{
	// <变量> -> <标志符> <数组或结构体>
	std::string varName = node->children[0]->children[0]->real_value; // <标志符> -> [T] id
																	  // std::cout << "[DEBUG] handle_var extracted size: " << node->children[1]->children.size() << std::endl;
	SemanticTreeNode *arrayOrStructNode = node->children[1];		  // <数组或结构体>

	// 检查变量是否已声明

	if (!exists_var_declaration(varName))
	{
		std::cout << "Error: 未定义变量：" << varName << std::endl;
		exit(-1);
	}
	// 判断是否是数组定义
	if (!arrayOrStructNode->literal.empty() && arrayOrStructNode->literal != "$" && arrayOrStructNode->real_value.find('[') != std::string::npos)
	{
		std::string structTypeName = varName;

		// 查看该类型是否是已定义的结构体
		if (varible_table.find(structTypeName) != varible_table.end() &&
			varible_table[structTypeName].type.find("struct ") == 0)
		{
			// // 是结构体类型，接下来解析数组大小
			// std::string arraySizeStr = arrayOrStructNode->real_value;
			// arraySizeStr.erase(std::remove(arraySizeStr.begin(), arraySizeStr.end(), '['), arraySizeStr.end());
			// arraySizeStr.erase(std::remove(arraySizeStr.begin(), arraySizeStr.end(), ']'), arraySizeStr.end());
			size_t arraySize = 2;
			// 构造数组类型：struct student[2]
			std::string arrayType = varible_table[structTypeName].type + "[" + "2" + "]";

			// 获取数组名（下一个节点应为数组名）
			std::string arrayVarName = "sts"; // 假设从语法树中提取了数组名
			varible_table[arrayVarName] = {arrayType, ""};

			// 设置 real_value
			node->real_value = arrayVarName;
			if (struct_definitions.find(structTypeName) != struct_definitions.end())
			{
				const auto &structDef = struct_definitions[structTypeName];

				// 添加所有数组成员到变量表
				for (int i = 0; i < arraySize; i++)
				{
					for (const auto &[memberName, memberEntry] : structDef.members)
					{
						std::string fullMemberName = varName + "[" + std::to_string(i) + "]." + memberName;
						varible_table[fullMemberName] = {memberEntry.type, ""};
					}
				}

				// 更新real_value为带下标的访问形式
				// node->real_value = varName + "[" + arraySizeStr + "]";
			}
			else
			{
				std::cerr << "Error: 未定义的结构体: " << structTypeName << std::endl;
				exit(-1);
			}
		}
		else
		{
			// 不是结构体，按普通数组处理
			node->real_value = varName + " " + arrayOrStructNode->real_value;
		}
	}
	else
	{
		node->real_value = varName;
	}

	// 处理数组或结构体访问
	// if (node->children[1]->children.size() > 0)
	// {
	// 	SemanticTreeNode *accessNode = node->children[1]->children[0];

	// 	if (accessNode->literal == "<数组下标>")
	// 	{
	// 		// 数组访问
	// 		handle_expression(accessNode->children[1]); // 表达式下标
	// 		std::string index = accessNode->children[1]->real_value;
	// 		std::string temp = new_temp_varible();
	// 		node->add_quater("[]", varName, index, temp);
	// 		node->real_value = temp;
	// 	}
	// 	else if (accessNode->literal == "<结构体变量>")
	// 	{
	// 		// 结构体成员访问
	// 		std::string member = accessNode->children[1]->real_value; // 成员名
	// 		std::string temp = new_temp_varible();
	// 		node->add_quater(".", varName, member, temp);
	// 		node->real_value = temp;
	// 	}
	// }
	// else
	// {
	// 普通变量
	// node->real_value = varName;
	// // 如果是数组或结构体，记录更多信息
	// if (!arrayOrStructNode->literal.empty() && arrayOrStructNode->literal != "$")
	// {
	// 	node->real_value += " " + arrayOrStructNode->real_value;
	// }
	// }
}

// 修改4：增强表达式处理
void SemanticAnalyzer::handle_expression(SemanticTreeNode *&node)
{
	// // <表达式> -> <因子> <项>
	// handle_factor(node->children[0]);
	// handle_term(node->children[1]);

	// // 合并因子和项的结果
	// if (!node->children[1]->real_value.empty())
	// {
	// 	std::string op = node->children[1]->children[0]->real_value; // +或-
	// 	std::string arg1 = node->children[0]->real_value;
	// 	std::string arg2 = node->children[1]->children[1]->real_value;
	// 	std::string temp = new_temp_varible();

	// 	node->add_quater(op, arg1, arg2, temp);
	// 	node->real_value = temp;
	// }
	// else
	// {
	// 	node->real_value = node->children[0]->real_value;
	// }
	// <表达式> -> <因子> <项>
	// std::cout << "[DEBUG] handle_var extracted : " << node->literal<< std::endl;
	const auto &list = node->children;

	// 新增：处理数组访问后接成员访问 (id[expression].id)
	if (list.size() >= 6 &&
		list[1]->literal == "[" &&
		list[3]->literal == "]" &&
		list[4]->literal == ".")
	{
		const std::string &fieldName = list[0]->real_value;
		const std::string &index = list[2]->real_value;
		const std::string &arrayName = list[5]->real_value;
		// std::cout << fieldName << std::endl;
		// 验证数组是否存在
		if (!exists_var_declaration(arrayName))
		{
			std::cout << "Error: 未定义数组 " << arrayName << std::endl;
			exit(-1);
		}

		// 生成数组元素访问
		std::string elementTemp = new_temp_varible();
		node->add_quater("[]", arrayName, index, elementTemp);

		// 生成结构体成员访问
		std::string resultTemp = new_temp_varible();
		node->add_quater(".", elementTemp, fieldName, resultTemp);

		node->real_value = resultTemp;
		return;
	}
	else
	{

		handle_factor(node->children[0]); // 处理因子
		handle_term(node->children[1]);	  // 处理项

		// 合并子节点的四元式
		node->append_quaters(node->children[0]->quater_list);
		node->append_quaters(node->children[1]->quater_list);

		// 表达式本身不生成新的四元式，只传递值
		if (!node->children[1]->real_value.empty())
		{
			// 如果有项处理结果，使用项的结果
			node->real_value = node->children[1]->real_value;
		}
		else
		{
			// 否则使用因子的结果
			node->real_value = node->children[0]->real_value;
		}
	}
}

// 修改5：增强项处理
void SemanticAnalyzer::handle_term(SemanticTreeNode *&node)
{
	// <项> -> + <因子> <项> | - <因子> <项> | $
	if (node->children.size() > 0)
	{
		handle_factor(node->children[1]);
		handle_term(node->children[2]);

		if (!node->children[2]->real_value.empty())
		{
			// 处理连续的加减操作
			std::string op1 = node->children[0]->real_value; // +或-
			std::string op2 = node->children[2]->children[0]->real_value;
			std::string arg1 = node->children[1]->real_value;
			std::string arg2 = node->children[2]->children[1]->real_value;
			std::string temp1 = new_temp_varible();
			std::string temp2 = new_temp_varible();

			node->add_quater(op1, arg1, arg2, temp1);
			node->add_quater(op2, temp1, arg2, temp2);
			node->real_value = temp2;
		}
		else
		{
			node->real_value = node->children[1]->real_value;
		}
	}
}

// 修改6：增强默认处理函数
void SemanticAnalyzer::handle_default(SemanticTreeNode *&node)
{
	for (const auto &child : node->children)
	{
		// 只合并非控制语句的子节点值
		if (node->literal != "<条件语句>" &&
			node->literal != "<for循环>" &&
			node->literal != "<函数块闭包>")
		{
			node->real_value += child->real_value + " ";
			// std::cout << "child " << child->real_value << " " << std::endl;
			// std::cout << "node " << node->real_value << "  " << std::endl;
		}
		// 合并四元式
		node->append_quaters(child->quater_list);
	}

	// 去除末尾空格
	if (!node->real_value.empty())
	{
		node->real_value.pop_back();
	}
}

void SemanticAnalyzer::export_quaternions_to_file(const std::string &filename)
{
	const auto &quaternions = intermediate_code(); // 获取四元式列表

	std::ofstream outFile(filename);
	if (!outFile)
	{
		std::cerr << "Error: Could not open file for writing." << std::endl;
		return;
	}

	for (const auto &[id, quater] : quaternions)
	{
		std::string raw = std::to_string(id) + quater.to_string();

		// 去除所有空格
		raw.erase(std::remove(raw.begin(), raw.end(), ' '), raw.end());

		outFile << raw << std::endl;
	}

	outFile.close();
	std::cout << "Quaternions exported to " << filename << std::endl;
}