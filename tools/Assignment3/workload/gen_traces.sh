./pin-3.13-98189-g60a6ef199-gcc-linux/pin -t obj-intel64/trace_generator.so -manual 1 -thread 1 -max 100000000 -- workloads/rodinia_3.1/openmp/backprop/backprop_layerforward 10000000 1
./pin-3.13-98189-g60a6ef199-gcc-linux/pin -t obj-intel64/trace_generator.so -manual 1 -thread 1 -max 100000000 -- workloads/rodinia_3.1/openmp/backprop/backprop_adjust_weights 50000000 1
./pin-3.13-98189-g60a6ef199-gcc-linux/pin -t obj-intel64/trace_generator.so -manual 1 -thread 1 -max 100000000 -- workloads/rodinia_3.1/openmp/srad/srad_v1/srad 100 0.5 502 458 1
