#include <iostream>
#include <stack>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <regex>

#define MAX_DECIMAL_PLACES 30

using namespace std;

typedef string::reverse_iterator rit;
typedef string::iterator it;

enum Equality {IS_LESS, IS_EQUAL, IS_GREATER};

stack <char> operators;
stack <string> numbers;
regex rgx_trailing_zeros("\\.0*$"); // decimal point and only zeros after it. (eg: 22.00)
regex rgx_zero_value("0*\\.?0*");
smatch matcher;

string subtraction(string x, string y);

Equality get_divisor_equality(string dividend, string divisor){
    if(divisor.length() > dividend.length()){
        return IS_GREATER;
    }
    if(divisor.length() < dividend.length()){
        return IS_LESS;
    }

    // The length is equal. Check for digits equality
    for(it i_dividend = dividend.begin(), i_divisor = divisor.begin(); i_divisor != divisor.end(); i_dividend++, i_divisor++){
        if(*i_divisor < *i_dividend){
            return IS_LESS;
        }
        if(*i_divisor > *i_dividend){
            return IS_GREATER;
        }
    }

    return IS_EQUAL;
}

bool is_num(char x){
    return (x >= '0' && x <= '9') || x == '.';
}

bool is_opening_bracket(char in){
    return in == '(' || in == '{' || in == '[';
}

bool is_closing_bracket(char in){
    return in == ')' || in == '}' || in == ']';
}

bool is_divisor_fraction(string divisor){
    return divisor.find('.') != string::npos;
}

bool is_value_zero(string value){
    if(regex_search(value, matcher, rgx_zero_value)){
        return matcher[0].length() == value.length();
    }

    return false;
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

void handle_fraction_divisor(string& divisor, string& dividend){
    if(divisor[0] == '.'){
        divisor.insert(0, 1, '0');
    }
    if(dividend[0] == '.'){
        dividend.insert(0, 1, '0');
    }

    size_t dec_pos_divisor = divisor.find('.');
    size_t dec_pos_dividend = dividend.find('.');
    size_t dec_places = divisor.length() - dec_pos_divisor - 1;

    if(dec_pos_dividend != string::npos){
        dividend.erase(dec_pos_dividend, 1);

        size_t new_dec_pos = dec_pos_dividend + dec_places;
        if(new_dec_pos > dividend.length()){
            dividend.append(new_dec_pos - dividend.length(), '0');
        }
        else if(new_dec_pos < dividend.length()){
            dividend.insert(dividend.end() - new_dec_pos + 1, '.');
        }
    }
    else{
        dividend.append(dec_places, '0');
    }

    divisor.erase(dec_pos_divisor, 1);
    while(divisor[0] == '0'){
        divisor.erase(0, 1);
    }
}

void prepare_division_values(string& divisor, string& dividend){
    while(divisor[0] == '0'){
        divisor.erase(0, 1);
    }
    while(dividend[0] == '0'){
        dividend.erase(0, 1);
    }

    size_t dec_pos_dividend = dividend.find('.');
    size_t dec_pos_divisor = divisor.find('.');
    regex rgx("0*$");

    if(dec_pos_dividend != string::npos){
        if(regex_search(dividend, matcher, rgx)){
            dividend.erase(dividend.length() - matcher[0].length());
        }
    }
    if(dec_pos_divisor != string::npos){
        if(regex_search(divisor, matcher, rgx)){
            divisor.erase(divisor.length() - matcher[0].length());
        }
    }
}

string reverse_string(string str) {
    string rev_str = "";
    for(rit i = str.rbegin(); i != str.rend(); i++){
        rev_str.push_back(*i);
    }

    return rev_str;
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

    int y_len = y.length();
    for(int i = x.length() - 1; i >= 0; i--){
        if(i >= y_len){
            result.push_back(x[i]);
        }
        else{
            op1 = (short)(x[i] - 48);
            op2 = (short)(y[i] - 48);

            local_res = op1 + op2 + carry;
            result.push_back(handle_carry_result(local_res, carry));
        }
    }

    return reverse_string(result);
}

string subtraction_for_division(string dividend, string divisor){
    short op1, op2, local_res;
    string result;
    char curr_num;

    for(rit i_dividend = dividend.rbegin(), i_divisor = divisor.rbegin(); i_dividend != dividend.rend(); i_dividend++, i_divisor++){
        if(i_divisor >= divisor.rend()){
            result.push_back(*i_dividend);
        }
        else{
            op1 = (short)(*i_dividend - 48);
            op2 = (short)(*i_divisor - 48);

            if(op1 < op2){
                // borrow
                int units = 1; // number of digits traversed back to get a borrow
                do {
                    curr_num = *(i_dividend+units);
                    units++;
                }
                while(curr_num == '0');

                units--; // the position of the first non-zero character encountered
                *(i_dividend+units) -= 1; // subtract 1 from the borrowed number
                units--;

                for(; units > 0; units--){
                    // replace all the zeros we came across, with 9
                    *(i_dividend+units) = '9';
                }

                op1 = 10 + op1;
            }

            local_res = op1 - op2;

            if(local_res < 0){
                break;
            }

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

string subtraction(string x, string y){
    bool isSigned = false;
    bool double_negative = false;
    short op1, op2, local_res;
    string result;
    char curr_num;
    int decimal_places = 0;

    decimal_places = balance_fraction(x, y);

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

    // remove the decimals
    if(decimal_places){
        x.erase(x.end() - (decimal_places+1));
        y.erase(y.end() - (decimal_places+1));
    }

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

string division(string dividend, string divisor){
    bool isSigned = false;

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
        // remove the signs
        dividend.erase(dividend.begin());
        divisor.erase(divisor.begin());
    }

    if(is_value_zero(divisor)){
        cerr<<"Error: Division by zero!\n";
        exit(1);
    }
    if(is_value_zero(dividend)){
        return "0";
    }

    string quotient = "0";
    string result;
    string decimal_part = "";
    bool quotient_has_decimal = false;

    prepare_division_values(divisor, dividend);
    if(is_divisor_fraction(divisor)){
        handle_fraction_divisor(divisor, dividend);
    }
    size_t dividend_decimal_pos = dividend.find('.');

    if(dividend_decimal_pos != string::npos){
        decimal_part = reverse_string(dividend.substr(dividend_decimal_pos +1 ));
        dividend.erase(dividend_decimal_pos);
    }

    Equality equality = get_divisor_equality(dividend, divisor);

    while(equality == IS_LESS || equality == IS_EQUAL){
        quotient = addition(quotient, "1");
        dividend = subtraction_for_division(dividend, divisor);

        equality = get_divisor_equality(dividend, divisor);
    }

    result = quotient;

    if(!decimal_part.empty()){
        short decimal_places = 1;
        result.push_back('.');
        quotient_has_decimal = true;
        quotient = "";

        if(dividend == "0"){
            dividend = decimal_part[decimal_part.length()-1];
        }
        else{
            dividend.push_back(decimal_part[decimal_part.length()-1]);
        }

        equality = get_divisor_equality(dividend, divisor);
        decimal_part.pop_back();

        while(equality == IS_GREATER && !decimal_part.empty()){
            result.push_back('0');
            dividend.push_back(decimal_part[decimal_part.length()-1]);

            equality = get_divisor_equality(dividend, divisor);
            decimal_part.pop_back();
        }

        while(decimal_places <= MAX_DECIMAL_PLACES){
            quotient = "0";
            equality = get_divisor_equality(dividend, divisor);

            while(equality == IS_LESS || equality == IS_EQUAL){
                quotient = addition(quotient, "1");
                dividend = subtraction_for_division(dividend, divisor);

                equality = get_divisor_equality(dividend, divisor);
            }
            result += quotient;
            decimal_places++;

            if(!decimal_part.empty() && dividend == "0"){
                dividend = decimal_part[decimal_part.length()-1];
                equality = get_divisor_equality(dividend, divisor);
                decimal_part.pop_back();

                while(equality == IS_GREATER && !decimal_part.empty()){
                    result.push_back('0');
                    dividend.push_back(decimal_part[decimal_part.length()-1]);

                    equality = get_divisor_equality(dividend, divisor);
                    decimal_part.pop_back();
                }
            }
            else if(!decimal_part.empty() && dividend != "0"){
                dividend.push_back(decimal_part[decimal_part.length()-1]);
                equality = get_divisor_equality(dividend, divisor);
                decimal_part.pop_back();

                while(equality == IS_GREATER && !decimal_part.empty()){
                    result.push_back('0');
                    dividend.push_back(decimal_part[decimal_part.length()-1]);

                    equality = get_divisor_equality(dividend, divisor);
                    decimal_part.pop_back();
                }
            }
            else if(decimal_part.empty() && dividend != "0"){
                dividend.push_back('0');
            }
            else {
                break;
            }
        }
    }

    if(quotient_has_decimal){
        return result;
    }

    if(dividend != "0" && equality == IS_GREATER){
        short decimal_places = 1;
        result.push_back('.');
        dividend.push_back('0');
        quotient = "";

        equality = get_divisor_equality(dividend, divisor);
        while(equality == IS_GREATER){
            dividend.push_back('0');
            result.push_back('0');

            equality = get_divisor_equality(dividend, divisor);
        }

        while(decimal_places <= MAX_DECIMAL_PLACES){
            quotient = "0";
            equality = get_divisor_equality(dividend, divisor);

            while(equality == IS_LESS || equality == IS_EQUAL){
                quotient = addition(quotient, "1");
                dividend = subtraction_for_division(dividend, divisor);

                equality = get_divisor_equality(dividend, divisor);
            }

            result += quotient;

            if(dividend == "0"){
                break;
            }

            dividend.push_back('0');
            decimal_places++;
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

    // clean the result string
    if(regex_search(res, matcher, rgx_trailing_zeros)){
        size_t dec_pos = res.find('.');

        if(res.length() - dec_pos == matcher[0].length()){
            res.erase(dec_pos);
        }
    }
    res.shrink_to_fit();

    cout<<operand1<<" "<<operators.top()<<" "<<operand2<<" = "<<res<<endl;

    numbers.push(res);
    operators.pop();
}

string inputs[] = {"2+5+0.3-18-.001+23*0.5", "2+5+(0.3-18)-(.001+23)*0.5", "2/5-6.001+0.2*0-5+.5/.2", "2/5-6.001+[0.2*{(0-5)+0.5}]/0.2"};

int main(){
    string input = inputs[3];
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
