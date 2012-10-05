#!/usr/local/bin/perl 

# -*-Perl-*-

$file = "m3chord.scale";

open(OUT, ">$file");

$base = 27.5;

@ints = (1.2, 9/8, 1.2, 9/8, 9/8);
@ints = (1, 1.2, 4/3, 1.2 * (4/3), 16/9);
$phs = 0;


$octmult = 1;
while( $val < 22050 ){
    for $i (0..$#ints){
	$val = $base * $ints[$i] * $octmult;
	printf OUT "%.4f\n", $val;
    }
    $octmult *= 2;
}
close $file;
print "wrote to $file\n";
