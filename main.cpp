#include <iostream>
#include <stack>
#include <string>
#include <stdexcept>
#include <unordered_map>

using namespace std;

stack <char> operators;
stack <string> numbers;

bool is_num(char x){
    return (int)x >= 48 && (int)x <= 57;
}

bool is_opening_bracket(char in){
    return in == '(' || in == '{' || in == '[';
}

bool is_closing_bracket(char in){
    return in == ')' || in == '}' || in == ']';
}

char get_opening_bracket(char in){
    switch(in){
    case ')':
        return '(';
    case '}':
        return '{';
    case ']':
        return '[';
    }
}

int perform_calculation(char op, int operand1, int operand2){
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

void calculate(){
    string operand2 = numbers.top();
    numbers.pop();
    string operand1 = numbers.top();
    numbers.pop();

//    int res = perform_calculation(operators.top(), operand1, operand2);
//    numbers.push(res);
//    operators.pop();
}

int main(){
    string input = "25+50";
    char in;
    unordered_map<char, short> op_prec;
    bool isNum = false;

    op_prec['['] = 7;
    op_prec['{'] = 6;
    op_prec['('] = 5;
    op_prec['/'] = 4;
    op_prec['*'] = 3;
    op_prec['+'] = 2;
    op_prec['-'] = 1;

    for (int i = 0; i < input.length(); i++){
        in = input.at(i);

        if(is_num(in)){
            numbers.push("");
            char n;
            while(1){
                try{
                    n = input.at(i);
                }
                catch(const out_of_range& err){
                    break;
                }

                if(is_num(n)){
                    numbers.top().push_back(n);
                    i++;
                }
                else{
                    i--; // restore i to previous value, lest the operator is skipped
                    break;
                }
            }
        }
        else if(!operators.empty()){
            char tos = operators.top();
            if (is_closing_bracket(in)){
                char opening_bracket = get_opening_bracket(in);
                while (!operators.empty() && operators.top() != opening_bracket){
                    calculate();
                }
                operators.pop(); // pop the opening bracket as well
            }
            else if (op_prec[in] < op_prec[tos] && !is_opening_bracket(tos)){
                while (!operators.empty() && !is_opening_bracket(operators.top()) && op_prec[in] <= op_prec[operators.top()]){
                    calculate();
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
        if (is_closing_bracket(operators.top())){
            char opening_bracket = get_opening_bracket(operators.top());
            while (!operators.empty() && operators.top() != opening_bracket){
                calculate();
            }
            operators.pop(); // pop the opening bracket as well
        }
        else{
            calculate();
        }
    }
//    cout<<"res: "<<numbers.top();

    return 0;
}
