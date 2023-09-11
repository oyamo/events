#include "../include/json.h"

void json_check_syntax(char *j_string)
{
    string_t *json_string = string_create_from_string(j_string);

    bool is_array = false;

    char USED_QUOTATION = ' ';

    if (endswith(SQUARE_CLOSE_BRACKETS, json_string->chars) &&
        startswith(SQUARE_OPEN_BRACKETS, json_string->chars))
    {
        is_array = true;
    }

    if (is_array)
    {
        for (size_t i = 0; i < strlen(json_string->chars); i++)
        {
            if (json_string->chars[i] == SINGLE_QUOTE || json_string->chars[i] == DOUBLE_QUOTE)
            {
                if(json_string->chars[i] != '\''){ 
                    USED_QUOTATION = '\''; 
                }else{ USED_QUOTATION = '"'; }
                break;
            }
        }

        if (!(endswith(SQUARE_CLOSE_BRACKETS, json_string->chars)) ||
            !(startswith(SQUARE_OPEN_BRACKETS, json_string->chars)))
        {
            puts("Unsupported File Format");
            return;
        }

        if (USED_QUOTATION == ' ')
        {
            puts("Unsupported File Format");
            return;
        }
    }

    else

    {
        for (size_t i = 0; i < strlen(json_string->chars); i++)
        {
            if (json_string->chars[i] == SINGLE_QUOTE || json_string->chars[i] == DOUBLE_QUOTE)
            {
                if(json_string->chars[i] != '\''){ 
                    USED_QUOTATION = '\''; 
                }else{ USED_QUOTATION = '"'; }
                break;
            }
        }

        if (!(endswith(CURLY_CLOSE_BRACKETS, json_string->chars)) ||
            !(startswith(CURLY_OPEN_BRACKETS, json_string->chars)))
        {
            puts("Unsupported File Format");
            return;
        }

        if (USED_QUOTATION == ' ')
        {
            puts("Unsupported File Format");
            return;
        }
    }

    string_t *curls = string_create();
    string_t *squares = string_create();
    string_t *symbols = string_create();

    int quotes = 0, line = 1, col = 1;

    // string_append(symbols,' ');

    for (size_t i = 0; i < strlen(json_string->chars); i++)
    {

        if (json_string->chars[i] == LINE_BREAK)
        {
            line++;
            col = 1;
        }

        if (json_string->chars[i] == USED_QUOTATION && quotes == 0)
        {
            quotes = 1;
            string_append(symbols, json_string->chars[i]);
        }

        if (quotes == 0)
        {

            if (json_string->chars[i] == SQUARE_OPEN_BRACKETS)
            {
                if (string_back(symbols) == CURLY_OPEN_BRACKETS ||
                    string_back(symbols) == CURLY_CLOSE_BRACKETS ||
                    string_back(symbols) == SQUARE_OPEN_BRACKETS ||
                    string_back(symbols) == USED_QUOTATION ||
                    (string_back(symbols) >= '0' && string_back(symbols) <= '9'))
                {
                    printf("syntax err [%d , %d]\n", line, col);
                    exit(1);
                }
                string_append(squares, json_string->chars[1]);
            }

            if (json_string->chars[i] == SQUARE_CLOSE_BRACKETS)
            {

                if (string_back(squares) == SQUARE_OPEN_BRACKETS)
                {
                    string_pop(squares);
                }
                else
                {
                    string_append(squares, json_string->chars[1]);
                }
            }

            if (json_string->chars[i] == CURLY_OPEN_BRACKETS)
            {
                if (string_back(symbols) == CURLY_OPEN_BRACKETS ||
                    string_back(symbols) == CURLY_CLOSE_BRACKETS ||
                    string_back(symbols) == SQUARE_OPEN_BRACKETS ||
                    string_back(symbols) == USED_QUOTATION ||
                    (string_back(symbols) >= '0' && string_back(symbols) <= '9'))
                {
                    printf("syntax err [%d , %d]\n", line, col);
                    exit(1);
                }
                string_append(curls, json_string->chars[i]);
            }

            if (json_string->chars[i] == COMMA && string_back(symbols) == CURLY_OPEN_BRACKETS)
            {
                printf("syntax err [%d , %d]\n", line, col);
                exit(1);
            }

            if (json_string->chars[i] == COLON && (string_back(symbols) == CURLY_CLOSE_BRACKETS ||
                                                   string_back(symbols) == SQUARE_CLOSE_BRACKETS ||
                                                   string_back(symbols) == COLON ||
                                                   string_back(symbols) != USED_QUOTATION))
            {
                printf("syntax err [%d , %d]\n", line, col);
                exit(1);
            }

            if (json_string->chars[i] == CURLY_CLOSE_BRACKETS)
            {
                if (string_back(symbols) == COMMA)
                {
                    printf("syntax err [%d , %d]\n", line, col);
                    exit(1);
                }
                if (string_back(curls) == CURLY_OPEN_BRACKETS)
                {
                    string_pop(curls);
                }
                else
                {
                    string_append(curls, json_string->chars[i]);
                }
            }

            if (json_string->chars[i] == COMMA ||
                json_string->chars[i] == COLON ||
                json_string->chars[i] == SQUARE_CLOSE_BRACKETS ||
                json_string->chars[i] == SQUARE_OPEN_BRACKETS ||
                json_string->chars[i] == CURLY_CLOSE_BRACKETS ||
                json_string->chars[i] == CURLY_OPEN_BRACKETS ||
                (json_string->chars[i] >= '0' && json_string->chars[i] <= '9'))
            {
                string_append(symbols, json_string->chars[i]);
            }
        }

        if (json_string->chars[i] == USED_QUOTATION && quotes == 1)
        {
            quotes = 0;
            string_append(symbols, json_string->chars[i]);
        }

        col++;
    }

    /*if(strlen(curls->chars) > 0 || strlen(squares->chars) > 0){
        puts("syntax error");

        return;
    }*/

    // string_destroy(symbols);
    // string_destroy(curls);
    // string_destroy(squares);

    /*string temp = arg;
    for(int i = 0; i<int(arg.length());i++){
        if(util.compare(temp[i],USED_QUOTATION)){
            temp = util.setCharAt(temp,i,DOUBLE_QUOTE);
        }

        if(util.compare(json_string->chars[i],DOUBLE_QUOTE)){

            if(util.compare(USED_QUOTATION,DOUBLE_QUOTE)) temp = util.setCharAt(temp,i,SINGLE_QUOTE);
            if(util.compare(USED_QUOTATION,SINGLE_QUOTE)) temp = util.setCharAt(temp,i,DOUBLE_QUOTE);
        }
    }*/
}

