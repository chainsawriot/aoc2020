#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

struct criterion {
    std::string name;
    std::pair<int, int> range1;
    std::pair<int, int> range2;
};


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


// std::vector<std::pair<int, int>> parse_criteria (std::vector<std::string> criteria) {
//     std::vector<std::pair<int, int>> res;
//     for (std::string& x: criteria) {
// 	std::vector<std::string> limits = explode(x, "[0-9]+");
// 	std::pair<int, int> x1;
// 	x1.first = std::stoi(limits[0]);
// 	x1.second = std::stoi(limits[1]);
// 	std::pair<int, int> x2;
// 	x2.first = std::stoi(limits[2]);
// 	x2.second = std::stoi(limits[3]);
// 	res.push_back(x1);
// 	res.push_back(x2);
//     }
//     return res;
// }

std::vector<struct criterion> parse_criteria (std::vector<std::string> criteria) {
    std::vector<struct criterion> res;
    for (std::string& x: criteria) {
	std::vector<std::string> limits = explode(x, "[0-9]+");
	std::vector<std::string> words = explode(x, "[a-z ]+");
	struct criterion crit;
	crit.name = words[0];
	std::pair<int, int> x1;
	x1.first = std::stoi(limits[0]);
	x1.second = std::stoi(limits[1]);
	std::pair<int, int> x2;
	x2.first = std::stoi(limits[2]);
	x2.second = std::stoi(limits[3]);
	crit.range1 = x1;
	crit.range2 = x2;
	res.push_back(crit);
    }
    return res;
}


bool check_value (int input, std::pair<int, int> crit) {
    return input >= crit.first && input <= crit.second;
}

// bool check_value2 (int input, std::vector<std::pair<int, int>> criteria) {
//     for (std::pair<int, int>& crit: criteria) {
// 	if (check_value(input, crit)) {
// 	    return true;
// 	}
//     }
//     return false;
// }

bool check_value2 (int input, std::vector<struct criterion> criteria) {
    for (struct criterion& crit: criteria) {
	if (check_value(input, crit.range1)) {
	    return true;
	}
	if (check_value(input, crit.range2)) {
	    return true;
	}
    }
    return false;
}

bool single_check (int input, struct criterion crit) {
    if (check_value(input, crit.range1) || check_value(input, crit.range2)) {
	return true;
    }  
    return false;
}


void pp(std::vector<struct criterion> input) {
    for (struct criterion& x: input) {
	std::cout << x.name << std::endl;
	std::cout << "First value: "<< std::endl;
	std::cout << x.range1.first << std::endl;
	std::cout << "Second value: "<< std::endl;
	std::cout << x.range1.second << std::endl;
	std::cout << "First value: "<< std::endl;
	std::cout << x.range2.first << std::endl;
	std::cout << "Second value: "<< std::endl;
	std::cout << x.range2.second << std::endl;
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

bool check_column (NumericVector input, struct criterion crit) {
    for (int i = 0; i < input.size(); i++) {
	if (!single_check(input[i], crit)) {
	    return false;
	}
    }
    return true;
}

void pp_vec (std::vector<int> input) {
    for (int& v: input) {
	std::cout << v;
	std::cout << ",";
    }
    std::cout << std::endl;
}

std::map<int, std::vector<int>> find_matching (NumericMatrix input, std::vector<std::string> crits) {
    auto criteria = parse_criteria(crits);
    std::map<int, std::vector<int>> matching;
    // std::cout << criteria.size() << std::endl;
    // pp(criteria);
    for (int i = 0; i < input.ncol(); i++) {
	// std::cout << i << std::endl;
	NumericVector g = input(_, i);
	matching[i] = {};
	for (int j = 0; j < criteria.size(); j++) {
	    if (check_column(g, criteria[j])) {
		matching[i].push_back(j);
	    }
	}
    }
    return matching;
}

bool key_find(std::string key, std::vector<std::string> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

std::vector<int> c_setdiff(std::vector<int> x, std::vector<int> consider) {
    std::vector<int> z;
    for (int& v: x) {
	if (std::find(consider.begin(), consider.end(), v) == consider.end()) {
	    z.push_back(v);
	}
    }
    return z;
}


std::vector<int> align (std::map<int, std::vector<int>> matching) {
    std::vector<int> alignment(matching.size());
    std::vector<int> consider;
    while (consider.size() < alignment.size()) {
	for (int val = 0; val < matching.size(); val++) {
	    auto x = c_setdiff(matching[val], consider);
	    if (x.size() == 1) {
	    // first is column index; second is criterion index
		alignment[val] = x[0];
		consider.push_back(x[0]);
	    }
	}
    }
    return alignment;
}

// Code quality sucks. But I don;t have time and I can;t sleep.

// [[Rcpp::export]]
long int d16_2(NumericMatrix input, std::vector<std::string> crits, std::string mytix) {
    auto criteria = parse_criteria(crits);
    auto matching = find_matching(input, crits);
    auto alignment =  align(matching);
    auto mytick = explode(mytix, "[0-9]+");
    long int sum = 1;
    for (int i = 0; i < alignment.size(); i++) {
	if (alignment[i] < 6) {
	    sum = sum * std::stoi(mytick[i]); 
	}
    }
    return sum;
}

/***R

testdata <- "class: 1-3 or 5-7
row: 6-11 or 33-44
seat: 13-40 or 45-50

your ticket:
7,1,14

nearby tickets:
7,3,47
40,4,50
55,2,20
38,6,12"

testcrit <- strsplit(testdata, "\n", TRUE)[[1]][1:3]
testtix <- strsplit(testdata, "\n", TRUE)[[1]][9:12]

stopifnot(check_test2(1, testcrit))
stopifnot(!check_test2(4, testcrit))


stopifnot(filter_tix(testtix, testcrit) == "7,3,47")

testdata_crit <- c("class: 0-1 or 4-19", "row: 0-5 or 8-19", "seat: 0-13 or 16-19")

testdata_nb <- "3,9,18
15,1,5
5,14,9"

test_matrix <- matrix(as.numeric(str_split(paste0(testdata_nb, collapse=","), "[,\n]")[[1]]), nrow = 3, ncol = 3, byrow = TRUE)

stopifnot(d16_1(testtix, testcrit) == 71)



require(stringr)
input16 <- readLines(here::here("data/input16.txt"))
criteria <- input16[1:20]
nbtix <- input16[26:length(input16)]
yrtix <- input16[23]

valid_tix <- filter_tix(nbtix, criteria)


## 190x20
nb_matrix <- matrix(as.numeric(str_split(paste0(valid_tix, collapse=","), ",")[[1]]), nrow = 190, ncol = 20, byrow = TRUE)

d16_1(nbtix, criteria)

d16_2(nb_matrix, criteria, yrtix)


 */
