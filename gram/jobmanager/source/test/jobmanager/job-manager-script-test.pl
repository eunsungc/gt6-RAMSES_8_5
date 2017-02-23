#! /usr/bin/perl

use strict;
use Test::More;
use Globus::Core::Paths;
use IPC::Open2;
use POSIX qw(pause);

$^W = 1;

my $script = $Globus::Core::Paths::libexecdir . "/globus-job-manager-script.pl";
my @tests = qw(test_interactive_quit test_interactive_poll);

if ($ENV{CONTACT_LRM} eq 'fork') {
    plan tests => 3;
    test_interactive_quit();
    test_interactive_poll();
    test_interactive_multipoll();
} else {
    plan skip_all => "Test is fork-specific";
}


sub test_interactive_quit
{
    my ($child_out, $child_in);
    my $pid;
    my $out;

    $pid = open2(
            $child_out, $child_in,
            $script,
            '-m', 'fork',
            '-c', 'interactive');

    print $child_in "quit\n\n";
    waitpid $pid, 0;
    ok($? == 0, "test_interactive_quit");
}

sub test_interactive_poll
{
    my ($child_out, $child_in);
    my $pid;
    my $dummy_pid;
    my $out;

    $pid = open2(
            $child_out, $child_in,
            $script,
            '-m', 'fork',
            '-c', 'interactive');

    $dummy_pid = fork();

    if (!defined($dummy_pid))
    {
        ok(defined($dummy_pid));
        return;
    }
    elsif ($dummy_pid == 0)
    {
        pause();
        exit(0);
    }
    print $child_in "poll\n";
    print $child_in "\$description = { jobid => [ '$dummy_pid' ] };\n\n";
    while (($_ = <$child_out>) ne "\n")
    {
        if ($_ =~ /^GRAM_SCRIPT_LOG/)
        {
            next;
        }
        if ($_ ne "GRAM_SCRIPT_JOB_STATE:2\n")
        {
            ok($_, "GRAM_SCRIPT_JOB_STATE:2\n");
            kill 'TERM', $dummy_pid;
            waitpid($dummy_pid, 0);
            print $child_in "quit\n\n";
            waitpid($pid, 0);
            return;
        }
    }
    kill 'TERM', $dummy_pid;
    waitpid($dummy_pid, 0);

    print $child_in "poll\n";
    print $child_in "\$description = { jobid => [ '$dummy_pid' ] };\n\n";
    while (($_ = <$child_out>) ne "\n")
    {
        if ($_ =~ /^GRAM_SCRIPT_LOG/)
        {
            next;
        }
        if ($_ ne "GRAM_SCRIPT_JOB_STATE:8\n")
        {
            ok($_, "GRAM_SCRIPT_JOB_STATE:8\n");
            return;
        }
    }
    print $child_in "quit\n\n";

    waitpid $pid, 0;
    ok($? == 0, "test_interactive_poll");
}

sub test_interactive_multipoll
{
    my ($child_out, $child_in);
    my $pid;
    my $dummy_pid;
    my $out;

    $pid = open2(
            $child_out, $child_in,
            $script,
            '-m', 'fork',
            '-c', 'interactive');

    $dummy_pid = fork();

    if (!defined($dummy_pid))
    {
        ok(defined($dummy_pid));
        return;
    }
    elsif ($dummy_pid == 0)
    {
        pause();
        exit(0);
    }

    for (my $i = 0; $i < 100; $i++)
    {
        print $child_in "poll\n";
        print $child_in "\$description = { jobid => [ '$dummy_pid' ] };\n\n";
        while (($_ = <$child_out>) ne "\n")
        {
            if ($_ =~ m/^GRAM_SCRIPT_LOG/)
            {
                next;
            }
            elsif ($_ ne "GRAM_SCRIPT_JOB_STATE:2\n")
            {
                ok($_, "GRAM_SCRIPT_JOB_STATE:2\n");
                kill 'TERM', $dummy_pid;
                waitpid($dummy_pid, 0);
                print $child_in "quit\n\n";
                waitpid($pid, 0);
                return;
            }
        }
    }
    kill 'TERM', $dummy_pid;
    waitpid($dummy_pid, 0);

    print $child_in "poll\n";
    print $child_in "\$description = { jobid => [ '$dummy_pid' ] };\n\n";
    while (($_ = <$child_out>) ne "\n")
    {
        if ($_ =~ m/^GRAM_SCRIPT_LOG/)
        {
            next;
        }
        elsif ($_ ne "GRAM_SCRIPT_JOB_STATE:8\n")
        {
            ok($_, "GRAM_SCRIPT_JOB_STATE:8\n");
            return;
        }
    }
    print $child_in "quit\n\n";

    waitpid $pid, 0;
    ok($? == 0, "test_interactive_multipoll");
}
