#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H

#include <cstdint>
#include "facility_types.h"

namespace cmdc0de {


class ErrorType {
public:
	typedef uint32_t ErrorNum;
	//static const uint32_t NO_ERROR{ 0 };
public:
	ErrorType();
	ErrorType(const Facility &f);
	ErrorType(const Facility &f, const uint32_t &en);
	ErrorType(const ErrorType &r);
	bool ok();
	ErrorType &operator=(const ErrorType &r);
	uint8_t getError() {
		return ErrorNumber;
	}
private:
	uint32_t  FacilityNum:8;
	uint32_t  ErrorNumber:16;
};

}
#endif
