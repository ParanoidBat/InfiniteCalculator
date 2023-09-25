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

string reverse_string(string str) {
    string rev_str = "";
    for(string::reverse_iterator i = str.rbegin(); i != str.rend(); i++){
        rev_str.push_back(*i);
    }

    return rev_str;
}

char handle_carry_result(short res, bool& carry){
    if(res > 9){
        res = res - 9 - 1; // get the least significant digit of the number
        carry = true;
    }
    else{
        carry = false;
    }

    char char_res = (char)(res + 48); // get the ASCII representation of the actual number

    return char_res;
}

string addition(string x, string y) {
    bool carry = false;

    if(x.length() < y.length() ){
        swap(x, y);
    }

    string result = "";
    short op1, op2, local_res;
    char char_res;
    for(string::reverse_iterator ix = x.rbegin(), iy = y.rbegin(); ix != x.rend(); ix++, iy++ ){
        if (iy >= y.rend()){
            // the smaller number has ended.
            op1 = (short)(*ix - 48);

            local_res = op1 + carry;
            result.push_back(handle_carry_result(local_res, carry));
        }
        else{
            op1 = (short)(*ix - 48); // get the actual number instead of the ASCII
            op2 = (short)(*iy - 48);

            local_res = op1 + op2 + carry;
            result.push_back(handle_carry_result(local_res, carry));
        }
    }

    if(carry) {
        result.push_back('1');
    }

    return result;
}

void calculate(){
    string operand2 = numbers.top();
    numbers.pop();
    string operand1 = numbers.top();
    numbers.pop();

    string res;

    switch(operators.top()){
    case '+':
        res = addition(operand1, operand2);
        break;
    }

    numbers.push(reverse_string(res));
    operators.pop();
}

// TODO: Optimization -> when adding or subtracting, remove from the both operands the same amount of zeros from the right side.
// The same amount of zeros can then be appended to the result string. This will reduce the iterations
int main(){
    string input = "340282366920000000000000000000000000000+340282366920000000000000000000000000000";
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
        // TODO: Check if the following condition can be true. if not; remove condition
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
    cout<<"res: "<<numbers.top();

    return 0;
}
