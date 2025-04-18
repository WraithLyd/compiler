#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <memory>
#include <string>

// 基本类型枚举
enum class BasicType { Unknown, Int, Void };
inline constexpr const char *type_to_string(BasicType type) {
  switch (type) {
    case BasicType::Unknown:
      return "unknown";
    case BasicType::Int:
      return "int";
    case BasicType::Void:
      return "void";
  }
  return "unknown";
}

// 二元运算符枚举
enum class BinaryOp 
{ 
	Add, Sub, Not, Mul, Div, Mod,
	Les, Leq, Gre, Geq, Eql, Neq,
	And, Or
};
inline constexpr const char *op_to_string(BinaryOp op) {
  switch (op) {
    case BinaryOp::Add:		return "+";
    case BinaryOp::Sub:		return "-";
		case BinaryOp::Not:		return "!";
		case BinaryOp::Mul:		return "*";
		case BinaryOp::Div:		return "/";
		case BinaryOp::Mod:		return "%";
		case BinaryOp::Les:		return "<";
		case BinaryOp::Leq:		return "<=";
		case BinaryOp::Gre:		return ">";
		case BinaryOp::Geq:		return ">=";
		case BinaryOp::Eql:		return "==";
		case BinaryOp::Neq:		return "!=";
		case BinaryOp::And:		return "&&";
		case BinaryOp::Or:		return "||";
		}
  return "unknown";
}

enum class MulOp {  };
inline constexpr const char *op_to_string(MulOp op){
	switch (op) {
	}
	return "unknown";
}


// use C++ RTTI to check the type of a shared_ptr
template <typename T, typename U>
inline bool type_of(const std::shared_ptr<U> &node) {
  return std::dynamic_pointer_cast<T>(node) != nullptr;
}

#define ASSERT(expr, msg)                                                \
  do {                                                                   \
    if (!(expr)) {                                                       \
      std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ \
                << " (" << #expr << "): " << msg << std::endl;           \
      std::exit(1);                                                      \
    }                                                                    \
  } while (0)

#endif  // COMMON_HPP