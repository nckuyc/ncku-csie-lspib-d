#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        syslog(LOG_ERR, "Invalid number of arguments");
        return 1;
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    FILE *fp = fopen(writefile, "w");
    if (fp == NULL) {
        syslog(LOG_ERR, "Failed to open file %s", writefile);
        return 1;
    }

    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    fprintf(fp, "%s", writestr);
    fclose(fp);

    return 0;
}

