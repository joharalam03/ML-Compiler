//  Platform:   Linux

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

#define MAX_IDENTIFIER_LENGTH 12
#define MAX_IDENTIFIER 50
#define MAX_ARGS 50

typedef enum {
    VariableAssignment,
    PrintExpression,
    FunctionDefinition,
    FunctionBody,
    FunctionCall,
    NullStatement   //Default statement to return
} StatementType;

typedef enum {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    NullOperator //Default operator to return
} OperatorType;


typedef struct {
    char name[MAX_IDENTIFIER_LENGTH];
    float value;
    char call[50];  
} Variable;

Variable variables[MAX_IDENTIFIER];
int num_variables = 0;


typedef struct {
    char name[MAX_IDENTIFIER_LENGTH];
    char arguments[MAX_IDENTIFIER_LENGTH][MAX_ARGS];
    int num_args;
    int num_functionVariables;
    Variable functionVariables[MAX_IDENTIFIER];
    char returnExpression[50];
    char printExpression[50][50];
    int num_print;
    
} Function;

Function functions[MAX_IDENTIFIER];
int num_functions = 0;

char printExpressions[50][50];
int num_printExpressions = 0;

char functionCall[50][50];
int num_funcCall = 0;

//Store the optional command line arguments
char commandArgs[50][50];
int num_commandArgs = 0;

//Keep track of the current function
Function *curr_function = NULL;

//Track the line number
int line_num = 0;

//Check if ml syntax is correct
bool syntax_error = false;

void readFile(const char *ml_file);
void parseLine(char *line);
void checkSyntax(char *line);
StatementType getStatementType(char *line);
void handleVariables(char *line);
void trimWhitespace(char *token);
float fetchVariableValue(char *name);
float evaluateVariableExpression(char **expression);
OperatorType fetchOperator(char operator);
float performArithmeticOperation(float result, char operator, float curr_num);
void handlePrint(char *line);
void handleFunction(char *line);
void handleFunctionBody(char *line);
void handleReturn(char *line);
void handleFunctionCall(char *line);
void c_translate(const char *filename);
void execute(const char *c_filename);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "! Usage: %s (ml_program_file) [arguments.....]\n", argv[0]);
        return EXIT_FAILURE;
    }


    for (int i = 2; i < argc; i++) {
        strncpy(commandArgs[num_commandArgs], argv[i], 50);
        commandArgs[num_commandArgs][50] = '\0';
        num_commandArgs++;
    }


    readFile(argv[1]);

    if (syntax_error) {
        return EXIT_FAILURE;
    }

    //Create a C file and translate the ml code to C
    char c_filename[512];
    snprintf(c_filename, sizeof(c_filename), "ml-%d.c", getpid());

    //transalte ml to c
    c_translate(c_filename);

    //execute the file
    execute(c_filename);

    //delete generated file
    remove(c_filename);

    return 0;
}


void readFile(const char *ml_file) {
    FILE *file = fopen(ml_file, "r");

    if (!file) {   //Check if the file opened correctly
        fprintf(stderr, "! Error opening %s\n", ml_file);
        return;
    }

    char line[256];
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        // Remove trailing newline character if present
        line[strcspn(line, "\n")] = '\0';
        
        //Remove comments if present
        char *comment = strchr(line, '#');
        if (comment) {
            *comment = '\0';
        }

        char *ptr = line;
        if (!isspace(*ptr)) curr_function = NULL;   //Set current funciton to get out of the function body

        checkSyntax(line);
        parseLine(line);
    }

    fclose(file);
}

void checkSyntax(char *line) {
    char *ptr = line;
    while (isspace(*ptr)) {
        ptr++;
    }

    if (*ptr == '\0') {
        return;
    }

    //Comments have already been removed
    if (strstr(line, "=@!$^&_\''"">?~`")) {
        syntax_error = true;
        fprintf(stderr, "! Syntax error on line %d => Unsuppported characters included in the line.\n", line_num);
        EXIT_FAILURE;
    }

    //Check if a variable identifier contains a digit
    if (strstr(line, "<-")) {
        int num_char = 0;
        char *ptr = line;
        bool contains_digit = false;
        while (*ptr != '<' || isspace(*ptr)) {
            if (isdigit(*ptr)) {
                contains_digit = true;
            }
            num_char++;
            ptr++;
        }
        if (contains_digit) {
            syntax_error = true;
            fprintf(stderr, "! Syntax error on line %d => Variable identifier contains a digit.\n", line_num);
            EXIT_FAILURE;
        }
        
        if (num_char > 12) {
            syntax_error = true;
            fprintf(stderr, "! Syntax error on line %d => Variable identifier length exceeded (> 12).\n", line_num);
            EXIT_FAILURE;
        }
    }

    //Check if function identifier contains a digit
    if (strncmp(line, "function ", 9) == 0) {
        char *ptr = line + 9;
        int num_char = 0;
        bool contains_digit = false;
        while (*ptr != ' ') {
            if (isdigit(*ptr)) {
                syntax_error = true;
                contains_digit = true;
            }
            ptr++;
            num_char++;
        }
        if (contains_digit) {
            syntax_error = true;
            fprintf(stderr, "! Syntax error on line %d => Function identifier contains a digit.\n", line_num);
            EXIT_FAILURE;
        }

        if (num_char > 12) {
            syntax_error = true;
            fprintf(stderr, "! Syntax error on line %d => Function identifier lenght exceeded (> 12)\n", line_num);
            EXIT_FAILURE;
        }
    } 
    //Check function body's syntax
    if (curr_function != NULL) {
        char *ptr = line;

        while (isspace(*ptr)) {
            ptr++;
        }

        if (strstr(line, "<-")) {
            int num_char = 0;
            char *ptr = line;
            bool contains_digit = false;
            while (*ptr != '<' || isspace(*ptr)) {
                if (isdigit(*ptr)) {
                    contains_digit = true;
                }
                num_char++;
                ptr++;
            }
            if (contains_digit) {
                syntax_error = true;
                fprintf(stderr, "! Syntax error on line %d => Function's variable identifier contains a digit.\n", line_num);
                EXIT_FAILURE;
            }
            
            if (num_char > 12) {
                syntax_error = true;
                fprintf(stderr, "! Syntax error on line %d => Function's variable identifier length exceeded (> 12).\n", line_num);
                EXIT_FAILURE;
            }
        }
        
    }

    if (curr_function == NULL) {
        if (isspace(line[0])) {
            syntax_error = true;
            fprintf(stderr, "! Syntax error on line %d => Statements not part of a function shouldn't be indented or have any leading whitespace.\n", line_num);
            EXIT_FAILURE;
        }
    }
}

void parseLine(char *line) {
    StatementType statement = getStatementType(line);

    switch (statement) {
        case VariableAssignment:
            curr_function = NULL;
            handleVariables(line);
            break;
        
        case PrintExpression:
            curr_function = NULL;
            handlePrint(line);
            break;

        case FunctionDefinition:
            handleFunction(line);
            break;
        
        case FunctionCall:
            curr_function = NULL;
            handleFunctionCall(line);
            break;
        
        case FunctionBody:
            handleFunctionBody(line);
            break;

        case NullStatement:
            break;
    }
}

StatementType getStatementType(char *line) {
    if (isspace(line[0])) {
        return FunctionBody;
        
    } else if (strncmp(line, "function ", 9) == 0) {
        return FunctionDefinition;

    } else if (strstr(line, "<-")) {
        return VariableAssignment;

    } else if (strncmp(line, "print ", 6) == 0) {
        return PrintExpression;

    } else if (strstr(line, "(")) {
        return FunctionCall;

    }
    
    return NullStatement;
}

void handleVariables(char *line) {
    char variable_name[MAX_IDENTIFIER_LENGTH];
    float variable_value;

    char *ptr = line;
    while (isspace((unsigned char)*ptr)) {
        ptr++;
    }
    int idx= 0;
    while (*ptr != '<') {    //Copy the characters until assignment operator
        if (isspace(*ptr)) {    //Skip leading whitespace
            ptr++;
        }
        else {
        variable_name[idx++] = *ptr++;
        }
    }
    variable_name[idx] = '\0';
    
    ptr += 2;   //Skip '<-'
    while (isspace((unsigned char)*ptr)) {
        ptr++;
    }


    variable_value = evaluateVariableExpression(&ptr);   //Evaluate the right side of the variable assignment and assign to variable_value
    
    if (curr_function != NULL) {
        // Check whether the variable has been declared before
        bool is_declared = false;
        for (int i = 0; i < curr_function->num_functionVariables; i++) {
            if (strcmp(curr_function->functionVariables[i].name, variable_name) == 0) {
                curr_function->functionVariables[i].value = variable_value;    //Assign new value if declared before
                is_declared = true;
                break;
            }
        }

        if (!is_declared) {
            strncpy(curr_function->functionVariables[curr_function->num_functionVariables].name, variable_name, MAX_IDENTIFIER_LENGTH - 1);
            curr_function->functionVariables[curr_function->num_functionVariables].value = variable_value;
            curr_function->num_functionVariables++;
        }
    }

    else {
        
        // Check whether the variable has been declared before
        bool is_declared = false;
        for (int i = 0; i < num_variables; i++) {
            if (strcmp(variables[i].name, variable_name) == 0) {
                variables[i].value = variable_value;    //Assign new value if declared before
                is_declared = true;
                break;
            }
        }

        if (!is_declared) {     //If not declared before then add to global array
            strncpy(variables[num_variables].name, variable_name, sizeof(variables[num_variables].name) - 1);
            variables[num_variables].name[sizeof(variables[num_variables].name) - 1] = '\0';
            variables[num_variables].value = variable_value;
            num_variables++;
        }
    }
}



float fetchVariableValue(char *name) {

    //Check if the variable is local to the current function
    if (curr_function != NULL) {
        for (int i = 0; i < curr_function->num_functionVariables; i++) {
            if (strcmp(curr_function->functionVariables[i].name, name) == 0) {
                return curr_function->functionVariables[i].value;
            }
        }
    }

    //If the name is arg[0, 1, 2, ...]
    if (strncmp(name, "arg", 3) == 0 && isdigit(name[3])) {
        char comm_ref[10];    //Hold the reference of the arg i.e 0, 1, 2, ....
        int idx = 0;
        char *ptr = name + 3; //Skip 'arg'

       while (*ptr) {
        comm_ref[idx++] = *ptr++;
       }
       comm_ref[idx] = '\0';
       int reference = atoi(comm_ref);

       return atof(commandArgs[reference]);

    }

    //Check if in the golbal variables array
    for (int i = 0; i < num_variables; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value;
        }
    }
    return 0.0;
}


float evaluateVariableExpression(char **expression) {
    float result = 0.0;
    float curr_num = 0.0;
    StatementType operator = ADDITION;
    char *ptr = *expression;

    while (*ptr) {

        while (isspace((unsigned char)*ptr)) {
            ptr++;
        }

        if (*ptr == '(') {
            ptr++; 
            curr_num = evaluateVariableExpression(&ptr); //Recursively call the function to evaluate inside the parenthesis
        }

        if (isdigit(*ptr) || (*ptr == '-' && isdigit(*ptr + 1))) {
            char val[256];
            int val_idx = 0;

            while (isdigit(*ptr) || *ptr == '.') {
                val[val_idx++] = *ptr++;
            }
            val[val_idx] = '\0';
            curr_num = atof(val);
        }
        else if (isalpha(*ptr)) {
            char var[MAX_IDENTIFIER_LENGTH];
            int var_idx = 0;
            while (isalpha(*ptr)) {
                var[var_idx++] = *ptr++;
            }
            var[var_idx] = '\0';

            //Check if the variable is arg[0, 1, 2, ...]
            if (strcmp(var, "arg") == 0 && isdigit(*ptr)) {
                while (isdigit(*ptr)) {
                    var[var_idx++] = *ptr++;
                }
                var[var_idx] = '\0';
            }
            curr_num = fetchVariableValue(var);
        }
        else if (*ptr == '+' || *ptr == '-' || *ptr == '*' || *ptr == '/') {
            result = performArithmeticOperation(result, operator, curr_num);
            operator = fetchOperator(*ptr);
            ptr++;
        }

        else if (*ptr == ')') {
            ptr++;
            break;
        }
    }

    result = performArithmeticOperation(result, operator, curr_num);
    *expression = ptr;
    return result;
}

OperatorType fetchOperator(char operator) {
    switch (operator) {
        case '+':
            return ADDITION;
        
        case '-':
            return SUBTRACTION;
        
        case '*':
            return MULTIPLICATION;

        case '/':
            return DIVISION;
    }

    return NullOperator;
}

float performArithmeticOperation(float result, char operator, float curr_num) {
    switch (operator) {
        case ADDITION:
            return result + curr_num;
        case SUBTRACTION:
            return result - curr_num;
        case MULTIPLICATION:
            return result * curr_num;
        case DIVISION:
            return result / curr_num;
        default:
            return result;
    }
}


void handlePrint(char *line) {
    if (curr_function != NULL) {

        char *expr = line;
        while (isspace((unsigned char)*expr)) {  //Skip '\t'
            expr++;
        }
        expr += 5;      //Skip the print keyword

        while (isspace((unsigned char)*expr)) {  //Skip leading whitespaces
            expr++;
        }

        strncpy(curr_function->printExpression[curr_function->num_print], expr, 50);
        curr_function->printExpression[curr_function->num_print][50] = '\0';
        curr_function->num_print++;
    } 
    
    else {
        char *expr = line + 5;  //Skip the print keyword
        while (isspace((unsigned char)*expr)) {  //Skip leading whitespaces
            expr++;
        }

        strncpy(printExpressions[num_printExpressions], expr, 50);
        printExpressions[num_printExpressions][50] = '\0';
        num_printExpressions++;

    }

}

void handleFunction(char *line) {
    Function new_func;

    char *func_token = strtok(line, " ");
    func_token = strtok(NULL, " ");

    strncpy(new_func.name, func_token, MAX_IDENTIFIER_LENGTH);    //Store the function name

    new_func.num_args = 0;

    func_token = strtok(NULL, " ");

    while (func_token != NULL) {

        strncpy(new_func.arguments[new_func.num_args], func_token, MAX_IDENTIFIER_LENGTH);   //Store the function arguments
        new_func.num_args++;
        func_token = strtok(NULL, " ");
    }
    functions[num_functions] = new_func;
    curr_function = &functions[num_functions];
    curr_function->num_functionVariables = 0;
    curr_function->num_print = 0; 

    //This has been set to null termiantor because i cant fricking figure out why it already has weird characters
    curr_function->returnExpression[0] = '\0';

    num_functions++;

}

void handleFunctionBody(char *line) {
    char *ptr = line;

    while (isspace((unsigned char)*ptr)) {
        ptr++;
    }

    if (strstr(ptr, "<-")) {
        handleVariables(line);
        return;
    }
    else if (strncmp(ptr, "return ", 7) == 0) {
        handleReturn(line);
        return;
    }
    else if (strncmp(ptr, "print ", 6) == 0) {
        handlePrint(line);
        return;
    }
    else if(strstr(ptr, "(")) {
        handleFunctionCall(line);
        return;
    }
}
 
     
void handleReturn(char *line) {
    char *expr = line;

    while (isspace((unsigned char)*expr)) {  //Skip '\t'
        expr++;
    } 
    
    expr += 6;   //Skip 'return' keyword

    while (isspace((unsigned char)*expr)) {  //Skip leading whitespaces
        expr++;
    }

    strncpy(curr_function->returnExpression, expr, 50);
    curr_function->returnExpression[50] = '\0';
}


void handleFunctionCall(char *line) {
    if (strstr(line, "<-")) {       //If a variable calls a function
        handleVariables(line);
    }
    if (curr_function != NULL) {
        return;
    }
    else {
        strncpy(functionCall[num_funcCall], line, 50);
        num_funcCall++;
    }
}

void c_translate(const char *filename) {
    FILE *file = fopen(filename, "w");

    if (!file) {
        fprintf(stderr, "Encountered an error creating %s\n", filename);
        fclose(file);
        return;
    }

    fprintf(file, "#include <stdio.h>\n\n");

    //Function prototypes
    for (int i = 0; i < num_functions; i ++) {
        fprintf(file, "float %s(",  functions[i].name);
        for (int j = 0; j < functions[i].num_args; j++) {
            if (j > 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "float %s", functions[i].arguments[j]);
        }
        fprintf(file, ");\n\n");
    }

    //Global variables
    for (int i = 0; i < num_variables; i++) {
        if (variables[i].value == (int)variables[i].value) {
            fprintf(file, "int %s = %d;\n", variables[i].name, (int)variables[i].value);  //If whole number, print as integer

        } else {
            fprintf(file, "float %s = %.6f;\n", variables[i].name, variables[i].value);  //Else print upto 6 decimal places
        }
    }

    //Functions
    for (int i = 0; i < num_functions; i++) {
        fprintf(file, "float %s(", functions[i].name);

        //Function arguments
        for (int j = 0; j < functions[i].num_args; j++) {
            if (j > 0) {
                fprintf(file, ", ");
            }
            fprintf(file, "float %s", functions[i].arguments[j]);
        }
        fprintf(file, ") {\n");

        //Function local variables
        for (int k = 0; k < functions[i].num_functionVariables; k++) {
            if (functions[i].functionVariables[k].value == (int)functions[i].functionVariables[k].value) {
                fprintf(file, "    int %s = %d;\n", functions[i].functionVariables[k].name, (int)functions[i].functionVariables[k].value);  //If whole number, print as integer

            } else {
                fprintf(file, "    float %s = %f;\n", functions[i].functionVariables[k].name, functions[i].functionVariables[k].value);  //Else print upto 6 decimal places
            }
        }

        //Print expression
        for (int j = 0; j < functions[i].num_print; j++) {
            fprintf(file, "    if ((int)(%s) == %s) {\n", functions[i].printExpression[j], functions[i].printExpression[j]);      //If whole number, print as integer
            fprintf(file, "        printf(\"%%d\\n\", (int)(%s));\n", functions[i].printExpression[j]);
            fprintf(file, "    } else {\n");
            fprintf(file, "        printf(\"%%.6f\\n\", %s);\n", functions[i].printExpression[j]);       //Else print upto 6 decimal places
            fprintf(file, "    }\n");
        }

        //Return expression
        if (functions[i].returnExpression[0] != '\0') {
            fprintf(file, "    if ((int)(%s) == %s) {\n", functions[i].returnExpression, functions[i].returnExpression);      //If whole number, print as integer
            fprintf(file, "        return (int)(%s);\n", functions[i].returnExpression);
            fprintf(file, "    } else {\n");
            fprintf(file, "        return %s;\n", functions[i].returnExpression);     //Else print upto 6 decimal places
            fprintf(file, "    }\n");
        }

        fprintf(file, "}\n\n");
    }


    //Main function
    fprintf(file, "int main() {\n");

    //Print expression
    for (int i = 0; i < num_printExpressions; i++) {
        fprintf(file, "    if ((int)(%s) == %s) {\n", printExpressions[i], printExpressions[i]);   //If whole number, print as integer
        fprintf(file, "        printf(\"%%d\\n\", (int)(%s));\n", printExpressions[i]);
        fprintf(file, "    } else {\n");
        fprintf(file, "        printf(\"%%.6f\\n\", %s);\n", printExpressions[i]);    //Else print upto 6 deicmal places
        fprintf(file, "    }\n");
    }

    //Function call
    for (int i = 0; i < num_funcCall; i++) {
        fprintf(file, "    %s;\n", functionCall[i]);
    }

    fprintf(file, "    return 0;\n");
	fprintf(file, "}");
    fclose(file);
}

void execute(const char *c_filename) {
    char command[256];
    snprintf(command, sizeof(command), "gcc -o output %s", c_filename);

    int compileVal = system(command);

    //Check successful compilation of the code
    if (compileVal != 0) {
        fprintf(stderr, "Compilation failed with status %d\n", compileVal);
        return;
    }

    int executeVal;

    executeVal = system("./output");

    //Check successful execution
    if (executeVal != 0) {
        fprintf(stderr, "Execution failed with status %d\n", executeVal);
        return;
    }

    remove("./output");
}
