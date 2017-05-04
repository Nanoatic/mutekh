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

die "file name excpected" if (@ARGV < 1);

foreach my $filename (@ARGV)
{
    print "processing ".$filename." ...\n";
    my $state = 0;

    open(FILEI, "<".$filename) or die "unable to open file";
    open(FILEO, ">".$filename.".tmp") or die "unable to open tmp file";

    foreach my $line (<FILEI>)
    {
	if ($line =~ /^\/\*\s*backslash-region-end/)
	{
	    die "no backslash region to terminate" if (not $state);
	    $state = 0;
	}

	chomp($line);

	if ($state)
	{
	    $line =~ s/[\s\\]*$//;
	}

	if ($line =~ /^\/\*\s*backslash-region-begin/)
	{
	    die "unterminated backslash region" if ($state);
	    $state = 1;
	} 

	print FILEO $line."\n";
    }

    die "unterminated backslash region at end of file" if ($state);

    close (FILEI);
    close (FILEO);

    rename ("$filename.tmp", "$filename");

}

