package Globus::GRAM::Error;

=head1 NAME

Globus::GRAM::Error - GRAM Protocol Error Constants

=head1 DESCRIPTION

The Globus::GRAM::Error module defines symbolic names for the
Error constants in the GRAM Protocol.

=pod

The Globus::GRAM::Error module methods return an object consisting
of an integer erorr code, and (optionally) a string explaining the
error. 

=head2 Methods

=over 4

=item $error = new Globus::GRAM::Error($number, $string);

Create a new error object with the given error number and string
description. This is called by the error-specific factory methods described
below.

=cut

sub new
{
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self = {};
    my $value = shift;
    my $string = shift;

    $self->{value} = $value if defined($value);
    $self->{string} = $string if defined($string);

    bless $self, $class;

    return $self;
}

=item $error->string()

Return the error string associated with a Globus::GRAM::Error object.

=cut

sub string
{
    my $self = shift;
    return $self->{string};
}

=item $error->value()

Return the integer error code associated with a Globus::GRAM::Error object.

=cut

sub value
{
    my $self = shift;
    return $self->{value};
}

=item $error = Globus::GRAM::Error::PARAMETER_NOT_SUPPORTED()

Create a new PARAMETER_NOT_SUPPORTED GRAM error.

=cut

sub PARAMETER_NOT_SUPPORTED
{
    return new Globus::GRAM::Error(1);
}

=item $error = Globus::GRAM::Error::INVALID_REQUEST()

Create a new INVALID_REQUEST GRAM error.

=cut

sub INVALID_REQUEST
{
    return new Globus::GRAM::Error(2);
}

=item $error = Globus::GRAM::Error::NO_RESOURCES()

Create a new NO_RESOURCES GRAM error.

=cut

sub NO_RESOURCES
{
    return new Globus::GRAM::Error(3);
}

=item $error = Globus::GRAM::Error::BAD_DIRECTORY()

Create a new BAD_DIRECTORY GRAM error.

=cut

sub BAD_DIRECTORY
{
    return new Globus::GRAM::Error(4);
}

=item $error = Globus::GRAM::Error::EXECUTABLE_NOT_FOUND()

Create a new EXECUTABLE_NOT_FOUND GRAM error.

=cut

sub EXECUTABLE_NOT_FOUND
{
    return new Globus::GRAM::Error(5);
}

=item $error = Globus::GRAM::Error::INSUFFICIENT_FUNDS()

Create a new INSUFFICIENT_FUNDS GRAM error.

=cut

sub INSUFFICIENT_FUNDS
{
    return new Globus::GRAM::Error(6);
}

=item $error = Globus::GRAM::Error::AUTHORIZATION()

Create a new AUTHORIZATION GRAM error.

=cut

sub AUTHORIZATION
{
    return new Globus::GRAM::Error(7);
}

=item $error = Globus::GRAM::Error::USER_CANCELLED()

Create a new USER_CANCELLED GRAM error.

=cut

sub USER_CANCELLED
{
    return new Globus::GRAM::Error(8);
}

=item $error = Globus::GRAM::Error::SYSTEM_CANCELLED()

Create a new SYSTEM_CANCELLED GRAM error.

=cut

sub SYSTEM_CANCELLED
{
    return new Globus::GRAM::Error(9);
}

=item $error = Globus::GRAM::Error::PROTOCOL_FAILED()

Create a new PROTOCOL_FAILED GRAM error.

=cut

sub PROTOCOL_FAILED
{
    return new Globus::GRAM::Error(10);
}

=item $error = Globus::GRAM::Error::STDIN_NOT_FOUND()

Create a new STDIN_NOT_FOUND GRAM error.

=cut

sub STDIN_NOT_FOUND
{
    return new Globus::GRAM::Error(11);
}

=item $error = Globus::GRAM::Error::CONNECTION_FAILED()

Create a new CONNECTION_FAILED GRAM error.

=cut

sub CONNECTION_FAILED
{
    return new Globus::GRAM::Error(12);
}

=item $error = Globus::GRAM::Error::INVALID_MAXTIME()

Create a new INVALID_MAXTIME GRAM error.

=cut

sub INVALID_MAXTIME
{
    return new Globus::GRAM::Error(13);
}

=item $error = Globus::GRAM::Error::INVALID_COUNT()

Create a new INVALID_COUNT GRAM error.

=cut

sub INVALID_COUNT
{
    return new Globus::GRAM::Error(14);
}

=item $error = Globus::GRAM::Error::NULL_SPECIFICATION_TREE()

Create a new NULL_SPECIFICATION_TREE GRAM error.

=cut

sub NULL_SPECIFICATION_TREE
{
    return new Globus::GRAM::Error(15);
}

=item $error = Globus::GRAM::Error::JM_FAILED_ALLOW_ATTACH()

Create a new JM_FAILED_ALLOW_ATTACH GRAM error.

=cut

sub JM_FAILED_ALLOW_ATTACH
{
    return new Globus::GRAM::Error(16);
}

=item $error = Globus::GRAM::Error::JOB_EXECUTION_FAILED()

Create a new JOB_EXECUTION_FAILED GRAM error.

=cut

sub JOB_EXECUTION_FAILED
{
    return new Globus::GRAM::Error(17);
}

=item $error = Globus::GRAM::Error::INVALID_PARADYN()

Create a new INVALID_PARADYN GRAM error.

=cut

sub INVALID_PARADYN
{
    return new Globus::GRAM::Error(18);
}

=item $error = Globus::GRAM::Error::INVALID_JOBTYPE()

Create a new INVALID_JOBTYPE GRAM error.

=cut

sub INVALID_JOBTYPE
{
    return new Globus::GRAM::Error(19);
}

=item $error = Globus::GRAM::Error::INVALID_GRAM_MYJOB()

Create a new INVALID_GRAM_MYJOB GRAM error.

=cut

sub INVALID_GRAM_MYJOB
{
    return new Globus::GRAM::Error(20);
}

=item $error = Globus::GRAM::Error::BAD_SCRIPT_ARG_FILE()

Create a new BAD_SCRIPT_ARG_FILE GRAM error.

=cut

sub BAD_SCRIPT_ARG_FILE
{
    return new Globus::GRAM::Error(21);
}

=item $error = Globus::GRAM::Error::ARG_FILE_CREATION_FAILED()

Create a new ARG_FILE_CREATION_FAILED GRAM error.

=cut

sub ARG_FILE_CREATION_FAILED
{
    return new Globus::GRAM::Error(22);
}

=item $error = Globus::GRAM::Error::INVALID_JOBSTATE()

Create a new INVALID_JOBSTATE GRAM error.

=cut

sub INVALID_JOBSTATE
{
    return new Globus::GRAM::Error(23);
}

=item $error = Globus::GRAM::Error::INVALID_SCRIPT_REPLY()

Create a new INVALID_SCRIPT_REPLY GRAM error.

=cut

sub INVALID_SCRIPT_REPLY
{
    return new Globus::GRAM::Error(24);
}

=item $error = Globus::GRAM::Error::INVALID_SCRIPT_STATUS()

Create a new INVALID_SCRIPT_STATUS GRAM error.

=cut

sub INVALID_SCRIPT_STATUS
{
    return new Globus::GRAM::Error(25);
}

=item $error = Globus::GRAM::Error::JOBTYPE_NOT_SUPPORTED()

Create a new JOBTYPE_NOT_SUPPORTED GRAM error.

=cut

sub JOBTYPE_NOT_SUPPORTED
{
    return new Globus::GRAM::Error(26);
}

=item $error = Globus::GRAM::Error::UNIMPLEMENTED()

Create a new UNIMPLEMENTED GRAM error.

=cut

sub UNIMPLEMENTED
{
    return new Globus::GRAM::Error(27);
}

=item $error = Globus::GRAM::Error::TEMP_SCRIPT_FILE_FAILED()

Create a new TEMP_SCRIPT_FILE_FAILED GRAM error.

=cut

sub TEMP_SCRIPT_FILE_FAILED
{
    return new Globus::GRAM::Error(28);
}

=item $error = Globus::GRAM::Error::USER_PROXY_NOT_FOUND()

Create a new USER_PROXY_NOT_FOUND GRAM error.

=cut

sub USER_PROXY_NOT_FOUND
{
    return new Globus::GRAM::Error(29);
}

=item $error = Globus::GRAM::Error::OPENING_USER_PROXY()

Create a new OPENING_USER_PROXY GRAM error.

=cut

sub OPENING_USER_PROXY
{
    return new Globus::GRAM::Error(30);
}

=item $error = Globus::GRAM::Error::JOB_CANCEL_FAILED()

Create a new JOB_CANCEL_FAILED GRAM error.

=cut

sub JOB_CANCEL_FAILED
{
    return new Globus::GRAM::Error(31);
}

=item $error = Globus::GRAM::Error::MALLOC_FAILED()

Create a new MALLOC_FAILED GRAM error.

=cut

sub MALLOC_FAILED
{
    return new Globus::GRAM::Error(32);
}

=item $error = Globus::GRAM::Error::DUCT_INIT_FAILED()

Create a new DUCT_INIT_FAILED GRAM error.

=cut

sub DUCT_INIT_FAILED
{
    return new Globus::GRAM::Error(33);
}

=item $error = Globus::GRAM::Error::DUCT_LSP_FAILED()

Create a new DUCT_LSP_FAILED GRAM error.

=cut

sub DUCT_LSP_FAILED
{
    return new Globus::GRAM::Error(34);
}

=item $error = Globus::GRAM::Error::INVALID_HOST_COUNT()

Create a new INVALID_HOST_COUNT GRAM error.

=cut

sub INVALID_HOST_COUNT
{
    return new Globus::GRAM::Error(35);
}

=item $error = Globus::GRAM::Error::UNSUPPORTED_PARAMETER()

Create a new UNSUPPORTED_PARAMETER GRAM error.

=cut

sub UNSUPPORTED_PARAMETER
{
    return new Globus::GRAM::Error(36);
}

=item $error = Globus::GRAM::Error::INVALID_QUEUE()

Create a new INVALID_QUEUE GRAM error.

=cut

sub INVALID_QUEUE
{
    return new Globus::GRAM::Error(37);
}

=item $error = Globus::GRAM::Error::INVALID_PROJECT()

Create a new INVALID_PROJECT GRAM error.

=cut

sub INVALID_PROJECT
{
    return new Globus::GRAM::Error(38);
}

=item $error = Globus::GRAM::Error::RSL_EVALUATION_FAILED()

Create a new RSL_EVALUATION_FAILED GRAM error.

=cut

sub RSL_EVALUATION_FAILED
{
    return new Globus::GRAM::Error(39);
}

=item $error = Globus::GRAM::Error::BAD_RSL_ENVIRONMENT()

Create a new BAD_RSL_ENVIRONMENT GRAM error.

=cut

sub BAD_RSL_ENVIRONMENT
{
    return new Globus::GRAM::Error(40);
}

=item $error = Globus::GRAM::Error::DRYRUN()

Create a new DRYRUN GRAM error.

=cut

sub DRYRUN
{
    return new Globus::GRAM::Error(41);
}

=item $error = Globus::GRAM::Error::ZERO_LENGTH_RSL()

Create a new ZERO_LENGTH_RSL GRAM error.

=cut

sub ZERO_LENGTH_RSL
{
    return new Globus::GRAM::Error(42);
}

=item $error = Globus::GRAM::Error::STAGING_EXECUTABLE()

Create a new STAGING_EXECUTABLE GRAM error.

=cut

sub STAGING_EXECUTABLE
{
    return new Globus::GRAM::Error(43);
}

=item $error = Globus::GRAM::Error::STAGING_STDIN()

Create a new STAGING_STDIN GRAM error.

=cut

sub STAGING_STDIN
{
    return new Globus::GRAM::Error(44);
}

=item $error = Globus::GRAM::Error::INVALID_JOB_MANAGER_TYPE()

Create a new INVALID_JOB_MANAGER_TYPE GRAM error.

=cut

sub INVALID_JOB_MANAGER_TYPE
{
    return new Globus::GRAM::Error(45);
}

=item $error = Globus::GRAM::Error::BAD_ARGUMENTS()

Create a new BAD_ARGUMENTS GRAM error.

=cut

sub BAD_ARGUMENTS
{
    return new Globus::GRAM::Error(46);
}

=item $error = Globus::GRAM::Error::GATEKEEPER_MISCONFIGURED()

Create a new GATEKEEPER_MISCONFIGURED GRAM error.

=cut

sub GATEKEEPER_MISCONFIGURED
{
    return new Globus::GRAM::Error(47);
}

=item $error = Globus::GRAM::Error::BAD_RSL()

Create a new BAD_RSL GRAM error.

=cut

sub BAD_RSL
{
    return new Globus::GRAM::Error(48);
}

=item $error = Globus::GRAM::Error::VERSION_MISMATCH()

Create a new VERSION_MISMATCH GRAM error.

=cut

sub VERSION_MISMATCH
{
    return new Globus::GRAM::Error(49);
}

=item $error = Globus::GRAM::Error::RSL_ARGUMENTS()

Create a new RSL_ARGUMENTS GRAM error.

=cut

sub RSL_ARGUMENTS
{
    return new Globus::GRAM::Error(50);
}

=item $error = Globus::GRAM::Error::RSL_COUNT()

Create a new RSL_COUNT GRAM error.

=cut

sub RSL_COUNT
{
    return new Globus::GRAM::Error(51);
}

=item $error = Globus::GRAM::Error::RSL_DIRECTORY()

Create a new RSL_DIRECTORY GRAM error.

=cut

sub RSL_DIRECTORY
{
    return new Globus::GRAM::Error(52);
}

=item $error = Globus::GRAM::Error::RSL_DRYRUN()

Create a new RSL_DRYRUN GRAM error.

=cut

sub RSL_DRYRUN
{
    return new Globus::GRAM::Error(53);
}

=item $error = Globus::GRAM::Error::RSL_ENVIRONMENT()

Create a new RSL_ENVIRONMENT GRAM error.

=cut

sub RSL_ENVIRONMENT
{
    return new Globus::GRAM::Error(54);
}

=item $error = Globus::GRAM::Error::RSL_EXECUTABLE()

Create a new RSL_EXECUTABLE GRAM error.

=cut

sub RSL_EXECUTABLE
{
    return new Globus::GRAM::Error(55);
}

=item $error = Globus::GRAM::Error::RSL_HOST_COUNT()

Create a new RSL_HOST_COUNT GRAM error.

=cut

sub RSL_HOST_COUNT
{
    return new Globus::GRAM::Error(56);
}

=item $error = Globus::GRAM::Error::RSL_JOBTYPE()

Create a new RSL_JOBTYPE GRAM error.

=cut

sub RSL_JOBTYPE
{
    return new Globus::GRAM::Error(57);
}

=item $error = Globus::GRAM::Error::RSL_MAXTIME()

Create a new RSL_MAXTIME GRAM error.

=cut

sub RSL_MAXTIME
{
    return new Globus::GRAM::Error(58);
}

=item $error = Globus::GRAM::Error::RSL_MYJOB()

Create a new RSL_MYJOB GRAM error.

=cut

sub RSL_MYJOB
{
    return new Globus::GRAM::Error(59);
}

=item $error = Globus::GRAM::Error::RSL_PARADYN()

Create a new RSL_PARADYN GRAM error.

=cut

sub RSL_PARADYN
{
    return new Globus::GRAM::Error(60);
}

=item $error = Globus::GRAM::Error::RSL_PROJECT()

Create a new RSL_PROJECT GRAM error.

=cut

sub RSL_PROJECT
{
    return new Globus::GRAM::Error(61);
}

=item $error = Globus::GRAM::Error::RSL_QUEUE()

Create a new RSL_QUEUE GRAM error.

=cut

sub RSL_QUEUE
{
    return new Globus::GRAM::Error(62);
}

=item $error = Globus::GRAM::Error::RSL_STDERR()

Create a new RSL_STDERR GRAM error.

=cut

sub RSL_STDERR
{
    return new Globus::GRAM::Error(63);
}

=item $error = Globus::GRAM::Error::RSL_STDIN()

Create a new RSL_STDIN GRAM error.

=cut

sub RSL_STDIN
{
    return new Globus::GRAM::Error(64);
}

=item $error = Globus::GRAM::Error::RSL_STDOUT()

Create a new RSL_STDOUT GRAM error.

=cut

sub RSL_STDOUT
{
    return new Globus::GRAM::Error(65);
}

=item $error = Globus::GRAM::Error::OPENING_JOBMANAGER_SCRIPT()

Create a new OPENING_JOBMANAGER_SCRIPT GRAM error.

=cut

sub OPENING_JOBMANAGER_SCRIPT
{
    return new Globus::GRAM::Error(66);
}

=item $error = Globus::GRAM::Error::CREATING_PIPE()

Create a new CREATING_PIPE GRAM error.

=cut

sub CREATING_PIPE
{
    return new Globus::GRAM::Error(67);
}

=item $error = Globus::GRAM::Error::FCNTL_FAILED()

Create a new FCNTL_FAILED GRAM error.

=cut

sub FCNTL_FAILED
{
    return new Globus::GRAM::Error(68);
}

=item $error = Globus::GRAM::Error::STDOUT_FILENAME_FAILED()

Create a new STDOUT_FILENAME_FAILED GRAM error.

=cut

sub STDOUT_FILENAME_FAILED
{
    return new Globus::GRAM::Error(69);
}

=item $error = Globus::GRAM::Error::STDERR_FILENAME_FAILED()

Create a new STDERR_FILENAME_FAILED GRAM error.

=cut

sub STDERR_FILENAME_FAILED
{
    return new Globus::GRAM::Error(70);
}

=item $error = Globus::GRAM::Error::FORKING_EXECUTABLE()

Create a new FORKING_EXECUTABLE GRAM error.

=cut

sub FORKING_EXECUTABLE
{
    return new Globus::GRAM::Error(71);
}

=item $error = Globus::GRAM::Error::EXECUTABLE_PERMISSIONS()

Create a new EXECUTABLE_PERMISSIONS GRAM error.

=cut

sub EXECUTABLE_PERMISSIONS
{
    return new Globus::GRAM::Error(72);
}

=item $error = Globus::GRAM::Error::OPENING_STDOUT()

Create a new OPENING_STDOUT GRAM error.

=cut

sub OPENING_STDOUT
{
    return new Globus::GRAM::Error(73);
}

=item $error = Globus::GRAM::Error::OPENING_STDERR()

Create a new OPENING_STDERR GRAM error.

=cut

sub OPENING_STDERR
{
    return new Globus::GRAM::Error(74);
}

=item $error = Globus::GRAM::Error::OPENING_CACHE_USER_PROXY()

Create a new OPENING_CACHE_USER_PROXY GRAM error.

=cut

sub OPENING_CACHE_USER_PROXY
{
    return new Globus::GRAM::Error(75);
}

=item $error = Globus::GRAM::Error::OPENING_CACHE()

Create a new OPENING_CACHE GRAM error.

=cut

sub OPENING_CACHE
{
    return new Globus::GRAM::Error(76);
}

=item $error = Globus::GRAM::Error::INSERTING_CLIENT_CONTACT()

Create a new INSERTING_CLIENT_CONTACT GRAM error.

=cut

sub INSERTING_CLIENT_CONTACT
{
    return new Globus::GRAM::Error(77);
}

=item $error = Globus::GRAM::Error::CLIENT_CONTACT_NOT_FOUND()

Create a new CLIENT_CONTACT_NOT_FOUND GRAM error.

=cut

sub CLIENT_CONTACT_NOT_FOUND
{
    return new Globus::GRAM::Error(78);
}

=item $error = Globus::GRAM::Error::CONTACTING_JOB_MANAGER()

Create a new CONTACTING_JOB_MANAGER GRAM error.

=cut

sub CONTACTING_JOB_MANAGER
{
    return new Globus::GRAM::Error(79);
}

=item $error = Globus::GRAM::Error::INVALID_JOB_CONTACT()

Create a new INVALID_JOB_CONTACT GRAM error.

=cut

sub INVALID_JOB_CONTACT
{
    return new Globus::GRAM::Error(80);
}

=item $error = Globus::GRAM::Error::UNDEFINED_EXE()

Create a new UNDEFINED_EXE GRAM error.

=cut

sub UNDEFINED_EXE
{
    return new Globus::GRAM::Error(81);
}

=item $error = Globus::GRAM::Error::CONDOR_ARCH()

Create a new CONDOR_ARCH GRAM error.

=cut

sub CONDOR_ARCH
{
    return new Globus::GRAM::Error(82);
}

=item $error = Globus::GRAM::Error::CONDOR_OS()

Create a new CONDOR_OS GRAM error.

=cut

sub CONDOR_OS
{
    return new Globus::GRAM::Error(83);
}

=item $error = Globus::GRAM::Error::RSL_MIN_MEMORY()

Create a new RSL_MIN_MEMORY GRAM error.

=cut

sub RSL_MIN_MEMORY
{
    return new Globus::GRAM::Error(84);
}

=item $error = Globus::GRAM::Error::RSL_MAX_MEMORY()

Create a new RSL_MAX_MEMORY GRAM error.

=cut

sub RSL_MAX_MEMORY
{
    return new Globus::GRAM::Error(85);
}

=item $error = Globus::GRAM::Error::INVALID_MIN_MEMORY()

Create a new INVALID_MIN_MEMORY GRAM error.

=cut

sub INVALID_MIN_MEMORY
{
    return new Globus::GRAM::Error(86);
}

=item $error = Globus::GRAM::Error::INVALID_MAX_MEMORY()

Create a new INVALID_MAX_MEMORY GRAM error.

=cut

sub INVALID_MAX_MEMORY
{
    return new Globus::GRAM::Error(87);
}

=item $error = Globus::GRAM::Error::HTTP_FRAME_FAILED()

Create a new HTTP_FRAME_FAILED GRAM error.

=cut

sub HTTP_FRAME_FAILED
{
    return new Globus::GRAM::Error(88);
}

=item $error = Globus::GRAM::Error::HTTP_UNFRAME_FAILED()

Create a new HTTP_UNFRAME_FAILED GRAM error.

=cut

sub HTTP_UNFRAME_FAILED
{
    return new Globus::GRAM::Error(89);
}

=item $error = Globus::GRAM::Error::HTTP_PACK_FAILED()

Create a new HTTP_PACK_FAILED GRAM error.

=cut

sub HTTP_PACK_FAILED
{
    return new Globus::GRAM::Error(90);
}

=item $error = Globus::GRAM::Error::HTTP_UNPACK_FAILED()

Create a new HTTP_UNPACK_FAILED GRAM error.

=cut

sub HTTP_UNPACK_FAILED
{
    return new Globus::GRAM::Error(91);
}

=item $error = Globus::GRAM::Error::INVALID_JOB_QUERY()

Create a new INVALID_JOB_QUERY GRAM error.

=cut

sub INVALID_JOB_QUERY
{
    return new Globus::GRAM::Error(92);
}

=item $error = Globus::GRAM::Error::SERVICE_NOT_FOUND()

Create a new SERVICE_NOT_FOUND GRAM error.

=cut

sub SERVICE_NOT_FOUND
{
    return new Globus::GRAM::Error(93);
}

=item $error = Globus::GRAM::Error::JOB_QUERY_DENIAL()

Create a new JOB_QUERY_DENIAL GRAM error.

=cut

sub JOB_QUERY_DENIAL
{
    return new Globus::GRAM::Error(94);
}

=item $error = Globus::GRAM::Error::CALLBACK_NOT_FOUND()

Create a new CALLBACK_NOT_FOUND GRAM error.

=cut

sub CALLBACK_NOT_FOUND
{
    return new Globus::GRAM::Error(95);
}

=item $error = Globus::GRAM::Error::BAD_GATEKEEPER_CONTACT()

Create a new BAD_GATEKEEPER_CONTACT GRAM error.

=cut

sub BAD_GATEKEEPER_CONTACT
{
    return new Globus::GRAM::Error(96);
}

=item $error = Globus::GRAM::Error::POE_NOT_FOUND()

Create a new POE_NOT_FOUND GRAM error.

=cut

sub POE_NOT_FOUND
{
    return new Globus::GRAM::Error(97);
}

=item $error = Globus::GRAM::Error::MPIRUN_NOT_FOUND()

Create a new MPIRUN_NOT_FOUND GRAM error.

=cut

sub MPIRUN_NOT_FOUND
{
    return new Globus::GRAM::Error(98);
}

=item $error = Globus::GRAM::Error::RSL_START_TIME()

Create a new RSL_START_TIME GRAM error.

=cut

sub RSL_START_TIME
{
    return new Globus::GRAM::Error(99);
}

=item $error = Globus::GRAM::Error::RSL_RESERVATION_HANDLE()

Create a new RSL_RESERVATION_HANDLE GRAM error.

=cut

sub RSL_RESERVATION_HANDLE
{
    return new Globus::GRAM::Error(100);
}

=item $error = Globus::GRAM::Error::RSL_MAX_WALL_TIME()

Create a new RSL_MAX_WALL_TIME GRAM error.

=cut

sub RSL_MAX_WALL_TIME
{
    return new Globus::GRAM::Error(101);
}

=item $error = Globus::GRAM::Error::INVALID_MAX_WALL_TIME()

Create a new INVALID_MAX_WALL_TIME GRAM error.

=cut

sub INVALID_MAX_WALL_TIME
{
    return new Globus::GRAM::Error(102);
}

=item $error = Globus::GRAM::Error::RSL_MAX_CPU_TIME()

Create a new RSL_MAX_CPU_TIME GRAM error.

=cut

sub RSL_MAX_CPU_TIME
{
    return new Globus::GRAM::Error(103);
}

=item $error = Globus::GRAM::Error::INVALID_MAX_CPU_TIME()

Create a new INVALID_MAX_CPU_TIME GRAM error.

=cut

sub INVALID_MAX_CPU_TIME
{
    return new Globus::GRAM::Error(104);
}

=item $error = Globus::GRAM::Error::JM_SCRIPT_NOT_FOUND()

Create a new JM_SCRIPT_NOT_FOUND GRAM error.

=cut

sub JM_SCRIPT_NOT_FOUND
{
    return new Globus::GRAM::Error(105);
}

=item $error = Globus::GRAM::Error::JM_SCRIPT_PERMISSIONS()

Create a new JM_SCRIPT_PERMISSIONS GRAM error.

=cut

sub JM_SCRIPT_PERMISSIONS
{
    return new Globus::GRAM::Error(106);
}

=item $error = Globus::GRAM::Error::SIGNALING_JOB()

Create a new SIGNALING_JOB GRAM error.

=cut

sub SIGNALING_JOB
{
    return new Globus::GRAM::Error(107);
}

=item $error = Globus::GRAM::Error::UNKNOWN_SIGNAL_TYPE()

Create a new UNKNOWN_SIGNAL_TYPE GRAM error.

=cut

sub UNKNOWN_SIGNAL_TYPE
{
    return new Globus::GRAM::Error(108);
}

=item $error = Globus::GRAM::Error::GETTING_JOBID()

Create a new GETTING_JOBID GRAM error.

=cut

sub GETTING_JOBID
{
    return new Globus::GRAM::Error(109);
}

=item $error = Globus::GRAM::Error::WAITING_FOR_COMMIT()

Create a new WAITING_FOR_COMMIT GRAM error.

=cut

sub WAITING_FOR_COMMIT
{
    return new Globus::GRAM::Error(110);
}

=item $error = Globus::GRAM::Error::COMMIT_TIMED_OUT()

Create a new COMMIT_TIMED_OUT GRAM error.

=cut

sub COMMIT_TIMED_OUT
{
    return new Globus::GRAM::Error(111);
}

=item $error = Globus::GRAM::Error::RSL_SAVE_STATE()

Create a new RSL_SAVE_STATE GRAM error.

=cut

sub RSL_SAVE_STATE
{
    return new Globus::GRAM::Error(112);
}

=item $error = Globus::GRAM::Error::RSL_RESTART()

Create a new RSL_RESTART GRAM error.

=cut

sub RSL_RESTART
{
    return new Globus::GRAM::Error(113);
}

=item $error = Globus::GRAM::Error::RSL_TWO_PHASE_COMMIT()

Create a new RSL_TWO_PHASE_COMMIT GRAM error.

=cut

sub RSL_TWO_PHASE_COMMIT
{
    return new Globus::GRAM::Error(114);
}

=item $error = Globus::GRAM::Error::INVALID_TWO_PHASE_COMMIT()

Create a new INVALID_TWO_PHASE_COMMIT GRAM error.

=cut

sub INVALID_TWO_PHASE_COMMIT
{
    return new Globus::GRAM::Error(115);
}

=item $error = Globus::GRAM::Error::RSL_STDOUT_POSITION()

Create a new RSL_STDOUT_POSITION GRAM error.

=cut

sub RSL_STDOUT_POSITION
{
    return new Globus::GRAM::Error(116);
}

=item $error = Globus::GRAM::Error::INVALID_STDOUT_POSITION()

Create a new INVALID_STDOUT_POSITION GRAM error.

=cut

sub INVALID_STDOUT_POSITION
{
    return new Globus::GRAM::Error(117);
}

=item $error = Globus::GRAM::Error::RSL_STDERR_POSITION()

Create a new RSL_STDERR_POSITION GRAM error.

=cut

sub RSL_STDERR_POSITION
{
    return new Globus::GRAM::Error(118);
}

=item $error = Globus::GRAM::Error::INVALID_STDERR_POSITION()

Create a new INVALID_STDERR_POSITION GRAM error.

=cut

sub INVALID_STDERR_POSITION
{
    return new Globus::GRAM::Error(119);
}

=item $error = Globus::GRAM::Error::RESTART_FAILED()

Create a new RESTART_FAILED GRAM error.

=cut

sub RESTART_FAILED
{
    return new Globus::GRAM::Error(120);
}

=item $error = Globus::GRAM::Error::NO_STATE_FILE()

Create a new NO_STATE_FILE GRAM error.

=cut

sub NO_STATE_FILE
{
    return new Globus::GRAM::Error(121);
}

=item $error = Globus::GRAM::Error::READING_STATE_FILE()

Create a new READING_STATE_FILE GRAM error.

=cut

sub READING_STATE_FILE
{
    return new Globus::GRAM::Error(122);
}

=item $error = Globus::GRAM::Error::WRITING_STATE_FILE()

Create a new WRITING_STATE_FILE GRAM error.

=cut

sub WRITING_STATE_FILE
{
    return new Globus::GRAM::Error(123);
}

=item $error = Globus::GRAM::Error::OLD_JM_ALIVE()

Create a new OLD_JM_ALIVE GRAM error.

=cut

sub OLD_JM_ALIVE
{
    return new Globus::GRAM::Error(124);
}

=item $error = Globus::GRAM::Error::TTL_EXPIRED()

Create a new TTL_EXPIRED GRAM error.

=cut

sub TTL_EXPIRED
{
    return new Globus::GRAM::Error(125);
}

=item $error = Globus::GRAM::Error::SUBMIT_UNKNOWN()

Create a new SUBMIT_UNKNOWN GRAM error.

=cut

sub SUBMIT_UNKNOWN
{
    return new Globus::GRAM::Error(126);
}

=item $error = Globus::GRAM::Error::RSL_REMOTE_IO_URL()

Create a new RSL_REMOTE_IO_URL GRAM error.

=cut

sub RSL_REMOTE_IO_URL
{
    return new Globus::GRAM::Error(127);
}

=item $error = Globus::GRAM::Error::WRITING_REMOTE_IO_URL()

Create a new WRITING_REMOTE_IO_URL GRAM error.

=cut

sub WRITING_REMOTE_IO_URL
{
    return new Globus::GRAM::Error(128);
}

=item $error = Globus::GRAM::Error::STDIO_SIZE()

Create a new STDIO_SIZE GRAM error.

=cut

sub STDIO_SIZE
{
    return new Globus::GRAM::Error(129);
}

=item $error = Globus::GRAM::Error::JM_STOPPED()

Create a new JM_STOPPED GRAM error.

=cut

sub JM_STOPPED
{
    return new Globus::GRAM::Error(130);
}

=item $error = Globus::GRAM::Error::USER_PROXY_EXPIRED()

Create a new USER_PROXY_EXPIRED GRAM error.

=cut

sub USER_PROXY_EXPIRED
{
    return new Globus::GRAM::Error(131);
}

=item $error = Globus::GRAM::Error::JOB_UNSUBMITTED()

Create a new JOB_UNSUBMITTED GRAM error.

=cut

sub JOB_UNSUBMITTED
{
    return new Globus::GRAM::Error(132);
}

=item $error = Globus::GRAM::Error::INVALID_COMMIT()

Create a new INVALID_COMMIT GRAM error.

=cut

sub INVALID_COMMIT
{
    return new Globus::GRAM::Error(133);
}

=item $error = Globus::GRAM::Error::RSL_SCHEDULER_SPECIFIC()

Create a new RSL_SCHEDULER_SPECIFIC GRAM error.

=cut

sub RSL_SCHEDULER_SPECIFIC
{
    return new Globus::GRAM::Error(134);
}

=item $error = Globus::GRAM::Error::STAGE_IN_FAILED()

Create a new STAGE_IN_FAILED GRAM error.

=cut

sub STAGE_IN_FAILED
{
    return new Globus::GRAM::Error(135);
}

=item $error = Globus::GRAM::Error::INVALID_SCRATCH()

Create a new INVALID_SCRATCH GRAM error.

=cut

sub INVALID_SCRATCH
{
    return new Globus::GRAM::Error(136);
}

=item $error = Globus::GRAM::Error::RSL_CACHE()

Create a new RSL_CACHE GRAM error.

=cut

sub RSL_CACHE
{
    return new Globus::GRAM::Error(137);
}

=item $error = Globus::GRAM::Error::INVALID_SUBMIT_ATTRIBUTE()

Create a new INVALID_SUBMIT_ATTRIBUTE GRAM error.

=cut

sub INVALID_SUBMIT_ATTRIBUTE
{
    return new Globus::GRAM::Error(138);
}

=item $error = Globus::GRAM::Error::INVALID_STDIO_UPDATE_ATTRIBUTE()

Create a new INVALID_STDIO_UPDATE_ATTRIBUTE GRAM error.

=cut

sub INVALID_STDIO_UPDATE_ATTRIBUTE
{
    return new Globus::GRAM::Error(139);
}

=item $error = Globus::GRAM::Error::INVALID_RESTART_ATTRIBUTE()

Create a new INVALID_RESTART_ATTRIBUTE GRAM error.

=cut

sub INVALID_RESTART_ATTRIBUTE
{
    return new Globus::GRAM::Error(140);
}

=item $error = Globus::GRAM::Error::RSL_FILE_STAGE_IN()

Create a new RSL_FILE_STAGE_IN GRAM error.

=cut

sub RSL_FILE_STAGE_IN
{
    return new Globus::GRAM::Error(141);
}

=item $error = Globus::GRAM::Error::RSL_FILE_STAGE_IN_SHARED()

Create a new RSL_FILE_STAGE_IN_SHARED GRAM error.

=cut

sub RSL_FILE_STAGE_IN_SHARED
{
    return new Globus::GRAM::Error(142);
}

=item $error = Globus::GRAM::Error::RSL_FILE_STAGE_OUT()

Create a new RSL_FILE_STAGE_OUT GRAM error.

=cut

sub RSL_FILE_STAGE_OUT
{
    return new Globus::GRAM::Error(143);
}

=item $error = Globus::GRAM::Error::RSL_GASS_CACHE()

Create a new RSL_GASS_CACHE GRAM error.

=cut

sub RSL_GASS_CACHE
{
    return new Globus::GRAM::Error(144);
}

=item $error = Globus::GRAM::Error::RSL_FILE_CLEANUP()

Create a new RSL_FILE_CLEANUP GRAM error.

=cut

sub RSL_FILE_CLEANUP
{
    return new Globus::GRAM::Error(145);
}

=item $error = Globus::GRAM::Error::RSL_SCRATCH()

Create a new RSL_SCRATCH GRAM error.

=cut

sub RSL_SCRATCH
{
    return new Globus::GRAM::Error(146);
}

=item $error = Globus::GRAM::Error::INVALID_SCHEDULER_SPECIFIC()

Create a new INVALID_SCHEDULER_SPECIFIC GRAM error.

=cut

sub INVALID_SCHEDULER_SPECIFIC
{
    return new Globus::GRAM::Error(147);
}

=item $error = Globus::GRAM::Error::UNDEFINED_ATTRIBUTE()

Create a new UNDEFINED_ATTRIBUTE GRAM error.

=cut

sub UNDEFINED_ATTRIBUTE
{
    return new Globus::GRAM::Error(148);
}

=item $error = Globus::GRAM::Error::INVALID_CACHE()

Create a new INVALID_CACHE GRAM error.

=cut

sub INVALID_CACHE
{
    return new Globus::GRAM::Error(149);
}

=item $error = Globus::GRAM::Error::INVALID_SAVE_STATE()

Create a new INVALID_SAVE_STATE GRAM error.

=cut

sub INVALID_SAVE_STATE
{
    return new Globus::GRAM::Error(150);
}

=item $error = Globus::GRAM::Error::OPENING_VALIDATION_FILE()

Create a new OPENING_VALIDATION_FILE GRAM error.

=cut

sub OPENING_VALIDATION_FILE
{
    return new Globus::GRAM::Error(151);
}

=item $error = Globus::GRAM::Error::READING_VALIDATION_FILE()

Create a new READING_VALIDATION_FILE GRAM error.

=cut

sub READING_VALIDATION_FILE
{
    return new Globus::GRAM::Error(152);
}

=item $error = Globus::GRAM::Error::RSL_PROXY_TIMEOUT()

Create a new RSL_PROXY_TIMEOUT GRAM error.

=cut

sub RSL_PROXY_TIMEOUT
{
    return new Globus::GRAM::Error(153);
}

=item $error = Globus::GRAM::Error::INVALID_PROXY_TIMEOUT()

Create a new INVALID_PROXY_TIMEOUT GRAM error.

=cut

sub INVALID_PROXY_TIMEOUT
{
    return new Globus::GRAM::Error(154);
}

=item $error = Globus::GRAM::Error::STAGE_OUT_FAILED()

Create a new STAGE_OUT_FAILED GRAM error.

=cut

sub STAGE_OUT_FAILED
{
    return new Globus::GRAM::Error(155);
}

=item $error = Globus::GRAM::Error::JOB_CONTACT_NOT_FOUND()

Create a new JOB_CONTACT_NOT_FOUND GRAM error.

=cut

sub JOB_CONTACT_NOT_FOUND
{
    return new Globus::GRAM::Error(156);
}

=item $error = Globus::GRAM::Error::DELEGATION_FAILED()

Create a new DELEGATION_FAILED GRAM error.

=cut

sub DELEGATION_FAILED
{
    return new Globus::GRAM::Error(157);
}

=item $error = Globus::GRAM::Error::LOCKING_STATE_LOCK_FILE()

Create a new LOCKING_STATE_LOCK_FILE GRAM error.

=cut

sub LOCKING_STATE_LOCK_FILE
{
    return new Globus::GRAM::Error(158);
}

=item $error = Globus::GRAM::Error::INVALID_ATTR()

Create a new INVALID_ATTR GRAM error.

=cut

sub INVALID_ATTR
{
    return new Globus::GRAM::Error(159);
}

=item $error = Globus::GRAM::Error::NULL_PARAMETER()

Create a new NULL_PARAMETER GRAM error.

=cut

sub NULL_PARAMETER
{
    return new Globus::GRAM::Error(160);
}

=item $error = Globus::GRAM::Error::STILL_STREAMING()

Create a new STILL_STREAMING GRAM error.

=cut

sub STILL_STREAMING
{
    return new Globus::GRAM::Error(161);
}

=item $error = Globus::GRAM::Error::AUTHORIZATION_DENIED()

Create a new AUTHORIZATION_DENIED GRAM error.

=cut

sub AUTHORIZATION_DENIED
{
    return new Globus::GRAM::Error(162);
}

=item $error = Globus::GRAM::Error::AUTHORIZATION_SYSTEM_FAILURE()

Create a new AUTHORIZATION_SYSTEM_FAILURE GRAM error.

=cut

sub AUTHORIZATION_SYSTEM_FAILURE
{
    return new Globus::GRAM::Error(163);
}

=item $error = Globus::GRAM::Error::AUTHORIZATION_DENIED_JOB_ID()

Create a new AUTHORIZATION_DENIED_JOB_ID GRAM error.

=cut

sub AUTHORIZATION_DENIED_JOB_ID
{
    return new Globus::GRAM::Error(164);
}

=item $error = Globus::GRAM::Error::AUTHORIZATION_DENIED_EXECUTABLE()

Create a new AUTHORIZATION_DENIED_EXECUTABLE GRAM error.

=cut

sub AUTHORIZATION_DENIED_EXECUTABLE
{
    return new Globus::GRAM::Error(165);
}

=item $error = Globus::GRAM::Error::RSL_USER_NAME()

Create a new RSL_USER_NAME GRAM error.

=cut

sub RSL_USER_NAME
{
    return new Globus::GRAM::Error(166);
}

=item $error = Globus::GRAM::Error::INVALID_USER_NAME()

Create a new INVALID_USER_NAME GRAM error.

=cut

sub INVALID_USER_NAME
{
    return new Globus::GRAM::Error(167);
}

=item $error = Globus::GRAM::Error::LAST()

Create a new LAST GRAM error.

=cut

sub LAST
{
    return new Globus::GRAM::Error(168);
}

=back

=cut

1;
