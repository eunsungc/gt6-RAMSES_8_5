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
package Globus::Core::Paths;

require Exporter;

@ISA = qw(Exporter);

@EXPORT = qw($exec_prefix $prefix
             $sbindir $bindir
             $libexecdir $includedir
             $datarootdir
             $mandir
             $datadir $sysconfdir $sharedstatedir
             $localstatedir $perlmoduledir);

sub eval_path
{
    my $path = shift;
    my $last = $path;

    while ($path =~ m/\$\{([^}]*)\}/)
    {
        my $varname = $1;
        my $evaluated;
        eval "\$evaluated = \${$1}";

        $path =~ s/\${$varname}/$evaluated/g;
        if ($path eq $last)
        {
            die "Error evaluating $last\n";
        }
        $last = $path;
    }
    return $path;
}

if (exists $ENV{GLOBUS_LOCATION})
{
    $prefix = $ENV{GLOBUS_LOCATION};
}
else
{
    $prefix = "/home/esjung/gt6_inst";
}
$exec_prefix = "${prefix}";
$sbindir = "${exec_prefix}/sbin";
$bindir = "${exec_prefix}/bin";
$includedir = "${prefix}/include";
$datarootdir = "${prefix}/share";
$datadir = "${datarootdir}";
$mandir = "${datarootdir}/man";
$libexecdir = "${exec_prefix}/libexec";
$sysconfdir = "${prefix}/etc";
$sharedstatedir = "${prefix}/com";
$localstatedir = "${prefix}/var";
$perlmoduledir = "${libdir}/perl";

1;
