#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <gbpSID.h>

void SID_out(char *fmt, int mode, ...) {
    int     flag_write_time;
    int     i_level;
    FILE *  fp_info;
    va_list vargs;
    va_start(vargs, mode);

    fp_info = stdout;

    if(SID.I_am_Master) {
#if USE_MPI
        if(!SID.flag_results_on && check_mode_for_flag(mode, SID_INFO_RESULT)) {
            fprintf(fp_info, "\n");
            fprintf(fp_info, "-------------\n");
            fprintf(fp_info, "   Results   \n");
            fprintf(fp_info, "-------------\n");
            fprintf(fp_info, "\n");
            SID.flag_results_on = GBP_TRUE;
        }
#else
        if(!SID.indent) {
            fprintf(fp_info, "\n");
            SID.indent = GBP_TRUE;
        }
        // Write indenting text
        if(SID.indent) {
            for(i_level = 0; i_level < SID.level; i_level++)
                fprintf(fp_info, "%s", SID_LOG_INDENT_STRING);
        }
#endif

        // Write text
        if(!check_mode_for_flag(mode, SID_INFO_MASTER) || (check_mode_for_flag(mode, SID_INFO_MASTER) && SID.I_am_Master)) {
            vfprintf(fp_info, fmt, vargs);
            fprintf(fp_info, "\n");
        }
    }

    fflush(fp_info);
    va_end(vargs);
}
