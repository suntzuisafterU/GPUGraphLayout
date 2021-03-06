# nvidia-smi command shows this output
# Output for kepler1
Fri May 10 17:24:06 2019       
+-----------------------------------------------------------------------------+
| NVIDIA-SMI 418.40.04    Driver Version: 418.40.04    CUDA Version: 10.1     |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|===============================+======================+======================|
|   0  Quadro K600         Off  | 00000000:03:00.0 Off |                  N/A |
| 25%   40C    P0    N/A /  N/A |      0MiB /   980MiB |      0%      Default |
+-------------------------------+----------------------+----------------------+
|   1  Tesla K20c          Off  | 00000000:04:00.0 Off |                    0 |
| 30%   30C    P0    50W / 225W |      0MiB /  4743MiB |      0%      Default |
+-------------------------------+----------------------+----------------------+
|   2  Tesla K20c          Off  | 00000000:82:00.0 Off |                    0 |
| 30%   32C    P0    50W / 225W |      0MiB /  4743MiB |     72%      Default |
+-------------------------------+----------------------+----------------------+
                                                                               
+-----------------------------------------------------------------------------+
| Processes:                                                       GPU Memory |
|  GPU       PID   Type   Process name                             Usage      |
|=============================================================================|
|  No running processes found                                                 |
+-----------------------------------------------------------------------------+

# output for kepler2
Fri May 10 17:29:36 2019       
+-----------------------------------------------------------------------------+
| NVIDIA-SMI 418.40.04    Driver Version: 418.40.04    CUDA Version: 10.1     |
|-------------------------------+----------------------+----------------------+
| GPU  Name        Persistence-M| Bus-Id        Disp.A | Volatile Uncorr. ECC |
| Fan  Temp  Perf  Pwr:Usage/Cap|         Memory-Usage | GPU-Util  Compute M. |
|===============================+======================+======================|
|   0  Quadro K600         Off  | 00000000:03:00.0 Off |                  N/A |
| 25%   42C    P0    N/A /  N/A |      0MiB /   979MiB |      0%      Default |
+-------------------------------+----------------------+----------------------+
|   1  Tesla K20c          Off  | 00000000:04:00.0 Off |                    0 |
| 30%   29C    P0    49W / 225W |      0MiB /  4743MiB |      0%      Default |
+-------------------------------+----------------------+----------------------+
|   2  Tesla K20c          Off  | 00000000:82:00.0 Off |                    0 |
| 30%   28C    P0    50W / 225W |      0MiB /  4743MiB |     18%      Default |
+-------------------------------+----------------------+----------------------+
                                                                               
+-----------------------------------------------------------------------------+
| Processes:                                                       GPU Memory |
|  GPU       PID   Type   Process name                             Usage      |
|=============================================================================|
|  No running processes found                                                 |
+-----------------------------------------------------------------------------+

# kepler1 and kepler2 have identical GPU hardware.

# Following command shows available CUDA programs in toolkit
ls /usr/local/cuda/bin

