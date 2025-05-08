#ifndef AST_TREE_HPP
#define AST_TREE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>

#include "common.hpp"
#include "semantic/symbol_table.hpp"

extern int yylineno;

namespace AST {

class Node;
using NodePtr = std::shared_ptr<Node>;
class Node {
 public:
  int lineno;

  virtual std::vector<NodePtr> get_children() { return std::vector<NodePtr>(); }
  void print_tree(std::string prefix = "", std::string info_prefix = "");
  virtual std::string to_string() = 0;

  Node() : lineno(yylineno) {}
  virtual ~Node() = default;
};

class IntConst;
using IntConstPtr = std::shared_ptr<IntConst>;
class IntConst : public Node {
 public:
  int value;
  IntConst(int value) : value(value) {}
  std::string to_string() override {
    return "IntConst <value: " + std::to_string(value) + ">";
  }
};

class LVal;
using LValPtr = std::shared_ptr<LVal>;
class LVal : public Node {
 public:
  std::string name;
	bool is_arr;
	NodePtr index;
  SymbolPtr symbol;
  LVal(std::string ident) : name(ident), 
														is_arr(false), index(nullptr) {}
  LVal(std::string ident, NodePtr index) :
						name(ident), is_arr(true), index(index) {}
  std::string to_string() override { return "LVal <ident: " + name + ">"; }
	std::vector<NodePtr> get_children() { if(is_arr) return {index};  return {}; }
};

class ExpList;
using ExpListPtr = std::shared_ptr<ExpList>;
class ExpList : public Node {
	public:
		std::vector<NodePtr> args;
		ExpList() : args({}) {}
		ExpList(ExpListPtr ast) : args(ast->args) {}
		void add_arg(NodePtr exp)	{ args.push_back(exp); }
		std::string to_string() override { return "ExpList"; }
		std::vector<NodePtr> get_children() override { return args; }
};

class Decl;
using DeclPtr = std::shared_ptr<Decl>;
class Decl : public Node {
	public:
		NodePtr decl;
		Decl(NodePtr decl) : decl(decl) {}
		// std::string to_string() override {
		// 	return decl->to_string();
		// }
		std::vector<NodePtr> get_children() override { return {decl}; }
};

class InitVal;
using InitValPtr = std::shared_ptr<InitVal>;
class InitVal : public Node {
	public:
		std::vector<NodePtr> args;
		bool is_list = true;
		bool is_exp = false;
		InitVal() : args({}) {}
		InitVal(bool is_list) : args({}), is_list(is_list) {}
		InitVal(NodePtr exp) : args( {exp} ) {}
		void set_block() { is_list = false; }
		void this_is_exp() { is_exp = true; }
		void add_arg(NodePtr exp) { args.push_back(exp); }
		std::string to_string() override { return "InitVal"; }
		std::vector<NodePtr> get_children() override { return args; }
};

class UnaryExp;
using UnaryExpPtr = std::shared_ptr<UnaryExp>;
class UnaryExp : public Node {
 public:
  BinaryOp op;
  NodePtr exp;
  UnaryExp(BinaryOp op, NodePtr exp) : op(op), exp(exp) {}
  std::string to_string() override {
    return "UnaryExp <op: " + std::string(op_to_string(op)) + ">";
  }
  std::vector<NodePtr> get_children() override { return {exp}; }
};

class BinaryExp;
using BinaryExpPtr = std::shared_ptr<BinaryExp>;
class BinaryExp : public Node {
 public:
  BinaryOp op;
  NodePtr left, right;

  BinaryExp(BinaryOp op, NodePtr left, NodePtr right)
      : op(op), left(left), right(right) {}
  std::string to_string() override {
    return "BinaryExp <op: " + std::string(op_to_string(op)) + ">";
  }
  std::vector<NodePtr> get_children() override { return {left, right}; }
};

class FuncCall;
using FuncCallPtr = std::shared_ptr<FuncCall>;
class FuncCall : public Node {
	public:
	 std::string name;
	 std::vector<NodePtr> args;
	 FuncCall(char const *name) : name(name) {}
	 FuncCall(NodePtr exp) { add_arg(exp); }
	 void add_arg(NodePtr exp) { args.push_back(exp); }
	 std::string to_string() override { return "FuncCall <name: " + name + ">"; }
	 std::vector<NodePtr> get_children() override { return args; }
 };

class Block;
using BlockPtr = std::shared_ptr<Block>;
class Block : public Node {
 public:
  std::vector<NodePtr> stmts;
  Block() {}
  Block(NodePtr stmt) { add_stmt(stmt); }
  void add_stmt(NodePtr stmt) { stmts.push_back(stmt); }
  std::string to_string() override { return "Block"; }
  std::vector<NodePtr> get_children() override { return stmts; }
};

class AssignStmt;
using AssignStmtPtr = std::shared_ptr<AssignStmt>;
class AssignStmt : public Node {
 public:
  LValPtr lval;
  NodePtr exp;
  AssignStmt(LValPtr lval, NodePtr exp) : lval(lval), exp(exp) {}
  std::string to_string() override { return "AssignStmt" ; }
  std::vector<NodePtr> get_children() override { return {lval, exp}; }
};

class ReturnStmt;
using ReturnStmtPtr = std::shared_ptr<ReturnStmt>;
class ReturnStmt : public Node {
 public:
	bool is_void;
  NodePtr exp;
  ReturnStmt() : is_void(true) ,exp(nullptr) {}
  ReturnStmt(NodePtr exp) : is_void(false), exp(exp) {}
  std::string to_string() override { return "ReturnStmt"; }
  std::vector<NodePtr> get_children() override {
		if(is_void)	return {};
		return std::vector<NodePtr>{exp};
  }
};

class IfStmt;
using IfStmtPtr = std::shared_ptr<IfStmt>;
class IfStmt : public Node {
	public:
		NodePtr cond;
		NodePtr stmt;
		NodePtr else_stmt;
		IfStmt(NodePtr cond, NodePtr stmt) : 
					cond(cond), stmt(stmt), else_stmt(nullptr) {}
		IfStmt(NodePtr cond, NodePtr stmt, NodePtr else_stmt) : 
					cond(cond), stmt(stmt), else_stmt(else_stmt) {}
		std::string to_string() override { return "IfStmt"; }
		std::vector<NodePtr> get_children() override { 
			return else_stmt ? std::vector<NodePtr>{cond, stmt, else_stmt} : 
													std::vector<NodePtr>{cond, stmt};
		}
};

class WhileStmt;
using WhileStmtPtr = std::shared_ptr<WhileStmt>;
class WhileStmt : public Node {
	public:
		NodePtr cond;
		NodePtr stmt;
		WhileStmt(NodePtr cond, NodePtr stmt) : cond(cond), stmt(stmt) {}
		std::string to_string() override { return "WhileStmt"; }
		std::vector<NodePtr> get_children() override 
		{ 
			return {cond, stmt};
		}
};

class NullStmt;
using NullStmtPtr = std::shared_ptr<NullStmt>;
class NullStmt : public Node {
	public:
		NodePtr stmt;
		NullStmt() : stmt(nullptr) {}
		std::string to_string() override { return "NullStmt"; }
};

class VarDef;
using VarDefPtr = std::shared_ptr<VarDef>;
class VarDef : public Node {
 public:
  std::string ident;
	std::optional<NodePtr> val;
  SymbolPtr symbol;
  VarDef(char const *ident) : ident(ident) {}
  std::string to_string() override 
	{
		std::string res;
		res += "VarDef <ident: " + ident + " ";
		if(val.has_value())
			res += "=" + val.value()->to_string();
		res += ">";
		return res;
	}
};

class VarDecl;
using VarDeclPtr = std::shared_ptr<VarDecl>;
class VarDecl : public Node {
 public:
  BasicType btype;
  std::vector<VarDefPtr> defs;
  VarDecl(VarDefPtr def) : btype(BasicType::Unknown) { add_def(def); }
  void add_def(VarDefPtr def) { defs.push_back(def); }
  std::string to_string() override {
    return "VarDecl <btype: " + std::string(type_to_string(btype)) + ">";
  }
  std::vector<NodePtr> get_children() override {
    return std::vector<NodePtr>(defs.begin(), defs.end());
  }
};

class ArrLists;
using ArrListsPtr = std::shared_ptr<ArrLists>;
class ArrLists : public Node {
	public:
		std::vector<IntConstPtr> args;
		ArrLists() : args({}) {}
		ArrLists(ArrListsPtr ast) : args(ast->args) {}
		void add_list(IntConstPtr list) { args.push_back(list); }
		std::string to_string() override { return "ArrLists"; }
		std::vector<NodePtr> get_children() override {
			return std::vector<NodePtr>(args.begin(), args.end());
		}
};

class ArrDef;
using ArrDefPtr = std::shared_ptr<ArrDef>;
class ArrDef : public Node {
 public:
  std::string ident;
	ArrListsPtr arr;
	std::optional<NodePtr> val;
	SymbolPtr symbol;
  ArrDef(char const *ident) : ident(ident) {}
  std::string to_string() override { return "ArrDef <ident: " + ident + ">"; }
	std::vector<NodePtr> get_children() { if(val.has_value()) return { arr, val.value() }; return {arr}; }
};

class ArrDecl;
using ArrDeclPtr = std::shared_ptr<ArrDecl>;
class ArrDecl : public Node {
 public:
  BasicType btype;
  std::vector<ArrDefPtr> defs;
  ArrDecl(ArrDefPtr def) : btype(BasicType::Unknown) { add_def(def); }
  void add_def(ArrDefPtr def) { defs.push_back(def); }
  std::string to_string() override {
    return "ArrDecl <btype: " + std::string(type_to_string(btype)) + ">";
  }
  std::vector<NodePtr> get_children() override {
    return std::vector<NodePtr>(defs.begin(), defs.end());
  }
};

class FuncFParam;
using FuncFParamPtr = std::shared_ptr<FuncFParam>;
class FuncFParam : public Node {
	public:
		BasicType btype;
		std::string name;
		bool is_arr;
		ArrListsPtr args;
		FuncFParam(BasicType btype, char const *name, bool is_arr) : 
								btype(btype), name(name), is_arr(is_arr), args({}) {}
		FuncFParam(BasicType btype, char const *name, bool is_arr, ArrListsPtr args) : 
								btype(btype), name(name), is_arr(is_arr), args(args) {}
		std::string to_string() override { 
			return "FuncFParam <btype: " + std::string(type_to_string(btype)) +
							", name: " + name + ">";
		}
		std::vector<NodePtr> get_children() override {
			if(!is_arr)	return {};
			else return {args};
		}
};

class FuncFParams;
using FuncFParamsPtr = std::shared_ptr<FuncFParams>;
class FuncFParams : public Node {
	public:
		std::vector<FuncFParamPtr> args;
		FuncFParams() : args({}) {}
		FuncFParams(FuncFParamsPtr ast) : args(ast->args) {}
		void add_arg(FuncFParamPtr param) { args.push_back(param); }
		std::string to_string() override { return "FuncFParams"; }
		std::vector<NodePtr> get_children() override {
			return std::vector<NodePtr>(args.begin(), args.end());
		}
};

class FuncDef;
using FuncDefPtr = std::shared_ptr<FuncDef>;
class FuncDef : public Node {
 public:
  BasicType return_btype;
  std::string name;
	bool is_param;
	FuncFParamsPtr params;
  BlockPtr block;
  SymbolPtr symbol;
  FuncDef(BasicType return_btype, char const *name, BlockPtr block)
      : return_btype(return_btype), name(name), is_param(false), params(nullptr), block(block) {}
	FuncDef(BasicType return_btype, char const *name, FuncFParamsPtr params, BlockPtr block)
			: return_btype(return_btype), name(name), is_param(true), params(params), block(block) {}
  std::string to_string() override {
    return "FuncDef <return_btype: " +
           std::string(type_to_string(return_btype)) + ", name: " + name + ">";
  }
  std::vector<NodePtr> get_children() override { 
		if(is_param) return {params, block};
		else return {block};
	}
};

class CompUnit;
using CompUnitPtr = std::shared_ptr<CompUnit>;
class CompUnit : public Node {
 public:
  std::vector<NodePtr> units;  // FuncDef or VarDecl
  CompUnit(NodePtr unit) { add_unit(unit); }
  void add_unit(NodePtr unit) { units.push_back(unit); }
  std::string to_string() override { return "CompUnit"; }
  std::vector<NodePtr> get_children() override { return units; }
};

}  // namespace AST

#endif  // AST_TREE_HPP