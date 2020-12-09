#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


bool key_find(int key, std::vector<unsigned int> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

// [[Rcpp::export]]
std::vector<unsigned int> exclude (std::vector<unsigned int> input, unsigned int x) {
    for (int i = 0; i < input.size(); i++) {
	if (i == x) {
	    input.erase(input.begin()+(i));
	}
    }
    return input;
}

// [[Rcpp::export]]
std::vector<unsigned int> slice (std::vector<unsigned int> input, unsigned int last_pos, unsigned int width) {
    // will not emit input[last_pos], i.e. "width" items before the last_pos
    return std::vector<unsigned int>(input.begin()+last_pos-width, input.begin()+last_pos);
}

// [[Rcpp::export]]
bool check_valid(std::vector<unsigned int> input, unsigned int i, unsigned int width = 5) {
    unsigned int target = input[i];
    // std::cout << target <<std::endl;
    std::vector<unsigned int> search_space = slice(input, i, width);
    // for (int& z: search_space) {
    // 	std::cout << z <<std::endl;
    // }
    for (unsigned int x = 0; x < search_space.size(); x++) {
	//std::cout << x <<std::endl;
	unsigned int target_in_search_space = search_space[x];
	//	std::cout << target_in_search_space <<std::endl;
	unsigned int diff = target - target_in_search_space;
	// std::cout << diff <<std::endl;	
	if (key_find(diff, exclude(search_space, x))) {
	    return true;
	}
    }
    return false;
}

// [[Rcpp::export]]
std::vector<unsigned int> convert (std::vector<std::string> input) {
    std::vector<unsigned int> res;
    for (std::string& x : input) {
	res.push_back(std::stoul(x));
    }
    return res;
}

// [[Rcpp::export]]
unsigned int d9_1 (std::vector<std::string> input, unsigned int width = 25) {
    std::vector<unsigned int> input2 = convert(input);
    for (unsigned int i = width; i < input2.size(); i++) {
	if (!check_valid(input2, i, width)) {
	    return input2[i];
	}
    }
    return 0;
}

// [[Rcpp::export]]
unsigned int rcpp_sum (std::vector<unsigned int> input) {
    unsigned int sum;
    for (unsigned int& x: input) {
	sum += x;
    }
    return sum;
}

// [[Rcpp::export]]
std::vector<unsigned int> search (std::vector<unsigned int> input, unsigned int target) {
    for (int window_size = 2; window_size < input.size(); window_size ++) {
	for (int x = window_size; x < input.size(); x++) {
	    auto y = slice(input, x, window_size);
	    if (rcpp_sum(y) == target) {
		return y;
	    }
	}
    }
    return {};
}
// [[Rcpp::export]]
unsigned int d9_2 (std::vector<std::string> input, unsigned int width = 25) {
    unsigned int target = d9_1(input, width);
    std::vector<unsigned int> input2 = convert(input);
    std::vector<unsigned int> res = search(input2, target);
    auto max_v = *std::max_element(std::begin(res), std::end(res));
    auto min_v = *std::min_element(std::begin(res), std::end(res));
    return max_v + min_v;
}

/***R
testdata <- "35
20
15
25
47
40
62
55
65
95
102
117
150
182
127
219
299
277
309
576"

stopifnot(d9_1(strsplit(testdata, "\n", TRUE)[[1]], 5) == 127)
stopifnot(d9_2(strsplit(testdata, "\n", TRUE)[[1]], 5) == 62)

input9 <- readLines(here::here("data/input9.txt"))
d9_1(input9)
d9_2(input9)
 */
