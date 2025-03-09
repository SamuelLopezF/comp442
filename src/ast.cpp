#include "../include/ast.h"

namespace ast {

std::string getIndent(int indent) {
    return std::string(indent * 2, ' ');
}

std::string binaryOpToString(BinaryOp op) {
    switch (op) {
        case BinaryOp::Add: return "+";
        case BinaryOp::Sub: return "-";
        case BinaryOp::Mul: return "*";
        case BinaryOp::Div: return "/";
        case BinaryOp::And: return "and";
        case BinaryOp::Or: return "or";
        case BinaryOp::Eq: return "==";
        case BinaryOp::Neq: return "<>";
        case BinaryOp::Lt: return "<";
        case BinaryOp::Gt: return ">";
        case BinaryOp::Leq: return "<=";
        case BinaryOp::Geq: return ">=";
        default: return "unknown";
    }
}

std::string primitiveTypeToString(PrimitiveType::Kind kind) {
    switch (kind) {
        case PrimitiveType::Kind::Int: return "int";
        case PrimitiveType::Kind::Float: return "float";
        case PrimitiveType::Kind::Void: return "void";
        default: return "unknown";
    }
}

void IntegerLiteral::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "IntegerLiteral: " << value << std::endl;
}

void FloatLiteral::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "FloatLiteral: " << value << std::endl;
}

void VariableReference::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "Variable: " << name << std::endl;
}

void BinaryOperation::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "BinaryOperation: " << binaryOpToString(op) << std::endl;
    os << getIndent(indent + 1) << "Left:" << std::endl;
    left->print(os, indent + 2);
    os << getIndent(indent + 1) << "Right:" << std::endl;
    right->print(os, indent + 2);
}

void FunctionCall::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "FunctionCall: " << name << std::endl;
    os << getIndent(indent + 1) << "Arguments:" << std::endl;
    for (const auto& arg : args) {
        arg->print(os, indent + 2);
    }
}

void PrimitiveType::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "Type: " << primitiveTypeToString(kind) << std::endl;
}

void ArrayType::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "ArrayType:" << std::endl;
    os << getIndent(indent + 1) << "BaseType:" << std::endl;
    baseType->print(os, indent + 2);
    os << getIndent(indent + 1) << "Dimensions:" << std::endl;
    for (size_t i = 0; i < dimensions.size(); ++i) {
        os << getIndent(indent + 2) << dimensions[i] << std::endl;
    }
}

void AssignmentStatement::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "Assignment:" << std::endl;
    os << getIndent(indent + 1) << "Variable: " << variable << std::endl;
    os << getIndent(indent + 1) << "Expression:" << std::endl;
    expr->print(os, indent + 2);
}

void ReturnStatement::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "Return:" << std::endl;
    expr->print(os, indent + 1);
}

void ExpressionStatement::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "ExpressionStatement:" << std::endl;
    expr->print(os, indent + 1);
}

void VariableDeclaration::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "VariableDeclaration: " << name << std::endl;
    os << getIndent(indent + 1) << "Type:" << std::endl;
    type->print(os, indent + 2);
}

void FunctionDeclaration::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "FunctionDeclaration: " << name << std::endl;
    os << getIndent(indent + 1) << "ReturnType:" << std::endl;
    returnType->print(os, indent + 2);
    
    os << getIndent(indent + 1) << "Parameters:" << std::endl;
    for (const auto& param : params) {
        param->print(os, indent + 2);
    }
    
    os << getIndent(indent + 1) << "LocalVariables:" << std::endl;
    for (const auto& local : locals) {
        local->print(os, indent + 2);
    }
    
    os << getIndent(indent + 1) << "Body:" << std::endl;
    for (const auto& stmt : body) {
        stmt->print(os, indent + 2);
    }
}

void ClassDeclaration::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "ClassDeclaration: " << name << std::endl;
    
    if (!baseClasses.empty()) {
        os << getIndent(indent + 1) << "Inherits from: ";
        for (size_t i = 0; i < baseClasses.size(); ++i) {
            os << baseClasses[i];
            if (i < baseClasses.size() - 1) {
                os << ", ";
            }
        }
        os << std::endl;
    }
    
    os << getIndent(indent + 1) << "Attributes:" << std::endl;
    for (const auto& attr : attributes) {
        attr->print(os, indent + 2);
    }
    
    os << getIndent(indent + 1) << "Methods:" << std::endl;
    for (const auto& method : methods) {
        method->print(os, indent + 2);
    }
}

void Program::print(std::ostream& os, int indent) const {
    os << getIndent(indent) << "Program:" << std::endl;
    for (const auto& decl : declarations) {
        decl->print(os, indent + 1);
    }
}

} // namespace ast
