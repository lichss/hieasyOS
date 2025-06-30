#!/bin/bash


# 定义脚本路径
SCRIPT_PATH="script/report_size.sh"

# 检查脚本是否存在
if [ ! -f "$SCRIPT_PATH" ]; then
    echo "错误: 脚本文件 $SCRIPT_PATH 不存在" >&2
    exit 1
fi

# 检查脚本是否可执行
if [ ! -x "$SCRIPT_PATH" ]; then
    echo "错误: 脚本文件 $SCRIPT_PATH 不可执行" >&2
    exit 1
fi

# 尝试执行脚本
if ! "$SCRIPT_PATH"; then
    echo "错误: 脚本执行失败 (退出码: $?)" >&2
    exit 1
fi

echo "脚本执行成功"
exit 0