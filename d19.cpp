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

// unsigned int count_matches (std::string input) {
//     std::smatch mymatches;
//     std::regex myregex ("'\\([^)]*\\)'");
//     std::regex_match (input, mymatches, myregex );
//     return mymatches.size();
// };

std::string rewrite(std::string input, rulebook& rb, bool force = false) {
    std::vector<std::string> broken_content = explode(input, "[^ ]+");
    for (int i = 0; i < broken_content.size(); i++) {
	std::string content = broken_content[i];
	if (check(content)) {
	    std::string rule = rb[content];
	    if (force) {
		broken_content[i] = "c";
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

rulebook iter_once(rulebook& input, bool force = false) {
    rulebook newbook = input;
    for (auto& data: input) {
	newbook[data.first] = rewrite(data.second, input, force);
    }
    return newbook;
}

rulebook iter(rulebook& oldbook) {
    rulebook newbook = oldbook;
    while(!check_all_resolved(newbook)) {
	newbook = iter_once(oldbook);
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
std::string gen_regex(std::vector<std::string> rules_id, std::vector<std::string> rules_content, std::string which = "0", bool add_head = true) {
    auto rb = parse(rules_id, rules_content);
    auto final_rb = iter(rb);
    std::string res = std::regex_replace(final_rb[which], std::regex(" +"), "");
    if (add_head) {
	res = "^" + res + "$";
    }
    return res;
}


// [[Rcpp::export]]
unsigned int d19_1(std::vector<std::string> data, std::vector<std::string> rules_id, std::vector<std::string> rules_content, std::string which = "0") {
    std::string reg = gen_regex(rules_id, rules_content, which);
    unsigned int total = 0;
    for (std::string& d: data) {
	if (check(d, reg)) {
	    total++;
	}
    }
    return total;
}
//


rulebook fuck_iter(rulebook& oldbook) {
    rulebook newbook = oldbook;
    while(!check_all_resolved(newbook)) {
	newbook = iter_once(oldbook);
	oldbook = newbook;
    }
    newbook["8"] = "(:?" + newbook["42"] + ")+";
    std::string inner42 = "(?:" + newbook["42"]+ ")";
    std::string inner31 = "(?:" + newbook["31"] +")";
    std::string rep1 = "(" + inner42 + inner31 + ")";
    std::string rep2 = "(" + inner42 + inner42 + inner31 + inner31 + ")";
    std::string rep3 = "(" + inner42 + inner42 + inner42 + inner31 + inner31 + inner31 + ")";
    std::string rep4 = "(" + inner42 + inner42 + inner42 + inner42 + inner31 + inner31 + inner31 + inner31 + ")";
    std::string rep5 = "(" + inner42 + inner42 + inner42 + inner42 + inner42 + inner31 + inner31 + inner31 + inner31 + inner42 + ")";
    newbook["11"] = "((" + rep1 + ")|(" + rep2 + ")|(" + rep3 + ")|(" + rep4 + ")|("+ rep5 + ")";
    newbook["0"] = "8 11";
    rulebook finalbook = newbook;
    finalbook["0"] = rewrite(newbook["0"], newbook);
    return finalbook;
}

// [[Rcpp::export]]
std::string gen_regex2(std::vector<std::string> rules_id, std::vector<std::string> rules_content, std::string which = "0") {
    auto rb = parse(rules_id, rules_content);
    auto final_rb = fuck_iter(rb);
    return "^" + std::regex_replace(final_rb[which], std::regex(" +"), "") + "$";
}

// [[Rcpp::export]]
unsigned int d19_2(std::vector<std::string> data, std::vector<std::string> rules_id, std::vector<std::string> rules_content, std::string which = "0", int max_iter = 2) {
    std::string reg = gen_regex2(rules_id, rules_content, which);
    unsigned int total = 0;
    for (std::string& d: data) {
	if (check(d, reg)) {
	    total++;
	}
    }
    return total;
}

/***R
require(tidyverse)
"0: 4 1 5
1: 2 3 | 3 2
2: 4 4 | 5 5
3: 4 5 | 5 4
4: \"a\"
5: \"b\"" %>% gsub("\"", "", .) %>% str_split("\n", simplify = TRUE) %>% as.vector  -> testrules


rules_id <- testrules %>% str_split("\\: ") %>% map_chr(1)
rules_content <- testrules %>% str_split("\\: ") %>% map_chr(2)


"ababbb
bababa
abbbab
aaabbb
aaaabbb" %>% str_split("\n", simplify = TRUE) %>% as.vector -> testdata

d19_1(testdata, rules_id, rules_content)



## I am anonyed, let's hard code this fuck.
## 18 = 42 | 42 42 | 42 42 42 | 42 42 42 | ... = 42+
## 11 = 42 31 | 42 42 31 31 | 42 42 42 31 31 31 | ... = 42{n}31{n}
## but C++'s regex doesn't support recursion, i.e. those {n}
## so I cheat!

transform_rules <- function(rule){
    rule <- gsub('\"a\"', "a", rule)
    rule <- gsub('\"b\"', "b", rule)
    rule <- gsub("(\\d+)", ":\\1:", rule)
    while(any(grepl("\\d", rule))){        
        ids <- names(rule)[grep("\\d+", rule, invert = TRUE)]
        rule[ids] <- gsub(" ","", rule[ids])
        for(id in ids){
            k <- rule[id]
            if(nchar(k) > 1)
                k <- sprintf("(%s)", k)
            rule <- gsub(pattern     = sprintf(":%s:", id), 
                         replacement = k, 
                         x = rule)
        }
    }
    rule <- gsub("^", "^", rule)
    rule <- gsub("$", "$", rule)
    rule <- gsub(" ", "", rule)
    rule
}

d19_2 <- function(input) {
    mark <- which(input == "")
    msg <- input[(mark+ 1): length(input)]
    rules <- strsplit(input[1: (mark - 1)], ": ", fixed = TRUE)
    rule_id <- as.numeric(map_chr(rules, pluck, 1))
    rule <- map_chr(rules, pluck, 2)[order(rule_id)]
    names(rule) <- sort(rule_id)
    rule['8'] <- "42+"
    rule['11'] <- "(?P<eleven> 42 (?&eleven)? 31)"
    rule <- transform_rules(rule)
    return(sum(grepl(rule['0'], msg, perl = TRUE)))
}

input19 <- readLines(here::here("data/input19.txt"))

i19rules_id <- input19[1:129] %>% str_split("\\: ") %>% map_chr(1)
i19rules_content <- input19[1:129] %>% gsub("\"", "", .) %>% str_split("\\: ") %>% map_chr(2)

i19data <- input19[131:length(input19)]

d19_1(i19data, i19rules_id, i19rules_content)

d19_2(input19)
*/
