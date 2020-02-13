#include "DataFrameColumnCollection.h"

namespace wiserow {

DataFrameColumnCollection::DataFrameColumnCollection(const Rcpp::DataFrame& df, const OperationMetadata& metadata)
    : ColumnCollection(df.nrow())
{
    int upper_j = 0,
        current_j = 0;

    if (metadata.cols.ptr) {
        upper_j = static_cast<int>(metadata.cols.len);
    }
    else if (metadata.cols.is_null) {
        upper_j = df.ncol();
    }

    for (int j = 0; j < upper_j; j++) {
        current_j = metadata.cols.ptr ? metadata.cols.ptr[j] - 1 : j;

        switch(metadata.input_modes[current_j]) {
        case INTSXP: {
            Rcpp::IntegerVector vec(df[current_j]);

            if (!Rf_isFactor(df[current_j]) || metadata.factor_mode == INTSXP) {
                columns_.push_back(std::make_shared<SurrogateColumn<int>>(&vec[0], vec.length()));
            }
            else {
                Rcpp::StringVector levels = vec.attr("levels");
                Rcpp::StringVector factors(vec.length());
                for (R_xlen_t i = 0; i < vec.length(); i++) {
                    if (Rcpp::IntegerVector::is_na(vec[i])) {
                        factors[i] = NA_STRING;
                    }
                    else {
                        factors[i] = levels[vec[i] - 1];
                    }
                }

                columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringVector>>(factors));
            }

            break;
        }
        case REALSXP: {
            Rcpp::NumericVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<double>>(&vec[0], vec.length()));
            break;
        }
        case LGLSXP: {
            Rcpp::LogicalVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<int>>(&vec[0], vec.length(), true));
            break;
        }
        case STRSXP: {
            Rcpp::StringVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::StringVector>>(vec));
            break;
        }
        case CPLXSXP: {
            Rcpp::ComplexVector vec(df[current_j]);
            columns_.push_back(std::make_shared<SurrogateColumn<Rcpp::ComplexVector>>(vec));
            break;
        }
        default: { // nocov start
            // can never happen because OperationMetadata's constructor checks this too
            Rcpp::stop("[wiserow] data frames can only contain integers, doubles, logicals, characters, or complex.");
        } // nocov end
        }
    }
}

} // namespace wiserow
