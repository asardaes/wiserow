int_na_mat <- int_mat <- matrix(1:15000, ncol = 3L)
dbl_na_mat <- dbl_mat <- matrix(as.numeric(int_mat), ncol = 3L)
bool_na_mat <- bool_mat <- matrix(sample(c(TRUE, FALSE), 15000L, TRUE), ncol = 3L)
char_na_mat <- char_mat <- matrix(sample(letters, 15000L, TRUE), ncol = 3L)
cplx_na_mat <- cplx_mat <- matrix(cumsum(rep(1+1i, 15000L)), ncol = 3L)

int_na_mat[sample(15000L, 5000L)] <- NA_integer_
dbl_na_mat[sample(15000L, 5000L)] <- NA_real_
bool_na_mat[sample(15000L, 5000L)] <- NA
char_na_mat[sample(15000L, 5000L)] <- NA_character_
cplx_na_mat[sample(15000L, 5000L)] <- NA_complex_

df <- cbind(
    as.data.frame(int_na_mat),
    as.data.frame(dbl_na_mat),
    as.data.frame(bool_na_mat),
    as.data.frame(char_na_mat, stringsAsFactors = FALSE),
    as.data.frame(cplx_na_mat)
)

dt <- data.table::as.data.table(df)
