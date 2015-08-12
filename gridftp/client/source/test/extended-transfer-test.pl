#! /usr/bin/perl

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

#
# Test to exercise the "3rd party transfer" functionality of the Globus
# FTP client library.
#

use strict;
use Test::More;

my $test_exec = './extended-transfer-test';
my @tests;
my @todo;
use File::Basename;
use lib dirname($0);
use FtpTestLib;

my ($proto) = setup_proto();
my ($source_host, $source_file, $local_copy) = setup_remote_source();
my ($dest_host, $dest_file) = setup_remote_dest();

# Test #1-10. Basic functionality: Do a transfer of $testfile to/from
# localhost, varying parallelism level.
# Compare the resulting file with the real file
# Success if program returns 0, files compare,
# and no core file is generated.
sub basic_func
{
    my ($parallelism) = (shift);
    my ($errors,$rc) = ("",0);

    my $command = "$test_exec -P $parallelism -s $proto$source_host$source_file -d $proto$dest_host$dest_file";
    $errors = run_command($command, 0);
    if($errors eq "")
    {
        my ($output) = get_remote_file($dest_host, $dest_file);
        $errors = compare_local_files($local_copy, $output);
        unlink($output);
    }

    ok($errors eq "", "basic_func $parallelism: $command");

    clean_remote_file($dest_host, $dest_file);
}
for(my $par = 1; $par <= 10; $par++)
{
    push(@tests, "basic_func($par);");
}

# Test #11: Bad URL Source: Do a 3rd party transfer of a non-existent
# file from localhost.
# Success if program returns 1 and no core file is generated.
sub bad_url_src
{
    my ($errors,$rc) = ("",0);

    my $command = "$test_exec -s $proto$source_host$source_file/no-such-file-here -d $proto$dest_host$dest_file";
    $errors = run_command($command, 1);
    ok($errors eq '', "bad_url_src $command");
    
    clean_remote_file($dest_host, $dest_file);
}
push(@tests, "bad_url_src");

# Test #12: Bad URL: Do a 3rd party transfer of an unwritable location as the
# destination
# Success if program returns 1 and no core file is generated.
sub bad_url_dest
{
    my ($errors,$rc) = ("",0);

    my $command = "$test_exec -s $proto$source_host$source_file -d $proto$dest_host$dest_file/no-such-file-here";
    $errors = run_command($command, 1);
    ok($errors eq "", "bad_url_dest $command");
}
push(@tests, "bad_url_dest");

# Test #13-422: Do a transfer of $testfile to/from localhost, aborting
# at each possible position. Note that not all aborts may be reached.
# Success if no core file is generated for all abort points. (we could use
# a stronger measure of success here)
sub abort_test
{
    my ($errors,$rc) = ("", 0);
    my ($abort_point) = shift;
    my ($par) = shift;

    my $command = "$test_exec -P $par -a $abort_point -s $proto$source_host$source_file -d $proto$dest_host$dest_file";
    $errors = run_command($command, -2);

    ok($errors eq '', "abort_test $abort_point $command");
    
    clean_remote_file($dest_host, $dest_file);
}
for(my $i = 1; $i <= 43; $i++)
{
    for(my $j = 1; $j <= 10; $j++)
    {
        push(@tests, "abort_test($i, $j);");
    }
}

# Test #422-831. Restart functionality: Do a transfer of $testfile
# to/from localhost, restarting at each plugin-possible point.
# Compare the resulting file with the real file
# Success if program returns 0, files compare,
# and no core file is generated.
sub restart_test
{
    my ($errors,$rc) = ("",0);
    my ($restart_point) = shift;
    my ($par) = shift;

    my $command = "$test_exec -P $par -r $restart_point -s $proto$source_host$source_file -d $proto$dest_host$dest_file";
    $errors = run_command($command, 0);
    if($errors eq "")
    {
        my ($output) = get_remote_file($dest_host, $dest_file);
        $errors = compare_local_files($local_copy, $output);
        unlink($output);
    }

    ok($errors eq "", "restart_test $restart_point $command\n$errors");
    
    clean_remote_file($dest_host, $dest_file);
}
for(my $i = 1; $i <= 43; $i++)
{
    for(my $j = 1; $j <= 10; $j++)
    {
      push(@tests, "restart_test($i, $j);");
    }
}


=head2 I<perf_test> (Test 832)

Do an extended put of $testfile, enabling perf_plugin

=back

=cut
sub perf_test
{
    my ($errors,$rc) = ("",0);

    my $command = "$test_exec -M -s $proto$source_host$source_file -d $proto$dest_host$dest_file";
    $errors = run_command($command, 0);

    ok($errors eq "", "perf_test $command");
    
    clean_remote_file($dest_host, $dest_file);
}

push(@tests, "perf_test();");

=head2 I<throughput_test> (Test 833)

Do an extended put of $testfile, enabling throughput_plugin

=back

=cut
sub throughput_test
{
    my ($errors,$rc) = ("",0);

    my $command = "$test_exec -T -s $proto$source_host$source_file -d $proto$dest_host$dest_file";
    $errors = run_command($command, 0);

    ok($errors eq "", "throughput_test $command");
    
    clean_remote_file($dest_host, $dest_file);
}

push(@tests, "throughput_test();");

if(defined($ENV{FTP_TEST_RANDOMIZE}))
{
    shuffle(\@tests);
}

if(@ARGV)
{
    plan tests => scalar(@ARGV);

    foreach (@ARGV)
    {
        eval "&$tests[$_-1]";
    }
}
else
{
    plan tests => scalar(@tests), todo => \@todo;

    foreach (@tests)
    {
        eval "&$_";
    }
}
