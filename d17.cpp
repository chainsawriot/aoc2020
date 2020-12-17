#include <Rcpp.h>
#include <regex>
#include <algorithm>
#include <cmath>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


// Anticipate the next question will be 4d;
typedef std::tuple<int, int, int> cord;
typedef std::map<cord, char> cordbag;

cordbag parse_state (CharacterMatrix input){
    cordbag res;
    int nrow = input.nrow();
    int ncol = input.ncol();
    for (int y = 0; y < ncol; y++) {
	for (int x = 0; x < nrow; x++) {
	    cord ijcord = std::make_tuple(x, y, 0);
	    res[ijcord] = input(x, y)[0];
	}
    }
    return res;
}

void pp_cord (cord input) {
    std::cout << std::get<0>(input);
    std::cout << ",";
    std::cout << std::get<1>(input);
    std::cout << ",";
    std::cout << std::get<2>(input);
    std::cout << std::endl;
}

void pp_cordbag (cordbag input) {
    std::cout << "Item:" << input.size() << std::endl;
    for (auto& data: input) {
	pp_cord(data.first);
	std::cout << ":";
	std::cout << data.second;
	std::cout << std::endl;
    }
}

// int dist (cord x, cord y) {
//     int d = 0;
//     int v1 = std::get<0>(x);
//     int v2 = std::get<0>(y);
//     d += v2 - v1;
//     v1 = std::get<1>(x);
//     v2 = std::get<1>(y);
//     d += v2 - v1;
//     v1 = std::get<2>(x);
//     v2 = std::get<2>(y);
//     d += v2 - v1;
//     return d;
// }

std::vector<cord> get_neig (cord input) {
    std::vector<cord> res;
    std::vector<int> prox = {-1, 0, 1};
    for (int& x: prox) {
	for (int& y: prox) {
	        for (int& z: prox) {
		    cord newcord = std::make_tuple(std::get<0>(input) + x, std::get<1>(input) + y, std::get<2>(input) + z);
		    // int dist_pt = dist(newcord, input);
		    if (newcord != input) {
			res.push_back(newcord);
		    }
		}
	}
    }
    return res;
}

// [[Rcpp::export]]
void test_parse(CharacterMatrix input) {
    auto x = parse_state(input);
    pp_cordbag(x);
}

// query active neigbors!
// Notes to myself:
// A previous version did not use a reference, i.e. pass cordb by value;
// It is arse slow!
// Ditto for iter_single below!
int query_active(int x, int y, int z, cordbag& cordb) {
    int n_active = 0;
    cord input_cord = std::make_tuple(x, y, z);
    auto q = get_neig(input_cord);
    for (cord& g: q) {
	if (cordb[g] == '#' && g != input_cord) {
	    n_active++;
	}
    }
    return n_active;
}


// [[Rcpp::export]]
int testq(int x, int y, int z, CharacterMatrix input) {
    auto cordb = parse_state(input);
    return query_active(x, y, z, cordb);
}

char iter_single(int x, int y, int z, cordbag& cordb) {
    bool current_active = false;
    cord g = std::make_tuple(x, y, z);
    if (cordb.find(g) != cordb.end()) {
	if (cordb[g] == '#') {
	    current_active = true;
	}
    }
    int n_active = query_active(x, y, z, cordb);
    if (current_active && (n_active == 2 || n_active == 3)) {
	return '#';
    }
    if (!current_active && n_active == 3) {
	return '#';
    }
    return '.';
}

cordbag iter(cordbag cordb, int max_depth = 10) {
    cordbag newbg = cordb;
    for (int x = -max_depth; x <= max_depth; x++) {
	for (int y = -max_depth; y <= max_depth; y++) {
	    for (int z = -max_depth; z <= max_depth; z++) {
		newbg[std::make_tuple(x, y, z)] = iter_single(x, y, z, cordb);
	    }
	}
    }
    return newbg;
}

void pp_bg (cordbag cordb, int max_depth = 4) {
    for (int z = -max_depth; z <= max_depth; z ++) {
	std::cout << "Z: " << z << std::endl;
	for (int x = -max_depth; x <= max_depth; x++) {
	    for (int y = -max_depth; y <= max_depth; y++) {
		cord g = std::make_tuple(x, y, z);
		std::cout << cordb[g];
	    }
	    std::cout << std::endl;
	}
	std::cout << std::endl;
    }
}

unsigned int count_bg (cordbag input) {
    unsigned int total = 0;
    for (auto& data: input) {
	if (data.second == '#') {
	    total++;
	}
    }
    return total;
}

// [[Rcpp::export]]
char testis(int x, int y, int z, CharacterMatrix input) {
    auto cordb = parse_state(input);
    return iter_single(x, y, z, cordb);
}


// [[Rcpp::export]]
void testiter(CharacterMatrix input, int round = 1) {
    auto cordb = parse_state(input);
    int max_depth = input.ncol() + round + 1;
    for (int i = 0; i < round; i++) {
	cordb = iter(cordb, max_depth);
    }
    pp_bg(cordb);
}

// [[Rcpp::export]]
unsigned int d17_1(CharacterMatrix input, int round = 6) {
    auto cordb = parse_state(input);
    int max_depth = input.ncol() + round + 1;
    for (int i = 0; i < round; i++) {
	cordb = iter(cordb, max_depth);
    }
    return count_bg(cordb);
}

// As expected, part 2 is really 4d.
typedef std::tuple<int, int, int, int> cord4d;
typedef std::map<cord4d, char> cordbag4d;

cordbag4d parse_state4d (CharacterMatrix input){
    cordbag4d res;
    int nrow = input.nrow();
    int ncol = input.ncol();
    for (int y = 0; y < ncol; y++) {
	for (int x = 0; x < nrow; x++) {
	    cord4d ijcord = std::make_tuple(x, y, 0, 0);
	    res[ijcord] = input(x, y)[0];
	}
    }
    return res;
}

std::vector<cord4d> get_neig4d (cord4d input) {
    std::vector<cord4d> res;
    std::vector<int> prox = {-1, 0, 1};
    for (int& x: prox) {
	for (int& y: prox) {
	        for (int& z: prox) {
		    for (int& w: prox) {
			cord4d newcord = std::make_tuple(std::get<0>(input) + x, std::get<1>(input) + y, std::get<2>(input) + z, std::get<3>(input) + w);
			if (newcord != input) {
			    res.push_back(newcord);
			}
		    }
		}
	}
    }
    return res;
}

int query_active4d(int x, int y, int z, int w, cordbag4d& cordb) {
    int n_active = 0;
    cord4d input_cord = std::make_tuple(x, y, z, w);
    auto q = get_neig4d(input_cord);
    for (cord4d& g: q) {
	if (cordb[g] == '#' && g != input_cord) {
	    n_active++;
	}
    }
    return n_active;
}



char iter_single4d(int x, int y, int z, int w, cordbag4d& cordb) {
    bool current_active = false;
    cord4d g = std::make_tuple(x, y, z, w);
    if (cordb[g] == '#') {
	current_active = true;
    }
    int n_active = query_active4d(x, y, z, w, cordb);
    if (current_active && (n_active == 2 || n_active == 3)) {
	return '#';
    }
    if (!current_active && n_active == 3) {
	return '#';
    }
    return '.';
}

cordbag4d iter4d(cordbag4d cordb, int max_depth = 10) {
    cordbag4d newbg = cordb;
    for (int x = -max_depth; x <= max_depth; x++) {
	for (int y = -max_depth; y <= max_depth; y++) {
	    for (int z = -max_depth; z <= max_depth; z++) {
		for (int w = -max_depth; w <= max_depth; w++) {
		    newbg[std::make_tuple(x, y, z, w)] = iter_single4d(x, y, z, w, cordb);
		}
	    }
	}
    }
    return newbg;
}

unsigned int count_bg4d (cordbag4d input) {
    unsigned int total = 0;
    for (auto& data: input) {
	if (data.second == '#') {
	    total++;
	}
    }
    return total;
}

// [[Rcpp::export]]
unsigned int d17_2(CharacterMatrix input, int round = 6) {
    auto cordb = parse_state4d(input);
    int max_depth = input.ncol() + round + 1;
    for (int i = 0; i < round; i++) {
	cordb = iter4d(cordb, max_depth);
    }
    return count_bg4d(cordb);
}

// [[Rcpp::export]]
int test_getn4d () {
    auto x = get_neig4d(std::make_tuple(0,0,0,0));
    return x.size();
}

void pp_bg4g (cordbag4d cordb, int max_depth = 4) {
    for (int w = -max_depth; w <= max_depth; w ++) {
	for (int z = -max_depth; z <= max_depth; z ++) {
	    std::cout << "Z: " << z << " W:" << w << std::endl;
	    for (int x = -max_depth; x <= max_depth; x++) {
		for (int y = -max_depth; y <= max_depth; y++) {
		    cord4d g = std::make_tuple(x, y, z, w);
		    std::cout << cordb[g];
		}
		std::cout << std::endl;
	    }
	    std::cout << std::endl;
	}
    }
}

// [[Rcpp::export]]
void testiter4d(CharacterMatrix input) {
    auto cordb = parse_state4d(input);
    cordb = iter4d(cordb, 10);
    pp_bg4g(cordb);
}

// [[Rcpp::export]]
void testparse4d(CharacterMatrix input) {
    auto cordb = parse_state4d(input);
    pp_bg4g(cordb);
}

/***R
testdata <- ".#...####"

testmatrix <- matrix(strsplit(testdata, "")[[1]], nrow = 3, ncol = 3, byrow = TRUE)

## test_parse(testmatrix)

stopifnot(testq(0,0,0, testmatrix) == 1) ## at (0,1,0)
stopifnot(testq(0,1,0, testmatrix) == 1) ## at (1,2,0)
stopifnot(testq(2,1,0, testmatrix) == 3) ## at (2,0,0), (2,2,0), (1,2,0)
stopifnot(testq(2,2,0, testmatrix) == 2) ## at (1,2,0), (2,1,0)

## The test data is not on the same grid!
## after G1: it should be something like this
##    -1/0/1/2/3
## -1 .....
## 0 .....
## 1 .#.#.
## 2 ..##.
## 3 ..#..

## The example clopped only the active part!

stopifnot(testis(0,0,0, testmatrix) == ".")
stopifnot(testis(1,0,0, testmatrix) == "#")
stopifnot(testis(1,2,0, testmatrix) == "#")
stopifnot(testis(2,1,0, testmatrix) == "#")
stopifnot(testis(3,1,0, testmatrix) == "#")
stopifnot(testis(3,2,0, testmatrix) == ".")

##testiter(testmatrix, 3)
## Pass by reference, it's way better!

stopifnot(d17_1(testmatrix, 6) == 112)
stopifnot(d17_2(testmatrix, 6) == 848)

input17 <- readLines(here::here("data/input17.txt"))
inputmatrix <- matrix(strsplit(paste0(input17, collapse = ""), "", TRUE)[[1]], nrow = 8, ncol = 8, byrow = TRUE)

d17_1(inputmatrix, 6)

d17_2(inputmatrix, 6) ## still quite slow, but bearble.
 */
