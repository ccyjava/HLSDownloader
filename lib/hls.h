#ifndef __HLS_DownLoad__hls__
#define __HLS_DownLoad__hls__

#ifndef __cplusplus
typedef unsigned char bool;
static const bool false = 0;
static const bool true = 1;
#endif


#ifdef _WIN32

#define MASTER_PLAYLIST 0
#define MEDIA_PLAYLIST 1

#define ENC_AES_SAMPLE 0x02
#define ENC_AES128 0x01
#define ENC_NONE 0x00

#define KEYLEN 16

#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */

#define access _access
#endif

#include "misc.h"
#include <memory>
#include <string>

struct enc_aes128 {
    bool iv_is_static;
    uint8_t iv_value[KEYLEN];
    uint8_t key_value[KEYLEN];
};
typedef struct enc_aes128 enc_aes128;

struct hls_media_segment {
    std::string url;
    int sequence_number;
    struct enc_aes128 enc_aes;
    //std::shared_ptr<ByteBuffer> seg;
    hls_media_segment(){

    }
};
//typedef struct hls_media_segment hls_media_segment;
struct hls_media_playlist {
    char *url;
    char *source;
    unsigned int bitrate;
    bool encryption;
    int encryptiontype;
    int count;
    int target_duration;
    struct hls_media_segment *media_segment;
    struct enc_aes128 enc_aes;
    hls_media_playlist():media_segment(NULL),url(NULL),source(NULL){
    }
};
//typedef struct hls_media_playlist hls_media_playlist;
struct hls_master_playlist {
    char *url;
    char *source;
    int count;
    struct hls_media_playlist *media_playlist;
};
//typedef struct hls_master_playlist hls_master_playlist;
int get_playlist_type(char *source);
int handle_hls_master_playlist(struct hls_master_playlist *ma);
int handle_hls_media_playlist(struct hls_media_playlist *me,std::string user_agent);
int download_hls(struct hls_media_playlist *me,bool,int max_file_size,const char* custom_filename, std::string user_agent,bool force_overwrite);
int print_enc_keys(struct hls_media_playlist *me);
void print_hls_master_playlist(struct hls_master_playlist *ma);
void media_playlist_cleanup(struct hls_media_playlist *me);
void master_playlist_cleanup(struct hls_master_playlist *ma);
#endif /* defined(__HLS_DownLoad__hls__) */
