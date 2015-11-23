#include "node.h"

int Node::nid = 0;

Node::Node() {
    nid++;
}

Node::~Node() {}


NExpression::NExpression() {
    unique_id = nid;
}

NStatement::NStatement() {
    unique_id = nid;
}

void NStatement::print() {
}

NBlock::NBlock() {}
void NBlock::print() {
            std::string dotNodeName = "Expression"+std::to_string(unique_id);
            for (auto stmt : statements) {
                std::cout << dotNodeName << " -> Statement" << stmt->unique_id << ";\n";
                stmt->print();
            }
}


void NProgram::print() {
            std::cout << "digraph parseTree {\n";
            for (auto b : blocks) {
                b->print();
            }
            for (auto p : prototypes) {
                p->print();
            }
            std::cout << "}\n";
        }


NDouble::NDouble(double value) : value(value) { }
NInteger::NInteger(long long value) : value(value) { }
NProgram::NProgram() {}

NFunctionPrototype::NFunctionPrototype(const NIdentifier& type, const NIdentifier& id, const VariableList& arguments) : type(type), id(id), arguments(arguments) { }

NFunctionDeclaration::NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, const VariableList& arguments, NBlock& block) :
                                        type(type), id(id), arguments(arguments), block(block) { }

NIdentifier::NIdentifier(const std::string& name) : name(name) { }

NBinaryOperator::NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) : lhs(lhs), rhs(rhs), op(op) { }

NAssignment::NAssignment(NIdentifier& lhs, NExpression& rhs) : lhs(lhs), rhs(rhs) { }

NExpressionStatement::NExpressionStatement(NExpression& expression) : expression(expression) { }

NMethodCall::NMethodCall(const NIdentifier& id, ExpressionList& arguments) : id(id), arguments(arguments) { }

NMethodCall::NMethodCall(const NIdentifier& id) : id(id) { }
