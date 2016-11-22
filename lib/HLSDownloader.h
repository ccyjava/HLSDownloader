#include "misc.h"
#include "hls.h"
#include "string"
#include "apsdk/basic_types.h"

#define USER_AGENT "Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) " \
                   "AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 " \
                   "Mobile/10A5355d Safari/8536.25"
                   

class HLSDownloader{
public:

	explicit HLSDownloader():m_strUserAgent(USER_AGENT),m_iMaxSize(0){}
	explicit HLSDownloader(int size):m_strUserAgent(USER_AGENT),m_iMaxSize(size){}
	explicit HLSDownloader(std::string agent,int size):m_strUserAgent(agent),m_iMaxSize(size){}
	explicit HLSDownloader(std::string agent):m_strUserAgent(agent),m_iMaxSize(0){}

	~HLSDownloader(){}
	

	HRESULT download_hls_video(std::string url,std::string file_name,bool livestreaming = false);

	std::string m_strUserAgent;
	int m_iMaxSize;
private:
	
	static int s_iDownloadCount;

};