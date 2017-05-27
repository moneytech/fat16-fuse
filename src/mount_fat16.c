#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FUSE_USE_VERSION 26
#include <fuse.h>

#include "sector.h"
#include "log.h"

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

typedef struct {
  BYTE BS_jmpBoot[3];
  BYTE BS_OEMName[8];
  WORD BPB_BytsPerSec;
  BYTE BPB_SecPerClus;
  WORD BPB_RsvdSecCnt;
  BYTE BPB_NumFATS;
  WORD BPB_RootEntCnt;
  WORD BPB_TotSec16;
  BYTE BPB_Media;
  WORD BPB_FATSz16;
  WORD BPB_SecPerTrk;
  WORD BPB_NumHeads;
  DWORD BPB_HiddSec;
  DWORD BPB_TotSec32;
  BYTE BS_DrvNum;
  BYTE BS_Reserved1;
  BYTE BS_BootSig;
  DWORD BS_VollID;
  BYTE BS_VollLab[11];
  BYTE BS_FilSysType[8];
  BYTE Reserved2[448];
  WORD Signature_word;
} __attribute__ ((packed)) BPB_BS;

typedef struct {
  BYTE DIR_Name[11];
  BYTE DIR_Attr;
  BYTE DIR_NTRes;
  BYTE DIR_CrtTimeTenth;
  WORD DIR_CrtTime;
  WORD DIR_CrtDate;
  WORD DIR_LstAccDate;
  WORD DIR_FstClusHI;
  WORD DIR_WrtTime;
  WORD DIR_WrtDate;
  WORD DIR_FstClusLO;
  DWORD DIR_FileSize;
} __attribute__ ((packed)) DIR_ENTRY;

typedef struct {
  DWORD FirstRootDirSecNum;
  DWORD FirstDataSector;
  BPB_BS Bpb;
} VOLUME;


/* Prepares the volume to receive the BPB_BS structure and some useful
 * pre-calculated data found in the FAT16 volume structure */
VOLUME *pre_init_fat16(void)
{
  /* Opening a FAT16 image file */
  FILE *fd = fopen("fat16.img", "rb");

  if (fd == NULL) {
    log_msg("Missing FAT16 image file!\n");
    exit(EXIT_FAILURE);
  }

  VOLUME *Vol = malloc(sizeof *Vol);

  if (Vol == NULL) {
    log_msg("Out of memory!\n");
    exit(EXIT_FAILURE);
  }

  /* Reads the BPB */
  sector_read(fd, 0, &Vol->Bpb);

  /* First sector of the root directory */
  Vol->FirstRootDirSecNum = Vol->Bpb.BPB_RsvdSecCnt
    + (Vol->Bpb.BPB_FATSz16 * Vol->Bpb.BPB_NumFATS);

  /* Number of sectors in the root directory */
  DWORD RootDirSectors = ((Vol->Bpb.BPB_RootEntCnt * 32) +
    (Vol->Bpb.BPB_BytsPerSec - 1)) / Vol->Bpb.BPB_BytsPerSec;

  /* First sector of the data region (cluster #2) */
  Vol->FirstDataSector = Vol->Bpb.BPB_RsvdSecCnt + (Vol->Bpb.BPB_NumFATS *
    Vol->Bpb.BPB_FATSz16) + RootDirSectors;

  return Vol;
}

//------------------------------------------------------------------------------

void *fat16_init(struct fuse_conn_info *conn)
{
  log_msg("Chamando init\n");

  // Your code here 
  struct fuse_context *context;
  context = fuse_get_context();

  return NULL;
}

void fat16_destroy(void *data)
{
  log_msg("Chamando destroy\n");

  // Your code here
}

int fat16_getattr(const char *path, struct stat *stbuf)
{
  log_msg("calling getattr: %s\n", path);
  return 0;
}

int fat16_readdir(const char *path, void *buffer, fuse_fill_dir_t filler,
                  off_t offset, struct fuse_file_info *fi)
{
  int i;
  log_msg("calling readdir: %s", path);
  return 0;
}

int fat16_open(const char *path, struct fuse_file_info *fi)
{
  int i;
  log_msg("calling open: %s", path);
  return 0;
}

int fat16_read(const char *path, char *buffer, size_t size, off_t offset,
               struct fuse_file_info *fi)
{
  int i;
  log_msg("calling read: %s", path);
  return 0;
}

//------------------------------------------------------------------------------

struct fuse_operations fat16_oper = {
  .init       = fat16_init,
  .destroy    = fat16_destroy,
  .getattr    = fat16_getattr,
  .readdir    = fat16_readdir,
  .open       = fat16_open,
  .read       = fat16_read
};

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  int ret;

  /* Starting pre-initialization of a FAT16 volume */
  VOLUME *Vol = pre_init_fat16();

  ret = fuse_main(argc, argv, &fat16_oper, Vol);

  log_msg("ret: %d\n", ret);

  return ret;
}
