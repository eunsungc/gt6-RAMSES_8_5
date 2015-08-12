package Globus::GRAM::JobState;

=head1 NAME

Globus::GRAM::JobState - GRAM Protocol JobState Constants

=head1 DESCRIPTION

The Globus::GRAM::JobState module defines symbolic names for the
JobState constants in the GRAM Protocol.


=head2 Methods

=over 4


=item $value = Globus::GRAM::JobState::PENDING()

Return the value of the PENDING constant.

=cut

sub PENDING
{
    return 1;
}

=item $value = Globus::GRAM::JobState::ACTIVE()

Return the value of the ACTIVE constant.

=cut

sub ACTIVE
{
    return 2;
}

=item $value = Globus::GRAM::JobState::FAILED()

Return the value of the FAILED constant.

=cut

sub FAILED
{
    return 4;
}

=item $value = Globus::GRAM::JobState::DONE()

Return the value of the DONE constant.

=cut

sub DONE
{
    return 8;
}

=item $value = Globus::GRAM::JobState::SUSPENDED()

Return the value of the SUSPENDED constant.

=cut

sub SUSPENDED
{
    return 16;
}

=item $value = Globus::GRAM::JobState::UNSUBMITTED()

Return the value of the UNSUBMITTED constant.

=cut

sub UNSUBMITTED
{
    return 32;
}

=item $value = Globus::GRAM::JobState::STAGE_IN()

Return the value of the STAGE_IN constant.

=cut

sub STAGE_IN
{
    return 64;
}

=item $value = Globus::GRAM::JobState::STAGE_OUT()

Return the value of the STAGE_OUT constant.

=cut

sub STAGE_OUT
{
    return 128;
}

=item $value = Globus::GRAM::JobState::ALL()

Return the value of the ALL constant.

=cut

sub ALL
{
    return 0xFFFFF;
}

=back

=cut

1;
