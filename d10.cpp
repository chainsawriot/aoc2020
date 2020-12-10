#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

std::vector<unsigned int> convert (std::vector<std::string> input) {
    std::vector<unsigned int> res;
    for (std::string& x : input) {
	res.push_back(std::stoul(x));
    }
    return res;
}

std::vector<unsigned int> gen_seq(std::vector<std::string> input) {
    std::vector<unsigned int> res = convert(input);
    std::sort(res.begin(), res.end());
    unsigned int max_value = *std::max_element(std::begin(res), std::end(res)) + 3;
    res.insert(res.begin(), 0);
    res.push_back(max_value);
    return res;
}

std::map<int, int> count_diff(std::vector<unsigned int> input) {
    std::map<int, int> count;
    count[-1] = 0;
    count[-2] = 0;
    count[-3] = 0;
    for (int i = 1; i < input.size(); i++) {
	count[input[i-1] - input[i]] += 1;
    }
    return count;
}

// [[Rcpp::export]]
unsigned int d10_1 (std::vector<std::string> input) {
    std::map<int, int> res = count_diff(gen_seq(input));
    return res[-1] * res[-3];
}

unsigned int count(unsigned int val, std::vector<unsigned int> input) {
    unsigned int count = 0;
    for (const unsigned int& z: input) {
	if (z == val) {
	    count += 1;
	}
    }
    return count;
}

std::vector<unsigned int> group_consec (std::vector<unsigned int> input) {
    // if input[i+1] + 1 != input[i]
    std::vector<bool> res;
    for (int i = 0; i < (input.size() - 1); i++) {
	res.push_back(input[i] + 1 != input[i+1]);
    }
    std::vector<unsigned int> res2 = {0};
    unsigned int current_gp = 0;
    for (const bool& val: res) {
	if (val) {
	    current_gp += 1;
	}
	res2.push_back(current_gp);
    }
    return res2;
}

std::map<unsigned int, unsigned int> count_group(std::vector<unsigned int> input) {
    std::map<unsigned int, unsigned int> counter;
    counter[0] = 0;
    counter[1] = 0;
    counter[2] = 0;
    counter[3] = 0;
    counter[4] = 0;
    counter[5] = 0;
    unsigned int max_gp = *max_element(input.begin(), input.end());
    for (unsigned i = 0; i < max_gp; i++) {
	counter[count(i, input)] += 1;
    }
    return counter;
}

// group of 3, there are 2 paths: a -> c; a -> b -> c
// group of 4, there are 4 paths: a -> d; a -> b -> d; a -> c -> d; a -> b -> c -> d;
// group of 5. there are 7 paths:
// a -> e; a -> b -> e; a -> c -> e; a -> d -> e; a -> b -> c -> e; a -> b -> d -> e; a -> b -> c -> d -> e;

// combine by *

// Even unsigned int will overflow; use unsigned long int

// [[Rcpp::export]]
unsigned long int d10_2 (std::vector<std::string> input) {
    std::vector<unsigned int> gp = group_consec(gen_seq(input));
    std::map<unsigned int, unsigned int> res = count_group(gp);
    return std::pow(2, res[3]) * std::pow(4, res[4]) * std::pow(7, res[5]);
}

/***R
input10 <- readLines(here::here("data/input10.txt"))

testdata1 <- "16
10
15
5
1
11
7
19
6
12
4"

testdata2 <- "28
33
18
42
31
14
46
20
48
47
24
23
49
45
19
38
39
11
1
32
25
35
8
17
7
9
4
2
34
10
3"

stopifnot(d10_1(strsplit(testdata1, "\n", TRUE)[[1]]) == (7 * 5))
stopifnot(d10_1(strsplit(testdata2, "\n", TRUE)[[1]]) == (22 * 10))

stopifnot(d10_2(strsplit(testdata1, "\n", TRUE)[[1]]) == 8)
stopifnot(d10_2(strsplit(testdata2, "\n", TRUE)[[1]]) == 19208)

## F*ck scientific notation
options("scipen"=100, "digits"=4)

d10_1(input10)

d10_2(input10)

 */
