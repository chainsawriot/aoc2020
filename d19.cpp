#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


typedef std::map<std::string, std::string> rulebook;

rulebook parse(std::vector<std::string> rules_id, std::vector<std::string> rules_content) {
    rulebook res;
    for (int i = 0; i < rules_id.size(); i++) {
	res[rules_id[i]] = rules_content[i];
    }
    return res;
}

void pp_rb (rulebook input) {
    for (auto& data: input) {
	std::cout << data.first << ":" << data.second << std::endl;
    }
}

// [[Rcpp::export]]
void test_parse(std::vector<std::string> rules_id, std::vector<std::string> rules_content) {
    auto rb = parse(rules_id, rules_content);
    pp_rb(rb);
}

// [[Rcpp::export]]
bool check(std::string input, std::string reg = "^[0-9]+$") {
    const std::regex e (reg);
    if (std::regex_match(input, e)) {
	return true;
    }
    return false;
}

// [[Rcpp::export]]
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

unsigned int count_matches (std::string input) {
    std::smatch mymatches;
    std::regex myregex ("'\\([^)]*\\)'");
    std::regex_match (input, mymatches, myregex );
    return mymatches.size();
};

// if the variable rule is ridiculously long, replace it with a dummy, e.g. q
std::string rewrite(std::string input, rulebook& rb, int max_length = 450) {
    std::vector<std::string> broken_content = explode(input, "[^ ]+");
    for (int i = 0; i < broken_content.size(); i++) {
	std::string content = broken_content[i];
	if (check(content)) {
	    std::string rule = rb[content];
	    if (count_matches(rule) >= max_length) {
		broken_content[i] = "q";
	    } else if (!check(rule, ".+\\|.+")) {
		broken_content[i] = rule;
	    } else {
		broken_content[i] = "(?: " + rule + " )";
	    }
	}	
    }
    // join them back
    std::string output = "";
    for (std::string content: broken_content) {
	output = output + " " + content;
    }
    return output;
}

// [[Rcpp::export]]
void test_rw(std::vector<std::string> rules_id, std::vector<std::string> rules_content) {
    auto rb = parse(rules_id, rules_content);
    std::cout << rewrite("12 3 | 3 12", rb) << std::endl;
}

bool check_all_resolved (rulebook& input) {
    for (auto& data: input) {
	if (check(data.second, ".*[0-9].*")) {
	    return false;
	}
    }
    return true;
}

rulebook iter_once(rulebook& input, int max_length = 100) {
    rulebook newbook = input;
    for (auto& data: input) {
	newbook[data.first] = rewrite(data.second, input, max_length);
    }
    return newbook;
}

rulebook iter(rulebook& oldbook, int max_length = 100) {
    rulebook newbook = oldbook;
    while(!check_all_resolved(newbook)) {
	newbook = iter_once(oldbook, max_length);
	oldbook = newbook;
    }
    return newbook;
}


// [[Rcpp::export]]
void test_iter(std::vector<std::string> rules_id, std::vector<std::string> rules_content) {
    auto rb = parse(rules_id, rules_content);
    auto rb2 = iter_once(rb);
    auto rb3 = iter_once(rb2);
    pp_rb(rb3);
    if (check_all_resolved(rb2)) {
	std::cout << "rb2 is all resolved." << std::endl;
    }
    if (check_all_resolved(rb3)) {
	std::cout << "rb3 is all resolved" << std::endl;
    }
    auto rb4 = iter(rb);
    std::cout << "rb4: " << std::endl;
    pp_rb(rb4);
}

// [[Rcpp::export]]
std::string gen_regex(std::vector<std::string> rules_id, std::vector<std::string> rules_content, std::string which = "0", int max_length = 100) {
    auto rb = parse(rules_id, rules_content);
    auto final_rb = iter(rb, max_length);
    return "^" + std::regex_replace(final_rb[which], std::regex(" +"), "") + "$";
}


// [[Rcpp::export]]
unsigned int d19_1(std::vector<std::string> data, std::vector<std::string> rules_id, std::vector<std::string> rules_content, std::string which = "0", int max_length = 100) {
    std::string reg = gen_regex(rules_id, rules_content, which, max_length);
    unsigned int total = 0;
    for (std::string& d: data) {
	if (check(d, reg)) {
	    total++;
	}
    }
    return total;
}
//
