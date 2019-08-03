context("  Miscellaneous")

# uses row_sums as gateway because it was the first R function
test_that("Grain for interruption check is calculated correctly in C++.", {
    RcppParallel::setThreadOptions(1L)
    on.exit(RcppParallel::setThreadOptions())

    mat <- matrix(1L, nrow = 200000L, ncol = 2L)
    expected <- rowSums(mat)
    ans <- row_sums(mat, output_mode = "double")
    expect_equal(ans, expected)

    mat <- matrix(1L, nrow = 5L, ncol = 2L)
    expected <- rowSums(mat)
    ans <- row_sums(mat, output_mode = "double")
    expect_equal(ans, expected)

    mat <- matrix(1L, nrow = 50000L, ncol = 2L)
    expected <- rowSums(mat)
    ans <- row_sums(mat, output_mode = "double")
    expect_equal(ans, expected)
})
