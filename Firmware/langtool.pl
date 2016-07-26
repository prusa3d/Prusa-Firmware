#!/usr/bin/perl
# Processes language_xx.h files into language.cpp and language.h

use strict;
use warnings;

my @langs = ("en","cz","it","es","pl");

sub parselang 
{
    my ($filename) = @_;
 	open(my $fh, '<:encoding(UTF-8)', $filename)
#	open(my $fh, '<', $filename)
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

sub pgm_is_whitespace
{
	my ($c) = @_;
	if (! defined($c)) {
		print "pgm_is_whitespace: undefined\n";
		exit(1);
	}
    return $c == ord(' ') || $c == ord('\t') || $c == ord('\r') || $c == ord('\n');
}

sub pgm_is_interpunction
{
	my ($c) = @_;
    return $c == ord('.') || $c == ord(',') || $c == ord(';') || $c == ord('?') || $c == ord('!');
}

sub break_text_fullscreen
{
    my $lines = [];
    my ($text_str) = @_;
    if (! defined($text_str) || length($text_str) < 2) {
    	return $lines;
	}
	$text_str =~ s/^"//;
	$text_str =~ s/([^\\])"/$1/;
	$text_str =~ s/\\"/"/;

	my @msg = unpack("W*", $text_str);
    #my @msg = split("", $text_str);
    my $len = $#msg + 1;
    my $i = 0;

    LINE: 
    while ($i < $len) {
        while ($i < $len && pgm_is_whitespace($msg[$i])) {
            $i += 1;
        }
        if ($i == $len) {
            # End of the message.
            last LINE;
        }
        my $msgend2 = $i + ((20 > $len) ? $len : 20);
        my $msgend = $msgend2;
        if ($msgend < $len && ! pgm_is_whitespace($msg[$msgend]) && ! pgm_is_interpunction($msg[$msgend])) {
            # Splitting a word. Find the start of the current word.
            while ($msgend > $i && ! pgm_is_whitespace($msg[$msgend - 1])) {
                 $msgend -= 1;
            }
            if ($msgend == $i) {
                # Found a single long word, which cannot be split. Just cut it.
                $msgend = $msgend2;
            }
        }
        my $outstr = substr($text_str, $i, $msgend - $i);
        $i = $msgend;
        $outstr =~ s/~/ /g;
        #print "Output string: $outstr \n";
        push @$lines, $outstr;
    }

    return $lines;
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
 			print "Symbol $key undefined in language \"$lang\". Using the english variant.\n";
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
	my $strings = $texts{$key};
	if (@{$strings} == grep { $_ eq ${$strings}[0] } @{$strings}) {
		# All strings are English.
	    print $fh "extern const char* const ${key}_LANG_TABLE[1];\n";
		print $fh "#define $key LANG_TABLE_SELECT_EXPLICIT(${key}_LANG_TABLE, 0)\n";
	} else {
	    print $fh "extern const char* const ${key}_LANG_TABLE[LANG_NUM];\n";
		print $fh "#define $key LANG_TABLE_SELECT(${key}_LANG_TABLE)\n";
		print $fh "#define ${key}_EXPLICIT(LANG) LANG_TABLE_SELECT_EXPLICIT(${key}_LANG_TABLE, LANG)\n"
			if ($key eq "MSG_LANGUAGE_NAME" || $key eq "MSG_LANGUAGE_SELECT");
	}
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

my @keys = sort(keys %texts);
foreach my $key (@keys) {
	my $strings = $texts{$key};
	if (@{$strings} == grep { $_ eq ${$strings}[0] } @{$strings}) {
		# Shrink the array to a single value.
		$strings = [${$strings}[0]];
	}
	for (my $i = 0; $i <= $#{$strings}; $i ++) {
		my $suffix = uc($langs[$i]);
		if ($i == 0 || ${$strings}[$i] ne ${$strings}[0]) {
			print $fh "const char ${key}_${suffix}[] PROGMEM = ${$strings}[$i];\n";
		}
	}
	my $langnum = $#{$strings}+1;
	if ($langnum == $#langs+1) {
		$langnum = "LANG_NUM";
	}
    print $fh "const char * const ${key}_LANG_TABLE[$langnum] PROGMEM = {\n";
	for (my $i = 0; $i <= $#{$strings}; $i ++) {
		my $suffix = uc($langs[$i]);
		if ($i == 0 || ${$strings}[$i] ne ${$strings}[0]) {
			print $fh "\t${key}_${suffix}";
		} else {
			print $fh "\t${key}_EN";
		}
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

for my $lang (0 .. $#langs) {
	print "Language: $langs[$lang]\n";
	foreach my $key (@keys) {
		my $strings = $texts{$key};
		my $message = ${$strings}[$lang];
		if ($lang == 0 || ${$strings}[0] ne $message) {
			# If the language is not English, don't show the non-translated message.
			my $lines = break_text_fullscreen($message);
			my $nlines = @{$lines};
			if ($nlines > 1) {
				print "Multi-line message: $message. Breaking to $nlines lines:\n";
				print "\t$_\n" foreach (@{$lines});
			}
		}
	}
}

sub break_text_fullscreen
