context("  Row In")

test_that("row_in handles basic input errors.", {
    expect_error(row_in(matrix()), "empty")
    expect_error(row_in(data.frame()), "empty")
    expect_error(row_in(matrix(), sets = list(list())), "type")
    expect_error(row_in(data.frame(), sets = list(list())), "type")
})

test_that("row_in works for any combination of types.", {
    for (na in list(NA_integer_, NA_real_, NA, NA_character_, NA_complex_)) {
        expected <- row_nas(df, "count")
        ans <- row_in(df, "count", list(na))
        expect_identical(ans, expected)
        ans <- row_in(dt, "count", list(na))
        expect_identical(ans, expected)

        expected <- row_nas(int_na_mat, "none")
        ans <- row_in(int_na_mat, "none", list(na))
        expect_identical(ans, expected)
        ans <- row_in(int_na_mat, "all", list(na), negate = TRUE)
        expect_identical(ans, expected)
    }

    expected <- apply(bool_mat, 1L, function(row) { if (any(row)) which.max(row) else NA_integer_ })
    ans <- row_in(bool_mat, "which_first", list("TRUE"))
    expect_identical(ans, expected)
    ans <- row_in(bool_mat, "which_first", list(1+0i))
    expect_identical(ans, expected)

    expected <- apply(bool_na_mat, 1L, function(row) { sum(!is.na(row)) })
    ans <- row_in(bool_na_mat, "count", list(c(0L, 1L)))
    expect_identical(ans, expected)
    ans <- row_in(bool_na_mat, "count", list(c(0.0, 1.0)))
    expect_identical(ans, expected)
    ans <- row_in(bool_na_mat, "count", list(c(FALSE, TRUE)))
    expect_identical(ans, expected)

    expected <- row_compare(df, "any", "==", "e")
    ans <- row_in(df, "any", list("e"))
    expect_identical(ans, expected)
})

test_that("row_in can handle complex numbers against int/double.", {
    expect_true(row_in(data.frame(-1 + 0i, 1/3 + 0i), "all", list(-1L, 1/3)))
    expect_true(row_in(data.frame(-1 + 1i, 1/3 - 1i), "none", list(-1L, 1/3)))
})
