context("  Row sums")

test_that("row_sums throws for character matrices.", {
    suppressWarnings(
        expect_error(regexp = "not support", row_sums(char_mat, "integer"))
    )
})

test_that("row_sums for integer matrices works.", {
    expected <- rowSums(int_mat)
    ans <- row_sums(int_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(int_mat)
    expect_identical(ans, expected)

    expected <- rowSums(int_na_mat, na.rm = TRUE)
    ans <- row_sums(int_na_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(int_na_mat)
    expect_identical(ans, expected)

    expected <- rowSums(int_na_mat)
    ans <- row_sums(int_na_mat, output_mode = "double", na_action = "pass")
    expect_identical(ans, expected)
})

test_that("row_sums for double matrices works.", {
    expected <- rowSums(dbl_mat)
    ans <- row_sums(dbl_mat)
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(dbl_mat, output_mode = "integer")
    expect_identical(ans, expected)

    expected <- rowSums(dbl_na_mat, na.rm = TRUE)
    ans <- row_sums(dbl_na_mat)
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(dbl_na_mat, output_mode = "integer")
    expect_identical(ans, expected)

    expected <- rowSums(dbl_na_mat)
    ans <- row_sums(dbl_na_mat, na_action = "pass")
    expect_identical(ans, expected)
})

test_that("row_sums for logical matrices works.", {
    expected <- rowSums(bool_mat)
    ans <- row_sums(bool_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(bool_mat)
    expect_identical(ans, expected)

    expected <- rowSums(bool_na_mat, na.rm = TRUE)
    ans <- row_sums(bool_na_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(bool_na_mat)
    expect_identical(ans, expected)

    expected <- rowSums(bool_na_mat)
    ans <- row_sums(bool_na_mat, output_mode = "double", na_action = "pass")
    expect_identical(ans, expected)
})
