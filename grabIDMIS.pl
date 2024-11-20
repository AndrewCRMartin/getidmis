#!/usr/bin/perl -s
use strict;

$::baseURL = "https://extranets.who.int/inn";
$::tplURL  = "$::baseURL/Report.php?Request=%d\\&Report=INN_Phase_6b_Admin_Report";
$::cDir    = "/home/amartin/work/inn/idmis";

my $cFile = "$::cDir/Martin-cert.p12";
my $pFile = "$::cDir/Martin-cert.passwd";
my $passwd = `cat $pFile`;
chomp $passwd;

foreach my $reqID (@ARGV)
{
    my $url = sprintf($::tplURL, $reqID);
    my $exe = "curl -s --cert-type p12 --cert $cFile:$passwd $url";
    my $page = `$exe`;
    ProcessPage($reqID, $page);
}


sub ProcessPage
{
    my ($reqID, $page) = @_;
    $page =~ s/\r//g;
    my @lines = split(/\n/, $page);
    my $inData = 0;
    foreach my $line (@lines)
    {
        if($inData)
        {
            $line =~ m/href=\"(.*?)\"\>(.*?)\</;
            my $urlPart  = $1;
            my $filename = $2;
            $filename =~ s/\s/_/g;

            $urlPart  =~ s/^\.\///;
            $urlPart  =~ s/\(/\\\(/g;
            $urlPart  =~ s/\)/\\\)/g;
            $filename =~ s/\(/\\\(/g;
            $filename =~ s/\)/\\\)/g;
            my $url   = "$::baseURL/$urlPart";
            my $exe   = "curl -s --cert-type p12 --cert $cFile:$passwd --output $filename $url";
            print STDERR "$exe\n" if(defined($::v));
            `$exe`;

            $inData = 0;
        }
        elsif($line =~ /Annex/)
        {
            $inData = 1;            
        }
        elsif($line =~ /href=\"(.*?)\"\>Word Document/)
        {
            my $urlPart  = $1;
            my $filename = "${reqID}_1_6b_Admin_Report.doc";
            $urlPart =~ s/^\.\///;
            $urlPart =~ s/\&/\\\&/g;
            my $url  = "$::baseURL/$urlPart";
            my $exe  = "curl -s --cert-type p12 --cert $cFile:$passwd --output $filename $url";
            `$exe`;
        }
    }
}
