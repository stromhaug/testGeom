source $env(ASI_TEST_SCRIPTS)/inspection/recognize-holes/__begin

# Set working variables.
set datafile cad/holes/cf1145-101_1037625_c_rev1.stp
set radius 1e10
set refFids { 9 12 14 15 27 28 64 65 66 67 68 69 70 71 108 109 110 111 112 113 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167 }

__recognize-holes
