context("  Row sums")

test_that("row_sums can handle errors and edge cases appropriately.", {
    suppressWarnings(
        expect_error(regexp = "not support", row_sums(char_mat, "integer"))
    )

    expect_identical(row_sums(as.matrix(data.frame())), vector("integer"))
})

test_that("row_sums for integer matrices works.", {
    expected <- rowSums(int_mat)
    ans <- row_sums(int_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(int_mat)
    expect_identical(ans, expected)
})

test_that("row_sums for integer matrices with NAs works.", {
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

test_that("row_sums for integer matrices with column subset works.", {
    expected <- rowSums(int_mat[, -2L])
    ans <- row_sums(int_mat, output_mode = "double", cols = c(1L, 3L))
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, output_mode = "double", cols = -2L)
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, cols = integer())
    expect_identical(ans, integer(length(ans)))

    expect_error(row_sums(int_mat, cols = 10L), "Invalid column indices")
})

test_that("row_sums for integer matrices with row subset works.", {
    expected <- rowSums(int_mat[1:2000,])
    ans <- row_sums(int_mat, output_mode = "double", rows = 1:2000)
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, output_mode = "double", rows = -(2001:5000))
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, rows = integer())
    expect_identical(ans, integer())

    expect_error(row_sums(int_mat, rows = 10000L), "Invalid row indices")
})

test_that("row_sums for double matrices works.", {
    expected <- rowSums(dbl_mat)
    ans <- row_sums(dbl_mat)
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(dbl_mat, output_mode = "integer")
    expect_identical(ans, expected)
})

test_that("row_sums for double matrices with NAs works.", {
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

test_that("row_sums for double matrices with column subset works.", {
    expected <- rowSums(dbl_mat[, -2L])
    ans <- row_sums(dbl_mat, cols = c(1L, 3L))
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, cols = -2L)
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, cols = integer())
    expect_identical(ans, double(length(ans)))

    expect_error(row_sums(dbl_mat, cols = 10L), "Invalid column indices")
})

test_that("row_sums for double matrices with row subset works.", {
    expected <- rowSums(dbl_mat[1:2000,])
    ans <- row_sums(dbl_mat, rows = 1:2000)
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, rows = -(2001:5000))
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, rows = integer())
    expect_identical(ans, double())

    expect_error(row_sums(dbl_mat, rows = 10000L), "Invalid row indices")
})

test_that("row_sums for logical matrices works.", {
    expected <- rowSums(bool_mat)
    ans <- row_sums(bool_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(bool_mat)
    expect_identical(ans, expected)
})

test_that("row_sums for logical matrices with NAs works.", {
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

test_that("row_sums for logical matrices with column subset works.", {
    expected <- rowSums(bool_mat[, -2L])
    ans <- row_sums(bool_mat, output_mode = "double", cols = c(1L, 3L))
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, output_mode = "double", cols = -2L)
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, cols = integer())
    expect_identical(ans, integer(length(ans)))

    expect_error(row_sums(bool_mat, cols = 10L), "Invalid column indices")
})

test_that("row_sums for logical matrices with row subset works.", {
    expected <- rowSums(bool_mat[1:2000,])
    ans <- row_sums(bool_mat, output_mode = "double", rows = 1:2000)
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, output_mode = "double", rows = -(2001:5000))
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, rows = integer())
    expect_identical(ans, integer())

    expect_error(row_sums(bool_mat, rows = 10000L), "Invalid row indices")
})
