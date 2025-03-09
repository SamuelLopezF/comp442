#ifndef AST_BUILDER_H
#define AST_BUILDER_H

#include "ast.h"
#include "tokenizer.h"
#include <stack>
#include <map>
#include <string>
#include <memory>

// Structure to hold temporary production data during parsing
struct ProductionData {
    std::string name;
    std::shared_ptr<ast::ASTNode> node;
    std::vector<std::shared_ptr<ast::ASTNode>> children;
    std::map<std::string, std::string> attributes;

    ProductionData(const std::string& name) : name(name) {}
};

class ASTBuilder {
public:
    ASTBuilder();
    ~ASTBuilder();

    // Initialize the AST builder
    void initialize();
    
    // Process a production rule from the parser
    void processProduction(const std::string& nonTerminal, const std::string& production);
    
    // Process a token from the parser
    void processToken(const token::Token& token);
    
    // Get the completed AST
    std::shared_ptr<ast::Program> getAST();
    
    // Print the AST to stdout
    void printAST();

private:
    std::shared_ptr<ast::Program> ast;
    std::stack<ProductionData> nodeStack;
    
    // Current context
    ProductionData* currentClass;
    ProductionData* currentFunction;
    ProductionData* currentStatement;
    
    // Last token processed
    token::Token lastToken;
    
    // Helper methods for building specific node types
    void startProgram();
    void finishProgram();
    
    void startClassDeclaration();
    void finishClassDeclaration();
    
    void startImplementation();
    void finishImplementation();
    
    void startFunction();
    void finishFunction();
    
    void startStatement();
    void finishStatement();
    
    void startExpression();
    void finishExpression();
    
    // Helper for various contexts
    void handleIdentifier(const std::string& value);
    void handleLiteral(const std::string& type, const std::string& value);
    void handleOperator(const std::string& op);
    void handleType(const std::string& type);
    
    // Store the current context to retrieve later
    void pushContext();
    void popContext();
};

#endif // AST_BUILDER_H
