#include <iostream>
#include <stack>
#include <string>
#include <stdexcept>
#include <unordered_map>

using namespace std;

typedef string::reverse_iterator rit;

stack <char> operators;
stack <string> numbers;
// TODO: shrink to fit each string after the whole number has been read

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
    for(rit i = str.rbegin(); i != str.rend(); i++){
        rev_str.push_back(*i);
    }

    return rev_str;
}

char handle_carry_result(short res, bool& carry){
    if(res > 9){
        // get the least significant digit of the number
        res %= 10;
        carry = true;
    }
    else{
        carry = false;
    }

    char char_res = (char)(res + 48); // get the ASCII representation of the actual number

    return char_res;
}

char handle_carry_result(short res, short& carry){
    if(res > 9){
        carry = res/10;
        res %= 10;
    }
    else{
        carry = 0;
    }

    char char_res = (char)(res + 48);

    return char_res;
}

string addition(string x, string y) {
    bool carry = false;
    string result = "";
    short op1, op2, local_res;

    if(x.length() < y.length() ){
        swap(x, y);
    }

    for(rit ix = x.rbegin(), iy = y.rbegin(); ix != x.rend(); ix++, iy++ ){
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

    return reverse_string(result);
}

string subtraction(string x, string y){
    bool isSigned = false;
    short op1, op2, local_res;
    string result;
    char curr_num;

    if(x.length() < y.length()){
        swap(x, y);
        isSigned = true;
    }
    if(x.length() == y.length()){
        for(size_t i = 0; i < x.length(); i++){
            if(x[i] < y[i]){
                swap(x,y);
                isSigned = true;
                break;
            }
            if(x[i] > y[i]){
                break; // X is greater than Y
            }
        }
    }

    for(rit ix = x.rbegin(), iy = y.rbegin(); ix != x.rend(); ix++, iy++){
        if(iy >= y.rend()){
            result.push_back(*ix);
        }
        else{
            op1 = (short)(*ix - 48);
            op2 = (short)(*iy - 48);

            if(op1 < op2){
                // borrow
                int units = 1; // number of digits traversed back to get a borrow
                do {
                    curr_num = *(ix+units);
                    units++;
                }
                while(curr_num == '0');

                units--; // the position of the first non-zero character encountered
                *(ix+units) -= 1; // subtract 1 from the borrowed number
                units--;

                for(; units > 0; units--){
                    // replace all the zeros we came across, with 9
                    *(ix+units) = '9';
                }

                op1 = 10 + op1;
            }

            local_res = op1 - op2;

            char char_res = (char)(local_res + 48);
            result.push_back(char_res);
        }
    }

    if(isSigned){
        result.push_back('-');
    }

    return reverse_string(result);
}

string multiplication(string x, string y) {
    string result = "", str_op1 = "";
    short op1, op2, local_res;
    short carry = 0;
    stack<string> results;

    if(x.length() < y.length() ){
        swap(x, y);
    }

    for(rit ix = x.rbegin(); ix != x.rend(); ix++){
        results.push("");
        for(rit iy = y.rbegin(); iy != y.rend(); iy++){
            op1 = (short)(*ix - 48);
            op2 = (short)(*iy - 48);

            local_res = (op1*op2) + carry;
            results.top().push_back(handle_carry_result(local_res, carry));
        }
        if(carry){
            results.top().push_back((char)(carry+48));
            carry = 0;
        }
    }

    size_t no_of_results = results.size();
    result = reverse_string(results.top());
    results.pop();

    if(results.empty()){
        // single digits multiplication will only have one results element
        return result;
    }

    no_of_results--;
    result.insert(result.end(), no_of_results, '0');

    for( size_t i = no_of_results - 1; !results.empty(); i-- ){
        str_op1 = reverse_string(results.top());
        str_op1.insert(str_op1.end(), i, '0'); // pad with zero to emulate the 'x' in manual multiplication
        results.pop();

        result = addition(str_op1, result);
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
    case '*':
        res = multiplication(operand1, operand2);
        break;
    case '-':
        res = subtraction(operand1, operand2);
        break;
    }

    numbers.push(res);
    operators.pop();
}

// TODO: Optimization -> when adding or subtracting, remove from the both operands the same amount of zeros from the right side.
// The same amount of zeros can then be appended to the result string. This will reduce the iterations
int main(){
    string input = "874652-8645213";
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
