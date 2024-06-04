/*
 * position.h
 *
 *  Created on: Jun 4, 2024
 *      Author: Nathan Hsiao
 */

#ifndef POSITION_H_
#define POSITION_H_


void serial_output(char *str, position* p);

typedef struct {
    int left;
    int right;
    int up;
    int down;
    int idle;
} position;


#endif /* POSITION_H_ */
