#!/usr/local/bin/perl 

# -*-Perl-*-

$file = "blue3.scale";

open(OUT, ">$file");

$base = 27.5;

while( $base < 22050 ){
    printf OUT "%.4f\n", $base;
    $base *= 1.225;
}
close $file;
print "wrote to $file\n";
