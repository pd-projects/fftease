#!/usr/local/bin/perl 

# -*-Perl-*-

$file = "4th.scale";

open(OUT, ">$file");

$base = 27.5;

@ints = (1.2, 1.25);
$phs = 0;

while( $base < 22050 ){
    printf OUT "%.4f\n", $base;
    $base *= 1.33333333333;
}
close $file;
print "wrote to $file\n";
