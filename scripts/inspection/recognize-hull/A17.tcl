source $env(ASI_TEST_SCRIPTS)/inspection/recognize-hull/__begin

# Set working variables.
set datafile  cad/blends/0059_isolated_blends_test_13.brep
set maxSize   0
set refFids { 2 3 19 23 26 36 37 59 117 120 121 123 126 127 128 130 131 132 133 135 140 146 151 153 154 163 175 182 184 185 }

__recognize-hull