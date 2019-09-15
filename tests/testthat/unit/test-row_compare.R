context("  Row Compare")

test_that("row_compare throws if an element of values has length != 0", {
    expect_error(row_compare(dbl_mat, values = list(1:2)), "length equal to 1")
    expect_error(row_compare(df, values = list(1:2)), "length equal to 1")
    expect_error(row_compare(dt, values = list(1:2)), "length equal to 1")
})

test_that("row_compare throws for invalid types in values.", {
    expect_error(row_compare(df, values = list(list(1))))
})

test_that("row_compare works when target is logical.", {
    expect_true(row_compare(matrix(1L), "all", "==", TRUE))
    expect_true(row_compare(matrix(1.5), "all", ">", TRUE))

    expect_true(row_compare(matrix(0+0i), "all", "==", FALSE))

    expect_false(row_compare(matrix(0+1i), "all", "==", TRUE))
    expect_false(row_compare(matrix(0+1i), "all", "==", FALSE))

    expect_true(row_compare(matrix(1+0i), "all", "==", TRUE))
    expect_false(row_compare(matrix(2+0i), "all", "==", TRUE))
    expect_false(row_compare(matrix(2+0i), "all", "==", FALSE))

    expect_false(row_compare(matrix(-1+0i), "all", "==", TRUE))
    expect_false(row_compare(matrix(-1+0i), "all", "==", FALSE))
    expect_false(row_compare(matrix(-1+1i), "all", "==", TRUE))
    expect_false(row_compare(matrix(-1-1i), "all", "==", TRUE))

    expect_true(row_compare(matrix("TRUE"), "all", "==", TRUE))
    expect_true(row_compare(matrix("FALSE"), "all", "==", FALSE))
})

test_that("row_compare works when target value is NA.", {
    df <- data.frame(bool = NA,
                     int = NA_integer_,
                     dbl = NA_real_,
                     char = NA_character_,
                     cplx = NA_complex_)

    expect_true(row_compare(df, "all", "==", NA))
    expect_true(row_compare(df, "all", "is", NA_integer_))
    expect_true(row_compare(df, "all", "==", NA_real_))
    expect_true(row_compare(df, "all", "is", NA_character_))
    expect_true(row_compare(df, "all", "==", NA_complex_))

    expect_true(row_compare(df, "none", "!=", NA))
    expect_true(row_compare(df, "none", "!=", NA_integer_))
    expect_true(row_compare(df, "none", "!=", NA_real_))
    expect_true(row_compare(df, "none", "!=", NA_character_))
    expect_true(row_compare(df, "none", "!=", NA_complex_))

    expect_true(is.na(row_compare(df, "any", "<", NA)))
    expect_true(is.na(row_compare(df, "any", "<=", NA)))
    expect_true(is.na(row_compare(df, "any", ">", NA)))
    expect_true(is.na(row_compare(df, "any", ">=", NA)))
})

test_that("row_compare works when target value is infinite.", {
    df <- data.frame(dbl = Inf,
                     cplx = as.complex(Inf),
                     char = "Inf")

    expect_true(row_compare(df, "all", "==", Inf))
    expect_true(row_compare(df[, -3L], "all", "is", as.complex(Inf)))
    expect_true(row_compare(df[, -2L], "all", "==", "Inf"))

    expect_true(row_compare(df, "none", "!=", Inf))
    expect_true(row_compare(df[, -3L], "none", "!=", as.complex(Inf)))
    expect_true(row_compare(df[, -2L], "none", "!=", "Inf"))

    df <- data.frame(dbl = -Inf,
                     cplx = as.complex(-Inf),
                     char = "-Inf")

    expect_true(row_compare(df, "all", "==", -Inf))
    expect_true(row_compare(df[, -3L], "all", "is", as.complex(-Inf)))
    expect_true(row_compare(df[, -2L], "all", "==", "-Inf"))

    expect_true(row_compare(df, "none", "!=", -Inf))
    expect_true(row_compare(df[, -3L], "none", "!=", as.complex(-Inf)))
    expect_true(row_compare(df[, -2L], "none", "!=", "-Inf"))

    expect_true(row_compare(matrix(0L), "any", "<", Inf))
    expect_true(row_compare(matrix(Inf), "any", "<=", Inf))
    expect_true(row_compare(matrix("0"), "any", "<", Inf))
    expect_true(row_compare(matrix(Inf), "any", "<=", "Inf"))

    expect_true(row_compare(matrix(0L), "any", ">", -Inf))
    expect_true(row_compare(matrix(-Inf), "any", ">=", -Inf))
    expect_true(row_compare(matrix("0"), "any", ">", -Inf))
    expect_true(row_compare(matrix(-Inf), "any", ">=", "-Inf"))

    expect_error(row_compare(matrix(complex(1, 0, 1)), "any", "<", Inf))
    expect_error(row_compare(matrix(complex(1, 0, 1)), "any", "<=", as.complex(Inf)))
    expect_error(row_compare(matrix(complex(1, 0, 1)), "any", ">", -Inf))
    expect_error(row_compare(matrix(complex(1, 0, 1)), "any", ">=", as.complex(-Inf)))
})

test_that("row_compare behaves like R for int LHS and complex RHS.", {
    expect_true(sum(row_compare(int_mat, "all", "==", as.complex(c(5, 5005, 10005)))) == 1L)
})

test_that("values converted to characters are correctly formatted for row_compare's comparisons.", {
    expect_true(row_compare(data.frame(0.0, 0L, 0.), "all", "==", "0"))
    expect_true(row_compare(data.frame(1.0, 1L, 1.), "all", "==", "1"))
    expect_true(row_compare(data.frame(1.5), "any", "==", "1.5"))
    expect_true(row_compare(data.frame(1.5), "any", ">", "1.4"))
    expect_true(row_compare(data.frame(1.5), "any", "<", "1.6"))
    expect_true(row_compare(data.frame(1.5), "any", ">=", "1.5"))
    expect_true(row_compare(data.frame(1.5), "any", "<=", "1.5"))

    expect_true(row_compare(data.frame(complex(1, 0, 0)), "none", "!=", "0+0i"))
    expect_true(row_compare(data.frame(complex(1, Inf, 0)), "none", "!=", "Inf+0i"))
    expect_true(row_compare(data.frame(complex(1, -Inf, 0)), "none", "!=", "-Inf+0i"))
    expect_true(row_compare(data.frame(complex(1, Inf, -1)), "none", "!=", "Inf-1i"))
    expect_true(row_compare(data.frame(complex(1, -Inf, -1)), "none", "!=", "-Inf-1i"))
    expect_true(row_compare(data.frame(complex(1, 0, -1)), "none", "!=", "0-1i"))

    expect_true(row_compare(matrix("TRUE"), "any", "is", TRUE))
    expect_true(row_compare(matrix("FALSE"), "any", "is", FALSE))

    expect_true(row_compare(matrix(TRUE), "any", "is", "TRUE"))
    expect_true(row_compare(matrix(FALSE), "any", "is", "FALSE"))

    expect_true(row_compare(matrix("0"), "none", "!=", 0L))
    expect_true(row_compare(matrix("1.5"), "all", "==", 1.5))
    expect_true(row_compare(matrix("-1+1i"), "all", "==", -1+1i))
    expect_true(row_compare(matrix("1"), "any", "<", 2L))
    expect_true(row_compare(matrix("2"), "any", "<=", 2L))
    expect_true(row_compare(matrix("1"), "any", ">", 0L))
    expect_true(row_compare(matrix("0"), "any", ">=", 0L))
})

test_that("row_compare respects na_action.", {
    cols <- which(sapply(df, typeof) != "complex")

    expected <- sapply(4001:5000, df = df[, cols], function(i, df) { all(sapply(df[i, , drop = FALSE], ">", 0)) })

    ans <- row_compare(df, "all", ">", values = 0, na_action = "pass", rows = 4001:5000, cols = cols)
    expect_identical(ans, expected)
    ans <- row_compare(dt, "all", ">", values = 0, na_action = "pass", rows = 4001:5000, cols = cols)
    expect_identical(ans, expected)

    expected <- sapply(4001:5000, df = df[, cols], function(i, df) { all(sapply(df[i, , drop = FALSE], ">", 0), na.rm = TRUE) })

    ans <- row_compare(df, "all", ">", values = 0, na_action = "exclude", rows = 4001:5000, cols = cols)
    expect_identical(ans, expected)
    ans <- row_compare(dt, "all", ">", values = 0, na_action = "exclude", rows = 4001:5000, cols = cols)
    expect_identical(ans, expected)
})

test_that("row_sums for all/any/none behaves like R's all/any/any(!flags) with regards to NA.", {
    expected <- apply(int_na_mat, 1L, function(row) { all(row <= seq(from = 100L, by = 5000L, length.out = 3L)) })
    ans <- row_compare(int_na_mat, "all", "<=", seq(from = 100L, by = 5000L, length.out = 3L), na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(int_na_mat, 1L, function(row) { any(row <= seq(from = 100L, by = 5000L, length.out = 3L)) })
    ans <- row_compare(int_na_mat, "any", "<=", seq(from = 100L, by = 5000L, length.out = 3L), na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(int_na_mat, 1L, function(row) { all(row > seq(from = 100L, by = 5000L, length.out = 3L)) })
    ans <- row_compare(int_na_mat, "none", "<=", seq(from = 100L, by = 5000L, length.out = 3L), na_action = "pass")
    expect_identical(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- apply(bool_na_mat, 1L, function(row) { all(row > FALSE) })
    ans <- row_compare(bool_na_mat, "all", ">", FALSE, na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(bool_na_mat, 1L, function(row) { any(row > FALSE) })
    ans <- row_compare(bool_na_mat, "any", ">", FALSE, na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(bool_na_mat, 1L, function(row) { all(row == FALSE) })
    ans <- row_compare(bool_na_mat, "none", ">", FALSE, na_action = "pass")
    expect_identical(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- apply(dbl_na_mat, 1L, function(row) { all(row >= c(100.5, 5100.5, 10100.5)) })
    ans <- row_compare(dbl_na_mat, "all", ">=", c(100.5, 5100.5, 10100.5), na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(dbl_na_mat, 1L, function(row) { any(row >= c(100.5, 5100.5, 10100.5)) })
    ans <- row_compare(dbl_na_mat, "any", ">=", c(100.5, 5100.5, 10100.5), na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(dbl_na_mat, 1L, function(row) { all(row < c(100.5, 5100.5, 10100.5)) })
    ans <- row_compare(dbl_na_mat, "none", ">=", c(100.5, 5100.5, 10100.5), na_action = "pass")
    expect_identical(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- apply(char_na_mat, 1L, function(row) { all(row != "m") })
    ans <- row_compare(char_na_mat, "all", "!=", "m", na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(char_na_mat, 1L, function(row) { any(row != "m") })
    ans <- row_compare(char_na_mat, "any", "!=", "m", na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(char_na_mat, 1L, function(row) { all(row == "m") })
    ans <- row_compare(char_na_mat, "none", "!=", "m", na_action = "pass")
    expect_identical(ans, expected)

    # ----------------------------------------------------------------------------------------------

    expected <- apply(cplx_na_mat, 1L, function(row) { all(row == 10+10i) })
    ans <- row_compare(cplx_na_mat, "all", "is", 10+10i, na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(cplx_na_mat, 1L, function(row) { any(row == 10+10i) })
    ans <- row_compare(cplx_na_mat, "any", "is", 10+10i, na_action = "pass")
    expect_identical(ans, expected)

    expected <- apply(cplx_na_mat, 1L, function(row) { all(row != 10+10i) })
    ans <- row_compare(cplx_na_mat, "none", "is", 10+10i, na_action = "pass")
    expect_identical(ans, expected)
})

test_that("row_compare can support which_first.", {
    expected <- apply(int_mat, 1L, function(row) { which.max(row > 500L) })
    ans <- row_compare(int_mat, "which_first", ">", 500)
    expect_identical(ans, expected)
})
