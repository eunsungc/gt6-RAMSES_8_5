package Globus::GRAM::JobSignal;

=head1 NAME

Globus::GRAM::JobSignal - GRAM Protocol JobSignal Constants

=head1 DESCRIPTION

The Globus::GRAM::JobSignal module defines symbolic names for the
JobSignal constants in the GRAM Protocol.


=head2 Methods

=over 4


=item $value = Globus::GRAM::JobSignal::CANCEL()

Return the value of the CANCEL constant.

=cut

sub CANCEL
{
    return 1;
}

=item $value = Globus::GRAM::JobSignal::SUSPEND()

Return the value of the SUSPEND constant.

=cut

sub SUSPEND
{
    return 2;
}

=item $value = Globus::GRAM::JobSignal::RESUME()

Return the value of the RESUME constant.

=cut

sub RESUME
{
    return 3;
}

=item $value = Globus::GRAM::JobSignal::PRIORITY()

Return the value of the PRIORITY constant.

=cut

sub PRIORITY
{
    return 4;
}

=item $value = Globus::GRAM::JobSignal::COMMIT_REQUEST()

Return the value of the COMMIT_REQUEST constant.

=cut

sub COMMIT_REQUEST
{
    return 5;
}

=item $value = Globus::GRAM::JobSignal::COMMIT_EXTEND()

Return the value of the COMMIT_EXTEND constant.

=cut

sub COMMIT_EXTEND
{
    return 6;
}

=item $value = Globus::GRAM::JobSignal::STDIO_UPDATE()

Return the value of the STDIO_UPDATE constant.

=cut

sub STDIO_UPDATE
{
    return 7;
}

=item $value = Globus::GRAM::JobSignal::STDIO_SIZE()

Return the value of the STDIO_SIZE constant.

=cut

sub STDIO_SIZE
{
    return 8;
}

=item $value = Globus::GRAM::JobSignal::STOP_MANAGER()

Return the value of the STOP_MANAGER constant.

=cut

sub STOP_MANAGER
{
    return 9;
}

=item $value = Globus::GRAM::JobSignal::COMMIT_END()

Return the value of the COMMIT_END constant.

=cut

sub COMMIT_END
{
    return 10;
}

=back

=cut

1;
