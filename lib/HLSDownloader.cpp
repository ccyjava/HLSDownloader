#pragma warning(push, 0)
#pragma warning( disable : 4244 )
#pragma warning( disable : 4242 )
#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>

#ifdef __cplusplus
}
#endif 


#include "HLSDownloader.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "curls.h"
#include "hls.h"
#include "msg.h"
#include "misc.h" 

#include <apsdk/logging.h>

APSDK_DEFINE_CUSTOM_LOGID(MMHLSDownloader) 


HRESULT HLSDownloader::download_hls_video(std::string surl,std::string filename,bool livestreaming){

	curl_global_init(CURL_GLOBAL_ALL);
    av_register_all();
    const char * url = surl.c_str();
    char *hlsfile_source;
    struct hls_media_playlist media_playlist;

    if (get_data_from_url(url, &hlsfile_source, NULL, STRING,m_strUserAgent) == 0) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                    "");
        MSG_ERROR("No result from server.\n");
        return E_FAIL;
    }

    int playlist_type = get_playlist_type(hlsfile_source);

    if (playlist_type == MASTER_PLAYLIST) {
        struct hls_master_playlist master_playlist;
        master_playlist.source = hlsfile_source;
        master_playlist.url = strdup(url);
        if (handle_hls_master_playlist(&master_playlist)) {
            return E_FAIL;
        }

        int max = 0;
        for (int i = 0; i < master_playlist.count; i++) {
            if (master_playlist.media_playlist[i].bitrate > master_playlist.media_playlist[max].bitrate) {
                max = i;
            }
        }
        
        int quality_choice = max;

        media_playlist = master_playlist.media_playlist[quality_choice];
        master_playlist_cleanup(&master_playlist);
    } else if (playlist_type == MEDIA_PLAYLIST) {
        media_playlist.bitrate = 0;
        media_playlist.url = strdup(url);
    } else {
        return E_FAIL;
    }

    if (handle_hls_media_playlist(&media_playlist,m_strUserAgent)) {
        return E_FAIL;
    }

    if (media_playlist.encryption) {
        Log(LogID_MMHLSDownloader, LogLevel_Info, __FUNCTION__,
                    "HLS Stream is %s encrypted.",
                  media_playlist.encryptiontype == ENC_AES128 ? "AES-128" : "SAMPLE-AES");
    }

    Log(LogID_MMHLSDownloader, LogLevel_Info, __FUNCTION__,
                        "Media Playlist parsed successfully");

    if (download_hls(&media_playlist,livestreaming,m_iMaxSize,filename.c_str(),m_strUserAgent,true)) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                        "Download HLS Failed");
        return E_FAIL;
    }
    
    media_playlist_cleanup(&media_playlist);
    curl_global_cleanup();

    return S_OK;
}
