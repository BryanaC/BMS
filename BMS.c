#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

int openFile(char *filename);

int notValidCharColumnOne(char line[], FILE *file);

int labelTooLong(char line[], FILE *file);

int nonBlankCharCol8or9(char line[], FILE *file);

int illegalOPCode(char *line, FILE *file);

int operandWrongCol(char *line, FILE *file);

int lastLineEND(char *line, int end, FILE *file);

char filename[50]; //max characters of filename
char lineNumber[150]; //max lines of file

/******************************
* Author: Bryana Craig
* Assignment: C Homework 2
* Due Date: 10/10/2016
*******************************/
int main(int argc, char *argv[]) {

    char filename[100]; // longest filename

    memset(filename, 0, sizeof(filename));
    printf("Please enter the full filename of the file you wish to scan: ");
    scanf("%s", filename);

    openFile(filename);

    return 0;
}

/**********************************************************************
 * Author: Bryana Craig
 * Purpose: Opens file & creates output file for corrections
 * Inputs: FILE*, char[]*
 * Outputs: void
 * Error Handling: Opening the file, Creating the Processed File
 *********************************************************************/
int openFile(char *filename) {
    FILE *filepointer;
    char line[100];

    if ((filepointer = fopen(filename, "r")) == NULL) {
        perror("ERROR: File Not Found");
        return 1;
    }

    printf("Opening file: %s. . .\n\n", filename);

    // Creates file BMSOut.txt with processed lines and errors checked
    char correctionFile[10000] = "BMSOut.txt";
    printf("Processed File: %s\n", correctionFile);
    FILE *file;
    if ((file = fopen(correctionFile, "w")) == NULL) {
        perror("ERROR: File Creation Failed");
        return 1;
    }

    int errorNum = 0;
    int end = 0;
    while (fgets(lineNumber, 150, filepointer) != NULL) {
        if (end == 1) {
            int y;
            for (y = 0; y < 71; y++) {
                if (isspace(line[y]) == 0) {
                    fprintf(file, "%s!!!: ERROR- Invalid character after END called", lineNumber);
                    errorNum++;
                }
            }
        }

        int error = notValidCharColumnOne(lineNumber, file);

        if ((error == 0) && (end == 0)) {
            error = labelTooLong(line, file);
            errorNum = errorNum + error;
        }
        if ((error == 0) && (end == 0)) {
            error = nonBlankCharCol8or9(lineNumber, file);
            errorNum = errorNum + error;
        }
        if ((error == 0) && (end == 0)) {
            error = illegalOPCode(lineNumber, file);
            errorNum = errorNum + error;
        }
        if ((error == 0) && (end == 0)) {
            error = operandWrongCol(lineNumber, file);
            errorNum = errorNum + error;
        }
        if ((error == 0) && (end == 0)) {
            error = lastLineEND(lineNumber, end, file);
            errorNum = errorNum + error;
        }
        if ((error == 0) && (end == 0)) {
            fprintf(file, "%s", lineNumber);
        }
    }
    printf("\nEnd of Processing. %d errors encountered.\n", errorNum);
    return 0;
}

/*******************************************************************
 * Author: Bryana Craig
 * Purpose: Checks column one for invalid characters
 * Inputs: char*, FILE*
 * Outputs: int
 * Error Handling: Invalid first column
 ******************************************************************/
int notValidCharColumnOne(char *line, FILE *file) {
    if ((isalpha(line[0]) == 0) | (line[0] != ' ') | (line[0] != '*')) {
        return 0;
    } else {
        fprintf(file, "%s!!!: ERROR- The first column contains invalid characters\n", line);
        return 1;
    }
}

/********************************************************************************
 * Author: Bryana Craig
 * Purpose: Determines if line contains a label, checks if it is uppercase and
 * the appropriate length
 * Inputs: char*, FILE*
 * Outputs: int
 * Error Handling: Label uppercase format, label with spaces, label with too many
 * characters, unexpected characters in label space
 *********************************************************************************/
int labelTooLong(char *line, FILE *file) {
    if (isalpha(line[0]) != 0) {
        int x;
        // Checking label for all uppercase format
        for (x = 0; x < 7; x++) {
            if (lineNumber[x] != ' ') {
                if (isupper(lineNumber[x]) == 0) {
                    fprintf(file, "%s!!!: ERROR- The label is not properly formatted. Only uppercase"
                            " letters allowed\n", lineNumber);
                    return 1;
                }
            } else { // Checking for spaces in the label
                for (x + 1; x < 7; x++) {
                    if (line[x] != ' ') {
                        fprintf(file, "%s!!!: ERROR- Labels may not contain spaces\n", lineNumber);
                        return 1;
                    }
                }
            }
        }

        // Checking for label length, must be between 1 and 7 characters
        if (lineNumber[7] != ' ') {
            fprintf(file, "%s!!!: ERROR- Improper label length. Must be between 1 and 7\n", lineNumber);
            return 1;
        }
    }
    // Checking for unexpected characters in label space
    if (lineNumber[0] == ' ') {
        int x;
        for (x = 0; x < 7; x++) {
            if (lineNumber[x] != ' ') {
                fprintf(file, "%s!!!: ERROR- Unexpected characters in label space\n", lineNumber);
                return 1;
            }
        }
    }
    return 0;
}

/*************************************************************************
 * Author: Bryana Craig
 * Purpose: Check columns 8 and 9 for unexpected characters. Columns are
 * meant to be spaces.
 * Inputs: char*, FILE*
 * Outputs: int
 * Error Handling: unexpected characters in column 8 or 9
 **************************************************************************/
int nonBlankCharCol8or9(char *lineNumber, FILE *file) {
    if (lineNumber[0] != '*' && ((lineNumber[7] != ' ') | (lineNumber[8] != ' '))) {
        fprintf(file, "%s!!!: ERROR- Unexpected characters in column 8 or 9. Columns 8 and 9 are spaces\n", lineNumber);
        return 1;
    }
    return 0;
}

/***********************************************
 * Author: Bryana Craig
 * Purpose: Checks for valid op-code, DFHMDI,
 * DFHMDF, DFHMSD
 * Inputs: char*, FILE*
 * Outputs: int
 * Error Handling: illegal op-code, incorrect
 * op-code format
 **********************************************/
int illegalOPCode(char *lineNumber, FILE *file) {
    if (lineNumber[0] != '*') {
        if (lineNumber[9] == ' ') {
            return 0;
        }
        if (lineNumber[9] != 'E') {
            // op-code position 1
            if (lineNumber[9] != 'D') {
                fprintf(file, "%s!!!: ERROR- Invalid Op-code character at position 1\n", lineNumber);
                return 1;
            }
            // op-code position 2
            if (lineNumber[10] != 'F') {
                fprintf(file, "%s!!!: ERROR- Invalid Op-code character at position 2\n", lineNumber);
                return 1;
            }
            // op-code position 3
            if (lineNumber[11] != 'H') {
                fprintf(file, "%s!!!: ERROR- Invalid Op-code character at position 3\n", lineNumber);
                return 1;
            }
            // op-code position 4
            if (lineNumber[12] != 'M') {
                fprintf(file, "%s!!!: ERROR- Invalid Op-code character at position 4\n", lineNumber);
                return 1;
            }
            // op-code position 5
            if ((lineNumber[13] != 'D') && (lineNumber[13] != 'S')) {
                fprintf(file, "%s!!!: ERROR- Invalid Op-code character at position 5\n", lineNumber);
                return 1;
            }
            // op-code position 6
            if ((lineNumber[14] != 'I') && (lineNumber[14] != 'F') && (lineNumber[14] != 'D')) {
                fprintf(file, "%s!!!: ERROR- Invalid Op-code character at position 6\n", lineNumber);
                return 1;
            }
            // space following op-code
            if (lineNumber[15] != ' ') {
                fprintf(file, "%s!!!: ERROR- Expected a space after Op-code\n", lineNumber);
                return 1;
            }
        }
    }
    return 0;
}

/*******************************************************************
 * Author: Bryana Craig
 * Purpose: Checks for correct operand positioning
 * Inputs: char*, FILE*
 * Outputs: int
 * Error Handling: operand in wrong location
 *******************************************************************/
int operandWrongCol(char *lineNumber, FILE *file) {
    if ((lineNumber[0] == ' ')
        && (lineNumber[9] != 'D')
        && (lineNumber[9] != 'E')) {

        int x;

        for (x = 9; x < 15; x++) {

            if (lineNumber[x] != ' ') {

                fprintf(file, "%s!!!: ERROR- Operand in wrong position: column %d\n", lineNumber, x + 1);
                return 1;
            }
        }
    }
    return 0;
}

/**********************************************
 * Author: Bryana Craig
 * Purpose: Checks for the end of file
 * Inputs: char*, int, FILE*
 * Outputs: int
 * Error Handling: end is called
 **********************************************/
int lastLineEND(char *lineNumber, int end, FILE *file) {

    if ((lineNumber[0] != '*')
        && (isalpha(lineNumber[0]) != 0)) {

        if ((lineNumber[9] == 'E')
            && (lineNumber[10] == 'N')
            && (lineNumber[11] == 'D')) {

            end = 1;
        }
    }
    return 0;
}