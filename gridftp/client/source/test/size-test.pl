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


=head1 size-test

Tests to exercise the size checking of the client library.

=cut

use strict;
use POSIX;
use Test::More;
use File::Basename;
use lib dirname($0);
use FtpTestLib;
use File::Spec;

my $test_exec = './size-test';
my @tests;

my ($proto) = setup_proto();
my ($source_host, $source_file, $local_copy) = setup_remote_source();

sub check_size
{
    my ($errors,$rc) = ("",0);
    my $src_url = shift;
    my $size = shift;
    my $checked_size;

    unlink('core');
    
    my $command = "$test_exec -s $src_url 2>" . File::Spec::->devnull();
    $checked_size = `$command`;
    chomp($checked_size);
    $rc = $?;
    if($rc / 256 != 0 && $size >= 0)
    {
        $errors .= "\n# Test exited with " . $rc / 256;
    }
    if($size != -1 && $checked_size != $size)
    {
	$errors .= "\n# Size mismatch.";
    }

    ok($errors eq "", "check_size $command");
}

if(source_is_remote())
{
    print "using remote source, skipping check_size()\n";
}
else
{
    my @file_sizes;
    if(!defined($ENV{'FTP_TEST_BACKEND'}))
    {
        push(@file_sizes, '/etc/group');
        push(@file_sizes, '/bin/sh');
    }
    else
    {
        if(defined($ENV{'FTP_TEST_SOURCE_BIGFILE'}))
        {
            push(@file_sizes, $ENV{'FTP_TEST_SOURCE_BIGFILE'});
        }
        if(defined($ENV{'FTP_TEST_SOURCE_FILE'}))
        {
            push(@file_sizes, $ENV{'FTP_TEST_SOURCE_FILE'});
        }
        if(defined($ENV{'FTP_TEST_LOCAL_BIGFILE'}))
        {
            push(@file_sizes, $ENV{'FTP_TEST_LOCAL_BIGFILE'});
        }
    }
    push(@file_sizes, '/adsfadsfa');
    
    foreach(@file_sizes)
    {
        my $size = (stat($_))[7];
        if(!defined($size))
        {
    	    $size = -1;
        }

        push(@tests, "check_size('$proto$source_host$_', $size);");
    }

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

}
