#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

struct puzzle {
    int id;
    std::vector<std::string> content ;
    std::vector<std::string> edges;
};

typedef std::map<int, struct puzzle> puzzlebook;
typedef std::pair<int, int> edge;
typedef std::map<edge, unsigned int> edge_count;

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

void pp_puzzle (struct puzzle input) {
    std::cout << "Puzzle ID: " << input.id << std::endl;
    for (std::string& line: input.content) {
	std::cout << line << std::endl;
    }
    std::cout << "Top edge: " << input.edges[0] << std::endl;
    std::cout << "Bottom edge: " << input.edges[1] << std::endl;
    std::cout << "Left edge: " << input.edges[2] << std::endl;
    std::cout << "Right edge: " << input.edges[3] << std::endl;
}

struct puzzle parse_puzzle (std::vector<std::string> input) {
    struct puzzle output;
    auto line1 = explode(input[0], "[0-9]+");
    output.id = std::stoi(line1[0]);
    std::vector<std::string> edges;
    edges.push_back(input[1]);
    edges.push_back(input[10]);
    std::string e1 = "";
    std::string e2 = "";
    std::vector<std::string> content;
    for (int i = 1; i < input.size(); i ++) {
	content.push_back(input[i]);
	e1 += input[i][0];
	e2 += input[i][9];
    }
    edges.push_back(e1);
    edges.push_back(e2);
    output.content = content;
    output.edges = edges;
    return output;
}

// [[Rcpp::export]]
void test_parse(std::vector<std::string> input) {
    auto puz = parse_puzzle(input);
    pp_puzzle(puz);
}

// [[Rcpp::export]]
bool match_edges (std::string& e1, std::string& e2) {
    if (e1 == e2) {
	return true;
    }
    std::string ee1 = e1;
    std::reverse(ee1.begin(), ee1.end());
    if (ee1 == e2) {
	return true;
    }
    return false;
}

// [[Rcpp::export]]
void test_m(std::vector<std::string> input1, std::vector<std::string> input2) {
    auto puz1 = parse_puzzle(input1);
    auto puz2 = parse_puzzle(input2);
    pp_puzzle(puz1);
    pp_puzzle(puz2);
    for (int i = 0; i < puz1.edges.size(); i++) {
	for (int j = 0; j < puz2.edges.size(); j++) {
	    std::cout << i << "/" << j << std::endl;
	    if (match_edges(puz1.edges[i], puz2.edges[j])) {
		std::cout << "Match!" << std::endl;
	    }
	}
    }
}

puzzlebook parse_all(std::vector<std::string> input) {
    std::vector<std::string> x;
    puzzlebook output;
    for (std::string& line: input) {
	if (line != "") {
	    x.push_back(line);
	} else {
	    struct puzzle puz = parse_puzzle(x);
	    output[puz.id] = puz;
	    x = {};
	}
    }
    return output;
}

// [[Rcpp::export]]
void test_parse_all(std::vector<std::string> input) {
    auto puzbook = parse_all(input);
    for (auto& data: puzbook) {
	pp_puzzle(data.second);
    }
}

edge_count create_empty_count (puzzlebook input) {
    edge_count res;
    for (auto& data: input) {
	for (int i = 0; i < data.second.edges.size(); i++) {
	    edge ed = std::make_pair(data.first, i);
	    res[ed] = 0;
	}
    }
    return res;
}

edge_count count_edge(puzzlebook input) {
    edge_count x = create_empty_count(input);
    for (auto& puz: input) {
	for (auto& puz2: input) {
	    for (int i = 0; i< puz.second.edges.size(); i++) {
		edge ed = std::make_pair(puz.second.id, i);
		for (int j = 0; j< puz2.second.edges.size(); j++) {
		    if (puz.first != puz2.first) {
			if (match_edges(puz.second.edges[i], puz2.second.edges[j])) {
			    x[ed] += 1;
			}
		    }
		}
	    }
	}
    }
    return x;
}

// [[Rcpp::export]]
long long int d20_1(std::vector<std::string> input) {
    auto puzbook = parse_all(input);
    auto count = count_edge(puzbook);
    std::vector<int> corners;
    for (auto& puz: puzbook) {
	int tol = 0;
	for (int i = 0; i < puz.second.edges.size(); i++) {
	    edge ed = std::make_pair(puz.second.id, i);
	    if (count[ed] == 0) {
		tol += 1;
	    }
	}
	if (tol == 2) {
	    corners.push_back(puz.first);
	}
    }
    long long int res = 1;
    for (int& corn: corners) {
	res = res * corn;
    }
    return res;
}
