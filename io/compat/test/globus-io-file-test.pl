#!/usr/bin/env perl

# 
# Copyright 1999-2006 University of Chicago
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 


=pod

=head1 Tests for the globus IO file code

    Tests to exercise the file IO functionality of the globus IO library.

=cut

use strict;
use Test::More;
use File::Temp;
use File::Compare;

srand();

my $test_prog = 'globus_io_file_test';
my @tests;
my $valgrind="";

if (exists $ENV{VALGRIND})
{
    $valgrind = "valgrind --log-file=VALGRIND-$test_prog.log";
    if (exists $ENV{VALGRIND_OPTIONS})
    {
        $valgrind .= ' ' . $ENV{VALGRIND_OPTIONS};
    }
}

sub sig_handler
{
    if( -e "$test_prog.log.stdout" )
    {
        unlink("$test_prog.log.stdout");
    }

    if( -e "$test_prog.log.stderr" )
    {
        unlink("$test_prog.log.stderr");
    }
}

$SIG{'INT'}  = 'sig_handler';
$SIG{'QUIT'} = 'sig_handler';
$SIG{'KILL'} = 'sig_handler';

plan tests => 3;

my $testfile = mktemp("file_test_random_XXXXXXXX");
my $testfh;
my @chars = ("A".."Z", "a".."z", "0".."9");
my $rc;

open($testfh, ">$testfile");
print $testfh $chars[rand @chars] for 1..256;
close($testfh);

$rc = system("$valgrind ./$test_prog $testfile 1>$test_prog.log.stdout 2>$test_prog.log.stderr") / 256;

ok($rc == 0, "io file test exits with 0");

ok(File::Compare::compare("$test_prog.log.stdout", "$testfile") == 0, "io file test stdout matches");

ok(! -s "$test_prog.stderr", "io file test stderr empty");

if( -e "$test_prog.log.stdout" )
{
    unlink("$test_prog.log.stdout");
}

if( -e "$test_prog.log.stderr" )
{
    unlink("$test_prog.log.stderr");
}
unlink($testfile);
