int_mat <- matrix(1:10000, ncol = 2L)
dbl_mat <- matrix(as.numeric(int_mat), ncol = 2L)
bool_mat <- matrix(sample(c(TRUE, FALSE), 10000L, TRUE), ncol = 2L)
char_mat <- matrix(sample(letters, 10000L, TRUE), ncol = 2L)
