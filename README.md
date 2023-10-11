**Note:** Not technically an _infinite_ calculator, but it can calculate and display fully resolved result of large, really large numbers.  

**Infinite Calculator** is an arithmatic calculator to calculate values that primitive containers cannot hold and it can display fully resolved numbers instead of converting them in exponents (eg: `1.12345e8`).
It can calculate numbers beyond `big int` (8 bytes) capacity, the actual limit depends on the available memory.  

### What it calculates
It supports basic arithmatic functions of  
- Addition
- Subtraction
- Multiplication
- Division

Also supports the following, with their actual precendance  
- Paranthesis `()`
- Braces `{}`
- Square Brackets `[]`

### What are the limitations
Even though the result depends on the available memory, the actual string input expression can only have the length upto `2^32`. Also the calculator follows the precedence rules of **BODMAS**
(Brackets Of Division Multiplication Addition Subtraction).  
When designing the input, care must be taken to follow the bracket precedance rules, no whole number should start with a `0` and no whitespaces must be introduced in the input string.  
Failing to follow the above rules will produce invalid results. Where these cases could've been handled, I was interested in building the calclutor but not so much in making it fault tolerant, you are welcome to contribute to make it as such.  

It takes command line argument as an input. Though you also could just use an IDE and edit the input variable in the code.
