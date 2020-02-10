context("  Row extrema")

test_that("Error is thrown if complex numbers are involved.", {
    expect_error(wiserow:::row_extrema_matrix(cplx_mat), "typeof")
    expect_error(wiserow:::row_extrema_df(df), "complex")
})

test_that("Single-column inputs are correctly early-terminated.", {
    # ----------------------------------------------------------------------------------------------
    # matrix

    expected <- int_mat[, 2L]

    ans <- wiserow:::row_extrema_matrix(int_mat[, 2L, drop = FALSE])
    expect_identical(ans, expected)

    ans <- wiserow:::row_extrema_matrix(int_mat[, 2L, drop = FALSE], output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = expected))

    ans <- wiserow:::row_extrema_matrix(int_mat, cols = 2L)
    expect_identical(ans, expected)

    ans <- wiserow:::row_extrema_matrix(int_mat, cols = 2L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = expected))

    # ----------------------------------------------------------------------------------------------
    # data frame

    expected <- df$int.V2

    ans <- wiserow:::row_extrema_df(df["int.V2"])
    expect_identical(ans, expected)

    ans <- wiserow:::row_extrema_df(df["int.V2"], output_class = "data.frame")
    expect_identical(ans, data.frame(int.V2 = expected))

    ans <- wiserow:::row_extrema_df(df, cols = 2L)
    expect_identical(ans, expected)

    ans <- wiserow:::row_extrema_df(df, cols = 2L, output_class = "list")
    expect_identical(ans, as.list(expected))
})

test_that("Output is as expected when all columns are NA.", {
    # ----------------------------------------------------------------------------------------------
    # logical matrix

    mat <- matrix(NA, nrow = 2L, ncol = 2L)

    ans <- wiserow:::row_extrema_matrix(mat, ">")
    expect_identical(typeof(ans), "logical")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L)
    expect_identical(typeof(ans), "logical")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = NA))

    # ----------------------------------------------------------------------------------------------
    # integer matrix

    mat <- matrix(NA_integer_, nrow = 2L, ncol = 2L)

    ans <- wiserow:::row_extrema_matrix(mat, ">")
    expect_identical(typeof(ans), "integer")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L)
    expect_identical(typeof(ans), "integer")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = NA_integer_))

    # ----------------------------------------------------------------------------------------------
    # double matrix

    mat <- matrix(NA_real_, nrow = 2L, ncol = 2L)

    ans <- wiserow:::row_extrema_matrix(mat, ">")
    expect_identical(typeof(ans), "double")
    expect_true(all(is.infinite(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L)
    expect_identical(typeof(ans), "double")
    expect_true(all(is.infinite(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = -Inf))

    # ----------------------------------------------------------------------------------------------
    # character matrix

    mat <- matrix(NA_character_, nrow = 2L, ncol = 2L)

    ans <- wiserow:::row_extrema_matrix(mat, ">")
    expect_identical(typeof(ans), "character")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L)
    expect_identical(typeof(ans), "character")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_matrix(mat, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = NA_character_, stringsAsFactors = FALSE))

    # ----------------------------------------------------------------------------------------------
    # logical data frame

    df <- as.data.frame(matrix(NA, nrow = 2L, ncol = 2L))

    ans <- wiserow:::row_extrema_df(df, ">")
    expect_identical(typeof(ans), "logical")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L)
    expect_identical(typeof(ans), "logical")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = NA))

    # ----------------------------------------------------------------------------------------------
    # integer data frame

    df <- as.data.frame(matrix(NA_integer_, nrow = 2L, ncol = 2L))

    ans <- wiserow:::row_extrema_df(df, ">")
    expect_identical(typeof(ans), "integer")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L)
    expect_identical(typeof(ans), "integer")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = NA_integer_))

    # ----------------------------------------------------------------------------------------------
    # double data frame

    df <- as.data.frame(matrix(NA_real_, nrow = 2L, ncol = 2L))

    ans <- wiserow:::row_extrema_df(df, ">")
    expect_identical(typeof(ans), "double")
    expect_true(all(is.infinite(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L)
    expect_identical(typeof(ans), "double")
    expect_true(all(is.infinite(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = -Inf))

    # ----------------------------------------------------------------------------------------------
    # character data frame

    df <- as.data.frame(matrix(NA_character_, nrow = 2L, ncol = 2L))

    ans <- wiserow:::row_extrema_df(df, ">")
    expect_identical(typeof(ans), "character")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L)
    expect_identical(typeof(ans), "character")
    expect_true(all(is.na(ans)))

    ans <- wiserow:::row_extrema_df(df, ">", rows = 1L, output_class = "data.frame")
    expect_identical(ans, data.frame(V1 = NA_character_, stringsAsFactors = FALSE))
})
