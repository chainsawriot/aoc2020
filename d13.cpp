#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


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

std::pair<int, std::vector<int>> preprocess (std::vector<std::string> input) {
    // I am sure in part 2, they will say those xs are useful.
    std::pair<int, std::vector<int>> res;
    res.first = std::stoi(input[0]);
    std::vector<std::string> vals = explode(input[1], "[0-9]+");
    std::vector<int> vals_int;
    for (std::string& val: vals) {
	vals_int.push_back(std::stoi(val));
    }
    res.second = vals_int;
    return res;
}

// [[Rcpp::export]]
int d13_1(std::vector<std::string> input) {
    std::pair<int, std::vector<int> > input2 = preprocess(input);
    int current_time = input2.first;
    int which_bus = 0;
    int target_time = 0;
    bool found = false;
    while(!found) {
	for (int& bnum: input2.second) {
	    if (current_time % bnum == 0) {
		which_bus = bnum;
		target_time = current_time;
		found = true;
	    }
	}
	current_time++;
    }
    int res = (target_time - input2.first) * which_bus;
    return res;
}

std::vector<std::pair<int, int>> preprocess2 (std::vector<std::string> input) {
    // see?
    std::vector<std::pair<int, int>> res;
    std::vector<std::string> vals = explode(input[1], "[^,]+");
    for (int i = 0; i < vals.size(); i++) {
	if (vals[i] != "x") {
	    res.push_back(std::make_pair(std::stoi(vals[i]), i));
	}
    }
    return res;
}

bool check_bus (std::vector<std::pair<int, int>> x, int time_stamp) {
    // x contains all the constrains from preprocess2
    for (std::pair<int, int>& p: x) {
	if ((time_stamp + p.second) % p.first != 0) {
	    return false;
	}
    }
    return true;
}

// [[Rcpp::export]]
unsigned long int d13_2_stupid (std::vector<std::string> input, unsigned long int search_time = 0) {
    auto constrains = preprocess2(input);
    unsigned long int current_time = search_time;
    while (true) {
	// std::cout << current_time << std::endl;
	if (check_bus(constrains, current_time)) {
	    return current_time;
	}
	current_time += 1;
    }
    return 0;
}

// [[Rcpp::export]]
unsigned long int d13_2_better (std::vector<std::string> input, unsigned long int search_time = 0) {
    auto constrains = preprocess2(input);
    unsigned long int incre = constrains[0].first;
    unsigned long int offset = constrains[0].second;
    if (search_time != 0) {
	search_time = search_time - (search_time % incre) - offset;
    }
    unsigned long int current_time = search_time;
    // std::cout << constrains[0].first << std::endl;
    while (true) {
	// std::cout << current_time << std::endl;
	if (check_bus(constrains, current_time)) {
	    return current_time;
	}
	// std::cout << current_time << std::endl;
	current_time += incre;
    }
    return 0;
}

std::pair<int, int> max_constrain (std::vector<std::pair<int, int>> input) {
    std::vector<int> v;
    for (std::pair<int, int>& x: input) {
	v.push_back(x.first);
    }
    int max_index = std::max_element(v.begin(),v.end()) - v.begin();
    return input[max_index];
}

// [[Rcpp::export]]
void test3 (std::vector<std::string> input) {
    auto constrains = preprocess2(input);
    auto max_c = max_constrain(constrains);
    std::cout << max_c.first << std::endl;
    std::cout << max_c.second << std::endl;
}

// [[Rcpp::export]]
unsigned long int d13_2_still_search (std::vector<std::string> input, unsigned long int search_time = 0) {
    auto constrains = preprocess2(input);
    auto max_c = max_constrain(constrains);
    unsigned long int incre = max_c.first;
    unsigned long int offset = max_c.second;
    if (search_time != 0) {
	search_time = search_time - (search_time % incre) - offset;
    }
    unsigned long int current_time = search_time;
    // std::cout << constrains[0].first << std::endl;
    while (true) {
	std::cout << current_time << std::endl;
	if (check_bus(constrains, current_time)) {
	    return current_time;
	}
	// std::cout << current_time << std::endl;
	current_time += incre;
    }
    return 0;
}

// [[Rcpp::export]]
unsigned int inverse (unsigned int m, unsigned int z) {
    unsigned int ini = 0;
    while (true) {
	if ((m * ini) % z == 1) {
	    return ini;
	}
	ini++;
    }
}

// [[Rcpp::export]]
unsigned long long int d13_2_crt (std::vector<std::string> input, unsigned long int search_time = 0) {
    // Chinese Remainer Theorem
    // See explanation: https://www.youtube.com/watch?v=Y5RcMWiUyyE
    auto constrains = preprocess2(input);
    std::vector<int> offset;
    std::vector<int> z;
    for (std::pair<int, int>& x: constrains) {
	offset.push_back(x.first - x.second);
	z.push_back(x.first);
    }
    offset[0] = 0;
    unsigned long long M = 1;
    for (int& x: z) {
	M = M * x;
    }
    std::vector<unsigned long long int> m;
    for (int& x: z) {
	m.push_back(M / x);
    }
    std::vector<unsigned long long int> y;
    for (int i = 0; i < z.size(); i++) {
	y.push_back(inverse(m[i], z[i]));
    }
    int64_t amy = 0;
    for (int i = 0; i < z.size(); i++) {
	amy += offset[i] * m[i] * y[i];
    }
    return M;
}

// [[Rcpp::export]]
unsigned long long int d13_2 (std::vector<std::string> input) {
    auto constrains = preprocess2(input);
    unsigned long long int jump = constrains[0].first;
    unsigned long long int answer = constrains[0].first;
    for (int i = 1; i < constrains.size(); i++) {
	while ((answer + constrains[i].second) % constrains[i].first != 0) {
	    answer += jump;
	}
	jump = jump * constrains[i].first;
    }
    return answer;
}

/***R

testdata <- "939
7,13,x,x,59,x,31,19
"

stopifnot(d13_1(strsplit(testdata, "\n")[[1]]) == 295)

stopifnot(d13_2_better(strsplit(testdata, "\n")[[1]]) == 1068781)

## Raw iteration: for small data it's okay; but not possible to solve our problem
stopifnot(d13_2_stupid(c("", "17,x,13,19")) == 3417)
## Raw iteration with a small modification: for small data it's okay; but not possible to solve our problem; can limit search space
stopifnot(d13_2_better(c("", "17,x,13,19"),100) == 3417)
## Chinese Remainder theorem can't lead to correct answer, if all numbers are prime.
d13_2_crt(c("", "17,x,13,19"))
d13_2(c("", "17,x,13,19"))



stopifnot(d13_2(c("", "67,7,59,61")) == 754018)
stopifnot(d13_2(c("", "67,x,7,59,61")) == 779210)
stopifnot(d13_2(c("", "67,7,x,59,61")) == 1261476)
## reduce the fucking search space!


microbenchmark::microbenchmark(d13_2_stupid(strsplit(testdata, "\n")[[1]]))
microbenchmark::microbenchmark(d13_2_better(strsplit(testdata, "\n")[[1]]))
microbenchmark::microbenchmark(d13_2(strsplit(testdata, "\n")[[1]]))

microbenchmark::microbenchmark(d13_2_stupid(c("", "17,x,13,19")))
microbenchmark::microbenchmark(d13_2_better(c("", "17,x,13,19")))
microbenchmark::microbenchmark(d13_2(c("", "17,x,13,19")))

stopifnot(d13_2(c("", "17,x,13,19")) == 3417)

stopifnot(d13_2(c("", "67,x,7,59,61")) == 779210)

stopifnot(d13_2(c("", "67,7,x,59,61")) == 1261476)

stopifnot(d13_2(c("", "1789,37,47,1889")) == 1202161486)

input13 <- readLines(here::here("data/input13.txt"))

options("scipen"=100, "digits"=4)
d13_1(input13)

d13_2(input13)

 */
