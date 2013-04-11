#ifndef __URL_HELPER_H__
#define __URL_HELPER_H__

#include <string>
#include <map>

void SplitUrl             (std::string const& url, std::string& protocol, std::string& server, std::string& path);
bool RemoveProtocolFromUrl(std::string const& url, std::string& protocol, std::string& rest);

void SplitGetReq          (std::string et_req,     std::string& path,     std::map<std::string, std::string>& params);

#endif
