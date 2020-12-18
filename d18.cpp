#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

// [[Rcpp::export]]
std::vector<std::string> explode(std::string s) {
    // reinventing strsplit()
    std::vector<std::string> res;
    for (const char& c: s) {
	res.push_back(std::string(1, c));
    }
    return res;
}

// std::vector<std::string> parse_sy(std::vector<std::string> tokens) {
//     std::vector<std::string> exp;
//     std::stack<std::string> op;
//     std::vector<std::string> norm_op = {"+", "*", "("};
//     for (std::string& token: tokens) {
// 	if (std::find(norm_op.begin(), norm_op.end(), token) != norm_op.end()) {
// 	    op.push(token);
// 	} else if (token ==  ")") {
// 	    // push the op stack
// 	    while (op.top() != "(") {
// 		exp.push_back(op.top());
// 		op.pop();
// 	    }
// 	    op.pop(); // remove the "("
// 	} else if (token != " ") {
// 	    exp.push_back(token);
// 	} else {
// 	    // do nothing
	    
// 	}
//     }
//     while (!op.empty()) {
// 	exp.push_back(op.top());
// 	op.pop();
//     }
//     return exp;
// }

// https://en.wikipedia.org/wiki/Shunting-yard_algorithm


long long int parse_sy1(std::vector<std::string> tokens, std::map<std::string, int> precd) {
    std::stack<long long int> exp;
    std::stack<std::string> op;
    for (std::string& token: tokens) {
	if (token == "(") {
	    op.push(token);
	} else if (token == "*" || token == "+") {
	    while (!op.empty()) {
		std::string x = op.top();
		op.pop();
		if (x == "(" || precd[x] < precd[token]) {
		    op.push(x);
		    break;
		}
		long long int v1 = exp.top();
		exp.pop();
		long long int v2 = exp.top();
		exp.pop();
		if (x == "*") {
		    exp.push(v1 * v2);
		}
		if (x == "+") {
		    exp.push(v1 + v2);
		}
	    }
	    op.push(token);
	} else if (token ==  ")") {
	    while (!op.empty()) {
		std::string x = op.top();
		op.pop();
		if (x == "(") {
		    break;
		}
		long long int v1 = exp.top();
		exp.pop();
		long long int v2 = exp.top();
		exp.pop();
		if (x == "*") {
		    exp.push(v1 * v2);
		}
		if (x == "+") {
		    exp.push(v1 + v2);
		}
	    }
	} else if (token != " ") {
	    exp.push(std::stoll(token));
	} else {
	    // do nothing	    
	}
    }
    while (!op.empty()) {
	std::string x = op.top();
	op.pop();
	long long int v1 = exp.top();
	exp.pop();
	long long int v2 = exp.top();
	exp.pop();
	if (x == "*") {
	    exp.push(v1 * v2);
	}
	if (x == "+") {
	    exp.push(v1 + v2);
	}
    }
    //std::cout << exp.size() << std::endl;
    return exp.top();
}


// long long int cal_rpn (std::vector<std::string> input) {
//     std::stack<long long int> op;
//     for (std::string& token : input) {
// 	if (token != "*" && token != "+") {
// 	    long long int x = std::stoll(token);
// 	    op.push(x);
// 	}
// 	if (token == "*") {
// 	    long long int x = op.top();
// 	    op.pop();
// 	    long long int y = op.top();
// 	    op.pop();
// 	    op.push(x * y);
// 	}
// 	if (token == "+") {
// 	    long long int x = op.top();
// 	    op.pop();
// 	    long long int y = op.top();
// 	    op.pop();
// 	    op.push(x + y);
// 	}
//     }
//     return op.top();
// }

// [[Rcpp::export]]
long long int cal1 (std::string input) {
    std::map<std::string, int> precd;
    precd["+"] = 1;
    precd["*"] = 1;
    auto tokens = explode(input);
    return parse_sy1(tokens, precd);
}

// [[Rcpp::export]]
long long int d18_1(std::vector<std::string> input) {
    long long int ans;
    for (std::string& x: input) {
	ans += cal1(x);
    }
    return ans;
}

// [[Rcpp::export]]
long long int cal2 (std::string input) {
    std::map<std::string, int> precd;
    precd["+"] = 2;
    precd["*"] = 1;
    auto tokens = explode(input);
    return parse_sy1(tokens, precd);
}

// [[Rcpp::export]]
long long int d18_2(std::vector<std::string> input) {
    long long int ans;
    for (std::string& x: input) {
	ans += cal2(x);
    }
    return ans;
}

/***R

## input <- "1 + (2 * 3) + (4 * (5 + 6))"

## tokens <- strsplit(input, "", fixed = TRUE)[[1]]
## tokens <- tokens[tokens != " "]
## exp <- c()
## op <- c()

## for (token in tokens) {
##     if (!token %in% c("+", "*", "(", ")")) {
##         exp <- c(exp, token)
##     } else if (token %in% c("+", "*", "(")) {
##         op <- c(op, token)
##     } else if (token == ")") {
##         ## push the op stack until (
##         con <- TRUE
##         while (con) {
##             lastop <- tail(op, 1)
##             if (lastop != "(") {
##                 exp <- c(exp, lastop)
##                 op <- head(op, length(op) - 1)
##             } else {
##                 op <- head(op, length(op) - 1)
##                 con <- FALSE
##             }
##         }
##     }
## }

## c(exp, op)

Rcpp::sourceCpp("d18.cpp")

## part 1

stopifnot(cal1("1 + 2 * 3 + 4 * 5 + 6") == 71)
stopifnot(cal1("1 + (2 * 3) + (4 * (5 + 6))") == 51)

stopifnot(cal1("2 * 3 + (4 * 5)") == 26)
stopifnot(cal1("5 + (8 * 3 + 9 + 3 * 4 * 3)") == 437)
stopifnot(cal1("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))") == 12240)
stopifnot(cal1("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2") == 13632)

### part 2

stopifnot(cal2("1 + 2 * 3 + 4 * 5 + 6") == 231)

stopifnot(cal2("1 + (2 * 3) + (4 * (5 + 6))") == 51)
stopifnot(cal2("2 * 3 + (4 * 5)") == 46)
stopifnot(cal2("5 + (8 * 3 + 9 + 3 * 4 * 3)") == 1445)
stopifnot(cal2("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))") == 669060)
stopifnot(cal2("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2") == 23340)


input18 <- readLines(here::here("data/input18.txt"))

## F*ck scientific notation
options("scipen"=100, "digits"=4)
d18_1(input18)

d18_2(input18)
 */
