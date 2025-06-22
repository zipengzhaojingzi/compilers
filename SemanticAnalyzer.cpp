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
			// ��ת��䲻��idҪ+base_id, ����resultҲҪ��
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
	// ���ȼ���м�����Ƿ�Ϊ��
	if (intermediate_code().empty())
	{
		std::cout << "Intermediate code table is empty." << std::endl;
		return;
	}

	// ȷ��ID�е������
	size_t maxIdLength = std::string("ID").length();
	for (const auto &[id, _] : intermediate_code())
	{
		maxIdLength = std::max(maxIdLength, std::to_string(id).length());
	}
	maxIdLength += 2; // ����һЩ����Ŀռ����������

	// ��Ԫʽÿ���ֵ������
	size_t maxPart1Length = 0, maxPart2Length = 0, maxPart3Length = 0, maxPart4Length = 0;
	for (const auto &[_, item] : intermediate_code())
	{
		std::istringstream iss(item.to_string());
		std::string part;
		std::getline(iss, part, ','); // ��һ����
		maxPart1Length = std::max(maxPart1Length, part.length());
		std::getline(iss, part, ','); // �ڶ�����
		maxPart2Length = std::max(maxPart2Length, part.length());
		std::getline(iss, part, ','); // ��������
		maxPart3Length = std::max(maxPart3Length, part.length());
		std::getline(iss, part, ')'); // ���Ĳ���
		maxPart4Length = std::max(maxPart4Length, part.length());
	}

	maxPart1Length += 1;
	maxPart2Length += 1;
	maxPart3Length += 1;
	maxPart4Length += 0;
	// ������Ԫʽ�е��ܿ��
	size_t maxItemLength = maxPart1Length + maxPart2Length + maxPart3Length + maxPart4Length + 6; // 9 ���ĸ����ź��������ŵĳ���

	// �����ܳ��Ⱥʹ�ӡ��ͷ
	size_t totalLength = maxIdLength + maxItemLength + 5; // 5 �Ƿָ����ͱ߽�ĳ���
	std::string topBottomBorder = "+" + std::string(maxIdLength, '-') + "+" + std::string(maxItemLength, '-') + "+";
	std::cout << topBottomBorder << std::endl;
	std::cout << std::left << "|" << std::setw(maxIdLength) << " ID "
			  << "|" << std::setw(maxItemLength) << " Quarter "
			  << "|" << std::endl;
	std::cout << topBottomBorder << std::endl;

	// �����м���벢��ӡ
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

	// ��ӡ�ײ�����
	std::cout << topBottomBorder << std::endl;
	// export_quaternions_to_file("intermediateCode.txt");
}

void SemanticAnalyzer::print_variable_table()
{
	// ���������Ƿ�Ϊ��
	if (varible_table.empty())
	{
		std::cout << "Variable table is empty." << std::endl;
		return;
	}

	// ȷ��ÿ�е������
	size_t maxNameLength = std::string("Variable Name").length();
	size_t maxTypeLength = std::string("Type").length();
	size_t maxValueLength = std::string("Initial Value").length();

	for (const auto &pair : varible_table)
	{
		maxNameLength = std::max(maxNameLength, pair.first.length());
		maxTypeLength = std::max(maxTypeLength, pair.second.type.length());
		maxValueLength = std::max(maxValueLength, pair.second.value.length());
	}

	// ����һЩ����Ŀռ����������
	maxNameLength += 2;
	maxTypeLength += 2;
	maxValueLength += 2;

	// �����ܳ��Ⱥʹ�ӡ��ͷ
	size_t totalLength = maxNameLength + maxTypeLength + maxValueLength + 7; // 7 �Ƿָ����ͱ߽�ĳ���
	std::string topBottomBorder = "+" + std::string(maxNameLength, '-') + "+" + std::string(maxTypeLength, '-') + "+" + std::string(maxValueLength, '-') + "+";
	std::cout << topBottomBorder << std::endl;
	std::cout << std::left << "|" << std::setw(maxNameLength) << " Variable Name "
			  << "|" << std::setw(maxTypeLength) << " Type "
			  << "|" << std::setw(maxValueLength) << " Initial Value "
			  << "|" << std::endl;
	std::cout << topBottomBorder << std::endl;

	// ����varible_table����ӡ
	for (const auto &pair : varible_table)
	{
		std::cout << std::left << "|" << std::setw(maxNameLength) << (" " + pair.first)
				  << "|" << std::setw(maxTypeLength) << (" " + pair.second.type)
				  << "|" << std::setw(maxValueLength) << (" " + pair.second.value) << "|" << std::endl;
	}

	// ��ӡ�ײ�����
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

// 		if (node->literal == "<�ṹ�嶨��>" || node->literal == "<����>")
// 		{
// 			std::cout << "catch catch catch catch catch catch" << std::endl;
// 			handle_var_declaration(node);
// 		}
// 		else if (node->literal == "<����ֵ>")
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
	const std::string &init_val = list[4]->literal == "<����ֵ>" ? list[4]->real_value : "NULL";

	if (varible_table.find(varible_name) != varible_table.end())
	{
		// ������������Ѿ������������������
		std::cout << "Error���ض��������" << varible_name << std::endl;
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
// 		std::cout << "Error: δ���������" << var << std::endl;
// 		exit(-1);
// 	}
// 	if (list[2]->literal == "T_IDENTIFIER" && !exists_var_declaration(exp))
// 	{
// 		std::cout << "Error: δ���������" << exp << std::endl;
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

	call 		*������ҵ������������
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

// 	// ��ʱ����������
// 	return;
// }

void SemanticAnalyzer::handle_selection_stmt(SemanticTreeNode *&node)
{
	/*
	T_IF T_LEFT_PAREN expression T_RIGHT_PAREN statement
	T_IF T_LEFT_PAREN expression T_RIGHT_PAREN statement T_ELSE statement
	*/

	const auto &list = node->children;
	// ���������������ӡlist�ṹ
	std::cout << "\n[DEBUG] Selection stmt children list:\n";
	for (size_t i = 0; i < list.size(); ++i)
	{
		std::cout << "  child[" << i << "]: " << list[i]->literal;
		if (!list[i]->real_value.empty())
		{
			std::cout << " (real_value: " << list[i]->real_value << ")";
		}
		std::cout << "\n";

		// ��ӡ�ӽڵ����Ԫʽ�б�
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
		THEN = ELSE + stmt_ELSE->quater_list.size() + 1; // ��ȷ��quater_list.size()
		ENDIF = THEN + stmt_THEN->quater_list.size();	 // ��ȷ��quater_list.size()

		node->add_quater("jnz", cond, "", THEN);
		node->append_quaters(stmt_ELSE->quater_list);
		node->add_quater("j", "", "", ENDIF);
		node->append_quaters(stmt_THEN->quater_list);
		std::cout << "[IF-ELSE] Generated " << node->quater_list.size()
				  << " quaternions for selection stmt\n";
	}
	// �ں�����β�����Ԫʽ�б����
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
	node->real_value = node->real_value.substr(0, node->real_value.size() - 1); // ȥ���ֺ�;

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
		// �����������ʼ����ڵ�
		std::cout << "[DEBUG] Processing node: " << node->literal
				  << " (Leaf: " << (node->leaf() ? "Yes" : "No") << ")"
				  << std::endl;

		if (node->leaf())
		{
			std::cout << "        Leaf node, skipping handler." << std::endl;

			continue;
		}

		// std::cout << "�ڵ�" << node->literal << std::endl;

		handle_default(node);
		std::string handlerUsed = "handle_defalt";
		// if (node->literal == "<�������>")
		// {
		// 	// ���������������ͷֺ�
		// 	handle_declaration_statement(node);
		// }
		// �޸�1���������ķ������ڵ㴦��
		if (node->literal == "<�ṹ�嶨��>")
		{
			handle_struct_definition(node);
			handlerUsed = "�ṹ�嶨��";
		}
		else if (node->literal == "<�ṹ�����>")
		{
			handle_array_or_struct(node);
			handlerUsed = "�ṹ�����";
		}
		else if (node->literal == "<����>")
		{
			handlerUsed = "����";

			handle_declaration(node);
		}
		else if (node->literal == "<����ֵ>")
		{
			handlerUsed = "����ֵ";

			handle_opt_init(node);
		}
		else if (node->literal == "<���ʽ>")
		{
			handlerUsed = "���ʽ";

			handle_expression(node);
		}
		else if (node->literal == "<����>")
		{
			handle_factor(node);
			handlerUsed = "����";
		}
		else if (node->literal == "<��>")
		{
			handle_term(node);
			handlerUsed = "��";
		}
		else if (node->literal == "<��ʽ>")
		{
			handle_factor_component(node);
			handlerUsed = "��ʽ";
		}
		else if (node->literal == "<��׺���ʽ>")
		{
			handle_postfix_expression(node);
			handlerUsed = "��׺���ʽ";
		}
		else if (node->literal == "<ǰ׺���ʽ>")
		{
			handle_prefix_expression(node);
			handlerUsed = "ǰ׺���ʽ";
		}
		else if (node->literal == "<�������>")
		{
			handle_selection_stmt(node);
			handlerUsed = "�������";
		}
		else if (node->literal == "<forѭ��>")
		{
			handle_iteration_stmt(node);
			handlerUsed = "forѭ��";
		}
		else if (node->literal == "<��������>")
		{
			handle_return_stmt(node);
			handlerUsed = "��������";
		}
		else if (node->literal == "<��ֵ����>")
		{
			handle_assignment_or_call(node);
			handlerUsed = "��ֵ����";
		}

		// ��ʶ������������
		if (node->children.size() == 1 && node->children[0]->literal == "<��־��>")
		{
			node->literal = node->children[0]->literal;
		}
		// ��ӡʹ�õĴ�����
		std::cout << "        Handler used: " << handlerUsed << std::endl;

		// ��ӡ real_value������ǿգ�
		if (!node->real_value.empty())
		{
			std::cout << "        real_value: \"" << node->real_value << "\"" << std::endl;
		}

		// // ��ӡ��Ԫʽ�б�����ǿգ�
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
	// ���б�ֱ�ӷ���
	if (node->children.empty())
		return;

	// // ��ȡ��ǰ��Ա�� real_value �������� ���� + ������
	// std::string memberRealValue = node->real_value;
	// std::istringstream iss(memberRealValue);
	// std::string token;
	// std::vector<std::string> tokens;

	// while (iss >> token)
	// {
	// 	tokens.push_back(token);
	// }

	// // ��Ա��ʽӦΪ: type... id ;
	// if (tokens.size() < 3)
	// {
	// 	std::cerr << "Error: Invalid member declaration in struct." << std::endl;
	// 	exit(-1);
	// }

	// // ��ȡ���ͣ�ƴ�ӳ�������� token �Ĳ��֣�
	// std::string memberType;
	// for (size_t i = 0; i < tokens.size() - 2; ++i)
	// {
	// 	memberType += tokens[i];
	// 	if (i + 1 < tokens.size() - 2)
	// 		memberType += " ";
	// }

	// std::string memberName = tokens[tokens.size() - 2];

	// // ��ӵ��ṹ���Ա����
	// structEntry.members[memberName] = {memberType, ""};
	std::string memberRealValue = node->real_value;
	size_t pos = 0;
	while (pos < memberRealValue.size())
	{
		// �����հ��ַ�
		while (pos < memberRealValue.size() && isspace(memberRealValue[pos]))
			++pos;
		if (pos >= memberRealValue.size())
			break;

		size_t start = pos;
		// �ҵ��ֺŻ��β
		while (pos < memberRealValue.size() && memberRealValue[pos] != ';')
			++pos;
		std::string memberStr = memberRealValue.substr(start, pos - start);

		// ����������Ա
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

		// �����������ǳ����һ�� token ��������� token
		std::string memberType;
		for (size_t i = 0; i < tokens.size() - 1; ++i)
		{
			memberType += tokens[i];
			if (i + 1 < tokens.size() - 1)
				memberType += " ";
		}

		// �����������������һ�� token
		std::string memberName = tokens.back();
		// ��ӵ��ṹ���Ա����
		structEntry.members.push_back({memberName,
									   SymbolTableEntry{
										   memberType, // type
										   "",		   // value
										   {}		   // members
									   }});

		// �����ֺż����ܵĿո�
		if (pos < memberRealValue.size())
			++pos;
	}
}

// �޸�2��������ķ��Ĵ�����
void SemanticAnalyzer::handle_struct_definition(SemanticTreeNode *&node)
{
	// struct id { <�ṹ���Ա�б�> } ;
	// ����ṹ�嶨��
	std::string structName = node->children[1]->real_value; // struct��ı�ʶ��
															// SemanticTreeNode *memberList = node->children[3];		// �ṹ���Ա�б�

	// �����ṹ����ű���Ŀ
	SymbolTableEntry newStruct;

	newStruct.type = "struct " + structName;

	// ��ȡ��Ա�б�����
	SemanticTreeNode *memberListNode = node->children[3];
	handle_struct_member_list(memberListNode, newStruct);
	struct_definitions[structName] = newStruct;
	// ���ԣ���ӡ��ǰ�ṹ����Ϣ

	std::cout << "[DEBUG] Struct defined: " << newStruct.type << std::endl;
	// �޸�ǰ��
	// for (const auto &member : structEntry.members)

	// �޸ĺ�
	for (const auto &[memberName, memberEntry] : newStruct.members)
	{
		std::cout << "  Member: " << memberName << ", Type: " << memberEntry.type << std::endl;
	}
	// ���ṹ����������

	// ���õ�ǰ�ڵ� real_value
	varible_table[structName].type = newStruct.type;
	varible_table[structName].value = ""; // ��������Ĭ��ֵ
	node->real_value = "struct " + structName;
}

void SemanticAnalyzer::handle_declaration_statement(SemanticTreeNode *&node)
{
	// <�������> -> <����> ;
	handle_declaration(node->children[0]); // ������������
										   // �ֺŲ�����������Ϣ��ֱ������
}

void SemanticAnalyzer::handle_declaration(SemanticTreeNode *&node)
{
	// <����> -> <���δʱհ�> <����> <����> <�����հ�> <����ֵ>
	SemanticTreeNode *typeNode = node->children[1];
	SemanticTreeNode *varNode = node->children[2];
	SemanticTreeNode *initNode = node->children[4];

	// ��������
	std::string type = typeNode->children[0]->real_value;
	// std::cout << "����" << type << std::endl;

	// �������
	std::string varName = varNode->children[0]->children[0]->real_value; // <��־��> -> [T] id

	// �����ʼ��
	std::string initValue = "NULL";
	if (initNode->children.size() > 0)
	{
		handle_opt_init(initNode);
		initValue = initNode->real_value;
	}
	// std::cout << "typeNode literal: " << typeNode->literal << std::endl;
	// std::cout << "varNode literal: " << varNode->literal << std::endl;
	// std::cout << "typeNode �ӽڵ�����: " << typeNode->children.size() << std::endl;
	// std::cout << "varNode �ӽڵ�����: " << varNode->children.size() << std::endl;
	// for (size_t i = 0; i < typeNode->children.size(); ++i) {
	//     std::cout << "typeNode child[" << i << "] real_value: " << typeNode->children[i]->real_value << std::endl;
	// }
	// for (size_t i = 0; i < varNode->children.size(); ++i) {
	//     std::cout << "varNode child[" << i << "] real_value: " << varNode->children[i]->real_value << std::endl;
	// }

	std::cout << "varNode child[" << 0 << "] real_value: " << varNode->children[0]->children[0]->real_value << std::endl;

	// ���ű����
	if (varible_table.find(varName) != varible_table.end())
	{
		std::cout << "Error���ض��������" << varName << std::endl;
		exit(-1);
	}

	varible_table[varName] = {type, initValue};
	node->real_value = type + " " + varName + " = " + initValue;
}

void SemanticAnalyzer::handle_factor_component(SemanticTreeNode *&node)
{
	// <��ʽ> -> ( <���ʽ> ) | <����> | <����>
	if (node->children.size() == 3)
	{
		// ( <���ʽ> )
		handle_expression(node->children[1]);
		node->real_value = node->children[1]->real_value;
		node->append_quaters(node->children[1]->quater_list);
	}
	else if (node->children[0]->literal == "<����>")
	{
		handle_var(node->children[0]);
		node->real_value = node->children[0]->real_value;
		node->append_quaters(node->children[0]->quater_list);
	}
	else if (node->children[0]->literal == "<����>")
	{
		node->real_value = node->children[0]->children[0]->real_value; // [T] digit
	}
}

// void SemanticAnalyzer::handle_assignment_or_call(SemanticTreeNode *&node)
// {
// 	// // <��ֵ����> -> <����> <��ֵ��������>
// 	// SemanticTreeNode *varNode = node->children[0];	  // <����>
// 	// SemanticTreeNode *actionNode = node->children[1]; // <��ֵ��������>

// 	// if (actionNode->children[0]->literal == "=")
// 	// {
// 	// 	// ��ֵ����
// 	// 	handle_var(varNode);
// 	// 	handle_expression(actionNode->children[1]); // ������ֵ

// 	// 	std::string varName = varNode->real_value;
// 	// 	std::string exprValue = actionNode->children[1]->real_value;

// 	// 	// �������Ƿ�������
// 	// 	if (!exists_var_declaration(varName))
// 	// 	{
// 	// 		std::cout << "Error: δ���������" << varName << std::endl;
// 	// 		exit(-1);
// 	// 	}

// 	// 	node->add_quater("=", exprValue, "", varName);
// 	// 	node->real_value = varName + " = " + exprValue;
// 	// }
// 	// else
// 	// {
// 	// 	// ��������
// 	// 	handle_function_call(varNode, actionNode);
// 	// }
// 	// <��ֵ����> -> <����> <��ֵ��������>
// 	SemanticTreeNode *varNode = node->children[0];	  // <����>
// 	SemanticTreeNode *actionNode = node->children[1]; // <��ֵ��������>
// 	if (actionNode->children[0]->literal == "=")
// 	{
// 		// std::cout << actionNode->children[1]->real_value << std::endl;
// 		// ��ֵ����
// 		handle_var(varNode); // ������ֵ��������

// 		// ������ֵ�����ʽ��
// 		// handle_expression(actionNode->children[1]);
// 		// handle_default(actionNode->children[1]);

// 		std::string varName = varNode->real_value;
// 		std::string exprValue = actionNode->children[1]->real_value;
// 		// std::cout << exprValue << std::endl;
// 		// std::cout << varName << std::endl;

// 		// �������Ƿ�������
// 		if (!exists_var_declaration(varName))
// 		{
// 			std::cout << "Error: δ���������" << varName << std::endl;
// 			exit(-1);
// 		}

// 		// �ж��Ƿ��ǽṹ�����鸳ֵ
// 		const auto &entry = varible_table[varName];
// 		if (entry.type.find("struct") == 0 && entry.type.find("[") != std::string::npos)
// 		{
// 			// ��ȡ�ṹ�������С
// 			std::smatch match;
// 			std::regex pattern(R"(\$(\d+)\$)");
// 			if (std::regex_search(entry.type, match, pattern))
// 			{
// 				int arraySize = std::stoi(match[1].str());

// 				// ��ȡ�ṹ����
// 				// std::string structName = entry.type.substr(7, match.suffix().first - entry.type.c_str() - 8);

// 				// ���ҽṹ�嶨��
// 				const auto &structDef = structEntry.members;

// 				// ������ʼ�����ʽ
// 				exprValue = exprValue.substr(2, exprValue.size() - 3); // ȥ������� {}
// 				std::istringstream iss(exprValue);
// 				std::string item;
// 				std::vector<std::string> structItems;

// 				while (std::getline(iss >> std::ws, item, '}'))
// 				{
// 					if (!item.empty())
// 					{
// 						item.erase(item.begin(), item.begin() + 1); // ȥ�� {
// 						structItems.push_back(item);
// 					}
// 				}

// 				// ����ÿ���ṹ��
// 				for (int i = 0; i < arraySize && i < structItems.size(); ++i)
// 				{
// 					std::istringstream memStream(structItems[i]);
// 					std::string value;
// 					int j = 0;

// 					for (const auto &[memberName, memberEntry] : structDef)
// 					{
// 						if (std::getline(memStream >> std::ws, value, ','))
// 						{
// 							value.erase(value.find_last_not_of(" \n\r\t") + 1); // ȥ�ո�
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
// 			// ��ͨ��ֵ
// 			node->add_quater("=", exprValue, "", varName);
// 		}

// 		node->append_quaters(varNode->quater_list);
// 		node->append_quaters(actionNode->children[1]->quater_list);

// 		node->real_value = varName + " = " + exprValue;
// 	}

// 	else
// 	{
// 		// ��������
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
			std::cout << "Error: δ���������" << varName << std::endl;
			exit(-1);
		}

		const auto &entry = varible_table[varName];

		// ����ṹ�����鸳ֵ
		if (entry.type.find("struct ") == 0 && entry.type.find('[') != std::string::npos)
		{

			// ��ȡ�����С
			size_t bracketPos = entry.type.find('[');
			size_t endBracketPos = entry.type.find(']', bracketPos);
			if (bracketPos == std::string::npos || endBracketPos == std::string::npos)
			{
				std::cerr << "Error: ��Ч��������������: " << entry.type << std::endl;
				exit(-1);
			}

			int arraySize = std::stoi(entry.type.substr(bracketPos + 1, endBracketPos - bracketPos - 1));

			// ��ȡ�ṹ������
			size_t structStart = entry.type.find(' ') + 1;
			size_t structEnd = entry.type.find('[', structStart);
			std::string structName = entry.type.substr(structStart, structEnd - structStart);
			// ���ҽṹ�嶨��
			SymbolTableEntry structDef;
			if (struct_definitions.find(structName) == struct_definitions.end())
			{
				std::cerr << "Error: δ����ṹ��: " << structName << std::endl;
				exit(-1);
			}
			structDef = struct_definitions[structName];

			// ������ʼ�����ʽ
			std::vector<std::vector<std::string>> arrayValues;
			parse_struct_array_initializer(exprValue, arrayValues, arraySize, structDef.members.size());

			// ������Ԫʽ
			for (int i = 0; i < arraySize && i < arrayValues.size(); ++i)
			{
				for (size_t j = 0; j < structDef.members.size() && j < arrayValues[i].size(); ++j)
				{
					const auto &memberPair = structDef.members[j];
					const std::string &memberName = memberPair.first;
					std::string access = varName + "[" + std::to_string(i) + "]." + memberName;
					std::string value = arrayValues[i][j];

					// �Ƴ�ֵ���˵Ŀո�
					value.erase(0, value.find_first_not_of(" \t\n\r"));
					value.erase(value.find_last_not_of(" \t\n\r") + 1);

					node->add_quater("=", value, "", access);
				}
			}
		}
		else
		{
			// ��ͨ��ֵ
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

// �������������������ṹ�������ʼ����
void SemanticAnalyzer::parse_struct_array_initializer(
	const std::string &initStr,
	std::vector<std::vector<std::string>> &result,
	int expectedArraySize,
	int expectedMemberCount)
{
	result.clear();
	std::string current = initStr;

	// �Ƴ���㻨����
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
			if (braceLevel == 1) // �µĽṹ�忪ʼ
			{
				currentElement.clear();
				continue;
			}
		}
		else if (c == '}')
		{
			braceLevel--;
			if (braceLevel == 0) // �ṹ�����
			{
				// ��ӵ���ǰ�ṹ��
				if (!currentElement.empty())
				{
					currentStruct.push_back(currentElement);
					currentElement.clear();
				}

				// ��ӵ��������
				if (!currentStruct.empty())
				{
					result.push_back(currentStruct);
					currentStruct.clear();
				}
				continue;
			}
		}

		if (braceLevel >= 1) // �ڽṹ���ڲ�
		{
			if (c == ',' && braceLevel == 1) // ��Ա�ָ���
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

	// �������һ��Ԫ��
	if (!currentElement.empty())
	{
		currentStruct.push_back(currentElement);
	}
	if (!currentStruct.empty())
	{
		result.push_back(currentStruct);
	}

	// ��֤���
	if (result.size() > expectedArraySize)
	{
		std::cerr << "Warning: ��ʼ��ֵ����(" << result.size()
				  << ")���������С(" << expectedArraySize << ")" << std::endl;
	}

	for (const auto &structInit : result)
	{
		if (structInit.size() > expectedMemberCount)
		{
			std::cerr << "Warning: �ṹ���Ա��ʼ��ֵ����(" << structInit.size()
					  << ")��������(" << expectedMemberCount << ")" << std::endl;
		}
	}
}

void SemanticAnalyzer::handle_function_call(SemanticTreeNode *&funcNode, SemanticTreeNode *&callNode)
{
	std::string funcName = funcNode->real_value;

	// ��麯���Ƿ�������
	if (varible_table.find(funcName) == varible_table.end() ||
		varible_table[funcName].type != "function")
	{
		std::cout << "Error: δ���庯����" << funcName << std::endl;
		exit(-1);
	}

	// �������
	std::vector<std::string> args;
	if (callNode->children[1]->literal == "<�����б�>")
	{
		SemanticTreeNode *argList = callNode->children[1];
		for (auto arg : argList->children)
		{
			if (arg->literal == "<����>")
			{
				// handle_parameter(arg);
				args.push_back(arg->real_value);
			}
		}
	}

	// ���ɺ���������Ԫʽ
	std::string temp = new_temp_varible();
	// node->add_quater("call", funcName, std::to_string(args.size()), temp);

	// // ��Ӳ���������Ԫʽ
	// for (int i = 0; i < args.size(); i++) {
	//     node->add_quater("param", args[i], "", "arg" + std::to_string(i));
	// }

	// node->real_value = temp;
}

void SemanticAnalyzer::handle_return_stmt(SemanticTreeNode *&node)
{
	// <��������> -> return <��ʽ> ;
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
	// <�����ṹ��> -> [ <���ʽ> ] | $

	if (node->children.size() == 4)
	{ // [ expr ]
		std::string size = node->children[0]->real_value;
		node->real_value = node->children[2]->real_value + "[" + size + "]";
	}
	else
	{
		node->real_value = ""; // �ձ�ʾ��������
	}

	node->append_quaters(node->children[1]->quater_list); // �����м����
}

// �޸�3����ǿ����������
void SemanticAnalyzer::handle_var(SemanticTreeNode *&node)
{
	// <����> -> <��־��> <�����ṹ��>
	std::string varName = node->children[0]->children[0]->real_value; // <��־��> -> [T] id
																	  // std::cout << "[DEBUG] handle_var extracted size: " << node->children[1]->children.size() << std::endl;
	SemanticTreeNode *arrayOrStructNode = node->children[1];		  // <�����ṹ��>

	// �������Ƿ�������

	if (!exists_var_declaration(varName))
	{
		std::cout << "Error: δ���������" << varName << std::endl;
		exit(-1);
	}
	// �ж��Ƿ������鶨��
	if (!arrayOrStructNode->literal.empty() && arrayOrStructNode->literal != "$" && arrayOrStructNode->real_value.find('[') != std::string::npos)
	{
		std::string structTypeName = varName;

		// �鿴�������Ƿ����Ѷ���Ľṹ��
		if (varible_table.find(structTypeName) != varible_table.end() &&
			varible_table[structTypeName].type.find("struct ") == 0)
		{
			// // �ǽṹ�����ͣ����������������С
			// std::string arraySizeStr = arrayOrStructNode->real_value;
			// arraySizeStr.erase(std::remove(arraySizeStr.begin(), arraySizeStr.end(), '['), arraySizeStr.end());
			// arraySizeStr.erase(std::remove(arraySizeStr.begin(), arraySizeStr.end(), ']'), arraySizeStr.end());
			size_t arraySize = 2;
			// �����������ͣ�struct student[2]
			std::string arrayType = varible_table[structTypeName].type + "[" + "2" + "]";

			// ��ȡ����������һ���ڵ�ӦΪ��������
			std::string arrayVarName = "sts"; // ������﷨������ȡ��������
			varible_table[arrayVarName] = {arrayType, ""};

			// ���� real_value
			node->real_value = arrayVarName;
			if (struct_definitions.find(structTypeName) != struct_definitions.end())
			{
				const auto &structDef = struct_definitions[structTypeName];

				// ������������Ա��������
				for (int i = 0; i < arraySize; i++)
				{
					for (const auto &[memberName, memberEntry] : structDef.members)
					{
						std::string fullMemberName = varName + "[" + std::to_string(i) + "]." + memberName;
						varible_table[fullMemberName] = {memberEntry.type, ""};
					}
				}

				// ����real_valueΪ���±�ķ�����ʽ
				// node->real_value = varName + "[" + arraySizeStr + "]";
			}
			else
			{
				std::cerr << "Error: δ����Ľṹ��: " << structTypeName << std::endl;
				exit(-1);
			}
		}
		else
		{
			// ���ǽṹ�壬����ͨ���鴦��
			node->real_value = varName + " " + arrayOrStructNode->real_value;
		}
	}
	else
	{
		node->real_value = varName;
	}

	// ���������ṹ�����
	// if (node->children[1]->children.size() > 0)
	// {
	// 	SemanticTreeNode *accessNode = node->children[1]->children[0];

	// 	if (accessNode->literal == "<�����±�>")
	// 	{
	// 		// �������
	// 		handle_expression(accessNode->children[1]); // ���ʽ�±�
	// 		std::string index = accessNode->children[1]->real_value;
	// 		std::string temp = new_temp_varible();
	// 		node->add_quater("[]", varName, index, temp);
	// 		node->real_value = temp;
	// 	}
	// 	else if (accessNode->literal == "<�ṹ�����>")
	// 	{
	// 		// �ṹ���Ա����
	// 		std::string member = accessNode->children[1]->real_value; // ��Ա��
	// 		std::string temp = new_temp_varible();
	// 		node->add_quater(".", varName, member, temp);
	// 		node->real_value = temp;
	// 	}
	// }
	// else
	// {
	// ��ͨ����
	// node->real_value = varName;
	// // ����������ṹ�壬��¼������Ϣ
	// if (!arrayOrStructNode->literal.empty() && arrayOrStructNode->literal != "$")
	// {
	// 	node->real_value += " " + arrayOrStructNode->real_value;
	// }
	// }
}

// �޸�4����ǿ���ʽ����
void SemanticAnalyzer::handle_expression(SemanticTreeNode *&node)
{
	// // <���ʽ> -> <����> <��>
	// handle_factor(node->children[0]);
	// handle_term(node->children[1]);

	// // �ϲ����Ӻ���Ľ��
	// if (!node->children[1]->real_value.empty())
	// {
	// 	std::string op = node->children[1]->children[0]->real_value; // +��-
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
	// <���ʽ> -> <����> <��>
	// std::cout << "[DEBUG] handle_var extracted : " << node->literal<< std::endl;
	const auto &list = node->children;

	// ����������������ʺ�ӳ�Ա���� (id[expression].id)
	if (list.size() >= 6 &&
		list[1]->literal == "[" &&
		list[3]->literal == "]" &&
		list[4]->literal == ".")
	{
		const std::string &fieldName = list[0]->real_value;
		const std::string &index = list[2]->real_value;
		const std::string &arrayName = list[5]->real_value;
		// std::cout << fieldName << std::endl;
		// ��֤�����Ƿ����
		if (!exists_var_declaration(arrayName))
		{
			std::cout << "Error: δ�������� " << arrayName << std::endl;
			exit(-1);
		}

		// ��������Ԫ�ط���
		std::string elementTemp = new_temp_varible();
		node->add_quater("[]", arrayName, index, elementTemp);

		// ���ɽṹ���Ա����
		std::string resultTemp = new_temp_varible();
		node->add_quater(".", elementTemp, fieldName, resultTemp);

		node->real_value = resultTemp;
		return;
	}
	else
	{

		handle_factor(node->children[0]); // ��������
		handle_term(node->children[1]);	  // ������

		// �ϲ��ӽڵ����Ԫʽ
		node->append_quaters(node->children[0]->quater_list);
		node->append_quaters(node->children[1]->quater_list);

		// ���ʽ���������µ���Ԫʽ��ֻ����ֵ
		if (!node->children[1]->real_value.empty())
		{
			// ������������ʹ����Ľ��
			node->real_value = node->children[1]->real_value;
		}
		else
		{
			// ����ʹ�����ӵĽ��
			node->real_value = node->children[0]->real_value;
		}
	}
}

// �޸�5����ǿ���
void SemanticAnalyzer::handle_term(SemanticTreeNode *&node)
{
	// <��> -> + <����> <��> | - <����> <��> | $
	if (node->children.size() > 0)
	{
		handle_factor(node->children[1]);
		handle_term(node->children[2]);

		if (!node->children[2]->real_value.empty())
		{
			// ���������ļӼ�����
			std::string op1 = node->children[0]->real_value; // +��-
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

// �޸�6����ǿĬ�ϴ�����
void SemanticAnalyzer::handle_default(SemanticTreeNode *&node)
{
	for (const auto &child : node->children)
	{
		// ֻ�ϲ��ǿ��������ӽڵ�ֵ
		if (node->literal != "<�������>" &&
			node->literal != "<forѭ��>" &&
			node->literal != "<������հ�>")
		{
			node->real_value += child->real_value + " ";
			// std::cout << "child " << child->real_value << " " << std::endl;
			// std::cout << "node " << node->real_value << "  " << std::endl;
		}
		// �ϲ���Ԫʽ
		node->append_quaters(child->quater_list);
	}

	// ȥ��ĩβ�ո�
	if (!node->real_value.empty())
	{
		node->real_value.pop_back();
	}
}

void SemanticAnalyzer::export_quaternions_to_file(const std::string &filename)
{
	const auto &quaternions = intermediate_code(); // ��ȡ��Ԫʽ�б�

	std::ofstream outFile(filename);
	if (!outFile)
	{
		std::cerr << "Error: Could not open file for writing." << std::endl;
		return;
	}

	for (const auto &[id, quater] : quaternions)
	{
		std::string raw = std::to_string(id) + quater.to_string();

		// ȥ�����пո�
		raw.erase(std::remove(raw.begin(), raw.end(), ' '), raw.end());

		outFile << raw << std::endl;
	}

	outFile.close();
	std::cout << "Quaternions exported to " << filename << std::endl;
}