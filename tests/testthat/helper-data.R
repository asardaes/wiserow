int_na_mat <- int_mat <- matrix(1:10000, ncol = 2L)
dbl_na_mat <- dbl_mat <- matrix(as.numeric(int_mat), ncol = 2L)
bool_na_mat <- bool_mat <- matrix(sample(c(TRUE, FALSE), 10000L, TRUE), ncol = 2L)
char_na_mat <- char_mat <- matrix(sample(letters, 10000L, TRUE), ncol = 2L)

int_na_mat[sample(10000L, 1000L)] <- NA_integer_
dbl_na_mat[sample(10000L, 1000L)] <- NA_real_
bool_na_mat[sample(10000L, 1000L)] <- NA
char_na_mat[sample(10000L, 1000L)] <- NA_character_
