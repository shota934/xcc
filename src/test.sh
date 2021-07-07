#! /bin/bash
try(){
    expected="$1"
    input="$2"
    testcase="$3"
    ./xcc "$input"
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
	echo "$input => $actual"
	echo $testcase
    else
	echo "$expected expected, but got $actual"
	exit 1
    fi
}

try 30  test/test8.c                       "Passed test8"
try 90  test/test11.c                      "Passed test11"
try 10  test/test12.c                      "Passed test12"
try 21  test/test13.c                      "Passed test13"
try 100 test/test15.c                      "Passed test15"
try 30  test/test21.c                      "Passed test21"
try 30  test/test23.c                      "Passed test23"
try 20  test/test_struct.c                 "Passed test_struct"
try 10  test/test_ptr.c                    "Passed test_ptr"
try 30  test/test_ptr1.c                   "Passed test_ptr1"
try 70  test/test_struct_ptr.c             "Passed test_struct_ptr"
try 3   test/test_func_ptr.c               "Passed test_func_ptr"
try 21  test/test_struct5.c                "Passed test_struct5"
try 51  test/test_struct6.c                "Passed test_struct6"
try 90  test/test_struct7.c                "Passed test_struct7"
try 8   test/test_typedef.c                "Passed test_typedef"
try 55  test/test_for.c                    "Passed test_for"
try 55  test/test_while.c                  "Passed test_while"
try 10  test/test_if.c                     "Passed test_if"
try 10  test/test_switch1.c                "Passed test switch 1."
try 20  test/test_switch2.c                "Passed test switch 2."
try 30  test/test_switch3.c                "Passed test switch 3."
try 55  test/test_recursive_sum.c          "Passed recursive calling function."
try 1   test/test_and.c               "Passed test and."
try 3   test/test_or.c                "Passed test or."
try 1  test/test_equal.c              "Passed test equal."
try 2  test/test_not_equal.c          "Passed test not equal."
try 1  test/test_greater.c            "Passed test greater."
try 1  test/test_greater_than.c       "Passed test greater."
try 254 test/test_bit_reversal.c      "Passed test bit reversal."
try 1   test/test_logical_not.c       "Passed test logical not."
try 254 test/test_xor.c               "Passed test logical xor."
try 13  test/test_call_arr.c          "Passed test calling arr."
try 170 test/test_hex1.c             "Passed hex value case 1."
try 255 test/test_hex2.c             "Passed hex value case 2."
try 31  test/test_hex3.c             "Passed hex value case 3."
try 70  test/test_comment1.c         "Passed comment style1."
try 90  test/test_comment2.c         "Passed comment style2."
try 110 test/test_comment3.c         "Passed comment style3."
try 60  test/test_func_decl.c        "Passed function declaration."
try 100 test/test_static_func.c      "Passed static function."
try 200 test/test_extern_func.c      "Passed extern function."
try 21  test/test_typedef1.c         "Passed test typedef1."
try 20  test/test_typedef2.c         "Passed test typedef2."
try 30  test/test_typedef3.c         "Passed test typedef3."
try 50  test/test_union.c            "Passed test union."
try 60  test/test_include_stdio.c    "Passed test_include_stdio"
try 201 test/test_define_obj.c       "Passed test define_obj."
try 5   test/test_func_like_macro.c  "Passed test func like macro."
try 100 test/test_macro_if.c         "Passed test macro if."
try 200 test/test_macro_if_else.c    "Passed test macro if_else."
try 110 test/test_macro_if_not.c     "Passed test macro if_not."
try 8   test/test_sizeof_long.c      "Passed type size of long."
try 10  test/test_continue.c         "Passed continue."
try 2   test/test_div.c              "Passed test div."
try 3   test/test_mod.c              "Passed test mod."
try 55  test/sum.c                   "Passed test sum."
try 1   test/sum_arr.c               "Passed test sum of array."
try 1   test/test_array_op.c         "Passed test_array_op."
try 1   test/test_enum1.c            "Passed test_enum1."
try 1   test/test_enum1.c            "Passed test_enum1."
try 1   test/test_sizeof.c           "Passed test_sizeof."
try 0   test/test_sizeof1.c          "Passed test_sizeof1."
try 0   test/test_arr.c              "Passed test_arr."
try 0   test/test_sizeof_arr.c       "Passed test_sizeof_arr."
try 0   test/test_do_while.c         "Passed test_do_while."
try 0   test/test_goto.c             "Passed test_goto."

echo "Passed all Test cases."
