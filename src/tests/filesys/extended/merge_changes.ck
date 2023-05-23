# -*- perl -*-
use strict;
use warnings;
use tests::tests;
use tests::random;
check_expected (IGNORE_EXIT_CODES => 1, [<<'EOF']);
(merge_changes) begin
(merge_changes) file created
(merge_changes) reset is ok
(merge_changes) opened file
(merge_changes) get write number
(merge_changes) merge changes is correct
(merge_changes) end
EOF
pass;