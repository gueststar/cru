cmake_minimum_required(VERSION 3.13.4)

# Builds have been tested with clang and gcc. Uncomment at most
# one. If neither is chosen, the system default is used.

# set(CMAKE_C_COMPILER clang)
# set(CMAKE_C_COMPILER gcc)

project(
  cru VERSION 0.2.0
  DESCRIPTION "co-recursion utilities"
  HOMEPAGE_URL https://github.com/gueststar/cru
  LANGUAGES C)

string (TOUPPER ${CMAKE_PROJECT_NAME} MANUAL_TITLE)

set (MAINTAINER_EMAIL milonga@delayinsensitive.com)

include(GNUInstallDirs)

# See https://airbus-seclab.github.io/c-compiler-security/ for
# explanations of these options and further verifications worth
# trying. -fsanitize=safe-stack is omitted because it caused a linker
# error. -Wstrict-prototypes is omitted to allow old style function
# prototypes. -Wconditional-uninitialized is omitted because of too
# many false positives. -Werror=cast-function-type, -Wextra, and
# -Wbad-function-cast are omitted because they break compatibility of
# void pointers with uintptr_t in function type casts. -Wall is
# omitted because of too many unused-but-set variable false positives.
#
# Although using these compiler flags is considered good practice,
# future versions of these compilers might break the build by
# introducing new kinds of warnings based on future fads, which will
# be treated as errors. If the repo is old and not building, try
# disabling these options by deleting everything from "if" down to
# "endif()" inclusive.

if (CMAKE_C_COMPILER_ID MATCHES Clang)
  add_compile_options(-O2 -Werror -Walloca -Wcast-qual -Wconversion
         -Wformat=2 -Wformat-security -Wnull-dereference -Wstack-protector
         -Wstrict-overflow=3 -Wvla -Warray-bounds
         -Warray-bounds-pointer-arithmetic -Wassign-enum -Wconversion
         -Wfloat-equal -Wformat-type-confusion -Widiomatic-parentheses
         -Wimplicit-fallthrough -Wloop-analysis -Wpointer-arith
         -Wshift-sign-overflow -Wshorten-64-to-32 -Wswitch-enum
         -Wtautological-constant-in-range-compare -Wunreachable-code-aggressive
         -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fPIC
         -fstack-clash-protection)
elseif (CMAKE_C_COMPILER_ID MATCHES GNU)
   add_compile_options(-O2 -Werror -Wpedantic -Wformat=2
         -Wformat-overflow=2 -Wformat-truncation=2 -Wformat-security
         -Wnull-dereference -Wstack-protector -Wtrampolines -Walloca -Wvla
         -Warray-bounds=2 -Wimplicit-fallthrough=3 -Wtraditional-conversion
         -Wshift-overflow=2 -Wcast-qual -Wstringop-overflow=4 -Wconversion
         -Warith-conversion -Wlogical-op -Wduplicated-cond -Wformat-signedness -Wshadow
         -Wstrict-overflow=4 -Wundef -Wswitch-default -Wswitch-enum
         -Wstack-usage=1000000 -Wcast-align=strict -fstack-protector-strong
         -fstack-clash-protection -fPIC -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack
         -Wl,-z,separate-code)
else()
   message (STATUS "unknown C compiler ${CMAKE_C_COMPILER_ID}; build errors possible")
endif()

# --------------- markdown documents ----------------------------------------------------------------------

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
  DESTINATION ${CMAKE_INSTALL_DOCDIR})

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/img
  DESTINATION ${CMAKE_INSTALL_DOCDIR})

# --------------- header files ----------------------------------------------------------------------------

install(
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/cru
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

find_program(SED sed)
find_program(BASH bash)
find_program(GREP grep)
find_program(FIND find)
find_program(XARGS xargs)

if (NOT SED)
  message(FATAL_ERROR "sed utility not found")
elseif (NOT GREP)
  message(FATAL_ERROR "grep utility not found")
elseif (NOT FIND)
  message(FATAL_ERROR "find utility not found")
elseif (NOT XARGS)
  message(FATAL_ERROR "xargs utility not found")
endif ()

# Any header file among src/*.h containing a declaration of the form
#
# struct foo_s
#
# starting at the beginning of a line such that cru_ is not a prefix
# of foo causes a matching declaration of
#
# typedef struct foo_s *foo;
#
# in ptr.h, which is included in most other headers, via the variable
# @POINTERS@ in ptr.h.in. This approach is as good as any to deal with
# cyclic dependences among header files. Multiple shell commands are
# needed below because cmake restricts pipes and wild cards, but it
# implicitly pipes the result from each command to the next when run
# through ${CMAKE_COMMAND} -E env.

execute_process(
  OUTPUT_VARIABLE POINTERS
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
  COMMAND ${CMAKE_COMMAND} -E env ${FIND} . -name "*.h"
  COMMAND ${CMAKE_COMMAND} -E env ${XARGS} ${GREP} -h -E "^struct[[:blank:]]+[(a-z)|_]+"
  COMMAND ${CMAKE_COMMAND} -E env ${GREP} -v -E "^struct[[:blank:]]+cru_"
  COMMAND ${CMAKE_COMMAND} -E env ${SED} -e "s/\\(struct\\ \\)\\(\\ *\\)\\(.*\\)_s/typedef \\1\\3_s\\ \\*\\3;/"
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# the number of source files containing one or more call sites to
# _cru_malloc; see wrap.c

execute_process(
  OUTPUT_VARIABLE MALLOX
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src
  COMMAND ${CMAKE_COMMAND} -E env ${FIND} . -name "*.c"
  COMMAND ${CMAKE_COMMAND} -E env ${XARGS} ${GREP} -l ")\\ _cru_malloc"
  COMMAND ${CMAKE_COMMAND} -E env ${SED} -n $=
  OUTPUT_STRIP_TRAILING_WHITESPACE)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/src/ptr.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/ptr.h)

# --------------- manual pages ----------------------------------------------------------------------------

# Manual pages are compressed and configured with current dates
# etc. by a separate cmake script so that editing them will enable a
# rebuild.

# Building manpages and html documentation can be disabled optionally.
option (GEN_DOCS "generate documentation" ON)

# Building manpages or html documentation requires these two utilities.
find_program(DATE date)
find_program(SORT sort)

# This one is optional.
find_program(GZIP gzip)

if (GZIP)
  set (GZIP_MANPAGES ${GZIP} -rf man)
else ()
  set (GZIP_MANPAGES "")
  message (STATUS "gzip not found; manual pages not compressed")
endif ()

option (HTML_DOCS "translate documentation to html format" ON)
if (HTML_DOCS)
  set (HTML_DOCS_ON 1)
endif ()

configure_file(
  cmake/configure_manpages.cmake.in configure_manpages.cmake
  @ONLY)

if (${CMAKE_VERSION} VERSION_LESS 3.17)      # may have to use less portable commands
  add_custom_command(
	 OUTPUT man
	 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/man/*/*
	 COMMAND rm -rf man
	 COMMAND ${CMAKE_COMMAND} -P configure_manpages.cmake
	 COMMAND cp -r ${CMAKE_CURRENT_BINARY_DIR}/configured_man ${CMAKE_CURRENT_BINARY_DIR}/man
	 COMMAND ${GZIP_MANPAGES})
else ()
  add_custom_command(
	 OUTPUT man
	 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/man/*/*
	 COMMAND ${CMAKE_COMMAND} -E rm -rf man
	 COMMAND ${CMAKE_COMMAND} -P configure_manpages.cmake
	 COMMAND ${CMAKE_COMMAND} -E copy_directory configured_man man
	 COMMAND ${GZIP_MANPAGES})
endif ()

add_custom_target(
  manpages
  DEPENDS man)

if (NOT GEN_DOCS)
  message(STATUS "not building documentation")
elseif (NOT SORT)
  message(STATUS "sort utility not found; documentation not buildable")
elseif (NOT DATE)
  message(STATUS "date utility not found; documentation not buildable")
else ()
  install(
	 DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/man3 ${CMAKE_CURRENT_BINARY_DIR}/man/man7
	 DESTINATION ${CMAKE_INSTALL_MANDIR})
  if (HTML_DOCS)
	 install(
		DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html
		DESTINATION ${CMAKE_INSTALL_DOCDIR})
  endif ()
endif ()

# --------------- utilities -------------------------------------------------------------------------------

# scripts useful during development to maintain consecutively numbered
# error codes in source files

if (NOT BASH)
  message(WARNING "bash shell not found; developer scripts unavailable")
else ()
  string (TOUPPER ${CMAKE_PROJECT_NAME} PROJECT_NAMESPACER)
  configure_file (
	 ${CMAKE_CURRENT_SOURCE_DIR}/devutils/renumber.in ${CMAKE_CURRENT_BINARY_DIR}/renumber
	 @ONLY)
  configure_file (
	 ${CMAKE_CURRENT_SOURCE_DIR}/devutils/reserve.in ${CMAKE_CURRENT_BINARY_DIR}/reserve
	 @ONLY)
  set (ERROR_CODE_MACRO "IER\\|FAIL")
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/devutils/enumerate.in ${CMAKE_CURRENT_BINARY_DIR}/numb
	 @ONLY)
  set (ERROR_CODE_MACRO "MUGGLE")
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/devutils/enumerate.in ${CMAKE_CURRENT_BINARY_DIR}/mug
	 @ONLY)
  set (ERROR_CODE_MACRO "KILL_SITE")
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/devutils/enumerate.in ${CMAKE_CURRENT_BINARY_DIR}/kin
	 @ONLY)
endif ()

# --------------- uninstall target ------------------------------------------------------------------------

if(NOT TARGET uninstall)
  configure_file(
    cmake/cmake_uninstall.cmake.in cmake_uninstall.cmake
    @ONLY)
  add_custom_target(
    uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

# --------------- shared library --------------------------------------------------------------------------

# The option -g can be used to make valgrind display line
# numbers. This option should be commented out for production code
# builds.

# add_compile_options (-g)

# The library depends on every .c file in the src/ directory, so this
# part saves listing all forty-something of them. Note that some
# schools of thought deem it bad cmake practice to avoid enumerating
# them explicitly.

execute_process(
  OUTPUT_VARIABLE C_FILES
  COMMAND ${FIND} ${CMAKE_CURRENT_SOURCE_DIR}/src -name "*.c"
  OUTPUT_STRIP_TRAILING_WHITESPACE)

string(REPLACE "\n" ";" C_FILES ${C_FILES})

add_library(cru SHARED ${C_FILES})

# It's standard practice on GNU/Linux systems to install a shared
# library with symbolic links like libcru.so -> libcru.so.X ->
# libcru.so.X.Y, where X and Y are respectively the major and minor
# version numbers. The convention is that libraries differing only in
# Y are API compatible but an increment to X signifies a breaking (or
# fixing) change. To install these symbolic links, set the SOVERSION
# target property to X and VERSION target property to X.Y.

set_target_properties (
  cru PROPERTIES SOVERSION ${cru_VERSION_MAJOR} VERSION ${cru_VERSION_MAJOR}.${cru_VERSION_MINOR})

target_include_directories(
  cru
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cru>
    $<INSTALL_INTERFACE:include/cru>
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/src)

install(
  TARGETS cru
  EXPORT cruConfig
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  EXPORT cruConfig
  DESTINATION share/cru/cmake)

if (GEN_DOCS)
  add_dependencies(cru manpages)
endif ()

# --------------- linked libraries ------------------------------------------------------------------------

find_library(PTHREAD pthread)

if (NOT PTHREAD)
   message(FATAL_ERROR "pthread library not found")
endif ()

# Nthm is a small thread synchronization and message passing library
# written by the same author.

find_library(NTHM nthm)

if (NOT NTHM)
  message (FATAL_ERROR "nthm not found; see https://github.com/gueststar/nthm")
endif ()

target_link_libraries(cru pthread nthm)

# There is a choice of memory allocators. Mimalloc is slightly faster
# than the others more often than not. Valgrind tests have been
# observed to fail with tcmalloc due to a bug in valgrind or tcmalloc,
# but production code doesn't appear to be affected. If no memory
# manager is available, cru defaults to the system library version of
# malloc with a likely performance hit.

find_library(MIMALLOC mimalloc)
find_library(JEMALLOC jemalloc)
find_library(TCMALLOC tcmalloc_minimal)

if (MIMALLOC)
  message (STATUS "using mimalloc memory allocator")
  target_link_libraries(cru mimalloc)
elseif (JEMALLOC)
  message (STATUS "using jemalloc memory allocator")
  target_link_libraries(cru jemalloc)
elseif (TCMALLOC)
  message (WARNING "using tcmalloc memory allocator; expect valgrind issues")
  target_link_libraries(cru tcmalloc_minimal)
else ()
  message (WARNING "no memory allocator found; expect some performance degradation")
endif()

# The Judy array library implements fast hash tables and related
# structures essential for acceptable performance, but an API
# compatible working prototype version of cru is buildable without
# it. See https://sourceforge.net/projects/judy/ or your distro's
# package manager. If Judy arrays are unavailable or inadequate, see
# src/emu.c for reference implementations of the necessary replacement
# functions, whose names end with _nj, and src/table.c for the call
# sites.

find_library (JUDY Judy)

if (JUDY)
  message (STATUS "using Judy array library")
  target_link_libraries(cru Judy)
else ()
  message (WARNING "Judy array library not in use; expect severe performance degradation")
endif()

# --------------- options ---------------------------------------------------------------------------------

# Cru can detect deadlock automatically and raise an error instead of
# hanging when user code ineptly requests depth first traversal on
# cyclic graphs. There is a slight performance penalty and a
# vanishingly small risk of a false positive detection. Disabling this
# option risks unlimited charges if computing resources are billed by
# the hour.

option (DEADLOCK_DETECTION "enable deadlock detection on depth first traversal" ON)

if (DEADLOCK_DETECTION)
  message (STATUS "deadlock detection enabled")
else ()
  message (STATUS "deadlock detection disabled")
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)

# --------------- automated tests -------------------------------------------------------------------------

include(CTest)

find_program(VALGRIND valgrind)

if (VALGRIND)
  set (MEMCHECK_COMMAND ${VALGRIND} --leak-check=full)
  set (DRDCHECK_COMMAND ${VALGRIND} --tool=drd)
  message (STATUS "valgrind memory and thread safety tests enabled")
else ()
  message (STATUS "valgrind not found; some tests omitted")
endif ()

function(testme testname)
  add_executable(${testname} test/${testname}.c)
  target_link_libraries(${testname} cru)
  target_include_directories(
	 ${testname}
	 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cru>
	 PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/test)
  add_test (NAME ${testname} COMMAND ${testname})
  if (VALGRIND)
	 add_test (NAME ${testname}_mem COMMAND ${MEMCHECK_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/${testname})
	 set_tests_properties(${testname}_mem PROPERTIES PASS_REGULAR_EXPRESSION "no leaks|ely lost: 0")
	 set_tests_properties(${testname}_mem PROPERTIES FAIL_REGULAR_EXPRESSION "Invalid")
	 add_test (NAME ${testname}_drd COMMAND ${DRDCHECK_COMMAND} ${CMAKE_CURRENT_BINARY_DIR}/${testname})
	 set_tests_properties(${testname}_drd PROPERTIES TIMEOUT 10800 FAIL_REGULAR_EXPRESSION "Acquired|Conflicting")
  endif ()
endfunction()

# Configure every .c file in the test/ directory as a test.

execute_process(
  OUTPUT_VARIABLE T_FILES
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
  COMMAND ${CMAKE_COMMAND} -E env ${FIND} . -name "*.c"
  COMMAND ${CMAKE_COMMAND} -E env ${SED} -E "s/\.c$//"
  COMMAND ${CMAKE_COMMAND} -E env ${SED} -E "s/^\.\\///"
  OUTPUT_STRIP_TRAILING_WHITESPACE)

# newline separated list of executable test files used by the mtest script
string(REPLACE "\n" " \\\n" X_FILES ${T_FILES})

# semicolon separated list of executable test files used for valgrind tests
string(REPLACE "\n" ";" T_FILES ${T_FILES})

foreach(T_FILE IN LISTS T_FILES)
  testme(${T_FILE})
endforeach ()

# --------------- manual tests ----------------------------------------------------------------------------

# These extra thorough tests are in bash scripts that aren't run by
# ctest but can be invoked manually in a shell from the build
# directory.

if (NOT BASH)
  message(WARNING "bash shell not found; test scripts unavailable")
else ()
  # verify polling of kill switches and response to them
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/test/ktest.in ${CMAKE_CURRENT_BINARY_DIR}/ktest)
  # verify correct handling of memory allocation errors
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/test/mtest.in ${CMAKE_CURRENT_BINARY_DIR}/mtest)
  # rebuild for production if either of the above tests is interrupted
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/test/untest.in ${CMAKE_CURRENT_BINARY_DIR}/untest)
endif ()
