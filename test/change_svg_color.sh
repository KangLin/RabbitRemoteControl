if [ $# -lt 3 ]; then
    echo "Input parameters[$#] is fail. "
    echo "Please using:"
    echo "$0 old_color new_color path"
    exit
fi

sed -i "s/fill=\"$1\"/fill=\"$2\"/g" `grep "fill=\"$1\"" -rl $3`
