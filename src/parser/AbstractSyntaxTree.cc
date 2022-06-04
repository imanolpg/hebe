/*!
 * @author Imanol
 * @date 3/6/22
 * @project hebe
 */


#include <memory>
#include <string>
#include <vector>


namespace {

    /**
     * Base class for all nodes
     */
    class ExprAST {
    public:
        virtual ~ExprAST() = default;
    };


    /**
     * Number node class
     */
    class NumberExprAST : public ExprAST {
        double Val;

    public:
        NumberExprAST(double Val_V) : Val(Val_V) {}
    };


    /**
     * Variable ndoe class
     */
    class VariableExprAST : public ExprAST {
        std::string Name;

    public:
        VariableExprAST(const std::string &Name) : Name(Name) {}
    };


    /**
     * Operator node class
     */
    class BinaryExprAST : public ExprAST {
        char Op;
        std::unique_ptr<ExprAST> LHS, RHS;

    public:
        BinaryExprAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS) :
                Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
    };


    /**
     * Function calls node class
     */
    class CallExprAST : public ExprAST {
        std::string Callee;
        std::vector<std::unique_ptr<ExprAST>> Args;

    public:
        CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args) :
                Callee(Callee), Args(std::move(Args)) {}
    };


    /**
     * Function Prototype node class
     */
    class PrototypeAST {
        std::string Name;
        std::vector<std::string> Args;

    public:
        PrototypeAST(const std::string &Name, std::vector<std::string> Args) : Name(Name), Args(std::move(Args)) {}

        const std::string &getName() const { return Name; }
    };


    /**
     * Function definition node class
     */
    class FunctionAST {
        std::unique_ptr<PrototypeAST> Proto;
        std::unique_ptr<ExprAST> Body;

    public:
        FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body) : Proto(std::move(Proto)),
        Body(std::move(Body)) {}
    };


}




