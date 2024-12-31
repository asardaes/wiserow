test_that("row_duplicated throws for invalid output class.", {
    expect_error(row_duplicated(int_mat, output_class = "vector"), "output class")
    expect_error(row_duplicated(df, output_class = "vector"), "output class")
})

# since matrices have same type, we test here general workflow without caring about promotions
test_that("row_duplicated works as expected for matrices.", {
    for (mat in list(bool_na_mat, int_na_mat, dbl_na_mat, cplx_na_mat, char_na_mat)) {
        ground_truth <- t(apply(mat, 1L, duplicated))
        ans <- row_duplicated(mat)
        expect_identical(ans, ground_truth)

        expected <- apply(ground_truth, 1L, any)
        ans <- row_duplicated(mat, "any")
        expect_identical(ans, expected, "any")

        expected <- apply(ground_truth, 1L, function(row) { all(!row) })
        ans <- row_duplicated(mat, "none")
        expect_identical(ans, expected, "none")

        expected <- apply(ground_truth, 1L, function(row) { if (any(row)) which.max(row) else NA_integer_ })
        ans <- row_duplicated(mat, "which_first")
        expect_identical(ans, expected, "which_first")

        expected <- apply(ground_truth, 1L, sum)
        ans <- row_duplicated(mat, "count")
        expect_identical(ans, expected, "count")
    }
})

test_that("row_duplicated's internal promotions work as expected.", {
    types <- c("bool", "int", "dbl", "cplx", "char")
    for (start_type in types) {
        for (target_type in setdiff(types, start_type)) {
            cols <- c(paste(start_type, paste0("V", 1:3), sep = "."),
                      paste(target_type, paste0("V", 1:3), sep = "."))

            expected <- as.data.frame(t(apply(df[, cols], 1L, duplicated)))
            ans <- row_duplicated(df, cols = cols)
            expect_equal(ans, expected, ignore_attr = TRUE)
        }
    }
})
