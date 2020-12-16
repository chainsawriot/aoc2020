#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


std::vector<std::string> explode(std::string s, std::string regex_string = "[^- :]+") {
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


std::vector<std::pair<int, int>> parse_criteria (std::vector<std::string> criteria) {
    std::vector<std::pair<int, int>> res;
    for (std::string& x: criteria) {
	std::vector<std::string> limits = explode(x, "[0-9]+");
	std::pair<int, int> x1;
	x1.first = std::stoi(limits[0]);
	x1.second = std::stoi(limits[1]);
	std::pair<int, int> x2;
	x2.first = std::stoi(limits[2]);
	x2.second = std::stoi(limits[3]);
	res.push_back(x1);
	res.push_back(x2);
    }
    return res;
}

bool check_value (int input, std::pair<int, int> crit) {
    return input >= crit.first && input <= crit.second;
}

bool check_value2 (int input, std::vector<std::pair<int, int>> criteria) {
    for (std::pair<int, int>& crit: criteria) {
	if (check_value(input, crit)) {
	    return true;
	}
    }
    return false;
}


void pp(std::vector<std::pair<int, int>> input) {
    for (std::pair<int, int>& x: input) {
	std::cout << "First value: "<< std::endl;
	std::cout << x.first << std::endl;
	std::cout << "Second value: "<< std::endl;
	std::cout << x.second << std::endl;
    }
}


// std::vector<struct criterion> parse_criteria (std::vector<std::string> input) {
//     std::vector<struct criterion> res;
//     for (std::string& crit: input) {
// 	auto x = parse_criterion(crit);
// 	res.push_back(x);
//     }
//     return res;
// }

// [[Rcpp::export]]
bool check_test2 (int input_num, std::vector<std::string> input) {
    auto x = parse_criteria(input);
    //pp(x);
    return check_value2(input_num, x);
}

// [[Rcpp::export]]
unsigned int d16_1 (std::vector<std::string> tiks, std::vector<std::string> crits) {
    auto criteria = parse_criteria(crits);
    unsigned int total = 0;
    for (std::string& x: tiks) {
	auto y = explode(x, "[0-9]+");
	for (std::string& z: y) {
	    int val = std::stoi(z);	    
	    if (!check_value2(val, criteria)) {
		total += val;
	    }
	}
    }
    return total;
}

// [[Rcpp::export]]
std::vector<std::string> filter_tix (std::vector<std::string> tiks, std::vector<std::string> crits) {
    auto criteria = parse_criteria(crits);
    std::vector<std::string> res;
    for (std::string& x: tiks) {
	bool incl = true;
	auto y = explode(x, "[0-9]+");
	for (std::string& z: y) {
	    int val = std::stoi(z);	    
	    if (!check_value2(val, criteria)) {
		incl = false;
	    }
	}
	if (incl) {
	    res.push_back(x);
	}
    }
    return res;
}

struct criterion {
    std::string name;
    std::pair<int, int> range1;
    std::pair<int, int> range2;
};

