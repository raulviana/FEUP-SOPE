#!/usr/bin/env bash

show_usage() {
  echo "Usage: $0 <zip file>"

  exit 1
}

print_result() {
  local res=${1:-none}
  local str=${2:-''}
  local ret=${3:-0}

  # add an empty line
  echo ""

  # success (OK) in green; failure (FAILED) in red; unknown is ??????
  case "$res" in
    ok) printf "[   \e[1;32mOK\e[0m   ]";;
    failed) printf "[ \e[1;31mFAILED\e[0m ]";;
    *) printf "[ ?????? ]";;
  esac

  # print the message
  printf " $str\n"

  # exit using the return code, if any (0 by default)
  exit $ret
}

print_success() { print_result ok "$@"; }
print_failure() { print_result failed "$@"; }

# check the number of arguments
if [ $# -ne 1 ]; then
  show_usage
fi

filename="$(basename $1)"
regex='^T[1-7]G(0[1-9]|[1-9][0-9])\.zip$'

# check file name
if [[ ! "$filename" =~ $regex ]]; then
  print_failure "Invalid filename ($filename) => TxGyy.zip [0 < x < 8; yy > 0]!" 2
fi

gid="${filename%%.*}"
path=/tmp/${gid}

# remove folder if it already exists
test -d $path && rm -rf $path

# extract zip file
if ! unzip "$1" -d $path; then
  print_failure "Unable to extract zip file!" 3
fi

# number of files and number of folders inside /tmp/TxGyy/
files=$(($(find $path/* -maxdepth 0 -type f | wc -l)))
dirs=$(($(find $path/* -maxdepth 0 -type d | wc -l)))

# check if the contents of the zip file are inside a "root" TxGyy folder
if [ $files -ne 0 ] || [ $dirs -ne 1 ] || [ ! -d $path/$gid ]; then
  print_failure "All files must be within $gid folder!" 4
fi

cd $path/$gid

# check if code compiles using make command
if ! make > /dev/null 2>&1; then
  print_failure "The code must compile using 'make' command!" 5
fi

# check for binaries
if [ ! -f server ] || [ ! -f user ]; then
  print_failure "The makefile must generate 'server' and 'user' binary files!" 6
fi

# zip file seems to be ok
print_success "No errors found."
