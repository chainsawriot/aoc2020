#include <Rcpp.h>
#include <regex>

using namespace Rcpp;

std::pair<int, int> forward (std::pair<int, int> input) {
    input.second = floor((input.first + input.second) / 2.0);
    return input;
}

std::pair<int, int> backward (std::pair<int, int> input) {
    input.first = ceil((input.first + input.second) / 2.0);
    return input;
}

void forward_test(int a, int b) {
    std::pair<int, int> x;
    x.first = a;
    x.second = b;
    x = forward(x);
    std::cout << x.first << std::endl;
    std::cout << x.second << std::endl;
}

void backward_test(int a, int b) {
    std::pair<int, int> x;
    x.first = a;
    x.second = b;
    x = backward(x);
    std::cout << x.first << std::endl;
    std::cout << x.second << std::endl;
}

int stepping(std::string input, int x = 0, int y = 127, char forw_char = 'F', char back_char = 'B') {
    std::pair<int, int> pos(x, y);
    for (int i = 0; i < input.length(); i++) {
	if (input[i] == forw_char) {
	    pos = forward(pos);
	}
	if (input[i] == back_char) {
	    pos = backward(pos);
	}
    }
    return pos.first;
}

// [[Rcpp::export]]
int cal_seatid (std::string input) {
    int xpos = stepping(input.substr(0, 7), 0, 127, 'F', 'B');
    int ypos = stepping(input.substr(7, 9), 0, 7, 'L', 'R');
    return (xpos * 8) + ypos;
}

// [[Rcpp::export]]
int d5_1 (CharacterVector input) {
    std::vector<int> seatids;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	seatids.push_back(cal_seatid(line));
    }
    int max_id = *std::max_element(seatids.begin(), seatids.end());
    return max_id;
}


bool key_find(int key, std::vector<int> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

// [[Rcpp::export]]
int d5_2 (CharacterVector input) {
    std::vector<int> seatids;
    for (CharacterVector::iterator i = input.begin(); i != input.end(); ++i) {
	std::string line = as<std::string>(*i);
	seatids.push_back(cal_seatid(line));
    }
    int max_id = *std::max_element(seatids.begin(), seatids.end());
    int min_id = *std::min_element(seatids.begin(), seatids.end());
    for (int i = min_id; i < max_id; i++) {
	if (!key_find(i, seatids)) {
	    return i;
	}
    }
    return 0;
}

/*** R
input5 <- readLines(here::here("data/input5.txt"))

stopifnot(cal_seatid("FBFBBFFRLR") == 357)
stopifnot(cal_seatid("BFFFBBFRRR") == 567)
stopifnot(cal_seatid("FFFBBBFRRR") == 119)
stopifnot(cal_seatid("BBFFBBFRLL") == 820)

d5_1(input5)
d5_2(input5)

*/

