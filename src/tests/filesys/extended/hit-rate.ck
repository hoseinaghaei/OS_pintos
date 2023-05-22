# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::random;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(hit-rate) begin
(hit-rate) create file
(hit-rate) open file
(hit-rate) reset cache
(hit-rate) reset cache
(hit-rate) your cache is the best cache
(hit-rate) end
EOF
pass;