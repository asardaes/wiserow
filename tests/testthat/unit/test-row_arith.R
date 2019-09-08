context("  Row arith")
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

test_that("row_arith behaves as expected for data frames/tables.", {
    df <- dt[, sapply(df, typeof) != "character", with = FALSE]

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
                row <- unlist(df[i, cols, with = FALSE])
                row <- row[!is.na(row)]
                ans <- Reduce(operator, row)
                if (is.null(ans)) 0 else ans
            })
            names(expected) <- NULL

            ans <- row_arith(df, operator, rows = considered_rows, cols = cols, na_action = "exclude")
            expect_equal(ans, expected)

            expected <- sapply(considered_rows, function(i) {
                row <- unlist(df[i, cols, with = FALSE])
                ans <- Reduce(operator, row)
                if (is.null(ans)) FALSE else ans
            })
            names(expected) <- NULL

            ans <- row_arith(df, operator, rows = considered_rows, cols = cols, na_action = "pass")
            expect_equal(ans, expected)
        }
    }
})
