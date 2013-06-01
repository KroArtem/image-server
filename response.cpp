#include <stdio.h>
#include <stdlib.h>
#include "base64.h"
#include "webserver.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#else
#define SOCKET long int
#endif

using std::string;

/// TODO: Handle %0A etc.
string Htmlize(const string _in)
{
	string _out = "<pre>";

	for(size_t j = 0 ; j < _in.length() ; j++)
	{
		if(_in[j] == 10 || _in[j] == 13) _out += "</pre><br><pre>";
		else _out += _in[j];
	}

	return _out + "</pre>";
}

void FormResponse(http_request* r)
{
	SOCKET s = r->s_;

	if(r->method_=="POST")
	{
		printf("Path for post is %s\n", r->path_.c_str());
	}

	string title;
	string link = "<link rel='stylesheet' type='text/css' href='css1.css'>";
	string body;
	string background = "#ffe4c4";
	string links =
		"<table><tr><td><br><a href='/header'>Show HTTP header details</a></td></tr> "
		"<tr><td><br><a href='/form'>Form</a></td></tr> "
		"<tr><td><br><a href='/LoadImage'>Upload picture</a></td></tr> "
		"<tr><td><br><img src = \"001.jpeg\"></td</tr>"
      ;

	if(r->path_ == "/") 
	{
		title = "Web Server for applying filters to photos";
		body  = "<h1>Web Server </h1>"
		" "  + links;
	}

	else if(r->path_ == "/LoadImage")
	{
		title   = "Interaction";
		body	= "<form name=\"test\" action='/nameparams'>"
					"<p><b>Name:</b><br>"
					"<input name=\"nick\" type=\"text\" size=\"40\">"
					"  <p>Comment<Br>"
					"   <textarea name=\"comment\" cols=\"40\" rows=\"3\"></textarea></p>"
					"  <p><input type=\"submit\" value=\"Send\">"
					"   <input type=\"reset\" value=\"Clear\"></p>"
					" </form>"


					"<form enctype=\"multipart/form-data\" method=\"POST\">"
					"<p>Upload your photo to the server</p>"
					"<p><input type=\"file\" name=\"photo\" multiple accept=\"image/*,image/jpeg\">"
					"<input type=\"submit\" name=\"submmit\" value=\"Send\"></p>"
					" </form>";

		body += links;
	}

	else if( r->path_ == "/nameparams")
	{
		title = "result";

		body = "";

			string str = r->params_.begin()->second;				//receive command to use
			char *cstr = new char[str.length() + 1];
			strcpy(cstr, str.c_str());
			body += "<br>" + str;
		#ifdef _WIN32
			PROCESS_INFORMATION process;							//winapi specific stuff to call imagemagick
			STARTUPINFO startup;
			memset(&startup, 0, sizeof(startup));
			startup.cb = sizeof(startup);
			char fullParam[120];
			sprintf(fullParam, "C:\\Program Files\\IM\\convert.exe C:\\015.jpeg -%s C:\\015.png", str.c_str());
			CreateProcessA(0, fullParam, 0, 0, TRUE, 0, 0, 0, &startup, &process);
			Sleep(1000);											//not the best way to check whether it's ready
			TerminateProcess(process.hProcess,NO_ERROR);			//killing process after 1000 ms
		#endif
		body += "<hr>" + links;

	}
	else if (r->path_ == "/form")
	{
		title   = "Fill a form";

		body    = "<h1>Fill a form</h1>";
		body += "<form name='Filter' action='/nameparams'><p>"
				"<b> Фильтр: поддерживаются transverse, transpose,"
				"negate, monochrome, auto-gamma, auto-level </b>"
				"<br><input name='Filter' type='text' size='40'>"
				"</p><p><input type='submit' value='Send'>"
				"<input type='reset' value='Clear'></p></form>" ; 
		
		for (std::map<std::string, std::string>::const_iterator i = r->params_.begin();      i != r->params_.end();
		i++) 
		{
			printf("got param %s = %s", i->first.c_str(), i->second.c_str());
			body += "<br>" + i->first + " = " + i->second;
		}

		body += "<hr>" + links;
	}

	else if (r->path_ == "/header") 
	{
		title   = "some HTTP header details";
		body    = string ("<table>")                              +
		"<tr><td>Accept:</td><td>"          + r->accept_          + "</td></tr>" +
		"<tr><td>Accept-Encoding:</td><td>" + r->accept_encoding_ + "</td></tr>" +
		"<tr><td>Accept-Language:</td><td>" + r->accept_language_ + "</td></tr>" +
		"<tr><td>User-Agent:</td><td>"      + r->user_agent_      + "</td></tr>" +
		"<tr><td>Content-type:</td><td>"	+ r->content_type_	  + "</td></tr>" +
		"</table>"                                                +
		links;
	}

	else if (r->path_ == "/css1.css")
	{	
		r->method_	= "POST";
		r->content_type_ = "text/css";
	}

	else if(r->path_ == "/001.jpeg")
	{
		r->content_type_ = "image/jpeg";
	}

	else 
	{
		r->status_ = "404 Not Found";
		title      = "Wrong URL";
		body       = "<h1>Wrong URL</h1>";
		body      += "Path is : &gt;" + r->path_ + "&lt;"; 
	}

	r->answer_  = "<html><head><title>";
	r->answer_ += title;
	r->answer_ += "</title>";
	r->answer_ += link;
	r->answer_ += "</head><body bgcolor='" + background + "' style='cursor:crosshair' text='black' alink='green' vlink='black' link='black'>";
	r->answer_ += body;
	r->answer_ += "</body></html>";
}
