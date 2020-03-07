/*
 * template-utils.h
 *
 *  Created on: Mar 7, 2020
 *      Author: dcomes
 */

#ifndef UTILITY_TEMPLATE_UTILS_H_
#define UTILITY_TEMPLATE_UTILS_H_

#include <type_traits>

namespace cmdc0de {

template<typename T> const char *makeBinary(T v, char buffer[sizeof(T)*8+1]) {
	//static_assert(sizeof(buffer)>=(sizeof(T)*8)+1,"Buffer too small for makeBinary");
	int p = 0;
	for(int i=(sizeof(T)*8)-1;i!=0;--i,++p) {
		buffer[p] = (v&(1<<i))==0?'0':'1';
	}
	buffer[p] = '\0';
	return &buffer[0];
}

}

#endif /* UTILITY_TEMPLATE_UTILS_H_ */
