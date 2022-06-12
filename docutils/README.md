This directory is full of sed scripts used at build time for
preparing and formatting nroff and html documentation.
The scripts under nroff/ preprocess the nroff documents
for installation, and the scripts under html/ transform those
results further to html documents.

The `decls` script automatically extracts all declarations from the
header files to populate the `SEE ALSO` sections of manual pages.

The `errs` script extracts error code strings from the source file
errs.c to keep the manual consistent with them.
