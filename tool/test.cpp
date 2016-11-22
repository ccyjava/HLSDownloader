#include "hlscommon.h"
#include <iostream>
#include <vector>
#include "MultipleThreadQueue.h"



#include <vector>
#include <string>
#include <memory>

#include "HLSDownloader.h"
#ifdef HLS_DEBUG
int main(){
#else
int test(){	
#endif

	std::cout<<"start test"<<std::endl;

	HLSDownloader hls_downloader(100000);
    std::string url("https://manifest.googlevideo.com/api/manifest/hls_playlist/id/kzaVwF5MMq0.0/itag/91/source/yt_live_broadcast/requiressl/yes/ratebypass/yes/live/1/cmbypass/yes/goi/160/sgoap/gir%3Dyes%3Bitag%3D139/sgovp/gir%3Dyes%3Bitag%3D160/hls_chunk_host/r6---sn-n02xgoxufvg3-2gbl.googlevideo.com/gcr/jp/playlist_type/DVR/dover/4/upn/ZEUWTrVNPFQ/mm/32/mn/sn-n02xgoxufvg3-2gbl/ms/lv/mt/1476340162/mv/u/pl/22/ip/167.220.232.144/ipbits/0/expire/1476362045/sparams/ip,ipbits,expire,id,itag,source,requiressl,ratebypass,live,cmbypass,goi,sgoap,sgovp,hls_chunk_host,gcr,playlist_type/signature/7FC298C3CDE97E715288BDFFDA500AD0AD3F92A6.73CCD9378DDCD3D51605B74901CDCC6C1A408BA1/key/dg_yt0/playlist/index.m3u8");
    hls_downloader.download_hls_video(url,"test.ts",true);
	return 0;
}