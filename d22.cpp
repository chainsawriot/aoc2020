#include <Rcpp.h>
#include <regex>
#include <algorithm>

using namespace Rcpp;
// [[Rcpp::plugins("cpp11")]]



struct hands {
    std::deque<int> p1;
    std::deque<int> p2;
};

struct hands make_hands (std::vector<int> p1, std::vector<int> p2) {
    struct hands hands;
    for (int& val: p1) {
	hands.p1.push_back(val);
    }
    for (int& val: p2) {
	hands.p2.push_back(val);
    }
    return hands;
}

void pp_deque (std::deque<int> input) {
    for (int& val: input) {
	std::cout << val << ",";
    }
    std::cout << std::endl;
}

long long int cal_score(std::deque<int>& input) {
    if (input.empty()) {
	return 0;
    }
    long long int total = 0;
    long long int size = input.size();
    for (int i = 0; i < size; i++) {
	total += input[i] * (size - i);
    }
    return total;
}

long long int naive_play(struct hands hands, bool return_total = true) {
    while (!(hands.p1.empty() || hands.p2.empty())) {
	int p1_play = hands.p1.front();
	hands.p1.pop_front();
	int p2_play = hands.p2.front();
	hands.p2.pop_front();
	if (p1_play > p2_play) {
	    hands.p1.push_back(p1_play);
	    hands.p1.push_back(p2_play);		
	} else {
	    hands.p2.push_back(p2_play);
	    hands.p2.push_back(p1_play);
	}
    }
    // lazy!
    if (return_total) {
	long long int total = cal_score(hands.p1) + cal_score(hands.p2);
	return total;
    } else {
	if (cal_score(hands.p1) > cal_score(hands.p2)) {
	    return 0;
	} else {
	    return 1;
	}
    }
}

typedef std::pair<bool, long long int> play_res;

// play_res.first is whether or not p1 is the winner
// play_res.second is the score


bool in_set (std::deque<int>& x, std::set<std::deque<int>>& setx) {
    auto search = setx.find(x);
    if (search == setx.end()) {
	return false;
    } else {
	return true;
    }
}

// Should use some memoization.
play_res recur_play(struct hands hands) {
    std::set<std::deque<int>> seen1, seen2;
    play_res res;
    while (!(hands.p1.empty() || hands.p2.empty())) {
	if (in_set(hands.p1, seen1) || in_set(hands.p2, seen2)) {
	    res.first = true;
	    res.second = 0;
	    return res;
	}
	// archive deck
	seen1.insert(hands.p1);
	seen2.insert(hands.p2);
	int p1_play = hands.p1.front();
	hands.p1.pop_front();
	int p2_play = hands.p2.front();
	hands.p2.pop_front();
	if (hands.p1.size() >= p1_play && hands.p2.size() >= p2_play) {
	    play_res res_temp = recur_play(hands); // don't pass by reference!
	    if (res_temp.first) {
		hands.p1.push_back(p1_play);
		hands.p1.push_back(p2_play);
	    } else {
		hands.p2.push_back(p2_play);
		hands.p2.push_back(p1_play);
	    }
	} else {
	    if (p1_play > p2_play) {
		hands.p1.push_back(p1_play);
		hands.p1.push_back(p2_play);
	    } else {
		hands.p2.push_back(p2_play);
		hands.p2.push_back(p1_play);
	    }   
	}
    }
    if (hands.p1.empty()) {
	res.first = false;
    } else {
	res.first = true;
    }
    long long int total = cal_score(hands.p1) + cal_score(hands.p2);
    res.second = total;
    std::cout << total << std::endl;
    return res;
}


// [[Rcpp::export]]
long long int d22_1(std::vector<int> p1, std::vector<int> p2) {
    struct hands hands = make_hands(p1, p2);
    return naive_play(hands, true);
}

// [[Rcpp::export]]
long long int d22_2(std::vector<int> p1, std::vector<int> p2) {
    struct hands hands = make_hands(p1, p2);
    auto x = recur_play(hands);
    return x.second;
}

