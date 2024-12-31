# NOTE: row_sums came first, so its tests check most cases

test_that("row_arith correctly infers output mode.", {
    ans <- row_arith(int_mat, "-", cols = 3:2)
    expect_identical(typeof(ans), "integer")
    expect_equal(ans, apply(int_mat[, 3:2], 1L, function(row) { Reduce("-", row) }))

    ans <- row_arith(int_mat, "*", cols = 3:2)
    expect_identical(typeof(ans), "integer")
    expect_equal(ans, apply(int_mat[, 3:2], 1L, function(row) { Reduce("*", row) }))

    ans <- row_arith(int_mat, "/", cols = 3:2)
    expect_identical(typeof(ans), "double")
    expect_equal(ans, apply(int_mat[, 3:2], 1L, function(row) { Reduce("/", row) }))

    ans <- row_arith(bool_mat, "-", cols = 3:2)
    expect_identical(typeof(ans), "integer")
    expect_equal(ans, apply(bool_mat[, 3:2], 1L, function(row) { Reduce("-", row) }))

    ans <- row_arith(bool_mat, "*", cols = 3:2)
    expect_identical(typeof(ans), "integer")
    expect_equal(ans, apply(bool_mat[, 3:2], 1L, function(row) { Reduce("*", row) }))

    ans <- row_arith(bool_mat, "/", cols = 3:2)
    expect_identical(typeof(ans), "double")
    expect_equal(ans, apply(bool_mat[, 3:2], 1L, function(row) { Reduce("/", row) }))
})

test_that("row_arith behaves as expected for data frames.", {
    local_edition(2)

    df <- df[, sapply(df, typeof) != "character"]

    considered_cols <- list(
        paste0("int.V", 1:3),
        paste0("bool.V", 1:3),
        c(paste0("int.V", 1:3), paste0("bool.V", 1:3)),
        1:9, # no complex
        1:12 # all
    )

    considered_rows <- 11:30

    for (operator in c("-", "*", "/")) {
        for (cols in considered_cols) {
            expected <- sapply(considered_rows, function(i) {
                row <- unlist(df[i, cols])
                row <- row[!is.na(row)]
                ans <- Reduce(operator, row)
                if (is.null(ans)) 0 else ans
            })
            names(expected) <- NULL

            ans <- row_arith(df, operator, rows = considered_rows, cols = cols, na_action = "exclude")
            expect_equal(ans, expected)

            expected <- sapply(considered_rows, function(i) {
                row <- unlist(df[i, cols])
                ans <- Reduce(operator, row)
                if (is.null(ans)) FALSE else ans
            })
            names(expected) <- NULL

            ans <- row_arith(df, operator, rows = considered_rows, cols = cols, na_action = "pass")
            expect_equal(ans, expected)
        }
    }
})

test_that("row_arith can accumulate.", {
    local_edition(2)

    rows <- 1001:1500

    expect_error(row_arith(int_mat, cumulative = TRUE, output_class = "vector"), "cumulative")
    expect_error(row_arith(int_mat, cumulative = TRUE, output_class = "list"), "cumulative")
    expect_error(row_arith(df, cumulative = TRUE, output_class = "vector"), "cumulative")
    expect_error(row_arith(df, cumulative = TRUE, output_class = "list"), "cumulative")

    # ----------------------------------------------------------------------------------------------

    expected <- t(apply(int_na_mat[rows,], 1L, cumsum))
    ans <- row_arith(int_na_mat, "+", cumulative = TRUE, rows = rows, na_action = "pass")
    expect_identical(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- t(apply(int_na_mat[rows,], 1L, function(row) {
        ans <- rep(0L, length(row))
        ans[!is.na(row)] <- cumsum(row[!is.na(row)])

        which_na <- which(is.na(row))
        which_na <- setdiff(which_na, 1L)
        if (length(which_na) > 0L) {
            for (j in which_na) ans[j] <- ans[j - 1L]
        }

        ans
    }))
    ans <- row_arith(int_na_mat, "+", cumulative = TRUE, rows = rows, na_action = "exclude")
    expect_identical(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- t(apply(cplx_na_mat[rows,], 1L, cumsum))
    ans <- row_arith(cplx_na_mat, "+", cumulative = TRUE, rows = rows, na_action = "pass")
    expect_equal(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- t(apply(cplx_na_mat[rows,], 1L, function(row) {
        ans <- rep(0L, length(row))
        ans[!is.na(row)] <- cumsum(row[!is.na(row)])

        which_na <- which(is.na(row))
        which_na <- setdiff(which_na, 1L)
        if (length(which_na) > 0L) {
            for (j in which_na) ans[j] <- ans[j - 1L]
        }

        ans
    }))
    ans <- row_arith(cplx_na_mat, "+", cumulative = TRUE, rows = rows, na_action = "exclude")
    expect_equal(ans, expected)

    # ----------------------------------------------------------------------------------------------

    df <- cbind(as.data.frame(int_mat), as.data.frame(dbl_mat))
    expected <- as.data.frame(t(apply(df[rows,], 1L, cumsum)))
    ans <- row_arith(df, "+", cumulative = TRUE, rows = rows)
    expect_equal(ans, expected, check.attributes = FALSE)

    # ----------------------------------------------------------------------------------------------

    df <- cbind(as.data.frame(cplx_mat), as.data.frame(dbl_mat))
    expected <- as.data.frame(t(apply(df[rows,], 1L, cumsum)))
    ans <- row_arith(df, "+", cumulative = TRUE, rows = rows)
    expect_equal(ans, expected, check.attributes = FALSE)
})
