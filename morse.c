/*
 * @file morse.c
 *
 * Contains the implementation for a function for decoding
 * Morse code to ASCII
 *
 * @author Dusan Ilic
 * @date 2021
 */

#include "morse.h"
#include <stdint.h>

char *codes[] = {
    ".-",           // A
    "-...",         // B
    "-.-.",         // C
    "-..",          // D
    ".",            // E
    "..-.",         // F
    "--.",          // G
    "....",         // H
    "..",           // I
    ".---",         // J
    "-.-",          // K
    ".-..",         // L
    "--",           // M
    "-.",           // N
    "---",          // O
    ".--.",         // P
    "--.-",         // Q
    ".-.",          // R
    "...",          // S
    "-",            // T
    "..-",          // U
    "...-",         // V
    ".--",          // W
    "-..-",         // X
    "-.--",         // Y
    "--..",         // Z
};

/**
 * @brief Compares two strings for equality
 *
 * Returns 1 if the strings are identical, o otherwise
 */
int strcmp(const char *x, const char *y)
{
	while (*x)
	{
		if (*x != *y) {
			return 0;
		}
		x++; y++;
	}
	if (*y) return 0;

	return 1;
}

char morse_to_ascii(const char* code)
{
    int i;
	for(i=NUM_OF_CODES; i!=0; i--)
	{
		if (strcmp(code, codes[i-1]) != 0) {
			return (char)('A' + i -1);
		}
	}
	return '*';
}
