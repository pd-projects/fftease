#!/usr/local/bin/perl 

# -*-Perl-*-

$file = "octfifth.scale";

open(OUT, ">$file");

$base = 27.5;

@ints = (1.5, 4/3);
$phs = 0;

while( $base < 22050 ){
    printf OUT "%.4f\n", $base;
    $base *= $ints[$phs++];
    $phs %= 2;
}
close $file;
print "wrote to $file\n";
