#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

typedef std::vector<std::vector<std::string>> cb;
typedef std::map<std::string, cb> allergencb;
typedef std::map<std::string, std::vector<std::string>> posalle;

allergencb parse(List ingredients, List contains) {
    allergencb x;
    for (int i = 0; i < ingredients.size(); i++) {
	std::vector<std::string> allergens = contains[i];
	std::vector<std::string> recipe = ingredients[i];
	for (std::string& alle: allergens) {
	    std::sort(recipe.begin(), recipe.end());
	    x[alle].push_back(recipe);
	}
    }
    return x;
}

// [[Rcpp::export]]
std::vector<std::string> cppintersect(std::vector<std::string>& a, std::vector<std::string>& b) {
    // a and b need to be sorted!
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    std::vector<std::string> c = {};
    std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
    return c;
}

std::vector<std::string> cppunion(std::vector<std::string>& a, std::vector<std::string>& b) {
    // a and b need to be sorted!
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    std::vector<std::string> c = {};
    std::set_union(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
    return c;
}


std::vector<std::string> mass_intersect(cb& input) {
    std::vector<std::string> c;
    for (int i = 0; i < input.size(); i++) {
	if (i == 0) {
	    c = input[i];
	} else {
	    c = cppintersect(c, input[i]);
	}
    }
    return c;
}

std::vector<std::string> mass_union(cb& input) {
    std::vector<std::string> c;
    for (int i = 0; i < input.size(); i++) {
	if (i == 0) {
	    c = input[i];
	} else {
	    c = cppunion(c, input[i]);
	}
    }
    return c;
}



void pp_svs(std::vector<std::string> x) {
    for (std::string& c: x) {
	std::cout << c << ", ";
    }
    std::cout << std::endl;
}

std::vector<std::string> cppsetdiff(std::vector<std::string>& a, std::vector<std::string>& b) {
    // a and b need to be sorted!
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    std::vector<std::string> c = {};
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(c, c.begin()));
    return c;
}

bool key_find(std::string key, std::vector<std::string> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}


// [[Rcpp::export]]
long long int d21_1(List ingredients, List contains) {
    auto x = parse(ingredients, contains);
    // std::cout << x.size() << std::endl;
    cb cb1;
    cb cb2;
    for (int i = 0; i < ingredients.size(); i++) {
	cb2.push_back(ingredients[i]);
    }
    posalle posa;
    for (auto& c: x) {	
	std::vector<std::string> res = mass_intersect(x[c.first]);
	posa[c.first] = res;
	cb1.push_back(res);
    }
    // for (auto& c: posa) {
    // 	std::cout << c.first << std::endl;
    // 	pp_svs(c.second);
    // }
    std::vector<std::string> all_ingredients = mass_union(cb2);
    std::vector<std::string> all_allergens = mass_union(cb1);
    std::vector<std::string> safe_ingredients = cppsetdiff(all_ingredients, all_allergens);
    long long total = 0;
    for (std::vector<std::string>& cookbook: cb2) {
	// pp_svs(cookbook);
	for (std::string& i: cookbook) {
	    if (key_find(i, safe_ingredients)) {
		// std::cout << i << std::endl;
		total += 1;
	    }
	}
    }
    return total;
}

bool check_empty(posalle& posa) {
    for (auto& data: posa) {
	if (data.second.size() != 0) {
	    return false;
	}
    }
    return true;
}

std::map<std::string, std::string> single_out(posalle& posa) {
    std::map<std::string, std::string> output;
    while(!check_empty(posa)) { 
	for (auto& data: posa) {
	    if (data.second.size() == 1) {
		std::string foreignname = data.second[0];
		output[data.first] = foreignname;
		for (auto& data2: posa) {
		    data2.second.erase(std::remove(data2.second.begin(), data2.second.end(), foreignname), data2.second.end());
		}
	    }
	}
    }
    return output;
}

// [[Rcpp::export]]
std::string d21_2(List ingredients, List contains) {
    auto x = parse(ingredients, contains);
    // std::cout << x.size() << std::endl;
    cb cb1;
    cb cb2;
    for (int i = 0; i < ingredients.size(); i++) {
	cb2.push_back(ingredients[i]);
    }
    posalle posa;
    for (auto& c: x) {	
	std::vector<std::string> res = mass_intersect(x[c.first]);
	posa[c.first] = res;
	cb1.push_back(res);
	// pp_svs(res);
    }
    std::string output = "";
    auto res = single_out(posa);
    for (auto& data: res) {
	output += data.second;
	output += ",";
    }
    return output.substr(0, output.size()-1);
}

/***R
require(magrittr)
require(stringr)

splitx <- function(x, y = "\n") {
    strsplit(x, y, TRUE)[[1]]
}

testdata <- "mxmxvkd kfcds sqjhc nhms (contains dairy, fish)
trh fvjkl sbzzf mxmxvkd (contains dairy)
sqjhc fvjkl (contains soy)
sqjhc mxmxvkd sbzzf (contains fish)" %>% splitx

## Don't wanna do any regex in C++ anymore, after day 19
test_contains <- str_extract(testdata, "\\([a-z ,]+\\)") %>% str_replace_all("\\(contains |\\)", "") %>% str_split(", ")
test_ingredients <- str_extract(testdata, "[a-z ]+") %>% str_trim %>% str_split(" ")


stopifnot(d21_1(test_ingredients, test_contains) == 5)
stopifnot(d21_2(test_ingredients, test_contains) == "mxmxvkd,sqjhc,fvjkl")

input21 <- readLines(here::here("data/input21.txt"))
contains <- str_extract(input21, "\\([a-z ,]+\\)") %>% str_replace_all("\\(contains |\\)", "") %>% str_split(", ")
ingredients <- str_extract(input21, "[a-z ]+") %>% str_trim %>% str_split(" ")

d21_1(ingredients, contains)

d21_2(ingredients, contains)
 */
