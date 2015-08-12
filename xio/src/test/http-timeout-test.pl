#! /usr/bin/env perl

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

use strict;
use Test::More;

my $type = 0;
if(@ARGV == 1)
{
    $type = 1;
}

my @todo;
my $test_exec="./http_timeout_test";
my $data_dir=$ENV{srcdir};

my @arg_sets = (
    "-t client -T 500 -a",
    "-t server -T 500 -r",
    "-t client -T 500 -r");

plan tests => scalar(@arg_sets);

foreach (@arg_sets)
{
    my $result;
    my $case=$_;
    my $clientorserver = $case;
    my $which = $case;

    $clientorserver =~ m/(client|server)/;
    $clientorserver = $1;
    $which =~ m/-(a|r)/;
    $which = $1;

    $result = system("$test_exec $case");

    ok($result == 0, "http_timeout_${clientorserver}_${which}");
}
