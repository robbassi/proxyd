#!/bin/bash

source rainbow.sh

INFILE="in.txt"
OUTFILE="out.txt"
PORT=$1

checkjob ()
{
    sleep 2
    if kill -0 $! > /dev/null 2>&1; then
	return 0
    else
	return 1
    fi    
}

status ()
{
    if checkjob; then
	echogreen "ok" 
    else
	echored "fail"
	exit 1
    fi
}

cleanup ()
{
    # remove out file if exists
    if [ -e $OUTFILE ]; then
    	rm $OUTFILE
    fi
    if pgrep proxyd > /dev/null; then
	kill $(pgrep proxyd)
    fi
}

trap cleanup EXIT

# listen on some socket and pipe to out file
nc -l $PORT > $OUTFILE &

echo -n "starting target server..."
status

# start the proxy
../proxyd > /dev/null &

echo -n "starting proxy..........."
status

# cat the in file over the local proxy
echo -n "sending file............."

cat $INFILE | nc localhost $PORT -X 5 -x localhost

if [ ! -e $OUTFILE ]; then
    echored "fail (could not send file)"
    exit 1
fi

# check the diff
diff $INFILE $OUTFILE 

if [ $? == 0 ]; then
    echogreen "ok"
    echogreen "test passed"
else
    echored "fail (received corrupt data)"
    echored "test failed"
fi

exit 0
