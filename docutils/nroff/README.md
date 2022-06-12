Each numbered script is a pipe from stdin to stdout written in
sed. All of them composed in order transform an individual manual page
in nroff format to one with some substitutions and minor formatting
conveniences. If more numbered scripts are added to this directory,
they will be detected and executed automatically by cmake.

The `10_breaks` script inserts line breaks into the `SEE ALSO`
sections of manual pages to avoid having them fill justified when
rendered by `nroff`.

The `20_antibreaks` script inserts implicit \c directives in nroff
files where they would seem to be an improvement.

The `30_dents` script automatically indents the fields in struct
synopses assuming they occur between opening and closing braces
and the parameter lists in function prototypes if they're broken
across lines.

The `40_code` script tweaks the appearance of verbatim code examples
with extra vertical space before and after them.

The `50_boil` script adds some boilerplate text to various pages.
