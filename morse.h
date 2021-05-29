/*
 * @file morse.h
 *
 * Contains a declaration for a function for decoding
 * Morse code to ASCII
 *
 * @author Dusan Ilic
 * @date 2021
 */

#ifndef MORSE_H_
#define MORSE_H_

#define MAX_CODE_LENGTH         (4)
#define NUM_OF_CODES            (26)

/**
 * @brief Morse code string to ASCII decoder
 *
 * Input is a string containing '-' and '.' (dash and dot)
 * Returns the decoded ASCII character. If not recognized
 * returns '*'
 *
 * @param code string Morse code (containing '.' and '-') to decode to ASCII
 * @return ASCII Code
 */
extern char morse_to_ascii(const char* code);

#endif /* MORSE_H_ */
