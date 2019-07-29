context("  Row sums")

test_that("row_sums for matrices works.", {
    expected <- rowSums(int_mat)
    ans <- row_sums(int_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(int_mat)
    expect_identical(ans, expected)
})
