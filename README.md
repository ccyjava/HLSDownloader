HLS Downloader
==============
Base on hlsdl:https://github.com/selsta/hlsdl


Can Download Video From m3u8 link


Compare to Origin Version(selsta):

* Support live streaming downloader

*  Support remux to mp4

* Support filter illegal stream from raw file

* adapt to c++(in order to using language multi thread support)



Command:
Usage: MMHLSDownloader.exe url [options]

	--best    or -b ... Automaticly choose the best quality.
	--verbose or -v ... Verbose more information.
	--output  or -o ... Choose name of output file.
	--live    or -l ... Url is Live Streaming Url.
	--max    or -m  ... Maximum Video File Size to Download.
	--help    or -h ... Print help.
	--force   or -f ... Force overwriting the output file.
	--userAgent   or -u ...Set the user agent.
	--quiet   or -q ... Print less to the console.
	--dump-dec-cmd  ... Print the openssl decryption command.
	--dump-ts-urls  ... Print the links to the .ts files.

