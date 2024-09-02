// CITS2002 Project 1 2024
// Student1:   24331036   Johar Khan
// Platform:   Linux

#include <stdio.h>

#define lineLength 256

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s (ml_program_file) [arguments.....]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *ml_file = argv[1]
    char c_run_file [64];
    char c_exe_file [64];

    snprintf(c_run_file, sizeof(c_run_file), "%iml.c", getpid());
    snprintf(c_exe_file, sizeof(c_exe_file), "%iml", getpid());

    void read_ml(ml_file);

}

void read_ml(*ml_file) {
    FILE *file = fopen(ml_file, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening %s\n", ml_file);
        exit(EXIT_FAILURE);
    }

    char line[lineLength];

    while(fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);

        //Check the end of the line
        for (int i = 0; i < len; i++) {
            if (line[i] == "\n") {
                line[i] = "\0";
                break;
            }
        }

        //Check for a comment
        for (int i = 0; i < len; i++) {
            if (line[i] == '#') {
                line[i] == '\0';
                break;
            }
        }
    }

}
