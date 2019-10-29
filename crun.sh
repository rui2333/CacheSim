source deps/env.sh;
scons -j16;
rm a.out;
./build/opt/zsim ../tests/simple.cfg > a.out
#python process_data.py
