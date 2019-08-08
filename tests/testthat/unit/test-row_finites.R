context("  Row Finites")

test_that("row_finites works as expected.", {
    dbl_cols <- sapply(df, is.double)
    cplx_cols <- sapply(df, is.complex)

    df[2001:4000, dbl_cols] <- lapply(df[2001:4000, dbl_cols], function(x) { replace(x, is.na(x), Inf) })
    df[2001:4000, cplx_cols] <- lapply(df[2001:4000, cplx_cols], function(x) { replace(x, is.na(x), as.complex(Inf)) })

    expected <- sapply(4001:5000, df = df, function(i, df) { all(sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "all", rows = 4001:5000)
    expect_identical(ans, expected)
    ans <- row_finites(df, "all", rows = 4001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(4001:5000, df = df, function(i, df) { all(!sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "none", rows = 4001:5000)
    expect_identical(ans, expected)
    ans <- row_finites(df, "none", rows = 4001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expected <- sapply(4001:5000, df = df, function(i, df) { any(sapply(df[i, , drop = FALSE], is.finite)) })
    ans <- row_finites(df, "any", rows = 4001:5000)
    expect_identical(ans, expected)
    ans <- row_finites(df, "any", rows = 4001:5000, output_class = "list")
    expect_identical(ans, as.list(expected))

    expect_true(all(row_finites(int_mat, "all")))
    expect_true(all(row_finites(dbl_mat, "all")))
    expect_true(all(row_finites(bool_mat, "all")))
    expect_true(all(row_finites(cplx_mat, "all")))
})

test_that("row_finites behaves like in R for NA/NaN values.", {
    df <- data.frame(NaN, NA_real_, NA_complex_, NA_integer_, NA_character_, NA)
    expect_true(row_finites(df, "none"))
})
