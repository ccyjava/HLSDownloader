#include "AVTransformFormat.h"

int main(int argc, char* argv[])
{
    
    if (argc < 3) {
        printf("usage: %s input output\n"
            "Remux a media file with libavformat and libavcodec.\n"
            , argv[0]);
        return 1;
    }
    
    return AVTransformFormat(argv[1],argv[2],"mp4");
}