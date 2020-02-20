
## Initial release
This is the 1st revision for the initial submission of the package.
The following adjustments have been made based on the received feedback:

* Single quotes have been added to 'Boost' in the description.
  Used packages were already quoted.
* The description has been reworded to hopefully make it more understandable.

Some remarks:

* All used packages are included in the DESCRIPTION file,
  but some are only in the LinkingTo section.
  Package 'BH', for instance, doesn't export any functions,
  so it couldn't be included in Depends or Imports.
* No specific functions are mentioned in the description.
  'string_ref' is a class, not a function;
  this detail has been added to the description.
* 'Type promotion according to R rules' means that, for example,
  an integer and a logical can be added together by promoting the logical to an integer.
  R has some particularities in this regard, e.g.,
  promoting a logical to a character results in the strings 'TRUE' or 'FALSE'.
  This has been taken into account in the package,
  and the description now mentions a simple example thereof.

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
