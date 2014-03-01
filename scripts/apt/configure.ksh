#!/bin/ksh93

function check_toolchains {
	#apt-get install imagemagick
	if [[ "$(which equivs-build)" = "" ]]; then 
		print "\- equivs not installed (sudo apt-get install equivs)"
	fi
}

typeset return=$(check_toolchains)

if [[ "$return" = "" ]]; then
	printf "all_ok"
else
	printf "errors:\n$return\n"
fi

