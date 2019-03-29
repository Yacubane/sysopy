./catcher $2 &
sleep 1
pid=$!
./sender $pid $1 $2