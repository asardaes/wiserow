context("  Row sums")

test_that("row_sums can handle errors and edge cases appropriately.", {
    suppressWarnings(
        expect_error(regexp = "not support", row_sums(char_mat, output_mode = "integer"))
    )

    suppressWarnings(
        expect_error(regexp = "not support", row_sums(char_mat, output_mode = "integer", cols = -2L))
    )

    suppressWarnings(
        expect_error(regexp = "not support", row_sums(char_mat, output_mode = "complex"))
    )

    expect_error(regexp = "row_sums can only return", row_sums(int_mat, output_mode = "character"))
    expect_error(regexp = "row_sums can only return", row_sums(int_mat, output_mode = "character", output_class = "list"))

    expect_identical(row_sums(as.matrix(data.frame())), vector("integer"))
    expect_identical(row_sums(data.frame()), vector("integer"))

    suppressWarnings(
        expect_error(regexp = "not support", row_sums(df))
    )
})

test_that("row_sums for integer matrices works.", {
    expected <- rowSums(int_mat)
    ans <- row_sums(int_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(int_mat)
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_sums(int_mat, output_class = "list")
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
    ans <- row_sums(int_mat, output_mode = "double", cols = c(1, 3))
    expect_identical(ans, expected)
    ans <- row_sums(int_mat, output_mode = "double", cols = c(TRUE, FALSE, TRUE))
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, output_mode = "double", cols = -2L)
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, cols = integer())
    expect_identical(ans, integer(length(ans)))

    expect_error(row_sums(int_mat, cols = 10L), "Invalid column indices")

    expected <- rowSums(int_mat[, c(1L, 1L, 3L)])
    ans <- row_sums(int_mat, output_mode = "double", cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_sums for integer matrices with row subset works.", {
    expected <- rowSums(int_mat[1001:2000,])
    ans <- row_sums(int_mat, output_mode = "double", rows = 1001:2000)
    expect_identical(ans, expected)
    ans <- row_sums(int_mat, output_mode = "double", rows = 1:5000 %in% 1001:2000)
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, output_mode = "double", rows = -c(1:1000, 2001:5000))
    expect_identical(ans, expected)

    ans <- row_sums(int_mat, rows = integer())
    expect_identical(ans, integer())

    expect_error(row_sums(int_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowSums(int_mat[rep(1001:2000, 2),])
    ans <- row_sums(int_mat, output_mode = "double", rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_sums for double matrices works.", {
    expected <- rowSums(dbl_mat)
    ans <- row_sums(dbl_mat)
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_sums(dbl_mat, output_class = "list")
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
    ans <- row_sums(dbl_mat, cols = c(1, 3))
    expect_identical(ans, expected)
    ans <- row_sums(dbl_mat, cols = c(TRUE, FALSE, TRUE))
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, cols = -2L)
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, cols = integer())
    expect_identical(ans, double(length(ans)))

    expect_error(row_sums(dbl_mat, cols = 10L), "Invalid column indices")

    expected <- rowSums(dbl_mat[, c(1L, 1L, 3L)])
    ans <- row_sums(dbl_mat, cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_sums for double matrices with row subset works.", {
    expected <- rowSums(dbl_mat[1001:2000,])
    ans <- row_sums(dbl_mat, rows = 1001:2000)
    expect_identical(ans, expected)
    ans <- row_sums(dbl_mat, rows = 1:5000 %in% 1001:2000)
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, rows = -c(1:1000, 2001:5000))
    expect_identical(ans, expected)

    ans <- row_sums(dbl_mat, rows = integer())
    expect_identical(ans, double())

    expect_error(row_sums(dbl_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowSums(dbl_mat[rep(1001:2000, 2),])
    ans <- row_sums(dbl_mat, rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_sums for logical matrices works.", {
    expected <- rowSums(bool_mat)
    ans <- row_sums(bool_mat, output_mode = "double")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_sums(bool_mat)
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_sums(bool_mat, output_class = "list")
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
    ans <- row_sums(bool_mat, output_mode = "double", cols = c(1, 3))
    expect_identical(ans, expected)
    ans <- row_sums(bool_mat, output_mode = "double", cols = c(TRUE, FALSE, TRUE))
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, output_mode = "double", cols = -2L)
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, cols = integer())
    expect_identical(ans, integer(length(ans)))

    expect_error(row_sums(bool_mat, cols = 10L), "Invalid column indices")

    expected <- rowSums(bool_mat[, c(1L, 1L, 3L)])
    ans <- row_sums(bool_mat, output_mode = "double", cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_sums for logical matrices with row subset works.", {
    expected <- rowSums(bool_mat[1001:2000,])
    ans <- row_sums(bool_mat, output_mode = "double", rows = 1001:2000)
    expect_identical(ans, expected)
    ans <- row_sums(bool_mat, output_mode = "double", rows = 1:5000 %in% 1001:2000)
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, output_mode = "double", rows = -c(1:1000, 2001:5000))
    expect_identical(ans, expected)

    ans <- row_sums(bool_mat, rows = integer())
    expect_identical(ans, integer())

    expect_error(row_sums(bool_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowSums(bool_mat[rep(1001:2000, 2),])
    ans <- row_sums(bool_mat, output_mode = "double", rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_sums for complex matrices works.", {
    expected <- rowSums(cplx_mat)
    ans <- row_sums(cplx_mat)
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_sums(cplx_mat, output_class = "list")
    expect_identical(ans, expected)

    expected <- rowSums(Mod(cplx_mat))
    ans <- row_sums(cplx_mat, output_mode = "double")
    expect_equal(ans, expected)

    expected <- as.integer(Mod(cplx_mat))
    dim(expected) <- dim(cplx_mat)
    expected <- rowSums(expected)
    ans <- row_sums(cplx_mat, output_mode = "integer")
    expect_equal(ans, expected)
})

test_that("row_sums for complex matrices with NAs works.", {
    expected <- rowSums(cplx_na_mat, na.rm = TRUE)
    ans <- row_sums(cplx_na_mat)
    expect_equal(ans, expected)

    expected <- rowSums(cplx_na_mat)
    ans <- row_sums(cplx_na_mat, na_action = "pass")
    expect_equal(ans, expected)
})

test_that("row_sums for complex matrices with column subset works.", {
    expected <- rowSums(cplx_mat[, -2L])
    ans <- row_sums(cplx_mat, cols = c(1, 3))
    expect_equal(ans, expected)
    ans <- row_sums(cplx_mat, cols = c(TRUE, FALSE, TRUE))
    expect_equal(ans, expected)

    ans <- row_sums(cplx_mat, cols = -2L)
    expect_equal(ans, expected)

    ans <- row_sums(cplx_mat, cols = integer())
    expect_equal(ans, complex(length(ans)))

    expect_error(row_sums(cplx_mat, cols = 10L), "Invalid column indices")

    expected <- rowSums(cplx_mat[, c(1L, 1L, 3L)])
    ans <- row_sums(cplx_mat, cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_sums for complex matrices with row subset works.", {
    expected <- rowSums(cplx_mat[1001:2000,])
    ans <- row_sums(cplx_mat, rows = 1001:2000)
    expect_equal(ans, expected)
    ans <- row_sums(cplx_mat, rows = 1:5000 %in% 1001:2000)
    expect_equal(ans, expected)

    ans <- row_sums(cplx_mat, rows = -c(1:1000, 2001:5000))
    expect_equal(ans, expected)

    ans <- row_sums(cplx_mat, rows = integer())
    expect_equal(ans, complex())

    expect_error(row_sums(cplx_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowSums(cplx_mat[rep(1001:2000, 2),])
    ans <- row_sums(cplx_mat, rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_sums for data frames works.", {
    df <- df[, sapply(df, typeof) != "character"]

    considered_cols <- list(
        paste0("int.V", 1:3),
        paste0("bool.V", 1:3),
        c(paste0("int.V", 1:3), paste0("bool.V", 1:3)),
        1:9, # no complex
        1:12 # all
    )

    for (cols in considered_cols) {
        expected <- rowSums(df[1001:5000, cols], na.rm = TRUE)
        names(expected) <- NULL

        ans <- row_sums(df, rows = 1001:5000, cols = cols, na_action = "exclude")
        expect_equal(ans, expected)

        ans <- row_sums(df, rows = as.character(1001:5000), cols = cols, na_action = "exclude", output_class = "list")
        expect_equal(ans, as.list(expected))
    }

    # ----------------------------------------------------------------------------------------------

    expected <- as.logical(rowSums(df[-10L, 7:9]))
    ans <- row_sums(df, rows = -10L, cols = 7:9, na_action = "pass", output_mode = "logical")
    expect_identical(ans, expected)
})

test_that("row_sums for data tables works.", {
    dt <- data.table::as.data.table(df[, sapply(df, typeof) != "character"])

    considered_cols <- list(
        1:3,
        7:9,
        c(1:3, 7:9),
        1:9,
        1:12
    )

    for (cols in considered_cols) {
        expected <- rowSums(dt[1001:5000, ..cols], na.rm = TRUE)
        names(expected) <- NULL

        ans <- row_sums(dt, rows = 1001:5000, cols = cols, na_action = "exclude")
        expect_equal(ans, expected)

        ans <- row_sums(dt, rows = 1001:5000, cols = cols, na_action = "exclude", output_class = "list")
        expect_equal(ans, as.list(expected))
    }

    # ----------------------------------------------------------------------------------------------

    expected <- as.logical(rowSums(dt[-10L, 7:9]))
    ans <- row_sums(dt, rows = -10L, cols = 7:9, na_action = "pass", output_mode = "logical")
    expect_identical(ans, expected)
})
