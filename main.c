#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

// Use to be 2-space indented, so some whitespace issues may remain.

typedef struct Array {
    void* data;
    size_t item_size;
    size_t size;
    size_t capacity;
} Array;

typedef size_t LeftBrace;

void destroy_array(Array* arr) {
    free(arr->data);
    arr->data = NULL;
    arr->item_size = 0;
    arr->size = 0;
    arr->capacity = 0;
}

bool init_array(Array* arr, const size_t item_size) {
    arr->data = malloc(item_size);
    if (!arr->data) {
        destroy_array(arr);
        return false;
    }
    arr->item_size = item_size;
    arr->size = 0;
    arr->capacity = 1;
    return true;
}

bool realloc_array(Array* arr) {
    void* temp = realloc(arr->data, 2 * arr->capacity * arr->item_size);
    if (!temp) {
        return false;
    }
    arr->data = temp;
    arr->capacity *= 2;
    return true;
}

bool push_item(Array* arr, const void* item) {
    if (arr->size == arr->capacity) {
        if (!realloc_array(arr)) {
            return false;
        }
    }

    for (size_t i = 0; i < arr->item_size; ++i) {
        *(((char*)arr->data) + arr->size * arr->item_size + i) = *((char*)item + i);
    }

    ++arr->size;
    return true;
}

void pop_item(Array* arr, void* item) {
    for (size_t i = 0; i < arr->item_size; ++i) {
        *((char*)item + i) = *(((char*)arr->data) + (arr->size - 1) * arr->item_size + i);
    }
    --arr->size;
}

bool test_char(const char ch) {
    switch (ch) {
        case '>':
        case '<':
        case '+':
        case '-':
        case '.':
        case ',':
        case '[':
        case ']':
            return true;
        default:
            return false;
    }
}

int main(void) {
    const size_t MAX_INDEX = 30000;

    Array prog;

    if (!init_array(&prog, sizeof(char))) {
        return -1;
    }

    printf("Enter program with '`' at the end:\n");

    char temp_char;
    while (scanf(" %c", &temp_char) && temp_char != '`') {
        if (test_char(temp_char)) {
            if (!push_item(&prog, &temp_char)) {
                destroy_array(&prog);
                return -1;
            }
        }
    }

    char data[MAX_INDEX] = {0};
    size_t data_index = 0;

    Array braces;
    if (!init_array(&braces, sizeof(LeftBrace))) {
        return -1;
    }

    LeftBrace temp;
    size_t prog_pos = 0;

    printf("Program running.\n\n");

    while (prog_pos < prog.size) {
        switch (*((char*)prog.data + prog_pos * prog.item_size)) {
            case '>':
                if (data_index == MAX_INDEX) {
                    data_index = 0;
                } else {
                    ++data_index;
                }
                break;
            case '<':
                if (data_index == 0) {
                    data_index = MAX_INDEX - 1;
                } else {
                    --data_index;
                }
                break;
            case '+':
                ++data[data_index];
                break;
            case '-':
                --data[data_index];
                break;
            case '.':
                putchar(data[data_index]);
                break;
            case ',':
                while (isspace(temp_char = getchar()))
                    ;
                data[data_index] = temp_char;
                break;
            case '[':
                if (data[data_index] == 0) {
                    size_t brace_layer = 0;
                    do {
                        switch (*((char*)prog.data + prog_pos * prog.item_size)) {
                            case '[':
                                ++brace_layer;
                                break;
                            case ']':
                                --brace_layer;
                                break;
                        }
                        ++prog_pos;
                    } while (brace_layer != 0);
                    --prog_pos;
                } else {
                    temp = prog_pos;
                    if (!push_item(&braces, &temp)) {
                        destroy_array(&prog);
                        destroy_array(&braces);
                        return -1;
                    }
                }
                break;
            case ']':
                pop_item(&braces, &temp);
                prog_pos = temp - 1;
                break;
        }
    ++prog_pos;
    }

    printf("\n\nPROGRAM RAN SUCCESSFULLY\n");

    destroy_array(&prog);
    destroy_array(&braces);

    return 0;
}
