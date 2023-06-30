#pragma once
#include <stdexcept>
using namespace std;

class SetFullException : public std::exception {
public:
	const char* what () const throw () {
        return "Cache eviction needed";
    }
};

