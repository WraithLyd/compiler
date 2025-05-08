#include "type_checker.hpp"

#include "common.hpp"

TypeChecker::TypeChecker() {
  // 你需要在这里对 symbol_table 进行初始化
  // 插入一些内置函数，如 read 和 write
	symbol_table->enter_scope();
	symbol_table = symbol_table->next;

	auto read_type = FuncType::create(PrimitiveType::Int, {});
	symbol_table->add_symbol("read", read_type);
	auto write_type = FuncType::create(PrimitiveType::Void, {PrimitiveType::Int});
	symbol_table->add_symbol("write", write_type);
	
}

TypePtr TypeChecker::check(AST::NodePtr node) {
	if (!node) {
		ASSERT(false, "[*] Null node in type checker.");
		return nullptr;
	}
#define CHECK_NODE(type)                                     \
  if (auto n = std::dynamic_pointer_cast<AST::type>(node)) { \
		std::cout<<node->to_string()<<std::endl;								 \
    return check##type(n);                                   \
  }

  // 递归检查 AST 的每个节点
  // 如果你添加了新的 AST 节点类型，记得在这里添加对应的检查函数
  CHECK_NODE(CompUnit)
	CHECK_NODE(Decl)

  CHECK_NODE(FuncDef)
//	CHECK_NODE(FuncFParam)
//	CHECK_NODE(FuncFParams)

  CHECK_NODE(VarDecl)
	
	CHECK_NODE(ArrDecl)
	CHECK_NODE(ArrDef)
	CHECK_NODE(ArrLists)

  CHECK_NODE(Block)
  CHECK_NODE(AssignStmt)
  CHECK_NODE(ReturnStmt)
	CHECK_NODE(IfStmt)
	CHECK_NODE(WhileStmt)
	CHECK_NODE(NullStmt)
	
	CHECK_NODE(InitVal)
  CHECK_NODE(LVal)
  CHECK_NODE(IntConst)
  CHECK_NODE(FuncCall)
//	CHECK_NODE(ExpList)
  CHECK_NODE(UnaryExp)
  CHECK_NODE(BinaryExp)

#undef CHECK_NODE

  ASSERT(false, "Unknown AST node type " + node->to_string() +
                    " in type checking at line " +
                    std::to_string(node->lineno));
}

TypePtr TypeChecker::checkCompUnit(AST::CompUnitPtr node) {
  for (auto &unit : node->units) 
		check(unit);
  return nullptr;
}

TypePtr TypeChecker::checkDecl(AST::DeclPtr node) {
	return check(node->decl);
}

TypePtr TypeChecker::checkFuncDef(AST::FuncDefPtr node) {
  // 在这个函数中，你需要判断函数是否已经被定义过
  // 如果函数已经被定义过，你需要报错
  // 否则，你需要将函数插入符号表，并在符号表中创建一个新的作用域
  // 再将函数参数也插入符号表，并将符号表中对应的 symbol 挂到 FuncDef 节点上
  // 最后检查函数体的语句块
	func_ret_type = PrimitiveType::create(node->return_btype);
	if(symbol_table->find_symbol(node->name, false))
	{
		ASSERT(false, "Func is defined");
		return nullptr;
	}
	std::vector<TypePtr> param_types;
	if(node->is_param)
		for(AST::FuncFParamPtr param : node->params->args)
		{
			if(param->is_arr)
			{
				std::vector<int> dims;
				for(auto dim : param->args->args)
					dims.push_back(dim->value);
				auto dim_type = PrimitiveType::create(param->btype);
				auto dims_type = ArrayType::create(dim_type, dims);
				param_types.push_back(dims_type);
			}else{
				param_types.push_back(PrimitiveType::create(param->btype));
			}
		}
	else
		param_types = {};
	auto return_type = PrimitiveType::create(node->return_btype);
	auto type = FuncType::create(return_type, param_types);
  node->symbol = symbol_table->add_symbol(node->name, type);
	symbol_table->enter_scope();
	symbol_table = symbol_table->next;
	if(node->is_param)
		for(auto param : node->params->args)
		{
			auto param_type = PrimitiveType::create(param->btype);
			auto param_symbol = symbol_table->add_symbol(param->name, param_type);
		}
	checkBlock(node->block, false);
	symbol_table->exit_scope();
	symbol_table = symbol_table->parent;
  return type;
}

TypePtr TypeChecker::checkVarDecl(AST::VarDeclPtr node) {
  for (auto var_def : node->defs) {
    checkVarDef(var_def, node->btype);
  }
  return nullptr;
}

TypePtr TypeChecker::checkVarDef(AST::VarDefPtr node, BasicType var_type) {
  // 你需要判断变量是否已经被定义过，并更新符号表
  auto type = PrimitiveType::create(var_type);
  // 判断变量是否已经被定义过
  // 如果有初始化表达式，你需要检查初始化表达式的类型是否和变量类型相同
  // 如果是数组，你还需要检查初始化表达式和数组的维度是否匹配，是否有溢出的情况
	if(symbol_table->find_symbol(node->ident, true))
	{
		ASSERT(false, "Var is defined");
		return nullptr;
	}
	// if(node->val.has_value() && !check(node->val.value())->equals(PrimitiveType::Int))
	// {
	// 	ASSERT(false, "VarDef type is not Int");
	// 	return nullptr;
	// }

  // 将变量插入符号表，并将符号表中的 symbol 挂到 VarDef 节点上
  node->symbol = symbol_table->add_symbol(node->ident, type);
	
	if(node->val.has_value())
		check(node->val.value());
  return PrimitiveType::Void;
}

TypePtr TypeChecker::checkArrDecl(AST::ArrDeclPtr node) {
  for (auto arr_def : node->defs) {
    checkArrDef(arr_def, node->btype);
  }
  return nullptr;
}

TypePtr TypeChecker::checkArrDef(AST::ArrDefPtr node, BasicType var_type) {
  // 你需要判断变量是否已经被定义过，并更新符号表
  auto type = PrimitiveType::create(var_type);
  // 判断变量是否已经被定义过
  // 如果有初始化表达式，你需要检查初始化表达式的类型是否和变量类型相同
  // 如果是数组，你还需要检查初始化表达式和数组的维度是否匹配，是否有溢出的情况
	if(symbol_table->find_symbol(node->ident, true))
	{
		ASSERT(false, "Arr various " + node->ident + " is defined");
		return nullptr;
	}
	// if(node->val.has_value() && (check(node->val.value())))
	// {
	// 	ASSERT(false, node->ident + "type is not Int");	
	// 	return nullptr;
	// }
	check(node->arr);
	std::vector<int> nums = {};
	auto dims = std::dynamic_pointer_cast<AST::ArrLists>(node->arr);
	for(auto dim : dims->args){
		nums.push_back(dim->value);
	}
	auto arr_type = ArrayType::create(type, nums);
  // 将变量插入符号表，并将符号表中的 symbol 挂到 VarDef 节点上
  node->symbol = symbol_table->add_symbol(node->ident, arr_type);

	AST::ArrListsPtr arr_rev;
	arr_rev = node->arr;
	std::reverse(arr_rev->args.begin(), arr_rev->args.end());
	if(node->val.has_value())
	{
		TypePtr type;
		type = checkInitVal(std::dynamic_pointer_cast<AST::InitVal>(node->val.value()), arr_rev, 0, node->to_string());
		if(type->equals(PrimitiveType::Int))
			ASSERT(false, "Array initializer must be an initializer list");
	}
		return PrimitiveType::Void;
}

TypePtr TypeChecker::checkArrLists(AST::ArrListsPtr node) {
	for(auto item : node->args)
		check(item);
	return PrimitiveType::Void;
}

TypePtr TypeChecker::checkBlock(AST::BlockPtr node, bool new_scope) {
  // 检查块内的每个语句
  // 如果 new_scope 为 true
  // 你需要在进入和退出块时更新符号表，创建、销毁新的作用域
	if(new_scope) {
		symbol_table->enter_scope();
		symbol_table = symbol_table->next;
	}

	for (auto stmt : node->stmts)
	{
    check(stmt);
	}

	if(new_scope){
		symbol_table->exit_scope();
		symbol_table = symbol_table->parent;
	}
	return PrimitiveType::Void;
}

TypePtr TypeChecker::checkAssignStmt(AST::AssignStmtPtr node) {
  TypePtr lval_type = check(node->lval);
  TypePtr expr_type = check(node->exp);
  // 判断赋值号两边的类型是否相同
  // 我们实验中只支持 int 类型
  // 因此你需要判断 lval_type 和 expr_type 是否都为 int 类型

	if(lval_type->equals(PrimitiveType::Int) && expr_type->equals(PrimitiveType::Int))
	  return lval_type;
	ASSERT(false, "lval type is " + lval_type->to_string() + " & rval type is " + expr_type->to_string());
	return nullptr;
}

TypePtr TypeChecker::checkReturnStmt(AST::ReturnStmtPtr node) {
  TypePtr expr_type = check(node->exp);
  // 判断返回值类型是否和函数声明的返回值类型相同
	if(expr_type->equals(func_ret_type))
	  return nullptr;
	ASSERT(false, "func return type is not equal");
	return nullptr;
}

TypePtr TypeChecker::checkIfStmt(AST::IfStmtPtr node) {
	TypePtr cond_type = check(node->cond);
	TypePtr if_type = check(node->stmt);
	if(node->else_stmt)
		TypePtr else_type = check(node->else_stmt);
	if(cond_type->equals(PrimitiveType::Int))
		return PrimitiveType::Void;
	ASSERT(false, "if cond is not int");
	return nullptr;
}

TypePtr TypeChecker::checkWhileStmt(AST::WhileStmtPtr node) {
	TypePtr cond_type = check(node->cond);
	TypePtr stmt_type = check(node->stmt);
	if(cond_type->equals(PrimitiveType::Int))
		return PrimitiveType::Void;
	ASSERT(false, "while cond is not int");
	return nullptr;
}

TypePtr TypeChecker::checkNullStmt(AST::NullStmtPtr node) {
	return PrimitiveType::Void;
}

TypePtr TypeChecker::checkInitVal(AST::InitValPtr node, std::optional<AST::ArrListsPtr> arr, int checked_cnt, std::string str) {

	if(!node)	return PrimitiveType::Void;
	// auto init_node = std::dynamic_pointer_cast<AST::InitVal>(node);

	if(!arr.has_value())
		if(node->args.size()!=1)
			ASSERT(false, "Excess elements in scalar initializer " + str + " " + std::to_string(node->args.size()));
		else
			return check(node->args[0]);
	if(node->is_exp)
		if(check(node->args[0])->equals(PrimitiveType::Int))
			return PrimitiveType::Int;
		else
			ASSERT(false, "type of array value is not int");
	if(!node->args.size())	return PrimitiveType::Void;
	int count = checked_cnt;
	int capacity = 1;
	std::vector<AST::IntConstPtr> arr_list = arr.value()->args;
	for(auto item : arr_list)
		capacity *= item->value;

	for(auto item : node->args)
	{
		if(auto n = std::dynamic_pointer_cast<AST::InitVal>(item))
		{
			int arr_size = 1;
			int i=0;
			auto sub_arr = new AST::ArrLists();
			for(i=0; i<arr_list.size()-1;i++)
			{
				arr_size *= arr_list[i]->value;
				if(count % arr_size != 0)
				{
					break;
				}else{
					sub_arr->args.push_back(arr_list[i]);
				}
			}
			checkInitVal(n, AST::ArrListsPtr(sub_arr), count, str);
			if(i!=arr_list.size()-1)	arr_size /= arr_list[i]->value;
			count += arr_size;
			ASSERT((count-checked_cnt)<=capacity, "Excess elements in array initializer");
		}else{
			count++;
			ASSERT((count-checked_cnt)<=capacity, "Excess elements in array initializer");

			if(!check(item)->equals(PrimitiveType::Int))
				ASSERT(false, "type of array value is not int");
		}
	}
	std::vector<int> dims;
	if(arr.has_value())
		for(auto i:arr.value()->args)
			dims.push_back(i->value);
	if(!dims.size())
		return PrimitiveType::Int;
	else
		return ArrayType::create(PrimitiveType::Int, dims);
}

TypePtr TypeChecker::checkLVal(AST::LValPtr node) {
  // 你需要在这里查找符号表，判断变量是否被定义过
  // 根据符号表中的信息设置 LVal 的类型
  // 若变量未定义，你需要报错
  // 否则，将符号表中的 symbol 挂到 LVal 节点上
  // 如果 LVal 是数组，你还需要根据下标索引来设置 LVal 的类型
	auto symbol = symbol_table->find_symbol(node->name, false);
	if(symbol == nullptr)
	{
		ASSERT(false, node->name + " LVal doesn't find");
		return nullptr;
	}
	node->symbol = symbol;
	auto type = symbol->type;
	if(!node->is_arr)
		return type;
	if(type->which_type() == PRIMI)
	{
		ASSERT(false, "LVal " + node->to_string() + " is not a array");
		return nullptr;
	}

	auto index = std::dynamic_pointer_cast<AST::ExpList>(node->index);
	auto arr_type = std::dynamic_pointer_cast<ArrayType>(type);
	for(auto item : index->args)
		if(!(check(item))->equals(PrimitiveType::Int))
		{
			ASSERT(false, "array dim is not int");
		}

	if(index->args.size() > arr_type->dims.size())
	{
		ASSERT(false, "Array too many indexes");
		return nullptr;
	}else if(index->args.size() == arr_type->dims.size()){
		return arr_type->element_type;
	}else{
		std::vector<int> dims(arr_type->dims.begin() + index->args.size(), arr_type->dims.end());
		return ArrayType::create(arr_type->element_type, dims);
	}
}

TypePtr TypeChecker::checkIntConst(AST::IntConstPtr node) {
  // 整数常量的类型是 int
  return PrimitiveType::Int;
}

TypePtr TypeChecker::checkFuncCall(AST::FuncCallPtr node) {
  // 首先需要查找函数是否被定义过
  // 然后需要判断函数调用的参数个数和类型是否和声明一致
  // 最后设置函数调用表达式的类型为函数的返回值类型
  // 并将函数的 symbol 挂到 FuncCall 节点上
	
	auto symbol = symbol_table->find_symbol(node->name, false);
	if(!symbol){
		ASSERT(false, "Undeclared function" + symbol->name);
		return nullptr;
	}
	
	if(symbol->type->which_type() != FUNC) {
		ASSERT(false, symbol->name + "is not a function");
		return nullptr;
	}

	auto type = std::dynamic_pointer_cast<FuncType>(symbol->type);
	if(type->param_types.size() != node->args.size()){
		ASSERT(false, "function" + symbol->name + "arugments number error");
		return nullptr;
	}
	
	for(int i=0; i<type->param_types.size(); i++)
	{
		auto item1 = node->args[i];
		auto item2 = type->param_types[i];
		auto item_type = check(item1);
		if(!item_type->equals(item2))
		{
			ASSERT(false, "funcion call element "+std::to_string(i)+" type "+item_type->to_string()+" "+type->param_types[i]->to_string()+" is not equal");
			return nullptr;
		}

	}
	// 你需要返回函数调用表达式的类型
  return type->return_type;
}

TypePtr TypeChecker::checkUnaryExp(AST::UnaryExpPtr node) {
  auto type = check(node->exp);
  // 一元表达式只支持 int 类型，因此你需要判断 type 是否为 int
	if(type->equals(PrimitiveType::Int))
	  return PrimitiveType::Int;
	ASSERT(false, "UnaryExp type is not Int");
	return nullptr;
}

TypePtr TypeChecker::checkBinaryExp(AST::BinaryExpPtr node) {
  TypePtr left_type = check(node->left);
  TypePtr right_type = check(node->right);
  // 二元表达式只支持 int 类型，因此你需要判断左右表达式的类型是否为 int

	if(left_type->equals(PrimitiveType::Int) && right_type->equals(PrimitiveType::Int))
	  return PrimitiveType::Int;
	ASSERT(false, "BinaryExp type is not Int. Left type " + left_type->to_string() + ". right type " + right_type->to_string() );
	return nullptr;
}
