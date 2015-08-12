#!/usr/bin/perl

$ENV{GLOBUS_GSSAPI_NAME_COMPATIBILITY} = 'STRICT_GT2';
my $valgrind = "";
if (exists $ENV{VALGRIND})
{
    $valgrind = "valgrind --log-file=VALGRIND-compare_name_test_strict_gt2.log";
    if (exists $ENV{VALGRIND_OPTIONS})
    {
        $valgrind .= ' ' . $ENV{VALGRIND_OPTIONS};
    }
}
system("$valgrind ./compare-name-test compare_name_test_strict_gt2.txt");
exit($? >> 8);
