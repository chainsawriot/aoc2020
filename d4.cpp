#include <Rcpp.h>
#include <regex>

using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

std::vector<std::string> normalize(CharacterVector input) {
    std::vector<std::string> sack;
    std::string record;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	if (line != "") {
	    record += line;
	    record += " ";
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


bool key_find(std::string key, std::vector<std::string> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

bool keys_find(std::vector<std::string> all_keys, std::vector<std::string> keys) {
    for (std::vector<std::string>::const_iterator i = all_keys.begin(); i != all_keys.end(); ++i) {
	if (!key_find(*i, keys)) {
	    return false;
	}
    }
    return true;
}

bool validate1(std::string input, std::vector<std::string> all_keys) {
    // explode by space
    std::vector<std::string> items = explode(input, "[^ ]+");
    std::vector<std::string> keys;
    for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i) {
	std::vector<std::string> keypair = explode(*i, "[^:]+");
	keys.push_back(keypair[0]);
    }
    return keys_find(all_keys, keys);
}

// [[Rcpp::export]]
int d4_1 (CharacterVector input) {
    int count = 0;
    // "cid" is optional
    std::vector<std::string> all_keys = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
    std::vector<std::string> items = normalize(input);
    for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i) {
	if (validate1(*i, all_keys)) {
	    count += 1;
	}
    }
    return count;
}

std::map<std::string, std::string> item_to_map (std::string item) {
    std::vector<std::string> items = explode(item, "[^ ]+");
    std::map<std::string, std::string> item_map;
    for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i) {
	std::vector<std::string> keypair = explode(*i, "[^:]+");
	item_map[keypair[0]] = keypair[1];
    }
    return item_map;
}

bool check_yr (std::string yr, int min_yr, int max_yr) {
    if (yr.length() != 4) {
	return false;
    }
    try{
	int yr_int = std::stoi(yr);
	if (yr_int < min_yr || yr_int > max_yr) {
	    return false;
	}
    } catch(...) {
	return false;
    }
    return true;
}

bool check_hex(std::string hex) {
    if (hex.length() != 7) {
	return false;
    }
    if (hex[0] != '#') {
	return false;
    }
    if (explode(hex.substr(1, 6), "[a-f0-9]").size() != 6) {
	return false;
    }
    return true;
}

bool check_pass(std::string hex) {
    if (hex.length() != 9) {
	return false;
    }
    if (explode(hex, "[0-9]").size() != 9) {
	return false;
    }
    return true;
}

bool check_height (std::string hex) {
    std::vector<std::string> unit = explode(hex, "cm|in");
    if (unit.size() == 0) {
	return false;
    }
    std::vector<std::string> value = explode(hex, "[0-9]+");
    if (value.size() == 0) {
	return false;
    }
    try {
	//std::cout << value[0] << std::endl;
	int ht_int = std::stoi(value[0]);
	if (unit[0] == "in" && ht_int >= 59 && ht_int <= 76) {
	    return true;
	}
	if (unit[0] == "cm" && ht_int >= 150 && ht_int <= 193) {
	    return true;
	}
    } catch(...) {
	return false;
    }
    return false;
}


bool validate_map (std::map<std::string, std::string> itemmap) {
    std::vector<std::string> keys;
    for (auto const& x : itemmap) {
	keys.push_back(x.first);
    }
    std::vector<std::string> all_keys = {"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
    if (!keys_find(all_keys, keys)) {
     	return false;
    }
    if (!check_yr(itemmap["byr"], 1920, 2002)) {
	return false;
    }
    if (!check_yr(itemmap["iyr"], 2010, 2020)) {
	return false;
    }
    if (!check_yr(itemmap["eyr"], 2020, 2030)) {
	return false;
    }
    if (!check_height(itemmap["hgt"])) {
	return false;
    }
    if (!check_hex(itemmap["hcl"])) {
	return false;
    }
    std::vector<std::string> ecl_valid_values = {"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};
    if (!key_find(itemmap["ecl"], ecl_valid_values)) {
	return false;
    }
    if (!check_pass(itemmap["pid"])) {
	return false;
    }
    return true;
}

bool validate2(std::string input) {
    std::map<std::string, std::string> inputmap = item_to_map(input);
    return validate_map(inputmap);
}


// [[Rcpp::export]]
int d4_2 (CharacterVector input) {
    int count = 0;
    std::vector<std::string> items = normalize(input);
    for (std::vector<std::string>::const_iterator i = items.begin(); i != items.end(); ++i) {
	if (validate2(*i)) {
	    count += 1;
	}
    }
    return count;
}

/***R
input4 <- c(readLines(here::here("data/input4.txt")), "")

testdata <- "ecl:gry pid:860033327 eyr:2020 hcl:#fffffd
byr:1937 iyr:2017 cid:147 hgt:183cm

iyr:2013 ecl:amb cid:350 eyr:2023 pid:028048884
hcl:#cfa07d byr:1929

hcl:#ae17e1 iyr:2013
eyr:2024
ecl:brn pid:760753108 byr:1931
hgt:179cm

hcl:#cfa07d eyr:2025 pid:166559648
iyr:2011 ecl:brn hgt:59in

"

stopifnot(d4_1(strsplit(testdata, "\n", TRUE)[[1]]) == 2)

d4_1(input4)

testdata2 <- "eyr:1972 cid:100
hcl:#18171d ecl:amb hgt:170 pid:186cm iyr:2018 byr:1926

iyr:2019
hcl:#602927 eyr:1967 hgt:170cm
ecl:grn pid:012533040 byr:1946

hcl:dab227 iyr:2012
ecl:brn hgt:182cm pid:021572410 eyr:2020 byr:1992 cid:277

hgt:59cm ecl:zzz
eyr:2038 hcl:74454a iyr:2023
pid:3556412378 byr:2007

"

stopifnot(d4_2(strsplit(testdata2, "\n", TRUE)[[1]]) == 0)

testdata3 <- "pid:087499704 hgt:74in ecl:grn iyr:2012 eyr:2030 byr:1980
hcl:#623a2f

eyr:2029 ecl:blu cid:129 byr:1989
iyr:2014 pid:896056539 hcl:#a97842 hgt:165cm

hcl:#888785
hgt:164cm byr:2001 iyr:2015 cid:88
pid:545766238 ecl:hzl
eyr:2022

iyr:2010 hgt:158cm hcl:#b6652a ecl:blu byr:1944 eyr:2021 pid:093154719

"

stopifnot(d4_2(strsplit(testdata3, "\n", TRUE)[[1]]) == 4)

d4_2(input4)
 */
