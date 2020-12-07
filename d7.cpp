#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]
bool key_find(std::string key, std::vector<std::string> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

// [[Rcpp::export]]    
std::vector<std::string> find_son(std::string key, std::vector<std::string> vec_a, std::vector<std::string> vec_b) {
    std::vector<std::string> res;
    for (int i = 0; i < vec_a.size(); i++) {
	if (key ==  vec_a[i]) {
	    res.push_back(vec_b[i]);
	}
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
std::vector<std::string> extract_sons (std::vector<std::string> host, std::vector<std::string> guest, std::string target) {
    std::vector<std::string> res;
    std::stack<std::string> sack;
    sack.push(target);
    while (!sack.empty()) {
	std::string current_target = sack.top();
	sack.pop();
	std::vector<std::string> sons = find_son(current_target, guest, host);
	if (sons.size() != 0) {
	    for (std::string& i : sons) {
		sack.push(i);
		res.push_back(i);
	    }
	}
    }
    return runique(res);
}

// [[Rcpp::export]]
int d7_1 (std::vector<std::string> host, std::vector<std::string> guest) {
    std::vector<std::string> res = extract_sons(host, guest, "shiny gold");
    return res.size();
}

std::vector< std::pair<std::string, int> > find_sonv(std::string key, std::vector<std::string> vec_a, std::vector<std::string> vec_b, std::vector<int> vec_c) {
    std::vector< std::pair<std::string, int> > res;
    for (int i = 0; i < vec_a.size(); i++) {
	if (key ==  vec_a[i]) {
	    res.push_back(make_pair(vec_b[i], vec_c[i]));
	}
    }
    return res;
}

// // [[Rcpp::export]]
// void test() {
//     std::string key = "a";
//     std::vector<std::string> va = {"a", "b", "c"};
//     std::vector<std::string> vb = {"e", "f", "g"};
//     std::vector<int> vc = {1,2,3};
//     auto res = find_sonv(key, va, vb, vc);
//     std::cout << res[0].first << std::endl;
//     std::cout << res[0].second << std::endl;    
// }

struct record {
    std::string bag;
    int x; // No of parents
    int y; // No of this bag in a parent
};

// [[Rcpp::export]]
int d7_2(std::string key, std::vector<std::string> host, std::vector<std::string> guest, std::vector<int> value) {
    int total = -1; // don't count the input bag
    std::stack<struct record> sack;
    struct record r;
    r.bag = key;
    r.x = 1;
    r.y = 1;
    sack.push(r);
    while (!sack.empty()) {
	struct record current_target = sack.top();
	int n_parent = current_target.x * current_target.y;
	total += n_parent;	
	//std::cout << current_target.bag << std::endl;
	auto sons = find_sonv(current_target.bag, host, guest, value);
	sack.pop();
	for (auto& i : sons) {
	    if (i.first != "no other") {
		struct record newrecord;
		newrecord.bag = i.first;
		newrecord.x = n_parent;
		newrecord.y = i.second;
		sack.push(newrecord);
	    }
	}
    }
    return total;
}

/***R
require(purrr)
require(magrittr)

## I cheat because I don't want to process the data with C++

safe_extract <- function(input) {
    res <- stringr::str_extract(input, "[0-9]+")
    ifelse(is.na(res), 0, as.numeric(res))
}

extract <- function(line) {
    res <- strsplit(line, " contain ")[[1]]
    host <- gsub(" bags?", "", res[1])
    guests <- 
        strsplit(res[2], ", ")[[1]] %>% purrr::map_chr(~stringr::str_replace(stringr::str_extract(., "[a-z][a-z ]+[a-z]"), " bags?", ""))
    n <- 
        strsplit(res[2], ", ")[[1]] %>% purrr::map_dbl(safe_extract)
    return(data.frame(host, guests, n))
}

prepro <- function(input) {
    x <- input %>% strsplit("\n", TRUE) 
    x[[1]] %>% purrr::map_dfr(extract)
}

tempdata <- "light red bags contain 1 bright white bag, 2 muted yellow bags.
dark orange bags contain 3 bright white bags, 4 muted yellow bags.
bright white bags contain 1 shiny gold bag.
muted yellow bags contain 2 shiny gold bags, 9 faded blue bags.
shiny gold bags contain 1 dark olive bag, 2 vibrant plum bags.
dark olive bags contain 3 faded blue bags, 4 dotted black bags.
vibrant plum bags contain 5 faded blue bags, 6 dotted black bags.
faded blue bags contain no other bags.
dotted black bags contain no other bags."

tempdata %>% prepro() -> testdata

stopifnot(d7_1(testdata$host, testdata$guest) == 4)

## part 2

stopifnot(d7_2("shiny gold", testdata$host, testdata$guest, testdata$n) == 32)

tempdata2 <- "shiny gold bags contain 2 dark red bags.
dark red bags contain 2 dark orange bags.
dark orange bags contain 2 dark yellow bags.
dark yellow bags contain 2 dark green bags.
dark green bags contain 2 dark blue bags.
dark blue bags contain 2 dark violet bags.
dark violet bags contain no other bags.
"

tempdata2 %>% prepro() -> testdata2

stopifnot(d7_2("shiny gold", testdata2$host, testdata2$guest, testdata2$n) == 126)

## real deal

input7 <- readLines(here::here("data/input7.txt")) %>% purrr::map_dfr(extract)

d7_1(input7$host, input7$guest)

d7_2("shiny gold", input7$host, input7$guest, input7$n)

 */
