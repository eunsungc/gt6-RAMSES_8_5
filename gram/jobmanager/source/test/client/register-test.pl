#! /usr/bin/perl
#
# Copyright 1999-2010 University of Chicago
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
# Ping a valid and invalid gatekeeper contact.

use strict;
use POSIX;
use Test;

my $test_exec = './register-test';

my $x509_certdir_string;

if ($ENV{CONTACT_STRING} eq "")
{
    die "CONTACT_STRING not set";
}
if (exists($ENV{X509_CERT_DIR}))
{
    $x509_certdir_string = "(X509_CERT_DIR $ENV{X509_CERT_DIR})";
}
else
{
    $x509_certdir_string = "";
}

my @tests;
my @todo;
my $testno = 1;

sub register_test
{
    my ($errors,$rc) = ("",0);
    my ($output);
    my ($contact, $rsl, $result, $fullarg) = @_;
    my $valgrind = "";

    if (exists $ENV{VALGRIND})
    {
        $valgrind = "valgrind --log-file=VALGRIND-register_test_" . $testno++ . ".log";
        if (exists $ENV{VALGRIND_OPTIONS})
        {
            $valgrind .= ' ' . $ENV{VALGRIND_OPTIONS};
        }
    }

    if (! defined($fullarg))
    {
        $fullarg='';
    }

    system("$valgrind $test_exec '$contact' '$rsl' $fullarg >/dev/null");
    $rc = $?>> 8;
    if($rc != $result)
    {
        $errors .= "Test exited with $rc. ";
    }

    if($errors eq "")
    {
        ok('success', 'success');
    }
    else
    {
        ok($errors, 'success');
    }
}
push(@tests, "register_test('$ENV{CONTACT_STRING}', '&(executable=/bin/sleep)(arguments=1)', 0);");
push(@tests, "register_test('$ENV{CONTACT_STRING}X', '&(executable=/bin/sleep)(arguments=1)', 7);");
push(@tests, "register_test('$ENV{CONTACT_STRING}', '&(executable=/no-such-bin/sleep)(arguments=1)', 5);");
# Explanation for these test cases:
# Both attempt to run the command
# grid-proxy-info -type | grep limited && globusrun -k $GLOBUS_GRAM_JOB_CONTACT
# In the 1st case, the credential is a limited proxy, so the job is canceled,
# causing the client to receive a FAILED notification.
# In the 2nd case, the credential is a full proxy, so the job is not canceled
# and the job terminates normally
push(@tests, "register_test('$ENV{CONTACT_STRING}',
    '&(executable=/bin/sh)' .
    '(arguments = -c \"grid-proxy-info -type | grep limited && eval \"\"globusrun -k \$GLOBUS_GRAM_JOB_CONTACT\"\"; sleep 30\")' .
    '(environment = ' .
    '   (GLOBUS_LOCATION \$(GLOBUS_LOCATION)) ' .
    '   (PATH \"/bin:/usr/bin:\" # \$(GLOBUS_LOCATION) # \"/bin\") '.
    '   $x509_certdir_string)' .
    '(library_path = \$(GLOBUS_LOCATION)/lib)', 8);");
push(@tests, "register_test('$ENV{CONTACT_STRING}', 
    '&(executable=/bin/sh)' .
    '(arguments = -c \"grid-proxy-info -type | grep limited && eval \"\"globusrun -k \$GLOBUS_GRAM_JOB_CONTACT\"\"; sleep 30\")' .
    '(environment = '. 
    '   (GLOBUS_LOCATION \$(GLOBUS_LOCATION))' .
    '   (PATH \"/bin:/usr/bin:\" # \$(GLOBUS_LOCATION) # \"/bin\") '.
    '   $x509_certdir_string)' .
    '(library_path = \$(GLOBUS_LOCATION)/lib)', 0, '-f');");

# Now that the tests are defined, set up the Test to deal with them.
plan tests => scalar(@tests), todo => \@todo;

foreach (@tests)
{
    eval "&$_";
}
