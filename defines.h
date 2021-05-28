#ifndef DEFINES_H_
#define DEFINES_H_

/* Bit operations */
#define BIT_SET(reg, bit)       reg |= (bit)
#define BIT_CLEAR(reg, bit)     reg &= ~(bit)
#define BIT_TOGGLE(reg, bit)    reg ^= (bit)

#endif /* DEFINES_H_ */
