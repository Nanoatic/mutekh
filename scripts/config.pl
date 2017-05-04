#!/usr/bin/perl

#
#     This file is part of MutekH.
#
#     MutekH is free software; you can redistribute it and/or modify it
#     under the terms of the GNU General Public License as published by
#     the Free Software Foundation; either version 2 of the License, or
#     (at your option) any later version.
#
#     MutekH is distributed in the hope that it will be useful, but
#     WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with MutekH; if not, write to the Free Software Foundation,
#     Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#
#     Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2010
#

use strict;
use Cwd;
use File::Basename;
use File::Compare;
use File::Path;
use Term::ANSIColor;
#use Data::Dumper;

my @config_files;
my @output_files;
my %config_opts;
my %inits;
my $quiet_flag = 0;
my $err_flag = 0;
my $bld_path = ".";
my $debug = $ENV{MUTEK_CONFIG_DEBUG};

my %param_h = (
	       "input" => "myconfig",
	       "build" => "default",
	       );

my %sec_types;
my %sec_types_req;
my %used_build = ( "default" => 1 );

my %vars;

###############################################################################
#	Reporting and messages
###############################################################################

sub mycolor
{
    return "" if (!-t STDERR);
    return "" if ($ENV{TERM} eq "dumb");
    return color(shift);
}

sub error
{
    $err_flag = 1;
    return if $quiet_flag;

    my ($msg, @list) = @_;
    my $tlist = join(", ", @list) if (@list);

    print STDERR mycolor('bold red')."error:".mycolor('clear red')."$msg$tlist\n".mycolor('reset');
}

sub error_loc
{
    my ($opt, $msg, @args) = @_;
    error("$opt->{file}:$opt->{location}: in $opt->{name} declaration: $msg", @args);
}

sub warning
{
    return if $quiet_flag;

    my ($msg, @list) = @_;
    my $tlist = join(", ", @list) if (@list);

    print STDERR mycolor('bold yellow')."warning:".mycolor('reset')."$msg$tlist\n";
}

sub warning_loc
{
    my ($opt, $msg, @args) = @_;
    warning("$opt->{file}:$opt->{location}: in $opt->{name} declaration: $msg", @args);
}

sub notice
{
    return if $quiet_flag;

    my ($msg, @list) = @_;
    my $tlist = join(", ", @list) if (@list);

    print STDERR mycolor('green')."notice:".mycolor('reset')."$msg$tlist\n";
}

sub debug
{
    return if !$debug;

    my ($level, $msg) = @_;

    print STDERR mycolor('blue')."debug:".mycolor('reset')."$msg\n";
}


###############################################################################
#	Configuration constraints parsing
###############################################################################

sub args_text_block
{
    my ($location, $opts, $tag, @args) = @_;

    $opts->{$tag} .= join(" ", @args)."\n";
}

sub args_text_line
{
    my ($location, $opts, $tag, @args) = @_;

    if ($opts->{$tag}) {
	error("$location: multiple `$tag' tags in use");
    }

    $opts->{$tag} = join(" ", @args);
}

sub args_function
{
    my ($location, $opts, $tag, $constr, $destr) = @_;

    $opts->{constructor} = $constr;
    $opts->{destructor} = $destr;
}

sub args_list_concat
{
    my ($location, $opts, $tag, @args) = @_;

    $opts->{$tag} ||= [];
    push(@{$opts->{$tag}}, @args);
}

sub args_word
{
    my ($location, $opts, $tag, @args) = @_;

    if ($opts->{$tag}) {
	error("$location: multiple `$tag' tags in use");
    }

    $opts->{$tag} = "@args";
}

sub args_list_add
{
    my ($location, $opts, $tag, @args) = @_;

    $opts->{$tag} ||= [];
    push(@{$opts->{$tag}}, [@args]);
}

sub args_single
{
    my ($location, $opts, $tag, @args) = @_;

    args_list_add($location, $opts, 'single', @args);
    args_list_add($location, $opts, 'depend', @args);
}

sub args_default
{
    my ($location, $opts, $tag, @args) = @_;
    my $value = "@args";

    warning($location.": default value redefined for `".$opts->{name}." token'") if (defined $opts->{default});

    $opts->{default} = $value;
}

sub args_flags
{
    my ($location, $opts, $tag, @args) = @_;

    foreach my $flag (@args)
    {
	if ( $flag !~ /^(internal|value|meta|root|noexport|mandatory|harddep|auto|private|maxval|minval|sumval)$/)
	{
	    error($location.": unknown flag `".$flag."' for `".$opts->{name}." token'");
	    next;
	}

	if (defined $opts->{flags}->{$flag})
	{
	    warning($location.": flag `".$flag."' is already set for `".$opts->{name}." token'");
	    next;
	}

	$opts->{flags}->{$flag} = 1;
	$opts->{flags}->{$flag."_keep"} = 1;
	$opts->{value} = 'defined' if $flag eq 'mandatory';
    }
}

sub args_module
{
    my ($location, $opts, $tag, $name, @desc) = @_;

    $opts->{module} = {
	name => $name,
	description => join(" ", @desc),
	path => $vars{CONFIGPATH}
    };
    $opts->{flags}->{root} = 1;
}

sub args_range
{
    my ($location, $opts, $tag, @args) = @_;

    if ( "@args" =~ /^\[\s*(-?\w+)\s*,\s*(-?\w+)\s*\]$/ ) {
	$opts->{range} = { 'min' => $1, 'max' => $2 };

    } else {
	my $sw = {};
	$sw->{$_} = 1 foreach ( @args );
	$opts->{switch} = $sw;
    }
}

my %config_cmd =
(
 "exclude" => \&args_list_concat,
 "default" => \&args_default,
 "depend" => \&args_list_add,
 "parent" => \&args_list_concat,
 "flags" => \&args_flags,
 "require" => \&args_list_add,
 "single" => \&args_single,
 "suggest" => \&args_list_concat,
 "suggest_when" => \&args_list_add,
 "when" => \&args_list_add,
 "module" => \&args_module,
 "provide" => \&args_list_concat,
 "range" => \&args_range,
 "desc" => \&args_text_block,
);

sub new_token_block
{
    my ($name, $file, $lnum) = @_;

    my $opts;

    if ($opts = $config_opts{$name}) {
	error("$file:$lnum: `$name' token already declared ".
	      "at $opts->{file}:$opts->{location}");
	return undef;
    }

    $opts = $config_opts{$name} = {};
    $opts->{location} = $lnum;
    $opts->{file} = $file;
    $opts->{name} = $name;
    $opts->{depnotice} = [];

    return ($opts, \%config_cmd);
}

my %init_cmd =
(
 "parent" => \&args_list_concat,
 "function" => \&args_function,
 "prototype" => \&args_text_line,
 "before" => \&args_list_concat,
 "after" => \&args_list_concat,
 "during" => \&args_word,
 "desc" => \&args_text_block,
);

sub new_init_block
{
    my ($name, $file, $lnum) = @_;

    my $init;

    if ($init = $inits{$name}) {
	error("$file:$lnum: `$name' init action already declared ".
	      "at $init->{file}:$init->{location}");
	return undef;
    }

    $init = $inits{$name} = {};
    $init->{location} = $lnum;
    $init->{file} = $file;
    $init->{name} = $name;

    return ($init, \%init_cmd);
}

my %config_blocks =
(
 "config" => \&new_token_block,
 "init" => \&new_init_block,
);

my %processed_files;

sub read_tokens_file
{
    my ($file) = @_;

    # skip already processed_files files
    $file = Cwd::realpath($file);

    return if ($processed_files{$file});
    $processed_files{$file} = 1;

    # process file

    if (!open(FILE, "< ".$file)) {
	return 1;
    }

    my $state = undef;
    my $lnum = 0;
    my $blk_name;
    my $opts;

    foreach my $line (<FILE>) {
	$lnum++;

	next if ($line =~ /^[ \t]*(\#.*)?$/);

	# catch blocks start and end

	if ($line =~ /^\s*%(\w+)\s+(\S+)/) {

	    $blk_name = $1;

	    if ($2 eq "end") {

		if (not $state) {
		    error("$file:$lnum: unexpected `%$blk_name end'");
		    next;
		}

		$state = undef;

	    } else {

		if ($state) {
		    error("$file:$lnum: unexpected `%$blk_name' block tag, previous ".
			  "block not terminated.");
		    $state = undef;
		}

		my $new_ = $config_blocks{$blk_name};

		if (!$new_) {
		    error("$file:$lnum: bad block type `%$blk_name'.");
		    next;
		}

		($opts, $state) = $new_->($2, $file, $lnum);
	    }

	    next;
	}

	# process block content

	if ($state) {
	    $line =~ s/^\s*//g;
	    $line =~ s/\$\((\w+)\)/$vars{$1}/ge;

	    my @line_l = split(/\s+/, $line);

	    # get pointer on function for tag
	    if (my $func_ptr = $state->{@line_l[0]}) {
		# call tag function
		$func_ptr -> ("$file:$lnum", $opts, @line_l[0], @line_l[1..@line_l - 1]);
	    } else {
		# error if unknow function
		error("$file:$lnum: bad tag name `".@line_l[0]."' in `%$blk_name' block");
	    }
	}
    }

    if ($state) {
	error("$file:$lnum: unexpected end of file, `%$blk_name end' expected");
    }

    close(FILE);

    return 0;
}

sub explore_token_dirs
{
    my ($dir) = @_;

    return if !$dir;
    error("Can not explore `$dir' directory") if ! -d $dir;

    # skip already processed_files dir
    $dir = Cwd::realpath($dir);

    return if ($processed_files{$dir});
    $processed_files{$dir} = 1;

    foreach my $ent (<$dir/*>)
    {
	explore_token_dirs($ent) if (-d $ent && !-l $ent);
    }

    foreach my $ent (<$dir/*.config>) {
	my $rp = Cwd::realpath($ent);
	push @config_files, $rp;
	$vars{CONFIGPATH} = dirname($rp);
	read_tokens_file($ent);
    }
}

###############################################################################
#	Token name resolve
###############################################################################

# config token name with optional condition attached

sub tokens_resolve_config_cond
{
    my ( $opt, $str, $tag ) = @_;

    my $res;

    if ( not $str =~ /^(\w+)(.*)$/ ) {
	error_loc($opt, "bad argument `$str` to `$tag' tag");
	return undef;
    }

    if ( my $token = $config_opts{$1} ) {
	$res = {
	    token => $token,
	    condition => $2,
	};
    }

    return $res;
}

# config token name with optional value attached

sub tokens_resolve_config_value
{
    my ( $opt, $str, $tag ) = @_;

    my $res;

    if ( not $str =~ /^(\w+)(?:=(.+))?$/ ) {
	error_loc($opt, "bad argument `$str` to `$tag' tag");
	return undef;
    }

    if ( my $token = $config_opts{$1} ) {
	$res = {
	    token => $token,
	    value => defined $2 ? $2 : 'defined',
	};
    }

    return $res;
}

# bare config token name

sub tokens_resolve_config_bare
{
    my ( $opt, $str, $tag ) = @_;

    return $config_opts{$str};
}

# bare init token name

sub tokens_resolve_init
{
    my ( $opt, $str, $tag ) = @_;

    return $inits{$str};
}

my %config_tokens_resolvers =
(
 "require" => \&tokens_resolve_config_cond,
 "suggest" => \&tokens_resolve_config_cond,
 "suggest_when" => \&tokens_resolve_config_cond,
 "when" => \&tokens_resolve_config_cond,
 "provide" => \&tokens_resolve_config_value,
 "parent" => \&tokens_resolve_config_bare,
 "depend" => \&tokens_resolve_config_bare,
 "single" => \&tokens_resolve_config_bare,
 "exclude" => \&tokens_resolve_config_bare,
);

my %init_tokens_resolvers =
(
 "parent" => \&tokens_resolve_config_bare,
 "after" => \&tokens_resolve_init,
 "before" => \&tokens_resolve_init,
 "need" => \&tokens_resolve_init,
 "during" => \&tokens_resolve_init,
);

sub tokens_resolve
{
    my ( $token_space, $resolvers ) = @_;

    # replace strings by token references in token tag data

    foreach my $opt ( values %$token_space ) {

	foreach my $tag ( keys %$resolvers ) {

	    my $r;
	    $r = sub {
		my $b = shift;

		return $b if !defined $b;

		if ( ref $b eq "HASH" ) {
		    return $b;

		} elsif ( ref $b eq "ARRAY" ) {
		    my $a = [];
		    foreach (@$b) {
			my $e = $r->($_);
			push @$a, $e if defined $e;
		    }
		    return $a;

		} else {
		    my $res;
		    my $token;

		    $res = $resolvers->{$tag}->( $opt, $b, $tag );

		    if (!$res) {
			error_loc($opt, "undeclared token `$b' used with `$tag' tag.");
		    }

		    return $res;
		}
	    };

	    $opt->{$tag} = $r->($opt->{$tag});
	}

    }
}

###############################################################################
#	Initialization actions
###############################################################################

my @init_actions;

sub process_inits_rec
{
    my ( $init, $sinit, $order, $iorder ) = @_;

    our %cycle;

    if ( $cycle{$init} ) {
	error_loc($init, "found cycle while processing `$order' tags");
	return;
    }

    $cycle{$init} = 1;

    sub process_inits_set
    {
        my ( $init, $sinit, $order, $iorder ) = @_;
    
        if ( $sinit->{"is$iorder"}->{$init} ) {
	    error("initialization order conflict `$sinit->{name} $order $init->{name}'");
	    return 1;
        }
    
        debug(1, "$sinit->{name} $order $init->{name}");
        $sinit->{"is$order"}->{$init} = 1;
    
        return 0;
    }
    
    sub process_inits_chld
    {
        my ( $init, $sinit, $order, $iorder ) = @_;
    
        foreach my $c ( @{$init->{childs}} ) {
	    process_inits_set( $c, $sinit, $order, $iorder );
    	    process_inits_chld( $c, $sinit, $order, $iorder );
        }
    }

    if ( $init->{$order} ) {
	foreach my $c ( @{$init->{$order}} ) {
	    next if process_inits_set( $c, $sinit, $order, $iorder );

	    process_inits_chld( $c, $sinit, $order, $iorder );
	    process_inits_rec( $c, $sinit, $order, $iorder );
	}
    }

    if ( $init->{during} ) {
	my $during = $init->{during};

#	process_inits_set( $during, $sinit, $order, $iorder ) if ( $init != $sinit );
	process_inits_rec( $during, $sinit, $order, $iorder );
    }

    $cycle{$init} = 0;
}

sub process_inits
{
    foreach my $init ( values %inits ) {
	$init->{defined} = !$init->{parent} || foreach_or_list( $init->{parent}, \&check_defined );
    }

    foreach my $init ( values %inits ) {
	next if !$init->{defined};

	# setup heirarchy
	if ( my $during = $init->{during} ) {

	    if ( $during->{defined} ) {

		$during->{childs} ||= [];
		push @{$during->{childs}}, $init;

		if ( $during->{constructor} ) {
		    error_loc($init, "init tokens used with `during' tag can not have `function' defined");
		}

	    } else {
		warning_loc($init, "initialization will not take place because `$during->{name}' is disabled") ;
		next;
	    }

	} elsif ( $init->{constructor} ) {
            warning_loc($init, "init token has `function' defined but is not attached to a parent token");
        }

        if ( $init->{prototype} && $init->{constructor} ) {
            error_loc($init, "init `prototype' can only be defined for non-leaf tokens (without `function')");
        }

	push @init_actions, $init;
    }

    foreach my $init ( @init_actions ) {
	process_inits_rec( $init, $init, "before", "after" );
	process_inits_rec( $init, $init, "after", "before" );
    };

    return if $err_flag;

    # reorder

    for ( my $chg = 1; $chg--; ) {

	for (my $i = 0; $i < scalar @init_actions; $i++) {
	    my $a = @init_actions[$i];

	    for (my $j = 0; $j < scalar @init_actions; $j++) {
		next if $i == $j;
		my $b = @init_actions[$j];

		if (($i < $j && $a->{isafter}->{$b}) ||
		    ($i > $j && $a->{isbefore}->{$b})) {
		    @init_actions[$i] = $b;
		    @init_actions[$j] = $a;
		    $chg = 1;
		    last;
		}
	    }
	}
    }

    # number

    for (my $i = 0; $i < scalar @init_actions; $i++) {
        my $a = @init_actions[$i];
        $a->{num} = $i;
    }

}

sub output_inits_details
{
    my ( $out, $actions ) = @_;

    foreach my $init (@$actions) {
        print {$out} "  $init->{name} ";
        print {$out} " $init->{constructor}(); " if ( $init->{constructor} );
        print {$out} " $init->{destructor}(); " if ( $init->{destructor} );
        print {$out} "\n";
    }
}

sub output_inits
{
    my ( $out, $actions ) = @_;

    foreach my $init (@$actions) {

        next if ( ! $init->{childs} );
        next if $init->{constructor};

        my @calls;

        foreach_recurs( $init, "childs", sub {
            my $chld = shift;
            push @calls, $chld;
        });

        print {$out} "#define $init->{name}_PROTOTYPES \\\n";
        foreach my $call ( @calls ) {
            my $args;
            my $par = $call;

            while ( $par = $par->{during} ) {
                last if ( $args = $par->{prototype} );                
            }
            print {$out} "  void $call->{constructor}($args); \\\n"
                if ( $call->{constructor} );
            print {$out} "  void $call->{destructor}($args); \\\n"
                if ( $call->{destructor} );
        }
        print {$out} "\n";

        @calls = sort { $a->{num} > $b->{num} } @calls;

        print {$out} "#define $init->{name}_INIT(...) \\\n";
        foreach my $call ( @calls ) {
            print {$out} "  $call->{constructor}(__VA_ARGS__); \\\n"
                if ( $call->{constructor} );
        }
        print {$out} "\n";

        print {$out} "#define $init->{name}_CLEANUP(...) \\\n";
        foreach my $call ( reverse @calls ) {
            print {$out} "  $call->{destructor}(__VA_ARGS__); \\\n"
                if ( $call->{destructor} );
        }
        print {$out} "\n";
    }
}

###############################################################################
#	Constraints rules checking
###############################################################################

# check a condition
sub check_condition
{
    my ($value, $cond) = @_;

    return ($value ne "undefined") if !$cond;

    if ( $cond =~ /^([=<>!]+)([^\s]*)$/ ) {
	my $op = $1;
	my $val = $2;

	if ($op eq "!")	{
	    return ($value eq "undefined");
	} elsif ($op eq "=") {
	    return ($value eq $val);
	} elsif  ($op eq ">") {
	    return ($value > $val);
	} elsif  ($op eq "<") {
	    return ($value < $val);
	}

    }

    error("bad operator in value test");
    return 0;
}

# check if a token is defined
sub check_defined
{
    my $token = shift;

    return $token->{getvalue}->( $token ) ne 'undefined';
}

# check a token condition rule
sub check_rule
{
    my $rule = shift;

    return check_condition( $rule->{token}->{getvalue}->( $rule->{token} ), $rule->{condition} );
}

# return true if one of the list elements evaluate to true
sub foreach_or_list
{
    my ( $list, $process, @args ) = @_;

    if ( $list ) {
	foreach ( @$list ) {
	    if ( $process->( $_, @args ) ) {
		return 1;
	    }
	}
    }

    return 0;
}

# return number of elements evaluating to true
sub foreach_count_list
{
    my ( $list, $except, $process, @args ) = @_;
    my $res = 0;

    if ( $list ) {
	foreach ( @$list ) {
	    if ( $process->( $_, @args ) ) {
		$res++;
	    }
	}
    }

    return $res == $except;
}

# return true if all of the list elements evaluate to true
sub foreach_and_list
{
    my ( $list, $process, @args ) = @_;

    if ( $list ) {
	foreach ( @$list ) {
	    if ( ! $process->( $_, @args ) ) {
		return 0;
	    }
	}
    }

    return 1;
}

# execute a closure if token flag is defined
sub if_flag
{
    my ( $token, $flag, $process, @args ) = @_;

    return 0 if ( !$token->{flags}->{$flag} );
    return $process->( $token, @args ) if $process;
    return 1;
}

sub is_equal
{
    my ( $a, $b, $process, @args ) = @_;

    return 0 if ( $a ne $b );
    return $process->( $a, @args ) if $process;
    return 1;
}

# iterate over all list and sub-lists elements
sub foreach_tag_args
{
    my ( $token, $tag, $process ) = @_;

    my $list = $token->{$tag};
    return if (!$list);

    my $r;
    $r = sub {
	my $list = shift;

	foreach ( @$list ) {
	    if ( ref $_ eq 'ARRAY' ) {
		$r->( $_ );
	    } else {
		$process->( $_ );
	    }
	}
    };

    $r->($list);
}

sub foreach_recurs
{
    my ( $token, $tag, $process ) = @_;

    my $list = $token->{$tag};
    return if (!$list);

    foreach ( @$list ) {
        $process->( $_ );
        foreach_recurs( $_, $tag, $process );
    }
}

sub get_token_name_list
{
    my ( $list, $sep, $prefix, $suffix ) = @_;
    my @names;
    $prefix ||= "`";
    $suffix ||= "'";
    push @names, $prefix.$_->{name}.$suffix foreach ( @$list );
    return join( $sep, @names );
}

sub get_rule_name_list
{
    my ( $list, $sep, $prefix, $suffix ) = @_;
    my @names;
    $prefix ||= "`";
    $suffix ||= "'";
    push @names, $prefix.$_->{token}->{name}.$_->{condition}.$suffix foreach ( @$list );
    return join( $sep, @names );
}

sub get_token_name
{
    my ( $token, $sep, $prefix, $suffix ) = @_;
    return $prefix.$token->{name}.$suffix;
}

sub get_rule_name
{
    my ( $rule, $sep, $prefix, $suffix ) = @_;
    return $prefix.$rule->{token}->{name}.$rule->{condition}.$suffix;
}

sub check_definable
{
    my ( $opt ) = @_;

    # this code shows how to simple check constraints. Most functions
    # below does the same with diagnostic printing code inserted.

    return 0 if $opt->{parent} && !foreach_or_list( $opt->{parent}, \&check_defined );
    return 0 if !foreach_and_list( $opt->{depend}, \&foreach_or_list, \&check_defined );
    return 0 if !foreach_and_list( $opt->{single}, \&foreach_count_list, 1, \&check_defined );

    # FIXME insert other tokens exclude list in token exclude list
    return 0 if foreach_or_list( $opt->{exclude}, \&check_defined );

    return 1 if !$opt->{flags}->{value};
    return 0 if !foreach_and_list( $opt->{require}, \&foreach_or_list, \&check_rule );
    return 1;
}

# check dependencies expressed with the `depend' and `parent' tags
sub process_config_depend
{

    # try to recursively define tokens marked with the `auto' flag
    sub process_auto
    {
	my ( $dep, $opt ) = @_;

	return 1 if ( check_defined( $dep ) );
	return 0 if !$dep->{flags}->{auto};
	return 0 if foreach_or_list( $opt->{exclude}, \&check_defined );

	# automatic token definition can be done only once to avoid loops
	$dep->{flags}->{auto} = 0;

	# try to recursively auto define parents and dependencies
	if ( !foreach_and_list( $dep->{depend}, sub {
	    return process_auto( $opt, shift->[0] );
        }) || ($dep->{parent} && !process_auto( $dep->{parent}->[0], $opt ) ) ) {
	    return 0;
	}

	if ( $dep->{userdefined} ) {
	    push @{$dep->{depnotice}}, "`$dep->{name}' token could be automatically defined ".
		"as a dependency of `$opt->{name}' but is explicitly undefined in ".
		"build configuration file.";

	    debug(1, "config prevents auto define of $dep->{name} as an autodep of $opt->{name}");
	    return 0;
	}

	$dep->{value} = 'defined';

	debug(1, "$dep->{name} had been defined as an autodep of $opt->{name}");

	return 1;
    }

    my ($opt) = @_;

    return 0 if ( !check_defined( $opt ) );
    return 0 if ( $opt->{flags}->{meta} || $opt->{flags}->{value} );

    # check if at least one parent is defined
    my $pres = 1;


    if ( $opt->{parent} && !foreach_or_list( $opt->{parent}, \&check_defined ) ) {
       # try define a parent with auto flag set
	$pres = foreach_or_list( $opt->{parent}, \&if_flag, 'auto', \&process_auto, $opt );
    }

    # check if all dependencies tags have at least one token defined
    my $dres = $pres && ( !$opt->{depend} || foreach_and_list( $opt->{depend}, sub {
	my $or_list = shift;

	return 1 if ( foreach_or_list( $or_list, \&check_defined ) );

	my $depnames = get_token_name_list( $or_list, " or " );

	# try to automatically define an `auto' dependency and parents
	if ( foreach_or_list( $or_list, \&if_flag, 'auto', \&process_auto, $opt ) ) {

	    return 1;

	# error if `harddep' dependency not satisfied
	} elsif ( $opt->{flags}->{harddep} || $opt->{flags}->{mandatory} ) {

	    $opt->{deperror} = "`$opt->{name}' token is required but has unmet dependencies: $depnames";

	    debug(1, "undefine $opt->{name} due to harddeps that are not satisfied: $depnames");

	} else {

	    push @{$opt->{depnotice}}, "`$opt->{name}' token will be undefined due to unmet dependencies: ".
		get_token_name_list( $or_list, " or " );

	    debug(1, "undefine $opt->{name} due to deps that are not satisfied: $depnames");
	}

	$opt->{value} = 'undefined';
	$opt->{depundef} = 1;
	return 0;
    }));

    # silently undefines token with undefined parent unless assigned in build configuration
    if ( !$pres ) {

	if ( $opt->{userdefined} ) {
	    push @{$opt->{depnotice}}, "`$opt->{name}' token is defined in build configuration ".
		"file but has undefined parent.";
	}

	$opt->{value} = 'undefined';
	$opt->{depundef} = 1;

	debug(1, "undefine $opt->{name} due to undefined parent");
    }

    # return changes status
    return !$dres || !$pres;
}

sub process_config_when
{
    my ( $opt ) = @_;

    return 0 if !$opt->{when} || $opt->{whendone};
    return 0 if ( check_defined( $opt ) || $opt->{userdefined} );

    my $res = foreach_or_list( $opt->{when}, \&foreach_and_list, \&check_rule );

    if ( $res ) {
	debug(1, "$opt->{name} defined thanks to one of its `when' rule");
	$opt->{value} = 'defined';

	# when rule is used only once
	$opt->{whendone} = 1;

	# clear existing undefine diagnostics
	$opt->{depnotice} = [];
	$opt->{deperror} = "";
	$opt->{depundef} = 0;
    }

    return $res;
}

sub process_config_exclude
{
    my ( $opt ) = @_;

    return 0 if ( !check_defined( $opt ) );

    foreach_and_list( $opt->{single}, sub {
	my $list = shift;

	my $res = foreach_count_list( $list, 1, \&check_defined );
	error("`$opt->{name}' requires that only one of these tokens is defined: ".
	      get_token_name_list( $list, " or " ) ) if !$res;
	return $res;
    });

    foreach_or_list( $opt->{exclude}, sub {
	my $ex = shift;

	return 0 if !check_defined( $ex );
	error("`$opt->{name}' and `$ex->{name}' can not be defined at the same time");
	return 1;
    });
}

sub process_config_require
{
    my ( $opt ) = @_;

    return 0 if ( $opt->{flags}->{value} || $opt->{flags}->{meta} );
    return 0 if ( !check_defined( $opt ) );

    return foreach_and_list( $opt->{require}, sub {
	my $rq = shift;

	my $res = foreach_or_list( $rq, \&check_rule );
	error("`$opt->{name}' is defined and requires ".get_rule_name_list( $rq ) ) if !$res;
	return $res;
    });
}

sub process_config_range
{
    my ( $opt ) = @_;

    if ($opt->{flags}->{value}) {

	# check value is in possibles values list
	if ($opt->{switch} && !$opt->{switch}->{$opt->{value}}) {
	    error("token `$opt->{name}' is set to `$opt->{value}' ".
		  "but allowed values are: ".join(" ", sort keys %{$opt->{switch}}) );
	}

	# check value range
	if ( $opt->{range} && ( $opt->{range}->{min} > $opt->{value} ||
				$opt->{range}->{max} < $opt->{value} ) ) {
	    error("token `$opt->{name}' is set to `$opt->{value}' but allowed ".
		  "range is: [$opt->{range}->{min}, $opt->{range}->{max}]" );
	}

    } else {
	error("token `$opt->{name}' is set to `$opt->{value}' value but lacks the `value' flag")
	    if ($opt->{value} !~ /(un)?defined/);
    }
}

sub process_config_suggest
{
    my ( $opt ) = @_;

    if ( check_defined( $opt ) ) {
	# suggest unmet condition that may be interesting with this token defined

	foreach_and_list( $opt->{suggest}, sub {
	    my $rule = shift;
	    my $res = check_rule( $rule );

	    if ( !$res ) {
		my $token = $rule->{token};

		# do not suggest if not a value token and can not define due to other constraints
		if ( $token->{flags}->{value} || check_definable( $token ) ) {
		    notice("`$opt->{name}' token is defined and suggests this configuration: ".
			   get_rule_name( $rule ) );
		}
	    }
	    return 1;
        });

    } elsif ( !$opt->{userdefined} ) {
	# suggest this token be defined

	foreach_and_list( $opt->{suggest_when}, sub {
	    my $list = shift;

	    my $res = foreach_and_list( $list, \&check_rule ) && check_definable( $opt );
	    notice("`$opt->{name}' token is currently undefined but suggested by: ".
		   get_rule_name_list( $list, " and " ) )." condition" if $res;
	    return 1;
        });
    }
}

sub tokens_set_methods
{
    foreach my $opt (values %config_opts) {

	# set getvalue method
	if ($opt->{flags}->{meta}) {

	    $opt->{providers} = [];
	    # meta token getvalue method returns 'defined' if one of its provider is defined
	    $opt->{getvalue} = sub {
		my ( $token ) = @_;

		foreach my $p (@{$token->{providers}}) {
		    return 'defined' if check_defined( $p );
		}
		return 'undefined';
	    }

	} elsif ($opt->{flags}->{value}) {

            my $combine;

	    # specify how to handle provide conflicts
            if ( $opt->{flags}->{maxval} ) {
                $combine = sub {
                    my ( $opt, $old, $new ) = @_;
                    return $old > $new ? $old : $new;
                }
            } elsif ( $opt->{flags}->{minval} ) {
                $combine = sub {
                    my ( $opt, $old, $new ) = @_;
                    return $old < $new ? $old : $new;
                }
            } elsif ( $opt->{flags}->{sumval} ) {
                $combine = sub {
                    my ( $opt, $old, $new ) = @_;
                    return $old + $new;
                }
            } else {
                $combine = sub {
                    my ( $opt, $old, $new ) = @_;
                    if ( $old != $new ) {
                        $opt->{deperror} = "Conflict between `$old' and `$new' values for `provide' on `$opt->{name}' token";
                    }
                    return $new;
                }
            }

	    # value token getvalue method returns value provided by provider tokens
	    $opt->{getvalue} = sub {
		my ( $token ) = @_;
		my $value = $token->{value};

		foreach my $p (@{$token->{providers}}) {
		    if ( check_condition( $p->{getvalue}->( $p ) ) ) {
                        my $new = $token->{provided}->{$p->{name}};
                        if ( $value eq "undefined" ) {
                            $value = $new;
                        } else {
                            $value = $combine->( $token, $value, $new );
                        }
		    }
		}

		return $value;
	    }

	} else {

	    # normal token getvalue method returns value
	    $opt->{getvalue} = sub {
		my ( $token ) = @_;

		return $token->{value};
	    }
	}

    }
}

sub tokens_provider
{
    foreach my $opt (values %config_opts) {

	# set token providers list
	foreach_tag_args( $opt, 'provide', sub {
	    my $c = shift;
	    my $t = $c->{token};

	    push @{$t->{providers}}, $opt;
	    $t->{provided}->{$opt->{name}} = $c->{value};
	});
    }
}

# flags exclusion table

#              value meta mandatory harddep auto
#  value              x      x        x      x
#  meta          y           x        x      x  
#  mandatory     y    y               x      x  
#  harddep       y    y      y                  
#  auto          y    y      y                  

my %flags_exclude = (
    "value/meta" => 1,
    "value/harddep" => 1,
    "value/auto" => 1,
    "value/mandatory" => 1,
    "meta/mandatory" => 1,
    "meta/harddep" => 1,
    "meta/auto" => 1,
    "mandatory/harddep" => 1,
    "mandatory/auto" => 1,
    );

sub tokens_check
{
    foreach my $opt (values %config_opts) {

	foreach my $fa (keys %{$opt->{flags}}) {
	    foreach my $fb (keys %{$opt->{flags}}) {
		if ( $flags_exclude{"$fa/$fb"} ) {
		    error_loc($opt, "flags `$fa' and `$fb' can not be used together.");
		}
	    }
	}

	my $private_check = sub {
	    my ( $p, $tag ) = @_;

	    if ( $p->{flags}->{private} && $p->{file} ne $opt->{file} && 
		 !foreach_or_list( $p->{parent}, \&is_equal, $opt ) ) {
		error_loc($opt, "token `$p->{name}' is private and can not be used with `$tag' outside `$p->{file}'.");
	    }
	};

	foreach_tag_args( $opt, 'provide', sub {
	    my $c = shift;
	    my $p = $c->{token};

	    if (!$p->{flags}->{meta} && !$p->{flags}->{value}) {
		error_loc($opt, "`provide' tag used on `$p->{name}' token without `meta' or `value' flag.");
	    }

	    if ($p->{flags}->{meta} && $c->{value} ne 'defined') {
		error_loc($opt, "can not specify a `provide' value for `$p->{name}' token with `meta' flag");
	    }

	    $private_check->( $p, 'provide' );
	});

	foreach_tag_args( $opt, 'require', sub {
	    my $p = shift->{token};

	    if (!$p->{flags}->{value}) {
		error_loc($opt, "`require' tag used on `$p->{name}' token without `value' flag; use `depend' instead");
	    }
	});

	if ( ( $opt->{range} || $opt->{switch} ) && !$opt->{flags}->{value} ) {
	    error_loc($opt, "`range' tag may only be used with `value' flagged tokens.")
	}

	if ( $opt->{flags}->{auto} && $opt->{default} eq 'defined' ) {
	    warning_loc($opt, "token has `auto' flag but is defined by default.")
	}

	if ($opt->{flags}->{value}) {

	    foreach my $tag (qw(depend when require provide suggest suggest_when exclude single)) {
		error_loc($opt, "token with `value' flag can't use the `$tag' tag.")
		    if ($opt->{$tag});
	    }
	} else {
            foreach my $f (qw(minval maxval sumval)) {
                error_loc($opt, "token with `$f' flag lacks the `value' flag.")
                    if ($opt->{flags}->{$f});
            }
        }

	if ($opt->{flags}->{meta}) {
	    foreach my $tag (qw(default depend when require suggest_when single)) {
		error_loc($opt, "token with `meta' flag can't use the `$tag' tag.")
		    if ($opt->{$tag});
	    }
	}

	if ($opt->{flags}->{mandatory}) {
	    foreach my $tag (qw(default when suggest_when suggest)) {
		error_loc($opt, "token with `mandatory' flag can't use the `$tag' tag.")
		    if ($opt->{$tag});
	    }
	}

	my %parents;

 	foreach_tag_args( $opt, 'parent', sub {
	    my $p = shift;

	    $parents{$p} = 1;
	    if ($p->{flags}->{value} || $p->{flags}->{meta}) {
		error_loc($opt, "token has the `$p->{name}' token with `value' or `meta' flag as parent.");
	    }

	    $private_check->( $p, 'parent' );
	});

	foreach_tag_args( $opt, 'depend', sub {
	    my $p = shift;

	    if ($parents{$p}) {
		warning_loc($opt, "use of `depend' on a parent token is useless.");
	    }

	    if ($p->{flags}->{value}) {
		error_loc($opt, "token depends on `$p->{name}' token with `value' flag; use `require' instead.");
	    }

	    $private_check->( $p, 'depend' );
	});

	foreach_tag_args( $opt, 'exclude', sub {
	    my $p = shift;

	    if ($p->{flags}->{value}) {
		error_loc($opt, "token excludes `$p->{name}' token with `value' flag; use `require' instead.");
	    }
	});

	foreach_tag_args( $opt, 'single', sub {
	    my $p = shift;
	    my $res = 0;

	    foreach_tag_args( $p, 'parent', sub {
		my $pp = shift;
		$res = 1 if ($pp == $opt);
	    });
	    if ( !$res ) {
		error_loc($opt, "use of `single' tag on `$p->{name}' token which is not a direct child.");
	    }
	});

	if (not $opt->{desc}) {
	    warning_loc($opt, "missing `desc` description tag");
	}

	if (uc($opt->{name}) ne $opt->{name})	{
	    warning_loc($opt, "token name is not strictly upper case");
	}

	if (not (lc($opt->{name}) =~ /^config_/)) {
	    warning_loc($opt, "token name does not begin with `CONFIG_' prefix");
	}

	if (!$opt->{parent} && !$opt->{flags}->{root}) {
	    warning_loc($opt, "token has no parent")
	}
    }
}

sub check_config
{
    # set default values
    foreach my $opt (values %config_opts) {
	if (not defined $opt->{value}) {
	    $opt->{default} = 'undefined' if !defined $opt->{default};
	    $opt->{value} = $opt->{default};
	    $opt->{vlocation} = "$opt->{file}:$opt->{location}";
	}
    }

    for ( my $chg = 1; $chg--; ) {
	# process `when' tags
	foreach my $opt (values %config_opts) {
	    if ( process_config_when($opt) ) {
		$chg = 1;
	    }
	}
	next if $chg;
	# checks and adjusts dependencies
	foreach my $opt (values %config_opts) {
	    if ( process_config_depend($opt) ) {
		$chg = 1;
	    }
	}
    }

    foreach my $opt (values %config_opts) {
	notice( $_ ) foreach ( @{$opt->{depnotice}} );
	error( $opt->{deperror} ) if ( $opt->{deperror} );
    }

    # store all token values of value and meta tokens
    foreach my $opt (values %config_opts) {
	next if !($opt->{flags}->{value} || $opt->{flags}->{meta});
	my $val = $opt->{getvalue}->( $opt );

	debug(1, "Setting $opt->{name} token value to $val")
	    if $opt->{value} eq $val;

	$opt->{value} = $val;
    }

    # check exclude, require and range tag constraints
    foreach my $opt (values %config_opts) {
	process_config_exclude($opt);
	process_config_require($opt);
	process_config_range($opt);
	process_config_suggest($opt);
    }
}

###############################################################################
#	Build configuration parsing
###############################################################################

sub read_build_config
{
    my ( $file, $section ) = @_;

    if ( ! -f $file ) {
        error("You need to specify a valid build configuration file in order to build MutekH");
        error("See: https://www.mutekh.org/trac/mutekh/wiki/BuildingExamples");
    }

    my $cd = dirname(Cwd::realpath($file));

    push @config_files, Cwd::realpath($file);

    $vars{CONFIGSECTION} = 'common';
    $vars{CONFIGPATH} = $cd;

    debug(1, "reading `$file' build configuration file...");

    if (!open(FILE, "<".$file)) {
	error("unable to open/read `$file' build configuration file");
	return 1;
    }

    my $lnum = 0;
    my @ignore = ( 0 );
    my @cur_sections = ( ["common"] );

    foreach my $line (<FILE>) {
	$lnum++;

	# skip empty lines and comment lines
	next if ($line =~ /^[ \t]*(\#.*)?$/);

	# replace variables
	$line =~ s/\$\((\w+)\)/$vars{$1}/ge;

	if ($line =~ /^\s* %(sub)?section \s+ ([*\w\d\s-]+)/x) {
	    my $s = $1;	# subsection ?
	    my @sections = split(/\s+/, $2);
	    my $i = 1;

	    if (!$s || !@ignore[0]) {

		foreach my $p (@sections) {

		    my $p_ = $p;
		    $p_ =~ s/\*/[\\w\\d]\+/g;

		    foreach (split(/:/, $section)) {
			if ( $_ =~ /^$p_$/ ) {
			    $i = 0;
			    $used_build{$_} = 1;
			    $vars{CONFIGSECTION} = $_ if ( !$s );
			}
		    }

		    last if !$i;
		}
	    }

	    if ( $s ) {
		unshift @ignore, $i;
		unshift @cur_sections, [ @sections ];
	    } else {
		@ignore = ( $i );
		@cur_sections = [ @sections ];
	    }
	    next;
	}

	if ($line =~ /^\s* %else\b/x) {
	    @ignore[0] = !@ignore[0] if !@ignore[1];
	    @cur_sections[0] = [];
	    next;
	}

	if ($line =~ /^\s* %end\b/x) {
	    if ( scalar @ignore < 2 ) {
		if ( scalar @ignore == 1 ) {
		    $line = "%common";
		} else {
		    error( "$file:$lnum: unbalanced %end.");
		}
	    } else {
		shift @ignore;
		shift @cur_sections;
	    }
	    next;
	}

	if ($line =~ /^\s* %common\b/x) {
	    $vars{CONFIGSECTION} = 'common';
	    @cur_sections = ( ["common"] );
	    @ignore = ( 0 );
	    next;
	}

	if ($line =~ /^\s* %types \s+ (\w[\w\d]*\b\s*)+$/x) {
	    foreach my $t (split(/\s+/, $1)) {
		if (!@ignore[0]) {
		    error( "$file: multiple `$t' section types in use" ) if ($sec_types{$t} == 1);
		    $sec_types{$t}++;
		}

		# keep track of available sections for a declared types
		if (!@ignore[1]) {
		    my $r = $sec_types_req{$t};
		    $r = [] if ( ! $r );
		    push @$r, $_ foreach (@{@cur_sections[0]});
		    $sec_types_req{$t} = $r;
		}
	    }
	    next;
	}

	next if @ignore[0];

	if ($line =~ /^\s* %set \s+ (\w[\w\d]*) \s+ (.*?) \s*$/x) {
	    $vars{$1} = $2;
	    next;
	}

	if ($line =~ /^\s* %append \s+ (\w[\w\d]*) \s+ (.*?) \s*$/x) {
	    $vars{$1} .= " $2";
	    next;
	}

	if ($line =~ /^\s* %error \s+ (.*)$/x) {
	    error("$file:$lnum: $1");
	    next;
	}

	if ($line =~ /^\s* %die \s+ (.*)$/x) {
	    error($1);
	    next;
	}

	if ($line =~ /^\s* %warning \s+ (.*)$/x) {
	    warning($1);
	    next;
	}

	if ($line =~ /^\s* %notice \s+ (.*)$/x) {
	    notice($1);
	    next;
	}

	if ($line =~ /^\s* %requiretypes \s+ (.+) $/x) {
	    foreach my $t (split(/\s+/, $1)) {
		next if ($sec_types{$t});

		my $r = $sec_types_req{$t};
		if ( !$r ) {
		    error("$file:$lnum: required `$t' type is never defined");
		} else {
		    error("no `$t' section in use, candidate sections are: ", @$r);
		}
	    }
	    next;
	}

	if ($line =~ /^\s* %include \s+ (\S+)/x) {
	    my $f = $1;
	    $f = "$cd/$f" unless $f =~ /^\//;
	    read_build_config( $f, $section );
	    next;
	}

	if ($line =~ /^\s* (\w[\w\d]*) (?: \s+(\S+) )?/x) {
	    my $opt = $config_opts{$1};
	    my $val = $2;

	    if (not $opt) {
		warning("$file:$lnum: undeclared configuration token `$1', ignored");

	    } else {
		if ($opt->{flags}->{internal} || $opt->{flags}->{meta}  || $opt->{flags}->{mandatory}) {
		    error("$file:$lnum: `".$opt->{name}."' token can not be defined in".
			  "build configuration file directly.");
		}

		$val = "defined" if (!defined $val);
		$opt->{value} = $val;
		$opt->{vlocation} = "$file:$lnum";
		$opt->{userdefined} = 1;
	    }
	    next;
	}

	warning("$file:$lnum: bad line format, ignored");
    }

    close(FILE);
    return 0;
}

###############################################################################
#	Output files
###############################################################################

sub write_header
{
    my $file = "$bld_path/config.h";

    push @output_files, $file;

    debug(1, "writing C header file to `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write configuration");
	return 1;
    }

    print FILE ("/*\n".
		" * This file has been generated by the configuration script.\n".
		" */\n\n");

    foreach my $opt (values %config_opts) {

	next if $opt->{flags}->{noexport};

	if ($opt->{value} eq "undefined") {
	    print FILE "#undef  ".$opt->{name}."\n";
	    next;
	}

	if ($opt->{value} eq "defined") {
	    print FILE "#define ".$opt->{name}."\n";
	    next;
	}

	print FILE "#define ".$opt->{name}." ".$opt->{value}."\n";
    }

    print FILE "\n#define __MUTEKH__\n";

    foreach my $var (sort keys %vars) {
	print FILE "#define BUILD_$var ".$vars{$var}."\n";
    }

    close(FILE);
    return 0;
}

sub write_depmakefile
{
    my $file = "$bld_path/config.deps";

    debug(1, "writing dependencies makefile to `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write configuration");
	return 1;
    }

    print FILE ("##\n".
		"## This file has been generated by the configuration script.\n".
		"##\n\n");

    print FILE $_." \\\n" foreach (@output_files);
    print FILE ":";
    print FILE " \\\n\t".$_ foreach (@config_files);
    print FILE "\n";

    close(FILE);
    return 0;
}

sub write_makefile
{
    my $file = "$bld_path/config.mk.tmp";
    my $file_ = "$bld_path/config.mk";

    push @output_files, $file_;

    debug(1, "writing makefile to `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write configuration");
	return 1;
    }

    print FILE ("##\n".
		"## This file has been generated by the configuration script.\n".
		"##\n");

    print FILE ("\n# configuration options\n\n");

    foreach my $opt (sort { $a->{name} cmp $b->{name} } values %config_opts) {
	next if $opt->{flags}->{noexport} or $opt->{flags}->{nomakefile};

	print FILE $opt->{name}."=".$opt->{value}."\n";
    }

    print FILE ("\n# configuration variables\n\n");

    foreach my $var (sort keys %vars) {
	print FILE "BUILD_$var=".$vars{$var}."\n";
    }

    close(FILE);

    if ( ! -f $file_ || compare($file, $file_) ) {
	unlink($file_);
	rename($file, $file_);
	return -1;
    } else {
	unlink($file);
	return $debug;  # assume overwriten if debug mode
    }
}

sub write_m4
{
    my $file = "$bld_path/config.m4";

    push @output_files, $file;

    debug(1, "writing m4 file to `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write configuration");
    }

    foreach my $opt (values %config_opts) {
	next if $opt->{flags}->{noexport};

	print FILE "m4_define(".$opt->{name}.", `".$opt->{value}."')\n";
    }

    close(FILE);
    return 0;
}

sub write_py
{
    my $file = "$bld_path/config.py";

    push @output_files, $file;

    debug(1, "writing python file to `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write configuration");
	return 1;
    }

    foreach my $opt (values %config_opts) {
	next if $opt->{flags}->{noexport};

	print FILE $opt->{name}." = '".$opt->{value}."'\n";
    }

    close(FILE);
    return 0;
}

sub write_inits
{
    my $file = "$bld_path/inits.c";

    push @output_files, $file;

    debug(1, "writing initialization code `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write initialization code");
	return 1;
    }

    output_inits( \*FILE, \@init_actions );

    close(FILE);
    return 0;
}

sub write_infos
{
    my $file = "$bld_path/build.log";

    push @output_files, $file;

    debug(1, "writing info text file `$file'");

    if (!open(FILE, ">>".$file)) {
	error(" unable to open `$file' to write build summary text file");
	return 1;
    }

    print FILE "Flattened build configuration file:\n\n";
    flat_config( \*FILE );
    print FILE "\n";

    print FILE "Initialization order:\n\n";
    output_inits_details( \*FILE, \@init_actions );
    print FILE "\n";

    print FILE "Configuration used:\n\n";
    tokens_list( \*FILE, 0 );
    print FILE "\n";

    close(FILE);
    return 0;
}

###############################################################################
#	Help and Documentation output
###############################################################################

sub flat_config
{
    my ( $out ) = @_;

    foreach my $name (sort keys %config_opts)
    {
	my $opt = $config_opts{$name};

	next if (!$opt->{userdefined});

	print {$out} "  $opt->{name} $opt->{value}\n";
    }
}

sub tokens_list
{
    my ( $out, $all ) = @_;

    printf {$out} (" %-6s%-40s %-16s %s \n", "", "Configuration token name", "Value", "Declare location");
    print {$out} "="x80, "\n\n";

    foreach my $name (sort keys %config_opts)
    {
	my $opt = $config_opts{$name};
	my $attr;

	if ( not $all )
	{
	    next if ($opt->{flags}->{internal} || $opt->{flags}->{meta});
	    next if !$opt->{flags}->{root} && !foreach_or_list( $opt->{parent}, \&check_defined );
	}

	if ($opt->{flags}->{value_keep}) {
	    $attr .= "v";
	} elsif ($opt->{value} eq 'defined') {
	    $attr .= "+";
	} else {
	    $attr .= " ";
	}
	$attr .= " ";
	if ($opt->{module}) {
	    $attr .= "X";
	}
	if ($opt->{userdefined}) {
	    $attr .= "U";
	}
	if ($opt->{flags}->{mandatory_keep}) {
	    $attr .= "M";
	}
	if ($opt->{flags}->{auto_keep}) {
	    $attr .= "A";
	}
	if ($opt->{flags}->{internal_keep}) {
	    $attr .= "i";
	}
	if ($opt->{flags}->{meta_keep}) {
	    $attr .= "m";
	}

	printf {$out} (" %-6s%-40s %-16s %-16s ", $attr, $name, $opt->{value},
	       basename($opt->{file}).":".$opt->{location});

	if ( $all && $opt->{vlocation}) {
	    print {$out} "  ".basename($opt->{vlocation});
	}

	print {$out} "\n";
    }

    print {$out} "\n";
    print {$out} "    (+) defined, (U) assigned in build config file, (X) module.\n";
    print {$out} "    (v) value, (A) automatic dependency, (M) mandatory.\n";
    if ( $all ) {
	print {$out} "    (i) for internal use, (m) meta: provided by other token.\n";
    }
    print {$out} "\n";
}

sub write_token_doc
{
    my ( $out, $opt ) = @_;

    my $pwd = Cwd::realpath($ENV{PWD});
    my $loc = "$opt->{file}:$opt->{location}";
    $loc =~ s/^$pwd\///;

    print {$out} "   Token description: $opt->{desc}\n\n";

    print {$out} "   This token in declared in \@sourcelink $loc.\n\n";
    print {$out} "   The default value is `$opt->{default}'.\n\n" if ( defined $opt->{default} );
    print {$out} "   This token is a meta token and can not be defined directly.\n\n" if ( $opt->{flags}->{meta} );

    sub print_list
    {
	my ( $out, $list, $title, $sep, $disp ) = @_;

	return if !$list;
	print {$out} "   $title:\n";
	print {$out} "   \@list\n";
	foreach ( @$list ) {
	    my $text = $disp->( $_, $sep, '#', ' ' );
	    $text =~ s/\w+/$& /g;
	    print {$out} "      \@item ".$text."\n";
	}
	print {$out} "   \@end list\n\n";
    }

    print_list($out, $opt->{parent}, "This token has the following parents", ", ", \&get_token_name );
    print_list($out, $opt->{depend}, "This token depends on", " or ", \&get_token_name_list );
    print_list($out, $opt->{require}, "This token requires", " or ", \&get_rule_name_list );
    print_list($out, $opt->{when}, "This token is automatically defined when", " and ", \&get_rule_name_list );
    print_list($out, $opt->{suggest}, "Defining this token suggest use of", " or ", \&get_rule_name );
    print_list($out, $opt->{suggest_when}, "Definition of this token is suggested when", " and ", \&get_rule_name_list );
    print_list($out, $opt->{exclude}, "This token can not be defined along with", "", \&get_token_name );
    print_list($out, $opt->{provide}, "Defining this token will also provide", "", \&get_rule_name );
    print_list($out, $opt->{providers}, "This token value is provided along with", "", \&get_token_name );

        # FIXME add range and more flags infos
}

sub tokens_info
{
    my ( $name ) = @_;
    my $opt = $config_opts{$name};

    if ( !$opt ) {
	error("no such token `$name'");
	return 1;
    }

    print STDOUT "   Token name: $name\n";
    print STDOUT "   Token current value: $opt->{value}\n";

    write_token_doc( \*STDOUT, $opt );

    return 0;
}

sub write_doc_header
{
    my ( $file ) = @_;

    push @output_files, $file;

    debug(1, "writing doc header file to `$file'");

    if (!open(FILE, ">".$file)) {
	error(" unable to open `$file' to write documentation");
	return 1;
    }

    print FILE ("/** \@file \@hidden\n".
		" * This file has been generated by the configuration script.\n".
		" */\n\n");

    foreach my $opt (values %config_opts) {

	next if $opt->{flags}->{noexport};
	my $mod = $opt;

	while ( $mod && !$mod->{module} && $mod->{parent} ) {
	    $mod = $mod->{parent}->[0];
	}

	print FILE "/**\n";
	write_token_doc( \*FILE, $opt );
	print FILE "   \@module {".$mod->{module}->{description}."}\n"
	    if $mod && $mod->{module} && $mod->{module}->{description};
	print FILE "   \@internal\n" if ( $opt->{flags}->{internal} );
	print FILE "*/\n#define ".$opt->{name}."\n\n";
	next;
    }

    close(FILE);
    return 0;
}

###############################################################################
#	Main function
###############################################################################

sub main
{
    foreach my $param (@ARGV) {
	error " bad command line option `$param'"
	    if (! ($param =~ /--([^=]+)=?(.*)/));

	my $name = $1;
	my $value = $2 ? $2 : 1;

	$name =~ s/-/_/g;
	$param_h{$name} = $value;

	debug(1, "command line option: `$param' $value");
    }

    if (not @ARGV or $param_h{help} or !$param_h{src_path}) {
	print "
Usage: config.pl [options]

	--input=file:...    Set build configuration file list (myconfig).
	--build=name:...    Set build configuration enabled section names.

	--src-path=dir:...  Set list of source directories to explore, default is `.'.
	--build-path=dir    Set output directory base, default is `./'.
	--build-name=name   Set build name, default is `arch-cpu'.
	--output-name=name  Set kernel binary name, default is 'kernel'.

        --config            Output .h, .py, .m4, .mk and .deps configuration in `config.*' files.
	--check             Check configuration constraints without output.
	--list[=all]        Display configuration tokens list.
	--info=token        Display informations about `token'.
	--docheader=file    Output header with documentation tags in `file' file.
	--quiet             Do not output diagnostic messages.

";
	return;
    }

    delete $vars{CONFIGSECTION};
    delete $vars{CONFIGPATH};
    $vars{SRC_DIR} = $ENV{MUTEK_SRC_DIR};
    $vars{BUILD_NAME} = $param_h{build_name};
    $vars{OUTPUT_NAME} = $param_h{output_name};

    $quiet_flag = defined $param_h{quiet};
    $debug |= defined $param_h{debug};

    debug(1, "explore source tree and parse .config token files");

    explore_token_dirs($_) foreach (split(/:/, $param_h{src_path}));

    debug(1, "resolve and check tokens");

    tokens_set_methods();
    tokens_resolve( \%config_opts, \%config_tokens_resolvers );
    tokens_resolve( \%inits, \%init_tokens_resolvers );
    tokens_provider();
    tokens_check();

    exit 1 if $err_flag;

    debug(1, "read build configuration files");

    if ($param_h{build_path}) {
	$bld_path = $param_h{build_path};
    }

    if ($param_h{docheader}) {
	write_doc_header($param_h{docheader});
	exit 0;
    }

    if ( !$param_h{input} ) {
	error("no build configuration file specified.");
    }

    read_build_config( $_, $param_h{build} )
	foreach (split(/:/, $param_h{input}));

    debug(1, "check build configuration files");

    foreach (split(/:/, $param_h{build})) {
	error("build section name `$_' never considered in configuration file")
	    if ( !$used_build{$_} );
    }

    exit 1 if $err_flag;

    check_config();
    process_inits();

    debug(1, "help and info display actions");

    if ($param_h{list}) {
	return tokens_list( \*STDOUT, $param_h{list} eq "all" );
    }

    if ($param_h{info}) {
	return tokens_info($param_h{info});
    }

    debug(1, "declare modules");

    foreach my $opt (values %config_opts) {
	if ( defined $opt->{module} && check_defined($opt) ) {
	    $vars{MODULES} .= " ".$opt->{module}->{name}.":".$opt->{module}->{path};
	}
    }

    debug(1, "setup some special variables");

    if ( !$vars{OUTPUT_NAME} ) {
	$vars{OUTPUT_NAME} = 'kernel';
    }

    if ( !$vars{BUILD_NAME} ) {
	my $arch = $config_opts{CONFIG_ARCH_NAME};
	my $cpu = $config_opts{CONFIG_CPU_NAME};
	$vars{BUILD_NAME} = $arch->{value}."-".$cpu->{value};
    }

    my $bld_name = $vars{OUTPUT_NAME}."-".$vars{BUILD_NAME};
    $bld_path .= $bld_name;

    debug(1, "take action based on command line options");

    exit 1 if $err_flag;

    delete $vars{CONFIGSECTION};
    delete $vars{CONFIGPATH};

    if ($param_h{config}) {
	mkpath($bld_path);
	error("unable to create build directory `$bld_path'") if (! -d $bld_path);
	exit 1 if $err_flag;

	debug(1, "build name is `$bld_name'");
	print $bld_name."\n";

	if (write_makefile()) {
	    write_header();
	    write_m4();
	    write_py();
	    write_inits();
	    write_depmakefile();
            write_infos();
	}
	return;
    }

}

main;
exit 0;


# Local Variables:
# tab-width: 8
# basic-offset: 4
# End:
