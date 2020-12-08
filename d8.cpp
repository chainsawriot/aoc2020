#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

struct instruction {
    std::string cmd;
    int value;
};

bool key_find(int key, std::vector<int> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

// [[Rcpp::export]]
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

std::vector<struct instruction> parse_input (std::vector<std::string> input) {
    std::vector<struct instruction> res;
    for (std::string& i : input) {
	std::vector<std::string> x = explode(i);
	struct instruction cmd;
	cmd.cmd = x[0];
	cmd.value = std::stoi(x[1]);
	res.push_back(cmd);
    }
    return res;
}

std::pair<bool, int> check_termination (std::vector<struct instruction> parsed_instruction) {
    int accum = 0;
    int cur_pt = 0;
    std::vector<int> sack = {};
    int terminal = parsed_instruction.size();
    bool finished = false;
    while (!key_find(cur_pt, sack)) {
	sack.push_back(cur_pt);
	auto cmd = parsed_instruction[cur_pt];
	if (cmd.cmd == "acc") {
	    accum += cmd.value;
	    cur_pt += 1;
	} else if (cmd.cmd == "jmp") {
	    cur_pt += cmd.value;
	} else {
	    cur_pt += 1;
	}
	if (cur_pt >= terminal) {
	    // std::cout << "term" << std::endl;
	    finished = true;
	    break;
	}
    }
    std::pair<bool, int> res;
    res.first = finished;
    res.second = accum;
    return res;
}


// [[Rcpp::export]]
int d8_1(std::vector<std::string> input) {
    auto parsed_instruction = parse_input(input);
    auto res = check_termination(parsed_instruction);
    return res.second;
}


// [[Rcpp::export]]
int d8_2(std::vector<std::string> input) {
    auto parsed_instruction = parse_input(input);
    for (int i = 0; i < parsed_instruction.size(); i++) {
	auto x = parsed_instruction;
	if (x[i].cmd == "jmp") {
	    x[i].cmd = "nop";
	} else if (x[i].cmd == "nop") {
	    x[i].cmd = "jmp";
	}
	// std::cout << x[i].cmd << std::endl;
	auto res = check_termination(x);
	if (res.first) {
	    return res.second;
	}
    }
    return 0;
}

/***R
testdata <- "nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
jmp -4
acc +6"

testdata2 <- "nop +0
acc +1
jmp +4
acc +3
jmp -3
acc -99
acc +1
nop -4
acc +6"


stopifnot(d8_1(strsplit(testdata, "\n", TRUE)[[1]]) == 5)
## check termination
stopifnot(d8_1(strsplit(testdata2, "\n", TRUE)[[1]]) == 8)
stopifnot(d8_2(strsplit(testdata, "\n", TRUE)[[1]]) == 8)

input8 <- readLines(here::here("data/input8.txt"))

d8_1(input8)

d8_2(input8)

 */
