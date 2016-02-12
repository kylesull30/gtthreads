echo ""
echo "==================="
echo ""
echo "Running unit tests:"

for i in ./tests/*_tests
do
    retval=$($i 2>&1)
    if [ $? -eq 0 ]
    then
        echo $i PASS
    else
        echo "ERROR in test $i"
        echo "$retval"
        exit 1
    fi
done

echo ""