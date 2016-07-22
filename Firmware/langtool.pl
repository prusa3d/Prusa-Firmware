#!/usr/bin/perl
# Processes language_xx.h files into language.cpp and language.h

use strict;
use warnings;

my @langs = ("en","cz","it","es","pl");

sub parselang 
{
    my ($filename) = @_;
 	open(my $fh, '<:encoding(UTF-8)', $filename)
  		or die "Could not open file '$filename' $!";
  	# Create a new hash reference.
	my $out = {};
	while (my $line = <$fh>) {
		chomp $line;
		next if (index($line, 'MSG') == -1);
    	$line =~ /(?is)\#define\s*(\S*)\s*(.*)/;
    	my $symbol = $1;
    	my $v = $2;
    	next if (! defined $symbol or length($symbol) == 0);
    	# Trim whitespaces from both sides
    	$v =~ s/^\s+|\s+$//g;
		#$string =~ s/" MACHINE_NAME "/Prusa i3/;
		$v =~ s/" FIRMWARE_URL "/https:\/\/github.com\/prusa3d\/Prusa-i3-Plus\//;
		$v =~ s/" PROTOCOL_VERSION "/1.0/;
		$v =~ s/" STRINGIFY\(EXTRUDERS\) "/1/;
		$v =~ s/" MACHINE_UUID "/00000000-0000-0000-0000-000000000000/;
		${$out}{$symbol} = $v;
	}
	return $out;
}

my %texts;
my $num_languages = 0;
foreach my $lang (@langs) {
	my $symbols = parselang("language_$lang.h");
 	foreach my $key (keys %{$symbols}) {
 		if (! (exists $texts{$key})) {
	 		$texts{$key} = [];
 		}
 		my $strings = $texts{$key};
 		die "Symbol $key defined first in $lang, undefined in the preceding language files."
 			if (scalar(@$strings) != $num_languages);
 		push @$strings, ${$symbols}{$key};
 	}
 	$num_languages += 1;
 	foreach my $key (keys %texts) {
 		my $strings = $texts{$key};
 		if (scalar(@$strings) != $num_languages) {
 			# die "Symbol $key undefined in $lang."
 			print "Symbol $key undefined in $lang. Using the english variant.\n";
 			push @$strings, ${$strings}[0];
 		}
 	}
}

my $filename = 'language_all.h';
open(my $fh, '>', $filename) or die "Could not open file '$filename' $!";

# For the programmatic access to the program memory, read
# http://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html

print $fh <<END
#ifndef LANGUAGE_ALL_H
#define LANGUAGE_ALL_H

#define LANG_NUM (${num_languages})

extern unsigned char lang_selected;

#define LANG_TABLE_SELECT_EXPLICIT(TABLE, LANG) ((const char*)(pgm_read_ptr(TABLE + (LANG))))
#define LANG_TABLE_SELECT(TABLE) LANG_TABLE_SELECT_EXPLICIT(TABLE, lang_selected)

END
;

foreach my $key (sort(keys %texts)) {
    print $fh "extern const char* const ${key}_LANG_TABLE[LANG_NUM];\n";
	print $fh "#define $key LANG_TABLE_SELECT(${key}_LANG_TABLE)\n";
	print $fh "#define ${key}_EXPLICIT(LANG) LANG_TABLE_SELECT_EXPLICIT(${key}_LANG_TABLE, LANG)\n"
		if ($key eq "MSG_LANGUAGE_NAME" || $key eq "MSG_LANGUAGE_SELECT");
}

print $fh <<END

extern char* CAT2(const char *s1,const char *s2);
extern char* CAT4(const char *s1,const char *s2,const char *s3,const char *s4);

#endif //LANGUAGE_ALL.H
END
;
close $fh;
print ".h created\n";



$filename = 'language_all.cpp';
open($fh, '>', $filename) or die "Could not open file '$filename' $!";

print $fh <<'END'
#include <avr/pgmspace.h>
#include "configuration_prusa.h"
#include "language_all.h"

#define LCD_WIDTH 20
extern unsigned char lang_selected;

END
;

foreach my $key (sort(keys %texts)) {
	my $strings = $texts{$key};
	for (my $i = 0; $i <= $#{$strings}; $i ++) {
		my $suffix = uc($langs[$i]);
		print $fh "const char ${key}_${suffix}[] PROGMEM = ${$strings}[$i];\n";
	}
    print $fh "const char * const ${key}_LANG_TABLE[LANG_NUM] PROGMEM = {\n";
	for (my $i = 0; $i <= $#{$strings}; $i ++) {
		my $suffix = uc($langs[$i]);
		print $fh "\t${key}_${suffix}";
		print $fh ',' if $i < $#{$strings};
		print $fh "\n";
	}
	print $fh "};\n\n";
}

print $fh <<'END'

char langbuffer[LCD_WIDTH+1];
char* CAT2(const char *s1,const char *s2) {
  unsigned char len=0;
  strncpy_P(langbuffer+len,s1,LCD_WIDTH-len);
  len+=strlen_P(s1);
  strncpy_P(langbuffer+len,s2,LCD_WIDTH-len);
  return langbuffer;
}
char* CAT4(const char *s1,const char *s2,const char *s3,const char *s4) {
  unsigned char len=0;
  strncpy_P(langbuffer+len,s1,LCD_WIDTH-len);
  len+=strlen_P(s1);
  strncpy_P(langbuffer+len,s2,LCD_WIDTH-len);
  len+=strlen_P(s2);
  strncpy_P(langbuffer+len,s3,LCD_WIDTH-len);
  len+=strlen_P(s3);
  strncpy_P(langbuffer+len,s4,LCD_WIDTH-len);
  return langbuffer;
}
END
;

print ".cpp created.\nDone!\n";
