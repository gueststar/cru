A bash script is derived from `renumber.in` and four sed scripts are
derived from `reserve.in` and `enumerate.in` by cmake at configuration
time and stored in the build directory. They aren't necessary for
anything in production, but a developer invoking `renumber` from the
command line with no parameters consecutively renumbers all of the
error codes and other things requiring distinct numbers in the source
files, thereby fixing duplicates or gaps introduced by changes to the
code. This operation is idempotent.
