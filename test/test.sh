#!/bin/bash

test_files()
{
	local F

	for F in ${@:-samples/*}
	do
		echo ">> processing $F"
		$BIN $F | diff - $F || exit $?
	done
}

test_find()
{
	$BIN - ${@:-?hello/world/country?name=England/city samples/hello.xml}
	$BIN - ${@:-?hello/world/country/city samples/hello.xml}
}

all()
{
	echo '-- test_find --------------------------------------'
	test_find

	echo '-- test_files -------------------------------------'
	test_files
}

readonly BIN='./xmlparse'

(cd .. && make clean && make) && make clean && make || exit $?
${@:-all}
