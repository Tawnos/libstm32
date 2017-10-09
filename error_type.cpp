#include "error_type.h"

using namespace cmdc0de;

ErrorType::ErrorType() :
		FacilityNum(Facility::ERROR_FACILITY_GENERAL), ErrorNumber(NO_ERROR) {
}

ErrorType::ErrorType(const Facility &f) : FacilityNum(f), ErrorNumber(NO_ERROR){

}

ErrorType::ErrorType(const Facility &f, const ErrorNum &en) : FacilityNum(f), ErrorNumber(en){

}

bool ErrorType::ok() {
	return ErrorNumber == NO_ERROR;
}

ErrorType::ErrorType(const ErrorType &r) {
	(*this) = r;
}

ErrorType &ErrorType::operator=(const ErrorType &r) {
	ErrorNumber = r.ErrorNumber;
	return (*this);
}
