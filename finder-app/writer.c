#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // 設定 syslog，使用 LOG_USER 設施
    openlog(NULL, 0, LOG_USER);

    // 檢查參數數量是否正確 (程式名稱 + 檔案路徑 + 寫入字串 = 3)
    if (argc != 3) {
        syslog(LOG_ERR, "Invalid number of arguments. Expected 2, got %d", argc - 1);
        printf("Usage: %s <file> <string>\n", argv[0]);
        closelog();
        return 1;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    // 開啟檔案進行寫入 (假設目錄已存在)
    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        // 若開檔失敗，記錄 ERROR 層級的 syslog
        syslog(LOG_ERR, "Failed to open file %s: %s", writefile, strerror(errno));
        closelog();
        return 1;
    }

    // 成功寫入前，記錄 DEBUG 層級的 syslog (完全依照作業要求的格式)
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);
    
    // 寫入字串
    fprintf(file, "%s", writestr);
    
    fclose(file);
    closelog();
    return 0;
}