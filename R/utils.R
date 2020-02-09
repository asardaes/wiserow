.supported_output_classes <- c(
    "vector",
    "list",
    "data.frame",
    "matrix"
)

.supported_modes <- c(
    "integer",
    "double",
    "logical",
    "character",
    "complex"
)

.supported_na_actions <- c(
    "exclude",
    "pass"
)

.supported_comp_operators <- c(
    "==",
    "!=",
    "<",
    "<=",
    ">",
    ">=",
    "is"
)

#' @importFrom RcppParallel defaultNumThreads
#'
num_workers <- function() {
    as.integer(Sys.getenv("RCPP_PARALLEL_NUM_THREADS", RcppParallel::defaultNumThreads()))
}

#' @importFrom glue glue
#'
validate_metadata <- function(.data, metadata) {
    if (!is.null(metadata$cols)) {
        metadata$cols <- handle_subset_ids(.data, metadata$cols, "column")
    }
    if (!is.null(metadata$rows)) {
        metadata$rows <- handle_subset_ids(.data, metadata$rows, "row")
    }

    nc <- ncol(.data)
    nr <- nrow(.data)

    if (any(metadata$cols > nc)) {
        stop(glue::glue("Invalid column indices, data has {nc} columns, received: ",
                        "[{ paste(metadata$cols, collapse = ',') }]"))
    }
    else if (any(metadata$cols < 1L)) {
        metadata$cols <- setdiff(1L:nc, -1L * metadata$cols)
    }

    if (any(metadata$rows > nr)) {
        stop(glue::glue("Invalid row indices, data has {nr} rows, received: ",
                        "[{ paste(metadata$rows, collapse = ',') }]"))
    }
    else if (any(metadata$rows < 1L)) {
        metadata$rows <- setdiff(1L:nr, -1L * metadata$rows)
    }

    metadata
}

#' @importFrom glue glue
#'
handle_subset_ids <- function(.data, ids, which_dim) {
    ans <- switch(typeof(ids),
                  "integer" = ids,
                  "double" = as.integer(ids),
                  "logical" = which(ids),
                  "character" = {
                      nms <- if (which_dim == "column") colnames(.data) else rownames(.data)
                      if (is.null(nms)) {
                          stop(glue::glue("Cannot index { which_dim }s with characters if ",
                                          "no { which_dim } names are present."))
                      }

                      match(ids, nms)
                  },
                  # default
                  stop(glue::glue("Unsupported type for subset indices: { typeof(ids) }")))

    if (anyNA(ans)) {
        stop("Subsetting indices (rows/cols) cannot have NA values.")
    }

    ans
}

#' @importFrom glue glue
#'
prepare_output <- function(.data, metadata, allow_cols = FALSE) {
    ans_len <- if (is.null(metadata$rows)) nrow(.data) else length(metadata$rows)

    if (allow_cols) {
        ncol <- if (is.null(metadata$cols)) ncol(.data) else length(metadata$cols)
    }
    else {
        ncol <- 1L
    }

    if (metadata$output_class == "vector") {
        ans <- vector(metadata$output_mode, ans_len)
    }
    else if (metadata$output_class == "list") {
        # no rep()! that only does shallow copies
        ans <- lapply(1L:ans_len, function(ignored) { vector(metadata$output_mode, 1L) })
    }
    else if (metadata$output_class == "data.frame") {
        ans <- as.data.frame(lapply(seq_len(ncol), function(ignored) { vector(metadata$output_mode, ans_len) }))
        names(ans) <- paste0("V", 1:ncol(ans))
    }
    else if (metadata$output_class == "matrix") {
        ans <- vector(metadata$output_mode, ans_len * ncol)
        dim(ans) <- c(ans_len, ncol)
    }
    else { # nocov start
        stop(glue::glue("Unsupported output class: {metadata$output_class}"))
    } # nocov end

    ans
}

#' @importFrom glue glue
#'
compute_output_mode <- function(types, not_allowed = "", error_msg = "Unsupported types for this operation: { not_allowed }") {
    if (any(types %in% not_allowed)) {
        stop(glue::glue(error_msg))
    }

    unique_types <- unique(types)

    if (length(unique_types) == 1L && unique_types == "logical") {
        unique_types
    }
    else {
        typeof(do.call(max, lapply(unique_types, function(type) { vector(type, 1L) })))
    }
}
