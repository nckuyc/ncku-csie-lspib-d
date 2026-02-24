#!/bin/bash

# 1. 檢查是否剛好有兩個輸入參數
if [ $# -ne 2 ]; then
    echo "Error: Invalid number of arguments."
    echo "Usage: $0 <filesdir> <searchstr>"
    exit 1
fi

FILESDIR=$1
SEARCHSTR=$2

# 2. 檢查第一個參數是否為實際存在的資料夾
if [ ! -d "$FILESDIR" ]; then
    echo "Error: Directory $FILESDIR does not exist."
    exit 1
fi

# 3. 計算檔案總數 (找 filesdir 底下所有的檔案並計算數量)
X=$(find "$FILESDIR" -type f | wc -l)

# 4. 計算包含該字串的總行數 (-r 代表遞迴搜尋資料夾，計算符合的行數)
Y=$(grep -r "$SEARCHSTR" "$FILESDIR" | wc -l)

# 5. 印出指定的輸出格式 (這行字必須一字不差)
echo "The number of files are $X and the number of matching lines are $Y"