#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

// [[Rcpp::export]]
NumericMatrix normalize(std::vector<std::string> input) {
    int max_col = input[0].length();
    int max_row = input.size();
    NumericMatrix m(max_row, max_col);
    for (int row = 0; row < max_row; row++) {
	for (int col = 0; col < max_col; col++) {
	    if (input[row][col] == '.') {
		m(row, col) = 2;
     	    }
	    if (input[row][col] == '#') {
		m(row, col) = 1;
     	    }
    	}
    }
    return m;
}

// [[Rcpp::export]]
std::vector<int> fetch_surrounding (NumericMatrix m, int row, int col, int prox = 1) {
    std::vector<int> res;
    for (int x = row - prox; x < row + (prox + 1); x++) {
	for (int y = col - 1; y < col + (prox + 1); y++) {
	    if (x >= 0 && y >= 0 && x < m.nrow() && y < m.ncol()) {
		if (!(x == row and y == col)) {
		    res.push_back(m(x, y));
		}
	    }
	}
    }
    return res;
}

int count(int val, std::vector<int> input) {
    int count = 0;
    for (const int& z: input) {
	if (z == val) {
	    count += 1;
	}
    }
    return count;
}

// [[Rcpp::export]]
int iteration_seat (NumericMatrix m, int row, int col) {
    int n_occ = count(1, fetch_surrounding(m, row, col));
    if (m(row, col) == 0) {
	if (n_occ == 0) {
	    return 1;
	}
    }
    if (m(row, col) == 1) {
	if (n_occ >= 4) {
	    return 0;
	}
    }
    return m(row, col);
}


// [[Rcpp::export]]
bool exact(NumericMatrix m1, NumericMatrix m2) {
    for (int row = 0; row < m1.nrow(); row++) {
	for (int col = 0; col < m1.ncol(); col++) {
	    if (m1(row, col) != m2(row, col)) {
		return false;
	    }
	}
    }
    return true;
}

int count_mat(int val, NumericMatrix m1) {
    int count = 0;
    for (int row = 0; row < m1.nrow(); row++) {
	for (int col = 0; col < m1.ncol(); col++) {
	    if (m1(row, col) == val) {
		count += 1;
	    }
	}
    }
    return count;
}


// [[Rcpp::export]]
NumericMatrix rcppstep (NumericMatrix m) {
    NumericMatrix m2 = clone(m);
    for (int row = 0; row < m2.nrow(); row++) {
	for (int col = 0; col < m2.ncol(); col++) {
	    m2(row, col) = iteration_seat(m, row, col);
	}
    }
    return m2;
} 


NumericMatrix iteration (NumericMatrix input) {
    NumericMatrix m1 = clone(input);
    while (true) {
	NumericMatrix m2 = rcppstep(m1);
	if (exact(m1, m2)) {
	    return m2;
	} else {
	    m1 = clone(m2);
	}
    }
    return NA_REAL;
}

// [[Rcpp::export]]
int d11_1 (std::vector<std::string> input) {
    NumericMatrix m1 = normalize(input);
    NumericMatrix m2 = iteration(m1);
    return count_mat(1, m2);
}

bool hitthewall (NumericMatrix m1, int row, int col) {
    return (row < 0 || row >= m1.nrow() || col < 0 || col >= m1.ncol()) ;
}

// [[Rcpp::export]]
std::vector<int> look(NumericMatrix m1, int row, int col, int h_dir = 1, int v_dir = 0) {
    std::vector<int> res;
    int cur_row = row;
    int cur_col = col;
    while (!hitthewall(m1, cur_row, cur_col)) {
	if (!(cur_row == row && cur_col == col)) {
	    res.push_back(m1(cur_row, cur_col));
	}
	cur_col += h_dir;
	cur_row += v_dir;
    }
    return res;
}

// [[Rcpp::export]]
bool detect_first(std::vector<int> input) {
    for (const int& x: input) {
	if (x == 1) {
	    return true;
	} else if (x == 0) {
	    return false;
	}
    }
    return false;
}

// [[Rcpp::export]]
int iteration_seat_look (NumericMatrix m, int row, int col) {
    int n_occ = 0;
    std::vector<int> dir1 = {-1, 1, 0, 0, -1, -1, 1, 1};
    std::vector<int> dir2 = {0, 0, -1, 1, -1, 1, -1, 1};
    for (int i = 0; i < dir1.size(); i++) {
	if (detect_first(look(m, row, col, dir1[i], dir2[i]))) {
	    n_occ += 1;
	}
    }
    if (m(row, col) == 0) {
	if (n_occ == 0) {
	    return 1;
	}
    }
    if (m(row, col) == 1) {
	if (n_occ >= 5) {
	    return 0;
	}
    }
    return m(row, col);
}

// [[Rcpp::export]]
NumericMatrix rcppstep2 (NumericMatrix m) {
    NumericMatrix m2 = clone(m);
    for (int row = 0; row < m2.nrow(); row++) {
	for (int col = 0; col < m2.ncol(); col++) {
	    m2(row, col) = iteration_seat_look(m, row, col);
	}
    }
    return m2;
} 

NumericMatrix iteration2 (NumericMatrix input) {
    NumericMatrix m1 = clone(input);
    while (true) {
	NumericMatrix m2 = rcppstep2(m1);
	if (exact(m1, m2)) {
	    return m2;
	} else {
	    m1 = clone(m2);
	}
    }
    return NA_REAL;
}

// [[Rcpp::export]]
int d11_2 (std::vector<std::string> input) {
    NumericMatrix m1 = normalize(input);
    NumericMatrix m2 = iteration2(m1);
    return count_mat(1, m2);
}

/***R

testdata <- "L.LL.LL.LL
LLLLLLL.LL
L.L.L..L..
LLLL.LL.LL
L.LL.LL.LL
L.LLLLL.LL
..L.L.....
LLLLLLLLLL
L.LLLLLL.L
L.LLLLL.LL"

stopifnot(d11_1(strsplit(testdata, "\n", TRUE)[[1]]) == 37)


testdata2 <- ".......#.
...#.....
.#.......
.........
..#L....#
....#....
.........
#........
...#....."

testdata3 <- ".##.##.
#.#.#.#
##...##
...L...
##...##
#.#.#.#
.##.##."

testdata4 <- ".............
.L.L.#.#.#.#.
............."

m1 <- normalize(strsplit(testdata2, "\n", TRUE)[[1]])
m2 <- normalize(strsplit(testdata3, "\n", TRUE)[[1]])
m3 <- normalize(strsplit(testdata4, "\n", TRUE)[[1]])

stopifnot(detect_first(look(m1, 4, 3, -1, 0)))
stopifnot(detect_first(look(m1, 4, 3, 1, 0)))
stopifnot(detect_first(look(m1, 4, 3, 0, -1)))
stopifnot(detect_first(look(m1, 4, 3, 0, 1)))
stopifnot(detect_first(look(m1, 4, 3, -1, -1)))
stopifnot(detect_first(look(m1, 4, 3, -1, 1)))
stopifnot(detect_first(look(m1, 4, 3, 1, -1)))
stopifnot(detect_first(look(m1, 4, 3, 1, 1)))

stopifnot(!detect_first(look(m2, 3, 3, -1, 0)))
stopifnot(!detect_first(look(m2, 3, 3, 1, 0)))
stopifnot(!detect_first(look(m2, 3, 3, 0, -1)))
stopifnot(!detect_first(look(m2, 3, 3, 0, 1)))
stopifnot(!detect_first(look(m2, 3, 3, -1, -1)))
stopifnot(!detect_first(look(m2, 3, 3, -1, 1)))
stopifnot(!detect_first(look(m2, 3, 3, 1, -1)))
stopifnot(!detect_first(look(m2, 3, 3, 1, 1)))

stopifnot(!detect_first(look(m3, 1, 0, 1, 0)))

stopifnot(d11_2(strsplit(testdata, "\n", TRUE)[[1]]) == 26)

input11 <- readLines(here::here("data/input11.txt"))

d11_1(input11)

d11_2(input11) ### slow, please wait.

 */
