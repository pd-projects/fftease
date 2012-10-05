#!/usr/local/bin/perl 

# -*-Perl-*-

$file = "13et.scale";

open(OUT, ">$file");

$base = 27.5;
$mult = 2 ** (1/13);
while( $base < 22050 ){
    printf OUT "%.4f\n", $base;
    $base *= $mult;
}
close $file;
print "wrote to $file\n";
