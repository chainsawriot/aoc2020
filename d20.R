puzzle <- matrix(c("4", "1", "3", "2"), nrow = 2, byrow = TRUE)

puzzle3 <- matrix(1:9, nrow = 3, byrow = TRUE)

# 0 = top, 1 = right, 2 = left, 3 = bottom
extract_edge <- function(input, which = 0) {
    if (which == 0) {
        x <- 1
        y <- seq_len(nrow(input))
    } else if (which == 1) {
        x <- seq_len(nrow(input))
        y <- nrow(input)
    } else if (which == 2) {
        x <- seq_len(nrow(input))
        y <- 1
    } else {
        x <- nrow(input)
        y <- seq_len(nrow(input))
    }
    return(input[x, y])    
}

rotate <- function(x) t(apply(x, 2, rev))

flip <- function(x) {
    x[,rev(seq_len(ncol(x)))]
}

rotate(rotate(rotate(puzzle)))


Rcpp::sourceCpp("d20.cpp")
testdata <- c(readLines(here::here("data/input20_testdata.txt")), "")

test_parse(testdata[1:11])

test_parse(testdata[1:11])
testdata[13:23]
test_parse(testdata[13:23])

##test_m(testdata[1:11], testdata[13:23])

stopifnot(d20_1(testdata) == 20899048083289)

input20 <- c(readLines(here::here("data/input20.txt")))


## F*ck scientific notation
options("scipen"=100, "digits"=4)
d20_1(input20)

testpp(testdata)

tinydata <- c(readLines(here::here("data/input20_tiny.txt")))

testpp(tinydata)

res <- list()
x <- 1
require(stringr)
stack <- c()
for (i in tinydata) {
    if (str_detect(i, "Tile")) {
        ## do nothing
        
    } else if (i == "") {
        print(stack)
        chars <- str_split(paste0(stack, collapse = ""), "")[[1]]
        print(chars)
        out <- matrix(chars, nrow = 2, byrow = TRUE)
        res[[x]] <- out
        x <- x + 1
        stack <- c()
    } else {
        stack <- c(stack, i)
    }
}

col_fig1 <- c(1)
col_fig2 <- c(0)
