dir="$(pwd)"

. $dir/venv/bin/activate
rm $dir/nohup.out
nohup python3 server.py ip=$1 port=$2 nohup.out &