#include <iostream>
#include <stack>
#include <string>
#include <stdexcept>
#include <unordered_map>

using namespace std;

typedef string::reverse_iterator rit;

struct Decimals {
    size_t x_pos, y_pos;
    size_t x_len, y_len;
    size_t dec_places_x, dec_places_y;
    size_t larger;

    Decimals() {}

    Decimals(size_t x_pos, size_t y_pos, size_t x_len, size_t y_len){
        this->x_pos = x_pos;
        this->y_pos = y_pos;
        this->x_len = x_len;
        this->y_len = y_len;

        dec_places_x = x_len-x_pos-1;
        dec_places_y = y_len-y_pos-1;

        larger = (dec_places_x > dec_places_y) ? dec_places_x : dec_places_y;
    }

    Decimals& operator=(const Decimals& decimals){
        x_pos = decimals.x_pos;
        y_pos = decimals.y_pos;
        x_len = decimals.x_len;
        y_len = decimals.y_len;

        dec_places_x = decimals.dec_places_x;
        dec_places_y = decimals.dec_places_y;
        larger = decimals.larger;

        return *this;
    }

    void print(){
        cout<<"x_pos: "<<x_pos <<", y_pos: "<<y_pos<<endl;
        cout<<"x_len: "<<x_len <<", y_len: "<<y_len<<endl;
        cout<<"dec_palces_x: "<<dec_places_x<<", dec_places_y: "<<dec_places_y<<endl;
    }
};

stack <char> operators;
stack <string> numbers;
// TODO: shrink to fit each string after the whole number has been read

string subtraction(string x, string y, bool from_div = false);

bool is_num(char x){
    return (x >= '0' && x <= '9') || x == '.';
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

void validate_dividend(string dividend, string divisor, bool& is_divisor_smaller){
    if(dividend.length() < divisor.length()){
        is_divisor_smaller = false;
    }
    else if(dividend.length() == divisor.length()){
        for(size_t i = 0; i < dividend.length(); i++){
            if(dividend[i] < divisor[i]){
                is_divisor_smaller = false;
                break;
            }
            if(dividend[i] > divisor[i]){
                break; // dividend is greater than divisor
            }
        }
    }
}

string addition_lower_decimal(string x, string y, bool& carry){
    if(x.empty()){
        carry = false;
        return y;
    }
    if(y.empty()){
        carry = false;
        return x;
    }

    string result = "";
    short op1, op2, local_res;

    if(x.length() < y.length() ){
        swap(x, y);
    }

    for(rit ix = x.rbegin(), iy = y.rbegin(); ix != x.rend(); ix++, iy++ ){
        if (iy >= y.rend()){
            op1 = (short)(*ix - 48);

            local_res = op1 + carry;
            result.push_back(handle_carry_result(local_res, carry));
        }
        else{
            op1 = (short)(*ix - 48);
            op2 = (short)(*iy - 48);

            local_res = op1 + op2 + carry;
            result.push_back(handle_carry_result(local_res, carry));
        }
    }

    return reverse_string(result);
}

string addition(string x, string y) {
    bool isSigned = false;
    string decimal_result = "";
    bool carry = false;
    string result = "";
    short op1, op2, local_res;

    if((x.at(0) == '-') != (y.at(0) == '-')){
        if(x.at(0) == '-'){
            x.erase(x.begin());
            swap(x, y); // let subtraction function handle the signing criteria
        }
        else{
            y.erase(y.begin());
        }

        return subtraction(x, y);
    }
    if(x.at(0) == '-' && y.at(0) == '-'){
        isSigned = true;

        // remove the signs
        x.erase(x.begin());
        y.erase(y.begin());
    }

    // check if decimal/s exist
    const size_t x_decimal = x.find('.');
    const size_t y_decimal = y.find('.');
    string x_lower_decimal, y_lower_decimal;

    if (x_decimal != string::npos || y_decimal != string::npos){
        if(x_decimal != string::npos){
            x_lower_decimal = x.substr(x_decimal+1, x.length()-1);
            x = x.substr(0, x_decimal);
        }
        else {
            x_lower_decimal = "";
        }

        if(y_decimal != string::npos){
            y_lower_decimal = y.substr(y_decimal+1, y.length()-1);
            y = y.substr(0, y_decimal);
        }
        else{
            y_lower_decimal = "";
        }
        decimal_result = addition_lower_decimal(x_lower_decimal, y_lower_decimal, carry);
    }

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
            op1 = (short)(*ix - 48); // get  the actual number instead of the ASCII
            op2 = (short)(*iy - 48);

            local_res = op1 + op2 + carry;
            result.push_back(handle_carry_result(local_res, carry));
        }
    }

    if(carry) {
        result.push_back('1');
    }
    if(isSigned){
        result.push_back('-');
    }

    return decimal_result.empty() ? reverse_string(result) : reverse_string(result)+"."+decimal_result;
}

int balance_fraction(string& x, string& y){
    const size_t x_decimal = x.find('.');
    const size_t y_decimal = y.find('.');
    int decimal_places;

    if(x_decimal == string::npos && y_decimal == string::npos) return 0; // None of them is a fraction

    // One of them is a fraction, the other is not. Make the other a fraction too
    if(x_decimal == string::npos && y_decimal != string::npos){
        decimal_places = y.length() - 1 - y_decimal;
        x.push_back('.');
        x.append(decimal_places, '0');

        return decimal_places;
    }
    if(x_decimal != string::npos && y_decimal == string::npos){
        decimal_places = x.length() - 1 - x_decimal;
        y.push_back('.');
        y.append(decimal_places, '0');

        return decimal_places;
    }

    // Both are fractions. Check if decimal places are equal
    int dec_places_x = x.length() - 1 - x_decimal;
    int dec_places_y = y.length() - 1 - y_decimal;

    if(dec_places_x > dec_places_y){
        decimal_places = dec_places_x;
        y.append(dec_places_x - dec_places_y, '0');

        return decimal_places;
    }
    else if(dec_places_x < dec_places_y){
        decimal_places = dec_places_y;
        x.append(dec_places_y - dec_places_x, '0');

        return decimal_places;
    }
    else{
        return dec_places_x;
    }
}

Decimals get_decimal_positions(string x, string y){
    const size_t x_decimal = x.find('.');
    const size_t y_decimal = y.find('.');

    return Decimals(x_decimal, y_decimal, x.length(), y.length());
}

string subtraction(string x, string y, bool from_div){
    bool isSigned = false;
    bool double_negative = false;
    short op1, op2, local_res;
    string result;
    char curr_num;
    int decimal_places = 0;
    Decimals decimals;

    if(from_div){
        decimals = get_decimal_positions(x, y);
        decimals.print();
    }
    else{
       decimal_places = balance_fraction(x, y);
    }


    if((x.at(0) == '-') != (y.at(0) == '-')){
        if(x.at(0) == '-'){
            x.erase(x.begin());
            isSigned = true;
        }
        else{
            y.erase(y.begin());
        }

        string res = addition(x, y);

        if(isSigned){
            res.insert(res.begin(), '-');
        }
        return res;
    }
    if(x.at(0) == '-' && y.at(0) == '-'){
        // remove the signs
        x.erase(x.begin());
        y.erase(y.begin());

        double_negative = true;
    }

    cout<<"before removing decimals. x: "<<x<<", y: "<<y<<endl;

    // remove the decimals
    if(decimal_places){
        x.erase(x.end() - (decimal_places+1));
        y.erase(y.end() - (decimal_places+1));
    }
    else if (from_div){
        x.erase(x.begin() + decimals.x_pos);
        y.erase(y.begin() + decimals.y_pos);
    }

    cout<<"after removing decimals. x: "<<x<<", y: "<<y<<endl;

    if(x.length() < y.length()){
        cout<<"swap by length\n";
        swap(x, y);
        isSigned = true;
    }

    if(x.length() == y.length()){
        for(size_t i = 0; i < x.length(); i++){
            if(x[i] < y[i]){
                cout<<"swap by digit\n";
                swap(x,y);
                isSigned = true;
                break;
            }
            if(x[i] > y[i]){
                break; // X is greater than Y
            }
        }
    }

    cout<<"after swapping. x: "<<x<<", y: "<<y<<endl;

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

    // Remove trailing zeros
    int zeros = 0;
    for(rit i = result.rbegin(); i != result.rend(); i++){
        if(*i != '0'){
            break;
        }
        zeros++;
    }
    for(;zeros > 0; zeros--){
        result.pop_back();
    }

    if (result.empty()){
        return "0";
    }

    if((isSigned && !double_negative) || (!isSigned && double_negative)){
        result.push_back('-');
    }
    string r_str = reverse_string(result);

    if(decimal_places){
        string::iterator it= r_str.end();
        r_str.insert(r_str.end() - decimal_places, '.');
    }
    else if(from_div){
        string::iterator it= r_str.end();
        r_str.insert(r_str.end() - decimals.larger, '.');
    }

    return r_str;
}

string multiplication(string x, string y) {
    string result = "", str_op1 = "";
    short op1, op2, local_res;
    short carry = 0;
    stack<string> results;
    bool isSigned = false;

    int decimal_places = 0;
    int x_dec = x.find('.');
    int y_dec = y.find('.');

    if(x_dec != string::npos){
        decimal_places += x.length() - 1 - x_dec;
        x.erase(x.begin() + x_dec);
    }
    if(y_dec != string::npos){
        decimal_places += y.length() - 1 - y_dec;
        y.erase(y.begin() + y_dec);
    }


    if((x.at(0) == '-') != (y.at(0) == '-')){
        isSigned = true;

        if(x.at(0) == '-'){
            x.erase(x.begin());
        }
        else{
            y.erase(y.begin());
        }
    }
    if(x.at(0) == '-' && y.at(0) == '-'){
        // remove the signs
        x.erase(x.begin());
        y.erase(y.begin());
    }

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
        if(isSigned){
            result.insert(result.begin(), '-');
        }
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

    if(isSigned){
        result.insert(result.begin(), '-');
    }
    if(decimal_places){
        string::iterator it= result.end();

        result.insert(result.end() - decimal_places, '.');
    }

    return result;
}

// TODO: Handle divide by 0 case
string division(string dividend, string divisor) {
    string quotient = "0", result = "";
    bool isSigned = false;
    bool is_divisor_smaller = true;
    short decimal_places = 4;

    if((dividend.at(0) == '-') != (divisor.at(0) == '-')){
        isSigned = true;

        if(dividend.at(0) == '-'){
            dividend.erase(dividend.begin());
        }
        else{
            divisor.erase(divisor.begin());
        }
    }
    if(dividend.at(0) == '-' && divisor.at(0) == '-'){
        dividend.erase(dividend.begin());
        divisor.erase(divisor.begin());
    }

    validate_dividend(dividend, divisor, is_divisor_smaller);

    while(dividend != "0" && decimal_places ){
        quotient = "0";

        while(is_divisor_smaller ){
            dividend = subtraction(dividend, divisor);
            quotient = addition(quotient, "1");

            if(dividend == "0"){
                break;
            }

            validate_dividend(dividend, divisor, is_divisor_smaller);
        }

        result = quotient;

        if(dividend == "0"){
            continue; // will essentially break the parent while loop
        }

        result = quotient + ".";

        cout<<"result(before going in 2nd while): "<<result<<endl;

        cout<<"before decimals. dividend: "<<dividend<<", divisor: "<<divisor<<endl;
        // Put decimals on dividend and divisor before doing fraction division
        if(dividend.find('.') == string::npos){
            dividend.push_back('.');
        }
        if(divisor.find('.') == string::npos){
            divisor.append(".0");
        }
        cout<<"after decimals. dividend: "<<dividend<<", divisor: "<<divisor<<endl;


        while(decimal_places){
            cout<<"decimal places left: "<<decimal_places<<endl;
            decimal_places--;
            quotient = "0";
            dividend.push_back('0');
            is_divisor_smaller = true;

            cout<<"dividend: "<<dividend<<", divisor: "<<divisor<<endl;

            if(dividend.length() < divisor.length()){
                cout<<"dividend is less than divisor\n";
                is_divisor_smaller = false;
                continue;
            }
            if(dividend.length() == divisor.length()){
                cout<<"dividend is equal to divisor\n";
                for(size_t i = 0; i < dividend.length(); i++){
                    if(dividend[i] < divisor[i]){
                        is_divisor_smaller = false;
                        break;
                    }
                    if(dividend[i] > divisor[i]){
                        break; // dividend is greater than divisor
                    }
                }
            }

            cout<<"(before nested while loop). dividend: "<<dividend<<", divisor: "<<divisor<<endl;

            while(is_divisor_smaller){
                dividend = subtraction(dividend, divisor, true);
                quotient = addition(quotient, "1");

            cout<<"(in nested while loop). dividend: "<<dividend<<", quotient: "<<quotient<<endl;

                if(dividend == "0"){
                    break;
                }
                if(dividend.length() < divisor.length()){
                    cout<<"dividend length is less\n";
                    is_divisor_smaller = false;
                    continue;
                }
                if(dividend.length() == divisor.length()){
                    cout<<"dividend is equal to divisor\n";
                    for(size_t i = 0, j = 0; i < dividend.length(); i++, j++){
                        if(dividend[i] == '.'){
                            j--;
                            continue;
                        }
                        if(dividend[i] < divisor[j]){
                            is_divisor_smaller = false;
                            break;
                        }
                        if(dividend[i] > divisor[j]){
                            break; // dividend is greater than divisor
                        }
                    }
                }
            }
            result += quotient;
            if(dividend == "0"){
                break;
            }
        }
    }

    if(isSigned){
        result.insert(result.begin(), '-');
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
    case '/':
        res = division(operand1, operand2);
        break;
    }

    numbers.push(res);
    operators.pop();
}

// TODO: Optimization -> when adding or subtracting, remove from the both operands the same amount of zeros from the right side.
// The same amount of zeros can then be appended to the result string. This will reduce the iterations
// TODO: Clean the results of fraction operations; for results that have only zeros after decimal(2.0000),
// The decimal should be removed before pushing on the stack
int main(){
    string input = "1/3";
    int input_len = input.length();
    char in;
    unordered_map<char, short> op_prec;
    bool is_negative_number = false;

    op_prec['['] = 7;
    op_prec['{'] = 6;
    op_prec['('] = 5;
    op_prec['/'] = 4;
    op_prec['*'] = 3;
    op_prec['+'] = 2;
    op_prec['-'] = 1;

    if(input.at(0) == '-'){
        is_negative_number = true;
    }

    for (int i = 0; i < input_len; i++){
        in = input.at(i);

        if(is_num(in) || is_negative_number){
            if(is_negative_number){
                numbers.push("-");
                i++;
            }
            else{
                numbers.push("");
            }

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

            is_negative_number = false;
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
                if(i < input_len-1 && input.at(i+1) == '-'){
                    is_negative_number = true;
                }
            }
            else {
                operators.push(in);
                if(i < input_len-1 && input.at(i+1) == '-'){
                    is_negative_number = true;
                }
            }
        }
        else {
            operators.push(in);
            if(i < input_len-1 && input.at(i+1) == '-'){
                is_negative_number = true;
            }
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
    cout<<"result: "<<numbers.top();

    return 0;
}
