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

use strict;
use Test::Harness;
use Cwd;
use Getopt::Long;
use File::Basename;
use lib dirname($0);
use FtpTestLib;

require 5.005;
use vars qw(@tests);

#$Test::Harness::verbose = 1;

@tests = qw(
            dir-test.pl
            caching-get-test.pl
            caching-transfer-test.pl
            create-destroy-test.pl
            exist-test.pl 
            extended-get-test.pl
            extended-put-test.pl
            extended-transfer-test.pl
            get-test.pl
            lingering-get-test.pl
            multiple-block-get-test.pl
            partial-get-test.pl
            partial-put-test.pl
            partial-transfer-test.pl
            plugin-test.pl
            put-test.pl
            size-test.pl 
            transfer-test.pl
            user-auth-test.pl
            );

if(defined($ENV{FTP_TEST_RANDOMIZE}))
{
    shuffle(\@tests);
}

my $runserver;
my $nogsi;
my $server_pid;
my $backend_pid;

GetOptions( 'runserver' => \$runserver,
            'nogsi' => \$nogsi);

if(defined($nogsi) or defined($ENV{FTP_TEST_NO_GSI}))
{
    $nogsi = 1;
    $ENV{FTP_TEST_NO_GSI} = 1;
    print "Not using GSI security.\n";
}
if(defined($runserver))
{
    ($server_pid,$backend_pid) = setup_server(1);
}

if(run_command("grid-proxy-info -exists -hours 2", 0) && !defined($nogsi))
{
    print "Security proxy required to run the tests.\n";
    exit 1;
}

print "Running sanity check\n";
my ($proto) = setup_proto();
my ($source_host, $source_file, $local_copy1) = setup_remote_source();
my ($local_copy2) = setup_local_source();
my ($dest_host, $dest_file) = setup_remote_dest();

if(run_command("./globus-ftp-client-get-test -s $proto$source_host$source_file", 0))
{
    print "Sanity check of source ($proto$source_host$source_file) failed.\n";
    if(defined($server_pid))
    {
        kill(9,$server_pid);
    }
    
    exit 1;
}
if(run_command("./globus-ftp-client-put-test -d $proto$dest_host$dest_file < $local_copy2", 0))
{
    print "Sanity check of local source ($local_copy2) to dest ($proto$dest_host$dest_file) failed.\n";
    clean_remote_file($dest_host, $dest_file);

    if(defined($server_pid))
    {
        kill(9,$server_pid);
    }
    
    exit 1;
}
clean_remote_file($dest_host, $dest_file);
print "Server appears sane, running tests\n";

push(@INC, $ENV{GLOBUS_LOCATION} . "/lib/perl");

eval runtests(@tests);

$@ && print "$@";

if($server_pid)
{
    kill(2,$server_pid);
    sleep(2);
    kill(2,$server_pid);
    $server_pid=0;
}
if($backend_pid)
{
    kill(2,$backend_pid);
    sleep(2);
    kill(2,$backend_pid);
    $backend_pid=0;
}

exit 0;

sub setup_server()
{
    my $backend_port;
    my $backend_args;
    my $run_backend=shift;
    my $server_pid;
    my $backend_pid;
    my $server_prog = "globus-gridftp-server";
    my $server_host = "localhost";
    my $server_port = 0;
    my $server_nosec = "";
    my $subject;
    if(defined($nogsi))
    {
        $server_nosec = "-aa";
    }

    if(!defined($nogsi))
    {
        chomp($subject = `openssl x509 -subject -noout -in \${X509_USER_CERT:-testcred.cert}`);
        $subject =~ s/^subject= *//;
        
        $ENV{GRIDMAP} = cwd() . "/gridmap";
        if ( -f $ENV{GRIDMAP})
        {
            system('mv $GRIDMAP $GRIDMAP.old');    
        }   
        if( 0 != system("grid-mapfile-add-entry -dn \"$subject\" -ln `id -un` -f $ENV{GRIDMAP} >/dev/null 2>&1") / 256)
        {
            print "Unable to create gridmap file\n";
            exit 1;
        }
    }

    my $server_args = "-no-fork -no-chdir -d 0 -p $server_port $server_nosec";
    if($run_backend == 1)
    {
        $backend_args = "-no-fork -no-chdir -dn -d 0 $server_nosec";
        $backend_pid = open(BACKEND, "$server_prog $backend_args |");
        if($backend_pid == -1)
        {
            print "Unable to start server\n";
            exit 1;
        }

        select((select(BACKEND), $| = 1)[0]);
        $backend_port = <BACKEND>;
        $backend_port =~ s/Server listening at .*?:(\d+)/\1/;
        chomp($backend_port);
        if($backend_port !~ /\d+/)
        {
            print "Unable to start server\n";
            exit 1;
        }
        $server_args = "$server_args -r localhost:$backend_port";
    }

    print "$server_args\n";
    $server_pid = open(SERVER, "$server_prog $server_args |");
    if($server_pid == -1)
    {
        print "Unable to start server\n";
        exit 1;
    }

    select((select(SERVER), $| = 1)[0]);
    $server_port = <SERVER>;
    $server_port =~ s/Server listening at .*?:(\d+)/\1/;
    chomp($server_port);
    if($server_port !~ /\d+/)
    {
        print "Unable to start server\n";
        exit 1;
    }
    
    print "Started server at port $server_port\n";

    # sleep a second, some hosts are slow....

    sleep 5;
    
    $ENV{GLOBUS_FTP_CLIENT_TEST_SUBJECT} = $subject;
    $ENV{FTP_TEST_SOURCE_HOST} = "$server_host:$server_port";
    $ENV{FTP_TEST_DEST_HOST} = "$server_host:$server_port";   

    return $server_pid,$backend_pid;
}
