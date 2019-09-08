context("  Miscellaneous")

# some of these use row_sums as gateway because it was the first R function

test_that("validate_metadata throws for invalid subset indices.", {
    expect_error(wiserow:::validate_metadata(int_mat, list(rows = "foo")), regexp = "no row names")
    expect_error(wiserow:::validate_metadata(int_mat, list(cols = "foo")), regexp = "no column names")
    expect_error(wiserow:::validate_metadata(df, list(rows = "foo")), regexp = "cannot have NA")
    expect_error(wiserow:::validate_metadata(df, list(cols = "foo")), regexp = "cannot have NA")
})

test_that("Functions will throw if control strings cannot be mapped to known enums.", {
    metadata <- op_ctrl(input_class = "mtx",
                        input_modes = "integer",
                        output_mode = "integer")

    suppressWarnings(
        expect_error(regexp = "unsupported input", .Call(wiserow:::`C_row_arith`, metadata, int_mat, integer(), list()))
    )

    metadata$input_class <- "matrix"
    metadata$output_mode <- "data.frame"

    suppressWarnings(
        expect_error(regexp = "unsupported mode", .Call(wiserow:::`C_row_arith`, metadata, int_mat, list(), list()))
    )

    metadata$output_mode <- "integer"
    metadata$output_class <- "none"

    suppressWarnings(
        expect_error(regexp = "unsupported output", .Call(wiserow:::`C_row_arith`, metadata, int_mat, list(), list()))
    )
})

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
