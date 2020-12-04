#include <Rcpp.h>
#include <regex>

using namespace Rcpp;

// [[Rcpp::plugins("cpp11")]]

char go_loc(std::string &row, int loc) {
    if (loc >= row.length()) {
	loc = loc % row.length();
    }
    return row[loc];
}

// [[Rcpp::export]]
int d3_1(CharacterVector input) {
    int horizontal_pos = 0;
    int count = 0;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	if (go_loc(line, horizontal_pos) == '#') {
	    count += 1;
	}
	horizontal_pos += 3;
    }
    return count;
}

// We cannot use iterator anymore for part 2
// [[Rcpp::export]]
int trial(CharacterVector input, int horizontal_step = 3, int vertical_step = 1) {
    int horizontal_pos = 0;
    int vertical_pos = 0;
    int count = 0;
    int nrow = input.size();
    while (vertical_pos < nrow) {
	std::string line = as<std::string>(input[vertical_pos]);
	if (go_loc(line, horizontal_pos) == '#') {
	    count += 1;
	}
	horizontal_pos += horizontal_step;
	vertical_pos += vertical_step;
    }
    return count;
}

// [[Rcpp::export]]
unsigned int d3_2(CharacterVector input) {
    // regular int will overflow
    return trial(input, 1, 1) * trial(input, 3, 1) * trial(input, 5, 1) * trial(input, 7, 1) * trial(input, 1,2);
}

/***R
testdata <- "..##.......
#...#...#..
.#....#..#.
..#.#...#.#
.#...##..#.
..#.##.....
.#.#.#....#
.#........#
#.##...#...
#...##....#
.#..#...#.#"
stopifnot(d3_1(strsplit(testdata, "\n", TRUE)[[1]]) == 7)
stopifnot(trial(strsplit(testdata, "\n", TRUE)[[1]], 1, 1) == 2)
stopifnot(trial(strsplit(testdata, "\n", TRUE)[[1]], 3, 1) == 7)
stopifnot(trial(strsplit(testdata, "\n", TRUE)[[1]], 5, 1) == 3)
stopifnot(trial(strsplit(testdata, "\n", TRUE)[[1]], 7, 1) == 4)
stopifnot(trial(strsplit(testdata, "\n", TRUE)[[1]], 1, 2) == 2)

input3 <- readLines(here::here("data/input3.txt"))

d3_1(input3)

d3_2(input3)
 */
