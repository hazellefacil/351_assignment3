#ifndef PRINT_STAT_FILE
# define PRINT_STAT_FILE

void printing_init();
static void* printingStatsThread(void* _arg);
void printing_cleanup(void);
#endif