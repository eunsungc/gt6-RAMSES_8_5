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
# Test to exercise the "put" functionality of the Globus FTP client library
# using the partial file attribute.
#

use strict;
use POSIX;
use Test::More;
use FileHandle;
use File::Basename;
use lib dirname($0);
use FtpTestLib;
use File::Spec;

my $test_exec = './partial-put-test';
my @tests;
my @todo;
my $fh = new FileHandle;
my $data;

my ($proto) = setup_proto();
my ($dest_host, $dest_file) = setup_remote_dest();

# Test #1-3. Basic functionality: create a dummy file in /tmp; modify
# that file remotely using partial file put; make sure the file is
# what it should be.
# Test with offset at 0, offset in the middle of the file, and offset
# past the edge of the file.
# Success if program returns 0, files compare, and no core file
# is generated.
sub basic_func
{
    my ($errors,$rc) = ("",0);
    my $newfile = new FileHandle;
    my $offset = shift;
    my $data = "";
    unlink($dest_file);

    # Create a file of known contents, for the partial update.
    open($newfile, ">$dest_file");
    binmode($newfile);
    for(my $i = 0; $i < 4096; $i++)
    {
	$data .= $i % 10;
    }
    $data .= "\n";
    print $newfile $data;
    close $newfile;
    
    my $command = "$test_exec -R $offset -d $proto$dest_host$dest_file -p >".File::Spec::->devnull()." 2>&1";
    open($newfile, "|$command");
    binmode($newfile);
    my $i = $offset;
    if($offset > 4096)
    {
        for($a = 4096; $a < $offset; $a++)
	{
	    $data .=  chr(0);
	}
    }
    for(my $a = ord("a"); $a < ord("z"); $a++)
    {
        print $newfile chr($a);
	substr($data, $i++, 1, chr($a));
    }
    close($newfile);

    $rc = $?;
    if($rc / 256 != 0)
    {
        $errors .= "\n# Test exited with " . $rc / 256;
    }

    open($newfile, "|diff - $dest_file");
    print $newfile $data;
    close($newfile);
    $rc = $? >> 8;
    if($rc != 0)
    {
	$errors .= "\n# Different from expected output.";
    }

    ok($errors eq "", "basic_func $command");
    unlink($dest_file);
}

if(source_is_remote())
{
    print "using remote source, skipping basic_func()\n";
}
else
{
    
push(@tests, "basic_func(0);");
push(@tests, "basic_func(100);");
push(@tests, "basic_func(5000);");

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

}
