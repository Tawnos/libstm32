#ifndef ERROR_TYPE_H
#define ERROR_TYPE_H

#include <cstdint>
#include "facility_types.h"

namespace cmdc0de {


class ErrorType {
public:
	typedef uint32_t ErrorNum;
	static const uint32_t NoError{ 0 };
public:
	ErrorType() :
		FacilityNum(Facility::ERROR_FACILITY_GENERAL), ErrorNumber(ErrorType::NoError) {
	}

	ErrorType(const Facility& f) : FacilityNum(f), ErrorNumber(ErrorType::NoError) {

	}

	ErrorType(const Facility& f, const ErrorNum& en) : FacilityNum(f), ErrorNumber(en) {

	}

	bool ok() { return ErrorNumber == ErrorType::NoError; }

	ErrorType(const ErrorType& r) { (*this) = r; }

	ErrorType& operator=(const ErrorType& r) {
		ErrorNumber = r.ErrorNumber;
		return (*this);
	}

	uint8_t getError() {
		return ErrorNumber;
	}
private:
	uint32_t  FacilityNum:8;
	uint32_t  ErrorNumber:16;
};

}
#endif
