echo 'Building ...'
gcc tron.c test.c assert.c -o testfile
echo 'Running ...'
./testfile
rm testfile
