#!/bin/bash


cd "$(dirname "$0")" || {
    echo "无法切换到脚本目录" >&2
    exit 1
}

# 获取上级目录路径
parent_dir=$(dirname "$PWD")

# 检查上级目录中是否存在 build 文件夹
if [ -d "$parent_dir/build" ]; then
    echo "report: ($parent_dir) build exists."
    echo ""
    echo "ELF File Size Report:"
    echo "===================="
    
    # 查找所有ELF文件并统计大小
    total_size=0
    file_count=0
    
    while IFS= read -r -d '' file; do
        if file "$file" | grep -q "ELF"; then
            size=$(stat -c "%s" "$file")
            human_size=$(numfmt --to=iec --suffix=B "$size")
            echo "$(basename "$file"): $human_size"
            total_size=$((total_size + size))
            file_count=$((file_count + 1))
        fi
    done < <(find "$parent_dir/build" -type f -print0)
    
    echo ""
    echo "Summary:"
    echo "--------"
    echo "Total ELF files: $file_count"
    echo "Combined size: $(numfmt --to=iec --suffix=B "$total_size")"
    
    exit 0
else
    echo "report:  ($parent_dir) build does not exist." >&2
    exit 1  # 失败退出
fi