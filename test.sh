#!/bin/bash
export ASAN_OPTIONS=verify_asan_link_order=0
# Trigger all your test cases with this script
if xargs -a tests/wrong_name.args ./vm_x2017 | diff - tests/wrong_name.out; then
echo "wrong_name: Passed"; else
echo "wrong_name: Failed (Please check file name)"; fi
if xargs -a tests/multiple_args.args ./vm_x2017 | diff - tests/multiple_args.out; then
echo "multiple_args: Passed"; else
echo "multiple_args: Failed (Please check command line arguments)"; fi
if xargs -a tests/pc_overflow.args ./vm_x2017 | diff - tests/pc_overflow.out; then
echo "pc_overflow: Passed"; else
echo "pc_overflow: Failed (Please check program counter value)"; fi
if xargs -a tests/no_entry.args ./vm_x2017 | diff - tests/no_entry.out; then
echo "no_entry: Passed"; else
echo "no_entry: Failed (Please check function label)"; fi
if xargs -a tests/wrong_bin1.args ./vm_x2017 | diff - tests/wrong_bin1.out; then
echo "wrong_bin1: Passed"; else
echo "wrong_bin1: Failed (Please check function numbers)"; fi
if xargs -a tests/wrong_reg.args ./vm_x2017 | diff - tests/wrong_reg.out; then
echo "wrong register address: Passed"; else
echo "wrong register address: Failed (Please check register address)"; fi
if xargs -a tests/wrong_call.args ./vm_x2017 | diff - tests/wrong_call.out; then
echo "wrong call function: Passed"; else
echo "wrong call function: Failed (Please check call function value)"; fi
if xargs -a tests/no_return.args ./vm_x2017 | diff - tests/no_return.out; then
echo "no return function: Passed"; else
echo "no return function: Failed (Please check return function)"; fi
if xargs -a tests/wrong_type.args ./vm_x2017 | diff - tests/wrong_type.out; then
echo "wrong value type: Passed"; else
echo "wrong value type: Failed (Please check value type)"; fi