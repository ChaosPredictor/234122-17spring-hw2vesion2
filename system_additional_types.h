/*
 * system_additional_types.h
 *
 *  Created on: May 14, 2017
 *      Author: master
 */

#ifndef SYSTEM_ADDITIONAL_TYPES_H_
#define SYSTEM_ADDITIONAL_TYPES_H_


typedef struct VisitorNodeStr {
	Visitor *visitor;
	struct VisitorNodeStr* next;
} VisitorNode;



#endif /* SYSTEM_ADDITIONAL_TYPES_H_ */
