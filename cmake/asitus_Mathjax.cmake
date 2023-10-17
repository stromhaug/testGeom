if (NOT DISTRIBUTION_GENERATE_DOC)
  ASITUS_UNSET(MATHJAX_EXE)
  ASITUS_UNSET(MATHJAX_DIR)
  return()
endif()

message (STATUS "Processing Mathjax 3-rd party")

ASITUS_FIND_PRODUCT_DIR ("${3RDPARTY_DIR}" "mathjax" MATHJAX_DIR)
set (MATHJAX_DIR "${3RDPARTY_DIR}/${MATHJAX_DIR}")
message (STATUS "... Mathjax dir: ${MATHJAX_DIR}")
set (MATHJAX_EXE "${MATHJAX_DIR}/MathJax.js")
if (EXISTS "${MATHJAX_EXE}")
  message (STATUS "... Mathjax executable found: ${MATHJAX_EXE}")
  set (MATHJAX_DIR "${MATHJAX_DIR}" CACHE PATH "Mathjax - formulas support for HTML documentation" FORCE)
else()
  set (MATHJAX_DIR "" CACHE PATH "Mathjax - formulas support for HTML documentation" FORCE)
endif()