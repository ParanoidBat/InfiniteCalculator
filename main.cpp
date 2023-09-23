#include <iostream>
#include <stack>
#include <unordered_map>
using namespace std;

bool is_num(char x){
    return (int)x >= 48 && (int)x <= 57;
}

int calculate(char op, int operand1, int operand2){
    switch(op){
    case '+':
        return operand1 + operand2;
    case '-':
        return operand1 - operand2;
    case '*':
        return operand1 * operand2;
    case '/':
        return operand1 / operand2;
    }
}

int main(){
    string input = "(1+2)*3+5";
    string expression = "";
    stack <char> operators;
    stack <int> numbers;
    unordered_map<char, short> op_prec;
    int res, operand1, operand2;

    op_prec['('] = 5;
    op_prec['/'] = 4;
    op_prec['*'] = 3;
    op_prec['+'] = 2;
    op_prec['-'] = 1;

    for (char in : input){
        if(is_num(in)){
            numbers.push((int)in - 48); // put in the actual number, not the ASCII
        }
        else if(!operators.empty()){
            char tos = operators.top();
            if (in == ')'){
                while (!operators.empty() && operators.top() != '('){
                    operand2 = numbers.top();
                    numbers.pop();
                    operand1 = numbers.top();
                    numbers.pop();

                    res = calculate(operators.top(), operand1, operand2);
                    numbers.push(res);
                    operators.pop();
                }
                operators.pop(); // pop the ( as well
            }
            else if (op_prec[in] < op_prec[tos] && tos != '('){
                while (!operators.empty() && op_prec[in] <= op_prec[operators.top()]){
                    operand2 = numbers.top();
                    numbers.pop();
                    operand1 = numbers.top();
                    numbers.pop();

                    res = calculate(operators.top(), operand1, operand2);
                    numbers.push(res);
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
        operand2 = numbers.top();
        numbers.pop();
        operand1 = numbers.top();
        numbers.pop();

        res = calculate(operators.top(), operand1, operand2);
        numbers.push(res);
        operators.pop();
    }
    cout<<"res: "<<numbers.top();

    return 0;
}
