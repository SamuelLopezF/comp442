#include "../include/ast_builder.h"
#include <iostream>
#include <sstream>

ASTBuilder::ASTBuilder() 
    : ast(nullptr), currentClass(nullptr), currentFunction(nullptr), currentStatement(nullptr) {
}

ASTBuilder::~ASTBuilder() {
}

void ASTBuilder::initialize() {
    // Clean up any existing data
    while (!nodeStack.empty()) {
        nodeStack.pop();
    }
    
    // Create the root program node
    ast = std::make_shared<ast::Program>();
    
    // Reset context pointers
    currentClass = nullptr;
    currentFunction = nullptr;
    currentStatement = nullptr;
}

void ASTBuilder::processProduction(const std::string& nonTerminal, const std::string& production) {
    // Log the production for debugging
    std::cout << "Processing production: " << nonTerminal << " -> " << production << std::endl;
    
    // Handle different non-terminals
    if (nonTerminal == "START") {
        if (production == "PROG") {
            startProgram();
        }
    }
    else if (nonTerminal == "PROG") {
        if (production == "CLASSIMPLFUNC PROG") {
            // Will be handled by CLASSIMPLFUNC
        }
        else if (production == "EPSILON") {
            finishProgram();
        }
    }
    else if (nonTerminal == "CLASSIMPLFUNC") {
        if (production == "CLASSDECL") {
            startClassDeclaration();
        }
        else if (production == "IMPLDEF") {
            startImplementation();
        }
        else if (production == "FUNCDEF") {
            startFunction();
        }
    }
    else if (nonTerminal == "CLASSDECL") {
        if (production.find("class id") != std::string::npos) {
            // The class id will be handled by processToken
        }
    }
    else if (nonTerminal == "VISMEMBERDECL") {
        if (production == "VISIBILITY MEMDECL VISMEMBERDECL") {
            // Handled by VISIBILITY and MEMDECL
        }
        else if (production == "EPSILON") {
            finishClassDeclaration();
        }
    }
    // ... Add more production handling as needed
    
    // Handle statement productions
    else if (nonTerminal == "STATEMENT") {
        startStatement();
        // The specific statement type will be determined based on tokens
    }
    
    // Handle expression productions
    else if (nonTerminal == "EXPR") {
        startExpression();
    }
}

void ASTBuilder::processToken(const token::Token& token) {
    // Store the token for later use
    lastToken = token;
    
    // Log for debugging
    std::cout << "Processing token: [" << token.type << ", " << token.value << "]" << std::endl;
    
    // Handle different token types
    if (token.type == "id") {
        handleIdentifier(token.value);
    }
    else if (token.type == "integer") {
        handleLiteral("Integer", token.value);
    }
    else if (token.type == "float") {
        handleLiteral("Float", token.value);
    }
    else if (token.type == "operator") {
        handleOperator(token.value);
    }
    else if (token.type == "reserved") {
        if (token.value == "class" || token.value == "implementation" || 
            token.value == "function" || token.value == "constructor") {
            // These tokens trigger productions that we handle elsewhere
        }
        else if (token.value == "int" || token.value == "float") {
            handleType(token.value);
        }
        else if (token.value == "public" || token.value == "private") {
            // Set visibility attribute in current context
            if (!nodeStack.empty()) {
                nodeStack.top().attributes["visibility"] = token.value;
            }
        }
    }
}

std::shared_ptr<ast::Program> ASTBuilder::getAST() {
    return ast;
}

void ASTBuilder::printAST() {
    if (ast) {
        ast->print(std::cout, 0);
    } else {
        std::cout << "AST not built yet." << std::endl;
    }
}

// Private helper methods implementation
void ASTBuilder::startProgram() {
    // Program node is already created in initialize()
    nodeStack.push(ProductionData("Program"));
    nodeStack.top().node = ast;
}

void ASTBuilder::finishProgram() {
    // Pop the program node from the stack
    if (!nodeStack.empty() && nodeStack.top().name == "Program") {
        nodeStack.pop();
    }
}

void ASTBuilder::startClassDeclaration() {
    // Create a new class declaration node
    std::string className = ""; // Will be filled by processToken
    auto classNode = std::make_shared<ast::ClassDeclaration>(className);
    
    // Add it to the program node
    ast->declarations.push_back(classNode);
    
    // Push it onto the stack
    nodeStack.push(ProductionData("ClassDecl"));
    nodeStack.top().node = classNode;
    
    // Update current context
    currentClass = &nodeStack.top();
}

void ASTBuilder::finishClassDeclaration() {
    // Pop the class declaration node from the stack
    if (!nodeStack.empty() && nodeStack.top().name == "ClassDecl") {
        // Finalize any properties
        auto classNode = std::static_pointer_cast<ast::ClassDeclaration>(nodeStack.top().node);
        
        nodeStack.pop();
    }
    
    // Reset current context
    currentClass = nullptr;
}

void ASTBuilder::startImplementation() {
    // Create a dummy implementation node (we'll create real function declarations)
    std::string className = ""; // Will be filled by processToken
    auto classNode = std::make_shared<ast::ClassDeclaration>(className);
    
    // Add it to the program node
    ast->declarations.push_back(classNode);
    
    // Push it onto the stack
    nodeStack.push(ProductionData("ImplDecl"));
    nodeStack.top().node = classNode;
}

void ASTBuilder::finishImplementation() {
    // Pop the implementation node from the stack
    if (!nodeStack.empty() && nodeStack.top().name == "ImplDecl") {
        nodeStack.pop();
    }
}
void ASTBuilder::startFunction() {
    // Create a new function node
    std::string funcName = ""; // Will be filled by processToken
    auto primitiveType = std::make_shared<ast::PrimitiveType>(ast::PrimitiveType::Kind::Void); // Default
    auto returnType = std::static_pointer_cast<ast::Type>(primitiveType);
    auto functionNode = std::make_shared<ast::FunctionDeclaration>(funcName, returnType);
    
    // Add it to the appropriate parent node
    if (currentClass != nullptr) {
        auto classNode = std::static_pointer_cast<ast::ClassDeclaration>(currentClass->node);
        classNode->methods.push_back(functionNode);
    }
    else {
        ast->declarations.push_back(functionNode);
    }
    
    // Push it onto the stack
    nodeStack.push(ProductionData("FunctionDecl"));
    nodeStack.top().node = functionNode;
    
    // Update current context
    currentFunction = &nodeStack.top();
}


void ASTBuilder::finishFunction() {
    // Pop the function node from the stack
    if (!nodeStack.empty() && nodeStack.top().name == "FunctionDecl") {
        nodeStack.pop();
    }
    
    // Reset current context
    currentFunction = nullptr;
}

void ASTBuilder::startStatement() {
    // Will create a specific statement node once we know what type it is
    nodeStack.push(ProductionData("Statement"));
    currentStatement = &nodeStack.top();
}

void ASTBuilder::finishStatement() {
    // Pop the statement node from the stack
    if (!nodeStack.empty() && nodeStack.top().name == "Statement") {
        // Add the statement to the function body if appropriate
        if (currentFunction != nullptr && nodeStack.top().node) {
            auto functionNode = std::static_pointer_cast<ast::FunctionDeclaration>(currentFunction->node);
            auto statementNode = std::static_pointer_cast<ast::Statement>(nodeStack.top().node);
            if (statementNode) {
                functionNode->body.push_back(statementNode);
            }
        }
        
        nodeStack.pop();
    }
    
    // Reset current context
    currentStatement = nullptr;
}

void ASTBuilder::startExpression() {
    // Will create a specific expression node once we know what type it is
    nodeStack.push(ProductionData("Expression"));
}

void ASTBuilder::finishExpression() {
    // Pop the expression node from the stack
    if (!nodeStack.empty() && nodeStack.top().name == "Expression") {
        // Add to parent based on context
        nodeStack.pop();
    }
}

void ASTBuilder::handleIdentifier(const std::string& value) {
    // Handle identifier based on context
    if (currentClass != nullptr && currentClass->node != nullptr) {
        // Update class name if needed
        auto classNode = std::static_pointer_cast<ast::ClassDeclaration>(currentClass->node);
        if (classNode->name.empty()) {
            classNode->name = value;
        }
    }
    else if (currentFunction != nullptr && currentFunction->node != nullptr) {
        // Update function name if needed
        auto functionNode = std::static_pointer_cast<ast::FunctionDeclaration>(currentFunction->node);
        if (functionNode->name.empty()) {
            functionNode->name = value;
        }
    }
    else if (!nodeStack.empty() && nodeStack.top().name == "Expression") {
        // Create a variable reference
        auto varRef = std::make_shared<ast::VariableReference>(value);
        nodeStack.top().node = varRef;
    }
}

void ASTBuilder::handleLiteral(const std::string& type, const std::string& value) {
    // Create a literal node
    if (type == "Integer") {
        auto literalNode = std::make_shared<ast::IntegerLiteral>(std::stoi(value));
        
        // Add to current context if appropriate
        if (!nodeStack.empty() && nodeStack.top().name == "Expression") {
            nodeStack.top().node = literalNode;
        }
    }
    else if (type == "Float") {
        auto literalNode = std::make_shared<ast::FloatLiteral>(std::stof(value));
        
        // Add to current context if appropriate
        if (!nodeStack.empty() && nodeStack.top().name == "Expression") {
            nodeStack.top().node = literalNode;
        }
    }
}

void ASTBuilder::handleOperator(const std::string& op) {
    // Store operator for later use in expressions
    if (!nodeStack.empty()) {
        nodeStack.top().attributes["operator"] = op;
    }
}

void ASTBuilder::handleType(const std::string& type) {
    // Create a type node based on the type string
    std::shared_ptr<ast::Type> typeNode;
    
    if (type == "int") {
        auto primitiveType = std::make_shared<ast::PrimitiveType>(ast::PrimitiveType::Kind::Int);
        typeNode = std::static_pointer_cast<ast::Type>(primitiveType);
    }
    else if (type == "float") {
        auto primitiveType = std::make_shared<ast::PrimitiveType>(ast::PrimitiveType::Kind::Float);
        typeNode = std::static_pointer_cast<ast::Type>(primitiveType);
    }
    else if (type == "void") {
        auto primitiveType = std::make_shared<ast::PrimitiveType>(ast::PrimitiveType::Kind::Void);
        typeNode = std::static_pointer_cast<ast::Type>(primitiveType);
    }
    
    // Store type information in current context
    if (!nodeStack.empty() && typeNode) {
        // If we're in a function declaration, update return type
        if (nodeStack.top().name == "FunctionDecl") {
            auto functionNode = std::static_pointer_cast<ast::FunctionDeclaration>(nodeStack.top().node);
            functionNode->returnType = typeNode;
        }
        else {
            nodeStack.top().attributes["type"] = type;
        }
    }
}

void ASTBuilder::pushContext() {
    // Save current context for later retrieval
}

void ASTBuilder::popContext() {
    // Restore previous context
}
