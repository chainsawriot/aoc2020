input16 <- readLines(here::here("data/input16.txt"))
criteria <- input16[1:20]
nbtix <- input16[26:length(input16)]
yrtix <- input16[23]

Rcpp::sourceCpp("d16.cpp")
pp(criteria[3])

testdata <- "class: 1-3 or 5-7
row: 6-11 or 33-44
seat: 13-40 or 45-50

your ticket:
7,1,14

nearby tickets:
7,3,47
40,4,50
55,2,20
38,6,12"

testcrit <- strsplit(testdata, "\n", TRUE)[[1]][1:3]
testtix <- strsplit(testdata, "\n", TRUE)[[1]][9:12]

stopifnot(check_test2(1, testcrit))
stopifnot(!check_test2(4, testcrit))

stopifnot(d16_1(testtix, testcrit) == 71)

d16_1(nbtix, criteria)

stopifnot(filter_tix(testtix, testcrit) == "7,3,47")

valid_tix <- filter_tix(nbtix, criteria)

## 190x20

nb_matrix <- matrix(as.numeric(str_split(paste0(valid_tix, collapse=","), ",")[[1]]), nrow = 190, ncol = 20, byrow = TRUE)

