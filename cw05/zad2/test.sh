handleSIGINT() { 
  pkill -P $$
}
trap handleSIGINT INT

rm -f fifo
./master fifo &
sleep 1
./slave fifo 5 &
sleep 1
./slave fifo 5 &
sleep 1
./slave fifo 5 &
sleep 1
./slave fifo 5 &
wait