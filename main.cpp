#include <iostream>
#include <stack>
#include <unordered_map>
using namespace std;

bool is_num(char x){
    return (int)x >= 48 && (int)x <= 57;
}

int main(){
    string input = "(1+2)*3+5";
    string expression = "";
    stack <char> operators;
    unordered_map<char, short> op_prec;

    op_prec['('] = 5;
    op_prec['/'] = 4;
    op_prec['*'] = 3;
    op_prec['+'] = 2;
    op_prec['-'] = 1;

    for (char in : input){
        if(is_num(in)){
            expression += in;
        }
        else if(!operators.empty()){
            char tos = operators.top();
            if (in == ')'){
                while (!operators.empty() && operators.top() != '('){
                    expression += operators.top();
                    operators.pop();
                }
                operators.pop(); // pop the ( as well
            }
            else if (op_prec[in] < op_prec[tos] && tos != '('){
                while (!operators.empty() && op_prec[in] <= op_prec[operators.top()]){
                    expression += operators.top();
                    operators.pop();
                }
                operators.push(in);
            }
            else {
                operators.push(in);
            }
        }
        else {
            operators.push(in);
        }
    }

    while (!operators.empty()){
        expression += operators.top();
        operators.pop();
    }
    cout<< expression;

    string final = "";
    for(int i = expression.length - 1; i >= 2; i--){ // the first two elements will be digits
        // if this is operator, check if next two are operands, without going out of bounds
        if(!is_num(expression[i]) && i-2 >= 0 && is_num(expression[i-1]) && is_num(expression[i-2]) ){

        }
    }

    return 0;
}
