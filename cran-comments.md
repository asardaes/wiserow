
## Update to version 0.1.1

- Fixed undefined behavior found by clang's sanitizer.
- Modified tests to account for different error messages.

## Test environments
* Local GNU/Linux, R release
* win-builder (devel and release)
* GitHub CI (devel, release, and oldrel)

## R CMD check results
* There were no ERRORs or WARNINGs
* There was 1 NOTE:
  + GNU make is a system requirement due to RcppParallel
