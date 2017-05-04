#!/usr/bin/perl

# Copyright (C) 2009 Alexandre Becoulet
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

use strict;
use integer;

my $region_state = 0;
my $block_state = 0;
my $count = 0;
my $loc = 1;

foreach my $line (<STDIN>)
{
    if ($line =~ /^\/\*\s*backslash-region-end\b/)
    {
	die "no backslash region to terminate" if (not $region_state);
	$region_state = 0;
    }

    $line =~ s/\s*$//;
    $line =~ s/(^|[^\t]+)(\t+)/$1." " x (length($2) * 8 - (length($1) & 7))/ge;

    $block_state = 0 if ($line eq "");

    if ($region_state || $block_state)
    {
	$line =~ s/[\s\\]*$//;

	if ( $line !~ /^\s*\#\s*define\b/ ) {
	    print STDERR "warning:$loc:backslash region doesn't start with #define\n"
		if ( $count == 1 );
	} else {
	    print STDERR "warning:$loc:#define in backslash region\n"
		if ( $count > 1 );
	}

	my $tab_count = 9 - (length($line) / 8);

	if (length($line) < 72)
	{
	    $line .= "\t" x (9 - (length($line) / 8)) . "\\";
	}
	else
	{
	    $line .= " \\";
	}

	$count++;
    }

    if ($line =~ /^\/\*\s*backslash-region-begin/)
    {
	die "unterminated backslash region" if ($region_state || $block_state);
	$region_state = 1;
	$count = 1;
    } 

    elsif ($line =~ /^\/\*\s*backslash-block/)
    {
	die "unterminated backslash region" if ($region_state || $block_state);
	$block_state = 1;
	$count = 1;
    } 

    print STDOUT $line."\n";
    $loc++;
}

die "unterminated backslash region at end of file" if ($region_state);

