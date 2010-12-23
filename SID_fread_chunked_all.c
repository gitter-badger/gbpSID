#include <gbpCommon.h>
#include <gbpSID.h>

size_t SID_fread_chunked_all(void   *buffer,
                             size_t  n_x_read,
                             SID_fp *fp){
  int    i_chunk;
  int    i_group;
  size_t i_x_read_chunk;
  size_t i_x_chunk;
  size_t n_skip;
  size_t n_x_chunk;
  size_t n_x_chunk_max;
  size_t header_size;
  size_t r_val=0;
  char   filename_chunk[256];
  size_t i_x_offset;
  i_x_offset=fp->last_item;
  for(i_chunk=0,i_x_read_chunk=0,i_x_chunk=i_x_offset;i_chunk<fp->chunked_header.n_chunk;i_chunk++){
    if(fp->i_x_start_chunk[i_chunk]<=i_x_chunk && fp->i_x_last_chunk[i_chunk]>=i_x_chunk){
      n_skip   =i_x_chunk-fp->i_x_start_chunk[i_chunk];
      n_x_chunk=MIN(n_x_read-i_x_read_chunk,fp->i_x_step_chunk[i_chunk]-n_skip);
    }
    else{
      n_x_chunk=0;
      n_skip   =0;
    }
    #ifdef USE_MPI
      MPI_Allreduce(&n_x_chunk,&n_x_chunk_max,1,MPI_SIZE_T,MPI_MAX,MPI_COMM_WORLD);
    #else
      n_x_chunk_max=n_x_chunk;
    #endif
    if(n_x_chunk_max>0){
      sprintf(filename_chunk,"%s.%d",fp->filename_root,i_chunk);
      SID_fopen(filename_chunk,"r",fp);
      if(n_x_chunk>0){
        SID_fseek(1,
                  fp->header_offset[i_chunk]+n_skip*fp->chunked_header.item_size,
                  fp);
        SID_fread_all((char *)buffer+i_x_read_chunk*fp->chunked_header.item_size,
                      fp->chunked_header.item_size,
                      n_x_chunk,
                      fp);
        i_x_chunk     +=n_x_chunk;
        i_x_read_chunk+=n_x_chunk;
      }
      SID_fclose(fp);
    }
  }
  fp->last_item+=n_x_read;
  return(r_val);
}

