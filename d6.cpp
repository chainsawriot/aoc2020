#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

std::vector<std::string> normalize(CharacterVector input, std::string delimit = " ") {
    std::vector<std::string> sack;
    std::string record;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	if (line != "") {
	    record += line;
	    record += delimit;
	} else {
	    sack.push_back(record);
	    record = "";
	}
    }
    return sack;
}


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

std::vector<std::string> runique(std::vector<std::string> input) {
    // well, it returns all unique values
    // akin unique in R
    std::sort(input.begin(), input.end());
    auto res = std::unique(input.begin(), input.end());
    input.erase(res, input.end());
    return input;
}

// [[Rcpp::export]]
int d6_1 (CharacterVector input) {
    std::vector<std::string> clean_input = normalize(input);
    int ans = 0;
    for (std::vector<std::string>::const_iterator i = clean_input.begin(); i != clean_input.end(); ++i) {
	int n_unique_answers = runique(explode(*i, "[a-zA-Z0-9]")).size();
	ans += n_unique_answers;
    }
    return ans;
}

bool key_find(std::string key, std::vector<std::string> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

// Not the speediest implementation, but quickest to implement.
std::vector<std::string> rintersect(std::string input) {
    std::vector<std::string> unique_ans = runique(explode(input, "[a-zA-Z0-9]"));
    std::vector<std::string> person_ans = explode(input, "[^ ]+");
    std::vector<std::string> common_ans;
    for (std::vector<std::string>::const_iterator i = unique_ans.begin(); i != unique_ans.end(); ++i) {
	bool incl = true;
	for (std::vector<std::string>::const_iterator j = person_ans.begin(); j != person_ans.end(); ++j) {
	    if (!key_find((*i), explode((*j), "[a-zA-Z0-9]"))) {
		incl = false;
	    }
	}
	if (incl) {
	    common_ans.push_back(*i);
	}
	incl = true;
    }
    return common_ans;
}

// [[Rcpp::export]]
int d6_2 (CharacterVector input) {
    std::vector<std::string> clean_input = normalize(input);
    int ans = 0;
    for (std::vector<std::string>::const_iterator i = clean_input.begin(); i != clean_input.end(); ++i) {
	int n_unique_answers = rintersect(*i).size();
	ans += n_unique_answers;
    }
    return ans;
}

/***R
input6 <- c(readLines(here::here("data/input6.txt")), "")
testdata <- "abc

a
b
c

ab
ac

a
a
a
a

b

"

stopifnot(d6_1(strsplit(testdata, "\n", TRUE)[[1]]) == 11)
stopifnot(d6_2(strsplit(testdata, "\n", TRUE)[[1]]) == 6)

d6_1(input6)
d6_2(input6)

 */
