#pragma once
#include <cmath>

bool isOfBase2(double number) {
	double res = log2(number);
	return ceil(res) ==  floor(res);
}

int log2i(double number) {
	return static_cast<int>(log2(number));
}

