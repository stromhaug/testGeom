set datafile cad/pockets/pockets_14.stp

# Read input geometry.
set datadir $env(ASI_TEST_DATA)
clear
load-step $datadir/$datafile

# Compute negative volume
compute-negative-volume -fids 1 2 3 4

set-as-part "negativeVolumeShape 1"

test-check-part-shape

test-check-solids-volumes 1.0e-4 102340.01646575992 

test-check-number-shape-entities -vertex 10 -edge 15 -wire 7 -face 7 -shell 1 -solid 1 -compsolid 0 -compound 0

test-check-shape-aabb-dim -xDim 65.744510856194211 -yDim 147.11476382996952 -zDim 15.000000000000011 -tol 1.0e-4