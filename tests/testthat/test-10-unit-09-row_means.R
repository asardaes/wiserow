test_that("row_means can handle errors and edge cases appropriately.", {
    suppressWarnings(
        expect_error(regexp = "not support", row_means(char_mat, output_mode = "integer"))
    )

    suppressWarnings(
        expect_error(regexp = "not support", row_means(char_mat, output_mode = "integer", cols = -2L))
    )

    suppressWarnings(
        expect_error(regexp = "not support", row_means(char_mat, output_mode = "complex"))
    )

    expect_error(regexp = "row_means can only return", row_means(int_mat, output_mode = "character"))
    expect_error(regexp = "row_means can only return", row_means(int_mat, output_mode = "character", output_class = "list"))

    expect_identical(row_means(as.matrix(data.frame())), vector("double"))
    expect_identical(row_means(data.frame()), vector("double"))

    expect_error(row_means(int_mat, cumulative = TRUE, output_class = "vector"), "cumulative")
    expect_error(row_means(int_mat, cumulative = TRUE, output_class = "list"), "cumulative")
    expect_error(row_means(df, cumulative = TRUE, output_class = "vector"), "cumulative")
    expect_error(row_means(df, cumulative = TRUE, output_class = "list"), "cumulative")
})

test_that("row_means for integer matrices works.", {
    expected <- rowMeans(int_mat)
    ans <- row_means(int_mat)
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_means(int_mat, output_mode = "integer")
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_means(int_mat, output_mode = "integer", output_class = "list")
    expect_identical(ans, expected)
})

test_that("row_means for integer matrices with NAs works.", {
    expected <- rowMeans(int_na_mat, na.rm = TRUE)
    ans <- row_means(int_na_mat)
    expect_equal(ans, expected)

    expected <- as.integer(expected)
    ans <- row_means(int_na_mat, output_mode = "integer")
    expect_equal(ans, expected)

    expected <- rowMeans(int_na_mat)
    ans <- row_means(int_na_mat, na_action = "pass")
    expect_equal(ans, expected)
})

test_that("row_means for integer matrices with column subset works.", {
    expected <- rowMeans(int_mat[, -2L])
    ans <- row_means(int_mat, cols = c(1, 3))
    expect_identical(ans, expected)
    ans <- row_means(int_mat, cols = c(TRUE, FALSE, TRUE))
    expect_identical(ans, expected)

    ans <- row_means(int_mat, cols = -2L)
    expect_identical(ans, expected)

    ans <- row_means(int_mat, cols = integer())
    expect_identical(ans, rep(NaN, length(ans)))

    expect_error(row_means(int_mat, cols = 10L), "Invalid column indices")

    expected <- rowMeans(int_mat[, c(1L, 1L, 3L)])
    ans <- row_means(int_mat, cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_means for integer matrices with row subset works.", {
    expected <- rowMeans(int_mat[1001:2000,])
    ans <- row_means(int_mat, rows = 1001:2000)
    expect_identical(ans, expected)
    ans <- row_means(int_mat, rows = 1:5000 %in% 1001:2000)
    expect_identical(ans, expected)

    ans <- row_means(int_mat, rows = -c(1:1000, 2001:5000))
    expect_identical(ans, expected)

    ans <- row_means(int_mat, rows = integer())
    expect_identical(ans, double())

    expect_error(row_means(int_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowMeans(int_mat[rep(1001:2000, 2),])
    ans <- row_means(int_mat, rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_means for double matrices works.", {
    expected <- rowMeans(dbl_mat)
    ans <- row_means(dbl_mat)
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_means(dbl_mat, output_class = "list")
    expect_identical(ans, expected)

    expected <- as.integer(expected)
    ans <- row_means(dbl_mat, output_mode = "integer")
    expect_identical(ans, expected)
})

test_that("row_means for double matrices with NAs works.", {
    expected <- rowMeans(dbl_na_mat, na.rm = TRUE)
    ans <- row_means(dbl_na_mat)
    expect_equal(ans, expected)

    expected <- as.integer(expected)
    ans <- row_means(dbl_na_mat, output_mode = "integer")
    expect_equal(ans, expected)

    expected <- rowMeans(dbl_na_mat)
    ans <- row_means(dbl_na_mat, na_action = "pass")
    expect_equal(ans, expected)
})

test_that("row_means for double matrices with column subset works.", {
    expected <- rowMeans(dbl_mat[, -2L])
    ans <- row_means(dbl_mat, cols = c(1, 3))
    expect_identical(ans, expected)
    ans <- row_means(dbl_mat, cols = c(TRUE, FALSE, TRUE))
    expect_identical(ans, expected)

    ans <- row_means(dbl_mat, cols = -2L)
    expect_identical(ans, expected)

    ans <- row_means(dbl_mat, cols = integer())
    expect_identical(ans, rep(NaN, length(ans)))

    expect_error(row_means(dbl_mat, cols = 10L), "Invalid column indices")

    expected <- rowMeans(dbl_mat[, c(1L, 1L, 3L)])
    ans <- row_means(dbl_mat, cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_means for double matrices with row subset works.", {
    expected <- rowMeans(dbl_mat[1001:2000,])
    ans <- row_means(dbl_mat, rows = 1001:2000)
    expect_identical(ans, expected)
    ans <- row_means(dbl_mat, rows = 1:5000 %in% 1001:2000)
    expect_identical(ans, expected)

    ans <- row_means(dbl_mat, rows = -c(1:1000, 2001:5000))
    expect_identical(ans, expected)

    ans <- row_means(dbl_mat, rows = integer())
    expect_identical(ans, double())

    expect_error(row_means(dbl_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowMeans(dbl_mat[rep(1001:2000, 2),])
    ans <- row_means(dbl_mat, rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_means for logical matrices works.", {
    expected <- rowMeans(bool_mat)
    ans <- row_means(bool_mat)
    expect_equal(ans, expected)

    expected <- as.logical(expected)
    ans <- row_means(bool_mat, output_mode = "logical")
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_means(bool_mat, output_mode = "logical", output_class = "list")
    expect_identical(ans, expected)
})

test_that("row_means for logical matrices with NAs works.", {
    expected <- rowMeans(bool_na_mat, na.rm = TRUE)
    ans <- row_means(bool_na_mat)
    expect_equal(ans, expected)

    expected <- as.logical(expected)
    ans <- row_means(bool_na_mat, output_mode = "logical")
    expect_identical(ans, expected)

    expected <- rowMeans(bool_na_mat)
    ans <- row_means(bool_na_mat, na_action = "pass")
    expect_equal(ans, expected)
})

test_that("row_means for logical matrices with column subset works.", {
    expected <- rowMeans(bool_mat[, -2L])
    ans <- row_means(bool_mat, cols = c(1, 3))
    expect_identical(ans, expected)
    ans <- row_means(bool_mat, cols = c(TRUE, FALSE, TRUE))
    expect_identical(ans, expected)

    ans <- row_means(bool_mat, cols = -2L)
    expect_identical(ans, expected)

    ans <- row_means(bool_mat, cols = integer())
    expect_identical(ans, rep(NaN, length(ans)))

    expect_error(row_means(bool_mat, cols = 10L), "Invalid column indices")

    expected <- rowMeans(bool_mat[, c(1L, 1L, 3L)])
    ans <- row_means(bool_mat, cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_means for logical matrices with row subset works.", {
    expected <- rowMeans(bool_mat[1001:2000,])
    ans <- row_means(bool_mat, rows = 1001:2000)
    expect_identical(ans, expected)
    ans <- row_means(bool_mat, rows = 1:5000 %in% 1001:2000)
    expect_identical(ans, expected)

    ans <- row_means(bool_mat, rows = -c(1:1000, 2001:5000))
    expect_identical(ans, expected)

    ans <- row_means(bool_mat, rows = integer())
    expect_identical(ans, double())

    expect_error(row_means(bool_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowMeans(bool_mat[rep(1001:2000, 2),])
    ans <- row_means(bool_mat, rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_means for complex matrices works.", {
    expected <- rowMeans(cplx_mat)
    ans <- row_means(cplx_mat)
    expect_identical(ans, expected)

    expected <- as.list(expected)
    ans <- row_means(cplx_mat, output_class = "list")
    expect_identical(ans, expected)

    expected <- rowMeans(Mod(cplx_mat))
    ans <- row_means(cplx_mat, output_mode = "double")
    expect_equal(ans, expected)

    expected <- as.integer(Mod(cplx_mat))
    dim(expected) <- dim(cplx_mat)
    expected <- as.integer(rowMeans(expected))
    ans <- row_means(cplx_mat, output_mode = "integer")
    expect_equal(ans, expected)
})

test_that("row_means for complex matrices with NAs works.", {
    local_edition(2)

    expected <- rowMeans(cplx_na_mat, na.rm = TRUE)
    ans <- row_means(cplx_na_mat)
    expect_equal(ans, expected)

    expected <- rowMeans(cplx_na_mat)
    ans <- row_means(cplx_na_mat, na_action = "pass")
    expect_equal(ans, expected)
})

test_that("row_means for complex matrices with column subset works.", {
    local_edition(2)

    expected <- rowMeans(cplx_mat[, -2L])
    ans <- row_means(cplx_mat, cols = c(1, 3))
    expect_equal(ans, expected)
    ans <- row_means(cplx_mat, cols = c(TRUE, FALSE, TRUE))
    expect_equal(ans, expected)

    ans <- row_means(cplx_mat, cols = -2L)
    expect_equal(ans, expected)

    ans <- row_means(cplx_mat, cols = integer())
    expect_equal(ans, rep(NaN * (1 + 1i), length(ans)))

    expect_error(row_means(cplx_mat, cols = 10L), "Invalid column indices")

    expected <- rowMeans(cplx_mat[, c(1L, 1L, 3L)])
    ans <- row_means(cplx_mat, cols = c(1L, 1L, 3L))
    expect_identical(ans, expected)
})

test_that("row_means for complex matrices with row subset works.", {
    expected <- rowMeans(cplx_mat[1001:2000,])
    ans <- row_means(cplx_mat, rows = 1001:2000)
    expect_equal(ans, expected)
    ans <- row_means(cplx_mat, rows = 1:5000 %in% 1001:2000)
    expect_equal(ans, expected)

    ans <- row_means(cplx_mat, rows = -c(1:1000, 2001:5000))
    expect_equal(ans, expected)

    ans <- row_means(cplx_mat, rows = integer())
    expect_equal(ans, complex())

    expect_error(row_means(cplx_mat, rows = 10000.0), "Invalid row indices")

    expected <- rowMeans(cplx_mat[rep(1001:2000, 2),])
    ans <- row_means(cplx_mat, rows = rep(1001:2000, 2))
    expect_identical(ans, expected)
})

test_that("row_means for data frames works.", {
    df <- df[, sapply(df, typeof) != "character"]

    considered_cols <- list(
        paste0("int.V", 1:3),
        paste0("bool.V", 1:3),
        c(paste0("int.V", 1:3), paste0("bool.V", 1:3)),
        1:9, # no complex
        1:12 # all
    )

    for (cols in considered_cols) {
        # rowMeans doesn't seem to be working here, don't know why
        expected <- apply(df[1001:5000, cols], 1L, mean, na.rm = TRUE)
        names(expected) <- NULL

        ans <- row_means(df, rows = 1001:5000, cols = cols, na_action = "exclude")
        expect_equal(ans, expected)

        ans <- row_means(df, rows = as.character(1001:5000), cols = cols, na_action = "exclude", output_class = "list")
        expect_equal(ans, as.list(expected))
    }

    # ----------------------------------------------------------------------------------------------

    expected <- as.logical(rowMeans(df[-10L, 7:9]))
    ans <- row_means(df, rows = -10L, cols = 7:9, na_action = "pass", output_mode = "logical")
    expect_identical(ans, expected)
})

test_that("row_means can accumulate.", {
    local_edition(2)

    cummeans <- function(x, na.rm = FALSE) {
        if (!na.rm) return(cumsum(x) / seq_along(x))

        is_na <- is.na(x)
        x[is_na] <- vector(typeof(x), 1L)

        ans <- cumsum(x)
        n <- seq_along(ans)

        if (any(is_na)) {
            for (i in which(is_na)) {
                n[i:length(n)] <- n[i:length(n)] - 1L
            }
        }

        ans <- ans / n
        ans[is.nan(ans)] <- vector(typeof(x), 1L)
        ans
    }

    for (mat in list(int_na_mat, dbl_na_mat, bool_na_mat, cplx_na_mat)) {
        expected <- t(apply(mat, 1L, cummeans))
        ans <- row_means(mat, cumulative = TRUE, na_action = "pass")
        expect_equal(ans, expected)

        expected <- t(apply(mat, 1L, cummeans, na.rm = TRUE))
        ans <- row_means(mat, cumulative = TRUE, na_action = "exclude")
        expect_equal(ans, expected)
    }
})
