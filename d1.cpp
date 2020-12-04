#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
int d1_1(NumericVector input) {
    for (NumericVector::iterator i = input.begin(); i != input.end(); ++i) {
	for (NumericVector::iterator j = input.begin(); j != input.end(); ++j) {
	    if (*i + *j == 2020) {
		return (*i) * (*j);
	    }
	}
    }
    return NA_REAL;
}

// [[Rcpp::export]]
int d1_2(NumericVector input) {
    for (NumericVector::iterator i = input.begin(); i != input.end(); ++i) {
	for (NumericVector::iterator j = input.begin(); j != input.end(); ++j) {
	    for (NumericVector::iterator k = input.begin(); k != input.end(); ++k) {
		if (*i + *j + *k == 2020) {
		    return (*i) * (*j) * (*k);
		}
	    }
	}
    }
    return NA_REAL;
}

/***R
# make sure it match the demo input

stopifnot(d1_1(c(1721, 979, 366, 299, 675, 1456)) == 514579)
stopifnot(d1_2(c(1721, 979, 366, 299, 675, 1456)) == 241861950)

input1 <- as.numeric(readLines(here::here("data/input1.txt")))
d1_1(input1)
d1_2(input1)
*/
