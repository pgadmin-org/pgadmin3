use Win32;

my $filename = shift @ARGV;

die "Usage: regenguids.pl <filename>\n\n" unless ($filename);

sub MakeGuid {
  my $u = Win32::GuidGen();
  $u =~ s/[{}]//g;
  return $u;
}

open(IN,"<$filename");
open(OUT, ">$filename.out") || die "Could not open output file ($filename.out)!\n";

while(<IN>) {
	chomp();
	if(/(.*)([0-9A-Fa-f]{8}-?[0-9A-Fa-f]{4}-?[0-9A-Fa-f]{4}-?[0-9A-Fa-f]{4}-?[0-9A-Fa-f]{12})(.*)/) {
		$newguid=MakeGuid();
		print OUT $1.$newguid.$3;
	} else { print OUT $_ ;}
	print OUT "\n";
}

