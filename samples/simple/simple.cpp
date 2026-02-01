/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/
/* <DESC>
 * Very simple HTTP GET
 * </DESC>
 */

#define CURL_STATICLIB
#include <xtl.h>
#include <curl/curl.h>
#include <stdio.h>



size_t CurlWriteCallback(char* buf, size_t size, size_t nmemb, void* up)
{ 
    //TRACE("CURL - Response received:\n%s", buf);
    //TRACE("CURL - Response handled %d bytes:\n%s", size*nmemb);

	OutputDebugString(buf);

    // tell curl how many bytes we handled
    return size*nmemb;
}


void DebugPrint(const char *fmt, ...)
{
    char buffer[512];  // Buffer for formatted string
    va_list args;
    va_start(args, fmt);
    _vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    OutputDebugStringA(buffer);
}



int main(void) {
	CURL *curl;
	CURLcode res;

 // Get libcurl version
    const char *curlVer = curl_version();
    DebugPrint("libcurl version: %s\n", curlVer);

	// Initialize xbox networking
	XNetStartupParams xnsp;
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
	xnsp.cfgPrivatePoolSizeInPages = 128; // == 256kb, default = 12 (48kb)
	xnsp.cfgEnetReceiveQueueLength = 64; // == 128kb, default = 8 (16kb)
	xnsp.cfgIpFragMaxSimultaneous = 64; // default = 4
	xnsp.cfgIpFragMaxPacketDiv256 = 64; // == 8kb, default = 8 (2kb)
	xnsp.cfgSockMaxSockets = 64; // default = 64
	xnsp.cfgSockDefaultRecvBufsizeInK = 128; // default = 16
	xnsp.cfgSockDefaultSendBufsizeInK = 128; // default = 16
	INT err = XNetStartup(&xnsp);

	// Check that we have an IP address
	XNADDR xna;
	DWORD dwState;
	do {
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(50);
	} while (dwState == XNET_GET_XNADDR_PENDING);
	//char szTitleIP[32];
	//XNetInAddrToString(xna.ina, szTitleIP, 32);

printf("libcurl version: %s\n", curl_version());

	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://www.testingmcafeesites.com/index.html");
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* Set up write callback */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CurlWriteCallback);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);

		/* Perform the request, res gets the return code */
		res = curl_easy_perform(curl);

		/* Get http response code */
		long http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	return 0;
}
