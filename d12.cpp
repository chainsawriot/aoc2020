#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]


class Ship {
public:
    int dir; // direction in absolute bearing
    int x; // x position
    int y; // y position
    void process_cmd(char, int);
    std::pair<int, int> conv();
    void cmd(std::string);
    void progn(std::vector<std::string>);
    int get_dist();
};

// Ship::Ship (int ini_dir) {
//     x = 0;
//     y = 0;
//     dir = ini_dir;
// }


std::pair<int, int> Ship::conv() {
    std::pair<int, int> res;
    res.first = 0;
    res.second = 0;
    if (dir == 0) {
	res.second = 1;
    }
    if (dir == 90) {
	res.first = 1;
    }
    if (dir == 180) {
	res.second = -1;
    }
    if (dir == 270) {
	res.first = -1;
    }
    return res;
}


void Ship::process_cmd(char side, int val) {
    if (side == 'R') {
	dir = (dir + val) % 360;
    }
    if (side == 'L') {
	dir = (360 + dir - val) % 360;
    }
    if (side == 'N') {
	y = y + val;
    }
    if (side == 'S') {
	y = y - val;
    }
    if (side == 'W') {
	x = x - val;
    }
    if (side == 'E') {
	x = x + val;
    }
    if (side == 'F') {
	std::pair<int, int> res = conv();
	x = x + (val * res.first);
	y = y + (val * res.second);
    }
}

void Ship::cmd(std::string input) {
    char side = input[0];
    int val = std::stoi(input.substr(1, input.length()));
    process_cmd(side, val);
}

void Ship::progn(std::vector<std::string> input) {
    for (std::string& x: input) {
	cmd(x);
    }
}

int Ship::get_dist() {
    return std::abs(x) + std::abs(y);
}

// void test_move2 () {
//     Ship myship;
//     myship.cmd("F10");
//     myship.cmd("N3");
//     myship.cmd("F7");
//     myship.cmd("R90");
//     myship.cmd("F11");
//     std::cout << myship.x << std::endl;
//     std::cout << myship.y << std::endl;
//     std::cout << myship.dir << std::endl;
// }

// void test_move3 () {
//     Ship myship(90);
//     std::vector<std::string> prog = {"F10", "N3", "F7", "R90", "F11"};
//     myship.progn(prog);
//     std::cout << myship.get_dist() << std::endl;
// }

// [[Rcpp::export]]
int d12_1(std::vector<std::string> input) {
    Ship myship;
    myship.x = 0;
    myship.y = 0;
    myship.dir = 90;
    myship.progn(input);
    return myship.get_dist();
}

class Ship2: public Ship {
public:
    int wp_x = 0;
    int wp_y = 0;
    void process_cmd(char side, int val);
    void cmd(std::string input);
    void progn(std::vector<std::string>);
};

void Ship2::process_cmd(char side, int val) {
    if (side == 'R') {
	int times = val / 90;
	for (int i = 0; i < times; i++) {
	    int old_wp_x = wp_x;
	    int old_wp_y = wp_y;
	    wp_x = old_wp_y;
	    wp_y = - old_wp_x;
	}
   }
    if (side == 'L') {
	int times = val / 90;
	for (int i = 0; i < times; i++) {
	    int old_wp_x = wp_x;
	    int old_wp_y = wp_y;
	    wp_x = - old_wp_y;
	    wp_y = old_wp_x;
	}

    }
    if (side == 'N') {
	wp_y = wp_y + val;
    }
    if (side == 'S') {
	wp_y = wp_y - val;
    }
    if (side == 'W') {
	wp_x = wp_x - val;
    }
    if (side == 'E') {
	wp_x = wp_x + val;
    }
    if (side == 'F') {
	x = x + (val * wp_x);
	y = y + (val * wp_y);
    }
}

void Ship2::cmd(std::string input) {
    char side = input[0];
    int val = std::stoi(input.substr(1, input.length()));
    process_cmd(side, val);
}

void Ship2::progn(std::vector<std::string> input) {
    for (std::string& cmdx: input) {
	cmd(cmdx);
    }
}

// [[Rcpp::export]]
int d12_2(std::vector<std::string> input) {
    Ship2 myship;
    myship.x = 0;
    myship.y = 0;
    myship.dir = 0;
    myship.wp_x = 10;
    myship.wp_y = 1;
    myship.progn(input);
    return myship.get_dist();
}

/***R
testdata <- "F10
N3
F7
R90
F11"


stopifnot(d12_1(strsplit(testdata, "\n", TRUE)[[1]]) == 25)

stopifnot(d12_2(strsplit(testdata, "\n", TRUE)[[1]]) == 286)

input12 <- readLines(here::here("data/input12.txt"))

d12_1(input12)

d12_2(input12)

 */
