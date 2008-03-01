#! /usr/local/bin/perl -w
######################################################################
# $Id$

# Usage: gen_test [-Ifoo ...] [-Dbar ...] fname ...
#
# Note that this program relies on unit tests being in the following
# format:
#
# class my_test_fixture {
# public:
#    my_test_fixture()
#    ~my_test_fixture()
#
#    void test_function();
# }; // class my_test_fixture
#
# A new object will be created for every test function call, so setup
# and teardown should be in the constructor and destructor.  No inner
# classes.  Namespaces _must_ end with a comment // namespace X

use strict;

# process parameters
my %defines;
my @includes;
my @filenames;

foreach my $param (@ARGV) {
	if ($param =~ /-D(.*)/) {
		my $exp = $1;
		if ($exp =~ /(.*)=(.*)/) {
			$defines{$1} = {$2};
		} else {
			$defines{$exp} = 1;
		}
	} elsif ($param =~ /I(.*)/) {
		push @includes, $1;
	} else {
		push @filenames, $param;
	}
}

# parse file(s)
my @namespace;
my %functions;

FILE: foreach my $fname (@filenames) {
	open INPUT, $fname or die "$fname: $!\n";

	my $line_no = 0;
    LINE: foreach my $line (<INPUT>) {
		$line_no++; chomp $line;
		
		# check for namespace beginning and end
		if ($line =~ /^\s*(namespace|class)\s+(\S+)/) {
			push @namespace, $2;
			next LINE;
		}

		if ($line =~ /^\s*\}\;?\s*\/\/\s*(namespace|class)\s+(\S+)/) {
			my $cur = pop @namespace;
			if ($cur ne $2) {
				die "$fname\($line_no\): error: namespace conflict " .
					"(end of $2 while in $cur)\n";
			}
			next LINE;
		}
		
		# check for functions
		if ($line =~ /^\s*void\s+([Tt]est[^\s\(]*)\s*\(/) {
			&add_function(\%functions, \@namespace, $1);
		}
    } # LINE
	
	close INPUT;
} # FILE

# print header
print "// " . localtime() . "

#ifdef WIN32
#pragma warning (disable : 4273)
#endif

#ifdef WIN32
#define TEST_DLL_API __declspec(dllexport)
#else
#define TEST_DLL_API 
#endif

";

# print include files
foreach (@filenames) {
	print "#include \"$_\"\n";
}

# print functions
print "

#include <string>

extern \"C\" {

";

my @test_functions = &collect_functions(\%functions);

print '

struct {
    char *name;
    void (*test)(void);
} test_functions[] = {
';

foreach (@test_functions) {
	print "$_,\n"
}

print '
    { 0, 0 }
};

#if defined(WIN32)
    TEST_DLL_API void *get_test_functions() {
        return test_functions;
    }
#endif

} // extern "C"
';

# subroutines

sub collect_functions {
	my ($cur, $ns) = @_;
	$ns = [] unless defined $ns;
	
	my @res;

	if (defined $cur->{__functions}) {
		foreach (@{ $cur->{__functions} }) {
			my $class = join('::', @$ns);
			my $uname = join('_', @$ns) . "_$_";
			my $cname = "$class\:\:$_";
			push @res, "    { \"$cname\", $uname }";
			
			print "
TEST_DLL_API void $uname\(\);
			
void $uname\(\) \{
    $class \*ptr \= new $class\(\)\;
    ptr\-\>$_\(\)\;
    delete ptr\;
\}
";
		}
	}

	foreach (keys %$cur) {
		my @temp = @$ns;
		push @temp, $_;

		push @res, &collect_functions($cur->{$_}, \@temp) unless $_ eq '__functions';
	}

	return @res;
} # collect_functions

sub add_function {
	my ($functions, $namespace, $func) = @_;
	
	# get node in tree (build if necessary)
	my $cur = $functions;
	foreach my $ns (@$namespace) {
		if (!defined($cur->{$ns})) {
			$cur->{$ns} = {};
		}
		$cur = $cur->{$ns};
	}
	
	# add function
	if (!defined($cur->{__functions})) {
		$cur->{__functions} = [];
	}
	
	push @{ $cur->{__functions} }, $func;
} # add_function

