#include <Rcpp.h>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]

bool key_find(long int key, std::vector<long int> keys) {
    return std::find(keys.begin(), keys.end(), key) != keys.end();
}

// [[Rcpp::export]]
std::vector<long int> find2(long int key, std::vector<long int> keys) {
    std::vector<long int> res;
    for (int i = 0; i < keys.size(); i++) {
	if (keys[i] == key) {
	    res.push_back(i);
	}
    }
    return res;
}

// [[Rcpp::export]]
long int play (std::vector<long int> input) {
    const long int n = input.size();
    const long int value = input[n - 1];
    const std::vector<long int> butlast = {input.begin(), input.end() - 1};
    const std::vector<long int> indices = find2(value, input);
    if (indices.size() == 1) {
	return 0;
    }
    return indices[indices.size() - 1] - indices[indices.size() - 2];
}

// [[Rcpp::export]]
long int d15_1(std::vector<long int> input, long int until_round = 2000) {
    long int current_round = input.size();
    while (current_round != until_round) {
	// std::cout << current_round << std::endl;
	input.push_back(play(input));
	current_round += 1;
    }
    return input[input.size() - 1];
}

// You can't do part 2 with a vector. Let's do it with
// std::map<long int, std::pair<long int, long int>>

// BTW: The sequence has a name: Van Eck's sequence
// https://oeis.org/A181391

std::map<long int, std::pair<long int, long int>> preprocess(std::vector<long int> input) {
    std::map<long int, std::pair<long int, long int>> res;
    for (int i = 0; i < input.size(); i++) {
	std::pair<long int, long int> x;
	x.first = i + 1;
	x.second = i + 1;
	res[input[i]] = x;
    }
    return res;
}

std::unordered_map<long int, std::pair<long int, long int>> preprocess2(std::vector<long int> input) {
    std::unordered_map<long int, std::pair<long int, long int>> res;
    for (int i = 0; i < input.size(); i++) {
	std::pair<long int, long int> x;
	x.first = i + 1;
	x.second = i + 1;
	res[input[i]] = x;
    }
    return res;
}


// void test() {
//     std::vector<long int> input = {0,3,6};
//     auto res = preprocess(input);
//     std::cout << input[input.size() -1] << std::endl;
//     std::cout << res[6].first << std::endl;
//     std::cout << res[6].second << std::endl;
//     if (res.find(8) == res.end()) {
// 	std::cout << "8 Not found" << std::endl;
//     }
//     if (res.find(6) == res.end()) {
// 	std::cout << "6 Not found" << std::endl;
//     }
// }

// prettyprint: Useful for debugging
void pp (std::map<long int, std::pair<long int, long int>> input) {
    for(auto it = input.cbegin(); it != input.cend(); ++it)
{
    std::cout << "Key: " <<std::endl;
    std::cout << it->first << "/" << it->second.first << " " << it->second.second << "\n";
 }
}

// void test2() {
//     std::vector<long int> input = {0,3,6};
//     auto res = preprocess(input);
//     pp(res);
// }

// [[Rcpp::export]]
long int d15_2 (std::vector<long int> input, long int round = 30000000) {
    auto res = preprocess2(input);
    long int cur = input.size() + 1;
    long int ans = -1;
    long int lastplay = input[input.size() - 1];
    while (cur < (round+1)) {
	//std::cout << "Current:" << std::endl;
	//std::cout << cur << std::endl;
	//std::cout << "Ans:" << std::endl;
	//std::cout << ans << std::endl;
	//pp(res);
	//std::cout << "====" <<std::endl;
	if (res.find(lastplay) == res.end()) {
	    ans = 0;
	} else {
	    ans = res[lastplay].first - res[lastplay].second;
	}
	// updating state
	if (res.find(ans) == res.end()) {
	    std::pair<long int, long int> x;
	    x.first = cur;
	    x.second = cur;
	    res[ans] = x;
	} else {
	    res[ans].second = res[ans].first;
	    res[ans].first = cur;
	}
	lastplay = ans;
	cur ++;
    }
    return ans;
}

// [[Rcpp::export]]
long int d15_2_map (std::vector<long int> input, long int round = 30000000) {
    auto res = preprocess(input);
    long int cur = input.size() + 1;
    long int ans = -1;
    long int lastplay = input[input.size() - 1];
    while (cur < (round+1)) {
	//std::cout << "Current:" << std::endl;
	//std::cout << cur << std::endl;
	//std::cout << "Ans:" << std::endl;
	//std::cout << ans << std::endl;
	//pp(res);
	//std::cout << "====" <<std::endl;
	if (res.find(lastplay) == res.end()) {
	    ans = 0;
	} else {
	    ans = res[lastplay].first - res[lastplay].second;
	}
	// updating state
	if (res.find(ans) == res.end()) {
	    std::pair<long int, long int> x;
	    x.first = cur;
	    x.second = cur;
	    res[ans] = x;
	} else {
	    res[ans].second = res[ans].first;
	    res[ans].first = cur;
	}
	lastplay = ans;
	cur ++;
    }
    return ans;
}


/***R
## Bruteforce work for the first star; it doesn't work any longer for the second star.

stopifnot(d15_1(c(1,3,2), 2020) == 1)
stopifnot(d15_1(c(2,1,3), 2020) == 10)
stopifnot(d15_1(c(1,2,3), 2020) == 27)
stopifnot(d15_1(c(2,3,1), 2020) == 78)
stopifnot(d15_1(c(3,2,1), 2020) == 438)
stopifnot(d15_1(c(3,1,2), 2020) == 1836)

stopifnot(d15_2(c(1,3,2), 2020) == 1)
stopifnot(d15_2(c(2,1,3), 2020) == 10)
stopifnot(d15_2(c(1,2,3), 2020) == 27)
stopifnot(d15_2(c(2,3,1), 2020) == 78)
stopifnot(d15_2(c(3,2,1), 2020) == 438)
stopifnot(d15_2(c(3,1,2), 2020) == 1836)

## I don't have time to test all cases for 30000000 iterations.
## Just test the last case.

stopifnot(d15_2(c(3,1,2), 30000000) == 362)

## what is the performance difference between std::map and std::unordered_map?

microbenchmark::microbenchmark(d15_2(c(3,1,2), 300000), times = 20)
microbenchmark::microbenchmark(d15_2_map(c(3,1,2), 300000), times = 20)

d15_1(c(0,12,6,13,20,1,17), 2020)
d15_2(c(0,12,6,13,20,1,17), 30000000)
 */
