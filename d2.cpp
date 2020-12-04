#include <Rcpp.h>
#include <regex>

using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

struct record {
    int min_freq;
    int max_freq;
    char checkchar;
    std::string password;
};

struct record parse(std::string s) {
    std::regex words_regex("[^- :]+");
    auto words_begin = std::sregex_iterator(s.begin(), s.end(), words_regex);
    auto words_end = std::sregex_iterator();
    std::array<std::string, 4> res;
    int j = 0;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
	res[j] = (*i).str();
	j += 1;
    }
    struct record r;
    r.min_freq = std::stoi(res[0]);
    r.max_freq = std::stoi(res[1]);
    r.checkchar = res[2][0];
    r.password = res[3];
    return r;
}

bool check_password(int min_freq, int max_freq, char checkchar, std::string &password) {
    // std::string is actually an array
    int occurence = 0;
    for (std::string::size_type i = 0; i < password.size(); i++) {
	if (password[i] == checkchar) {
	    occurence += 1;
	}
    }
    return occurence >= min_freq && occurence <= max_freq; 
}

bool check_password2(int min_freq, int max_freq, char checkchar, std::string &password) {
    // std::string is actually an array
    int occurence = 0;
    if (password[min_freq - 1] == checkchar) {
	occurence += 1;
    }
    if (password[max_freq - 1] == checkchar) {
	occurence += 1;
    }
    return occurence == 1;
}


bool line_check(std::string input_line) {
    struct record r = parse(input_line);
    return check_password(r.min_freq, r.max_freq, r.checkchar, r.password); 
}


bool line_check2(std::string input_line) {
    struct record r = parse(input_line);
    return check_password2(r.min_freq, r.max_freq, r.checkchar, r.password); 
}

// [[Rcpp::export]]
int d2_1(CharacterVector input) {
    int count = 0;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	bool valid = line_check(line);
	if (valid) {
	    count += 1;
	}
    }
    return count;
}

// [[Rcpp::export]]
int d2_2(CharacterVector input) {
    int count = 0;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	bool valid = line_check2(line);
	if (valid) {
	    count += 1;
	}
    }
    return count;
}

/***R
testdata <- c("1-3 a: abcde", "1-3 b: cdefg", "2-9 c: ccccccccc")
d2_1(testdata) == 2
d2_2(testdata) == 1

input2 <- readLines(here::here("data/input2.txt"))
d2_1(input2)
d2_2(input2)
*/
