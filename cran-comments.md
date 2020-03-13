
## Update to version 0.1.1

- Fixed undefined behavior found by clang's sanitizer.
- Modified tests to account for different error messages.

## Test environments
* Local GNU/Linux, R release
* Local Windows 10, R release
* win-builder (devel and release)
* Travis CI
  + Linux: devel and release
  + OSX: release
* AppVeyor (x32 and x64)

## R CMD check results
* There were no ERRORs or WARNINGs
* There was 1 NOTE:
  + GNU make is a system requirement due to RcppParallel
