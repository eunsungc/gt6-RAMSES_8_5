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


=head1 globus-ftp-client-lingering-get-test

Tests to exercise the deactivation of the client and control libraries while
an operation is left in progress.

=cut

use strict;
use Test::More;
use File::Basename;
use lib dirname($0);
use FtpTestLib;

my $test_exec = './lingering-get-test';
my @tests;

=head1 I<lingering_get> (Test 1)

Do a get of $test_url. But don't deal with data block before deactivating
the client library.

=cut

my ($proto) = setup_proto();
my ($source_host, $source_file, $local_copy) = setup_remote_source();

sub lingering_get
{
    my ($errors,$rc) = ("",0);

    my $command = "$test_exec -s $proto$source_host$source_file";
    $errors = run_command($command, 1);
    
    ok($errors eq "", "lingering_get $command");
}
push(@tests, "lingering_get();");

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
    plan tests => scalar(@tests);

    foreach (@tests)
    {
        eval "&$_";
    }
}
