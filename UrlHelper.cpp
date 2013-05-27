#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include "UrlHelper.h"

using namespace std;

string ReplaceInStr(const string& in, const string& search_for, const string& replace_with) 
{

  string ret = in;

  string::size_type pos = ret.find(search_for);

  while (pos != string::npos) 
  {
    ret = ret.replace(pos, search_for.size(), replace_with);

    pos =  pos - search_for.size() + replace_with.size() + 1;

    pos = ret.find(search_for, pos);
  }

  return ret;
}


bool RemoveProtocolFromUrl(string const& url, string& protocol, string& rest) 
{
  string::size_type pos_colon = url.find(":");

  if (pos_colon == string::npos) 
  {
    rest = url;  
    return false;
  }

  if (url.size() < pos_colon + 2) 
  {
    rest = url;
    return false;
  }

  if (url[pos_colon+1] != '/' ||
      url[pos_colon+2] != '/')  
  {
    rest = url;
    return false;
  }

  protocol = url.substr(0,pos_colon);
  rest     = url.substr(3+pos_colon);  // Skipping three characters ( '://' )

  return true;
}

void SplitGetReq(string _req, string& path, map<string, string>& params) 
{
  // Remove trailing newlines
  if (_req[_req.size()-1] == '\x0d' ||
      _req[_req.size()-1] == '\x0a')
      _req=_req.substr(0, _req.size()-1);

  if (_req[_req.size()-1] == '\x0d' ||
      _req[_req.size()-1] == '\x0a')
      _req=_req.substr(0, _req.size()-1);

  // Remove potential Trailing HTTP/1.x
  if (_req.size() > 7) 
  {
    if (_req.substr(_req.size()-8, 7) == "HTTP/1.") 
    {
      _req=_req.substr(0, _req.size()-9);
    }
  }

  string::size_type qm = _req.find("?");
  if (qm != string::npos) 
  {
    string url_params = _req.substr(qm+1);

    path = _req.substr(0, qm);

    // Appending a '&' so that there are as many '&' as name-value pairs.
    // It makes it easier to split the url for name value pairs, he he he
    url_params += "&";

    string::size_type next_amp = url_params.find("&");

    while (next_amp != string::npos) 
    {
      string name_value = url_params.substr(0,next_amp);
      url_params             = url_params.substr(next_amp+1);
      next_amp               = url_params.find("&");

      string::size_type pos_equal = name_value.find("=");

      string nam = name_value.substr(0,pos_equal);
      string val = name_value.substr(pos_equal+1);

      string::size_type pos_plus;
      while ( (pos_plus = val.find("+")) != string::npos ) 
      {
        val.replace(pos_plus, 1, " ");
      }

      // Replacing %xy notation
      string::size_type pos_hex = 0;
      while ( (pos_hex = val.find("%", pos_hex)) != string::npos ) 
      {
        stringstream h;
        h << val.substr(pos_hex+1, 2);
        h << hex;

        int i;
        h>>i;

        stringstream f;
        f << static_cast<char>(i);
        string s;
        f >> s;

        val.replace(pos_hex, 3, s);
        pos_hex ++;
      }

      params.insert(map<string,string>::value_type(nam, val));
    }
  }
  else 
  {
    path = _req;
  }
}

void SplitUrl(string const& url, string& protocol, string& server, string& path) 
{
  RemoveProtocolFromUrl(url, protocol, server);

  if (protocol == "http") 
  {
    string::size_type pos_slash = server.find("/");
  
    if (pos_slash != string::npos) 
    {
      path   = server.substr(pos_slash);
      server = server.substr(0, pos_slash);
    }
    else 
    {
      path = "/";
    }
  }
  else if (protocol == "file") 
  {
    path = ReplaceInStr(server, "\\", "/");
    server = "";
  }
  else 
  {
    cerr << "unknown protocol in SplitUrl: '" << protocol << "'" << endl;
  }
}
