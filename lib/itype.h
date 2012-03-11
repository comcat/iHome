/*
 * Copyright (c) Jack's Lab (www.jackslab.org)
 * Author: Jack Tan <jiankemeng@gmail.com>
*/

#ifndef __TYPE_H__
#define __TYPE_H__

typedef unsigned char	uint8_t;
typedef signed char	int8_t;
typedef unsigned int	uint16_t;
typedef signed int	int16_t;
typedef unsigned long	uint32_t;
typedef signed long	int32_t;

typedef unsigned char	bool;

#define true	1
#define false	0

#ifndef NULL
#define NULL (void*)0
#endif

#endif
