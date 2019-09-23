# 确认连通性.

# 注意, 该代码在linux下生效
import os
if __name__ == '__main__':
    exit_code = os.system('ping -c 1 -w 1 172.20.20.1')
    print(exit_code)
    exit(exit_code)
