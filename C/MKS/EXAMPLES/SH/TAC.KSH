#
# tac: cat backwards; reverse the order of lines in a file
#
awk '{print NR "#" $0}' "$@" |
sort -t# +0nr -1 |
sed 's/^[0-9]*#//'
