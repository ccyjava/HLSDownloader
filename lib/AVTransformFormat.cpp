#pragma warning( disable : 4244 )
#pragma warning( disable : 4242 )
#pragma warning( disable : 4996 )
#pragma warning( disable : 4706 )
#pragma warning( disable : 4505 )
#pragma warning( disable : 4018 )
extern "C"
{
#include "libavformat/avformat.h"
};

#include <apsdk/basic_types.h>
#include <apsdk/logging.h>
#include <string>
#include <vector>

APSDK_DEFINE_CUSTOM_LOGID(MMHLSDownloader)

HRESULT AVTransformFormat(std::string input_file,std::string output_file,std::string format_name){
	AVOutputFormat *ofmt = NULL;
    
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt;
    std::vector<int> streams_map;
    int output_streams_nb=0;

    AVBitStreamFilterContext* h264BitstreamFilterContext = av_bitstream_filter_init("h264_mp4toannexb");
    AVBitStreamFilterContext* aacBitstreamFilterContext = av_bitstream_filter_init("aac_adtstoasc");
    const char *in_filename = input_file.c_str(), *out_filename = output_file.c_str();
    int ret, i;
   
    av_register_all();
    Log(LogID_MMHLSDownloader, LogLevel_Info, __FUNCTION__,
                    "Starting transformat");
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                    "Could not open input file.");
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                    "Failed to retrieve input stream information");
        goto end;
    }
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    
    avformat_alloc_output_context2(&ofmt_ctx, NULL, format_name.c_str(), out_filename);
    if (!ofmt_ctx) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
            "Could not create output context");
        ret = AVERROR_UNKNOWN;
        goto end;
    }
    ofmt = ofmt_ctx->oformat;

    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        
        AVStream *in_stream = ifmt_ctx->streams[i];
        if(in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO || in_stream->codec->codec_type == AVMEDIA_TYPE_AUDIO){
            streams_map.push_back(output_streams_nb++);
            AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
            if (!out_stream) {
                Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                    "Failed allocating output stream");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
          
            ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
            if (ret < 0) {
                Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                    "Failed to copy context from input to output stream codec context");
                goto end;
            }
            out_stream->codec->codec_tag = 0;
            if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }else{
            streams_map.push_back(-1);
        }
    }
    
    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                "Could not open output file");
            goto end;
        }
    }
    
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
            "Error occurred when opening output file");
        goto end;
    }
    int frame_index=0;
    while (1) {
        AVStream *in_stream, *out_stream;
        
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        int current_index = streams_map[pkt.stream_index];
        if(current_index != -1){
            //printf("pkt %d %d\n",pkt.stream_index,current_index);
            in_stream  = ifmt_ctx->streams[pkt.stream_index];
            out_stream = ofmt_ctx->streams[current_index];
            //if(in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO || in_stream->codec->codec_type == AVMEDIA_TYPE_AUDIO){
                if(in_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO && in_stream->codec->codec_id == AV_CODEC_ID_H264){		    
        		    av_bitstream_filter_filter(h264BitstreamFilterContext, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);  
        		} else if(in_stream->codec->codec_id == AV_CODEC_ID_AAC) {
        		    av_bitstream_filter_filter(aacBitstreamFilterContext, out_stream->codec, NULL, &pkt.data, &pkt.size, pkt.data, pkt.size, 0);  
        		}       
                pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
                pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
                pkt.pos = -1;
                pkt.stream_index = current_index;
            //}
            ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
            if (ret < 0) {
                Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
                    "Error muxing packet");
                break;
            }
        }
        av_free_packet(&pkt);
        frame_index++;
    }
    
    av_write_trailer(ofmt_ctx);
    
    
end:
    av_bitstream_filter_close(h264BitstreamFilterContext);  
    av_bitstream_filter_close(aacBitstreamFilterContext);  
    avformat_close_input(&ifmt_ctx);
    
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        Log(LogID_MMHLSDownloader, LogLevel_Error, __FUNCTION__,
            "Error occurred.");
        return E_FAIL;
    }
    return S_OK;
}
