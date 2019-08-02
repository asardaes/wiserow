context("  Row sums")

test_that("row_sums for matrices works.", {
    expected <- rowSums(int_mat)
    ans <- row_sums(int_mat, output_mode = "double")
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat)
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(int_mat)
    expect_identical(ans, expected)

    expected <- rowSums(bool_mat)
    ans <- row_sums(bool_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(bool_mat, output_mode = "integer")
    expect_identical(ans, expected)

    suppressWarnings(
        expect_error(regexp = "not support", row_sums(char_mat, "integer"))
    )
})
