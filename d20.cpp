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

struct edge_match {
    edge e1;
    edge e2;
    bool flip;
};

typedef std::map<int, std::vector<struct edge_match>> match_book;

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
    edges.push_back(input.back());
    std::string e1 = "";
    std::string e2 = "";
    std::vector<std::string> content;
    for (int i = 1; i < input.size(); i ++) {
	content.push_back(input[i]);
	int width = input[i].length();
	e1 += input[i][0];
	e2 += input[i][width - 1];
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

// bool match_edges (std::string& e1, std::string& e2) {
//     if (e1 == e2) {
// 	return true;
//     }
//     std::string ee1 = e1;
//     std::reverse(ee1.begin(), ee1.end());
//     if (ee1 == e2) {
// 	return true;
//     }
//     return false;
// }

// [[Rcpp::export]]
int match_edges (std::string& e1, std::string& e2) {
    // 1 = direct match; 2 = flip match; 0 = not match
    if (e1 == e2) {
	return 1;
    }
    std::string ee1 = e1;
    std::reverse(ee1.begin(), ee1.end());
    if (ee1 == e2) {
	return 2;
    }
    return 0;
}


// void test_m(std::vector<std::string> input1, std::vector<std::string> input2) {
//     auto puz1 = parse_puzzle(input1);
//     auto puz2 = parse_puzzle(input2);
//     pp_puzzle(puz1);
//     pp_puzzle(puz2);
//     for (int i = 0; i < puz1.edges.size(); i++) {
// 	for (int j = 0; j < puz2.edges.size(); j++) {
// 	    std::cout << i << "/" << j << std::endl;
// 	    if (match_edges(puz1.edges[i], puz2.edges[j])) {
// 		std::cout << "Match!" << std::endl;
// 	    }
// 	}
//     }
// }

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

// edge_count create_empty_count (puzzlebook input) {
//     edge_count res;
//     for (auto& data: input) {
// 	for (int i = 0; i < data.second.edges.size(); i++) {
// 	    edge ed = std::make_pair(data.first, i);
// 	    res[ed] = 0;
// 	}
//     }
//     return res;
// }

match_book create_empty_mb (puzzlebook input) {
    match_book res;
    for (auto& data: input) {
	std::vector<struct edge_match> x = {};
	res[data.first] = x;
    }
    return res;
}


// edge_count count_edge(puzzlebook input) {
//     edge_count x = create_empty_count(input);
//     for (auto& puz: input) {
// 	for (auto& puz2: input) {
// 	    for (int i = 0; i< puz.second.edges.size(); i++) {
// 		edge ed = std::make_pair(puz.second.id, i);
// 		for (int j = 0; j< puz2.second.edges.size(); j++) {
// 		    if (puz.first != puz2.first) {
// 			if (match_edges(puz.second.edges[i], puz2.second.edges[j])) {
// 			    x[ed] += 1;
// 			}
// 		    }
// 		}
// 	    }
// 	}
//     }
//     return x;
// }

match_book count_edge(puzzlebook input) {
    match_book x = create_empty_mb(input);
    for (auto& puz: input) {
	for (auto& puz2: input) {
	    for (int i = 0; i< puz.second.edges.size(); i++) {
		edge e1 = std::make_pair(puz.second.id, i);
		for (int j = 0; j< puz2.second.edges.size(); j++) {
		    edge e2 = std::make_pair(puz2.second.id, j);
		    if (puz.first != puz2.first) {
			int m = match_edges(puz.second.edges[i], puz2.second.edges[j]);
			struct edge_match em;			
			em.e1 = e1;
			em.e2 = e2;
			if (m == 1) {
			    em.flip = false;
			}
			if (m == 2) {
			    em.flip = true;
			}
			if (m > 0) {
			    x[puz.first].push_back(em);
			}
		    }
		}
	    }
	}
    }
    return x;
}

void pp_mb(match_book input) {
    for (auto& data: input) {
	std::cout << "Puzzle: " << data.first << std::endl;
	for (struct edge_match& m: data.second) {
	    std::cout << m.e1.first << "/" << m.e1.second << "==" << m.e2.first << "/" << m.e2.second << std::endl;
	    if (m.flip) {
		std::cout << "(Flipped)" << std::endl;
	    }

	}
    }
}


// [[Rcpp::export]]
long long int d20_1(std::vector<std::string> input) {
    auto puzbook = parse_all(input);
    auto count = count_edge(puzbook);
    std::vector<int> corners;
    for (auto& puz: puzbook) {
	if (count[puz.first].size() == 2) {
	    corners.push_back(puz.first);
	}
	// int tol = 0;
	// for (int i = 0; i < puz.second.edges.size(); i++) {
	//     edge ed = std::make_pair(puz.second.id, i);
	//     if (count[ed] == 0) {
	// 	tol += 1;
	//     }
	// }
	// if (tol == 2) {
	//     corners.push_back(puz.first);
	// }
    }
    long long int res = 1;
    for (int& corn: corners) {
	res = res * corn;
    }
    return res;
}

// [[Rcpp::export]]
void testpp (std::vector<std::string> input) {
    auto puzbook = parse_all(input);
    auto count = count_edge(puzbook);
    pp_mb(count);
}
