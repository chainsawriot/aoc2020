#include <Rcpp.h>
#include <regex>
#include <algorithm>
#include <bitset>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


// [[Rcpp::export]]
char bit_or (char x, char m) {
    if (m == 'X') {
	    return x;
    }
    if (m == x) {
	return x;
    }
    return m;
}

// [[Rcpp::export]]
std::string apply_mask (std::string input, std::string mask) {
    std::string output = std::string(input.length(), ' ');
    for (int i = 0; i < input.length(); i++) {
	output[i] = bit_or(input[i], mask[i]);
    }
    return output;
}
// [[Rcpp::export]]
std::string int_binary(long int input) {
    std::bitset<36> output(input);
    return output.to_string();
}

// [[Rcpp::export]]
long int binary_int(std::string input) {
    std::bitset<36> output(input);
    return output.to_ulong();
}

// [[Rcpp::export]]
long int decode(long int input, std::string mask) {
    return binary_int(apply_mask(int_binary(input), mask));
}

std::vector<std::string> explode(std::string s, std::string regex_string = "[^ ]+") {
    // reinventing strsplit()
    std::regex words_regex(regex_string);
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), words_regex);
    auto words_end = std::sregex_iterator();
    std::vector<std::string> res;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
	res.push_back((*i).str());
    }
    return res;
}

std::tuple<bool, long int, long int, std::string> extract_cmd(std::string input) {
    if (input.substr(0, 4) == "mask") {
	return std::make_tuple(false, 0, 0, input.substr(7));
    }
    std::vector<std::string> output = explode(input, "[0-9]+");
    return std::make_tuple(true, std::stol(output[0]), std::stol(output[1]), "");
}

// [[Rcpp::export]]
long int d14_1 (std::vector<std::string> input) {
    std::string current_mask = "";
    std::map<long int, long int> memory;
    for (std::string& prog: input) {
	auto parsed_cmd = extract_cmd(prog);
	if (!std::get<0>(parsed_cmd)) {
	    // a mask
	    current_mask = std::get<3>(parsed_cmd);
	} else {
	    // a mem	    
	    memory[std::get<1>(parsed_cmd)] = decode(std::get<2>(parsed_cmd), current_mask);
	}
    }
    long int res = 0;
    for (auto const& x: memory) {
	res += x.second;
    }
    return res;
}

// [[Rcpp::export]]
char address_op (char x, char m) {
    if (m == '0') {
	return x;
    }
    if (m == '1') {
	return '1';
    }
    return 'X';
}

// WTF! I must need to know how to write higher order function in C++!

// [[Rcpp::export]]
std::string address_apply_mask (std::string input, std::string mask) {
    std::string output = std::string(input.length(), ' ');
    for (int i = 0; i < input.length(); i++) {
	output[i] = address_op(input[i], mask[i]);
    }
    return output;
}


int num_x(std::string input) {
    int total = 0;
    for (int i = 0; i < input.length(); i++) {
	if (input[i] == 'X') {
	    total += 1;
	}
    }
    return total;
}

// [[Rcpp::export]]
std::string replace_x(std::string input, std::string sub_binary) {
    int x = 0;
    for (int y = 0; y < input.length(); y++) {
	if (input[y] == 'X') {
	    input[y] = sub_binary[x];
	    x += 1;
	}
    }
    return input;
}

// [[Rcpp::export]]
std::vector<long int> gen_add(std::string input) {
    std::vector<long int> res = {};
    int numx = num_x(input);
    int poss = std::pow(2, numx);
    for (int i = 0; i < poss; i++) {
	std::string binary = int_binary(i);
	std::string sub_binary = binary.substr(binary.size() - numx);
	std::string ad = replace_x(input, sub_binary);
	res.push_back(binary_int(ad));
    }
    return res;
}

// [[Rcpp::export]]
long int decode2(long int input, std::string mask) {
    return binary_int(address_apply_mask(int_binary(input), mask));
}

// [[Rcpp::export]]
long int d14_2 (std::vector<std::string> input) {
    std::string current_mask = "";
    std::map<long int, long int> memory;
    for (std::string& prog: input) {
	auto parsed_cmd = extract_cmd(prog);
	if (!std::get<0>(parsed_cmd)) {
	    // a mask
	    current_mask = std::get<3>(parsed_cmd);
	} else {
	    auto addresses_bin = address_apply_mask(int_binary(std::get<1>(parsed_cmd)), current_mask);
	    auto addresses = gen_add(addresses_bin);
	    for (long int& ad: addresses) {
	 	 memory[ad] = std::get<2>(parsed_cmd);
	     }
	}
    }
    long int res = 0;
    for (auto const& x: memory) {
	res += x.second;
    }
    return res;
}

/***R
testdata <- "mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X
mem[8] = 11
mem[7] = 101
mem[8] = 0"

stopifnot(d14_1(strsplit(testdata, "\n", TRUE)[[1]]) == 165)


testdata2 <- "mask = 000000000000000000000000000000X1001X
mem[42] = 100
mask = 00000000000000000000000000000000X0XX
mem[26] = 1"

stopifnot(d14_2(strsplit(testdata2, "\n", TRUE)[[1]]) == 208)


options("scipen"=100, "digits"=4)
input14 <- readLines(here::here("data/input14.txt"))

d14_1(input14)

d14_2(input14)

 */
