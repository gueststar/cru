Each numbered script is a pipe from stdin to stdout written in
sed. All of them composed in order transform an individual manual page
in nroff format to a similar looking responsive html page using the
Bootstrap framework. If more numbered scripts are added to this
directory, they will be detected and executed in order automatically
by cmake.

Html styling is mostly hard coded except for a few settings defined in
cmake/configure_manpages.cmake.in. These scripts aren't designed to
cope with all aspects of nroff format, but only to work on the subset
of nroff used for manpages in this repo under the man/ directory.

To make a self-hostable html manual readable offline, the stylesheet
bootstrap.min.css must be downloadable by cmake while building.
Otherwise, the html manual is built with links to an external cdn,
making it unreadable offline even if self-hosted.
