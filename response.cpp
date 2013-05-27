#include <stdio.h>
#include <stdlib.h>
#include "base64.h"
#include "webserver.h"
#include <windows.h>
#include <winbase.h>


using std::string;

#ifdef _WIN32
#else
#define SOCKET long int
#endif


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
		"<tr><td><br><a href='/SetValue'>Set some value</a></td></tr> "
		"<tr><td><br><a href='/GetValue'>Get some value</a></td></tr> "
		"<tr><td><br><a href='/GetProperty'>Get some property</a></td></tr> "
		"<tr><td><br><a href='/SetProperty'>Set some property</a></td></tr>"
		"<tr><td><br><a href='/LoadImage'>Upload picture</a></td></tr> "
      ;

	if(r->path_ == "/") 
	{
		title = "Web Server for applying filters to photos";
		body  = "<h1>Web Server </h1>"
		" "  + links;
	}
	else if(r->path_ == "/upload.php")
	{
		printf("Got method[%s] for upload.php\n", r->method_.c_str());

		printf("Uploaded data:\n%s\n", r->content_.c_str());

		printf("Decode64:\n%s\n", base64_decode(r->content_).c_str());
	}
	else if(r->path_ == "/SetValue")
	{
		body = "";

		body += "<br>Var name : " + r->params_["VarName"];
		body += "<br>Var value: " + r->params_["VarValue"];
	}
	else if(r->path_ == "/ExecScript")
	{
		body = string("<br>Executing:<br><br>") + Htmlize(r->params_["script"]);
	}
	else if(r->path_ == "/form2")
	{
		PROCESS_INFORMATION process;								//winapi specific stuff to call imagemagick
		STARTUPINFO startup;
		DWORD code;
		memset(&startup, 0, sizeof(startup));
		startup.cb = sizeof(startup);
		CreateProcessA(0, "C:\\Program Files\\IM\\imdisplay.exe", 0, 0, TRUE, 0, 0, 0, &startup, &process);

		title   = "Interaction";
		//body = "<form name=\"test\" method=\"post\" action=\"input1.php\">"

		body =		"<form name=\"test\" action='/nameparams'>"
					"<p><b>Name:</b><br>"
					"<input name=\"nick\" type=\"text\" size=\"40\">"
					"</p>"
					"  <p><b>Browser:</b><Br>"
					"   <input type=\"radio\" name=\"browser\" value=\"ie\"> Internet Explorer<Br>"
					"   <input type=\"radio\" name=\"browser\" value=\"opera\"> Opera<Br>"
					"   <input type=\"radio\" name=\"browser\" value=\"firefox\"> Firefox<Br>"
					"  </p>"
					"  <p>Comment<Br>"
					"   <textarea name=\"comment\" cols=\"40\" rows=\"3\"></textarea></p>"
					"  <p><input type=\"submit\" value=\"Send\">"
					"   <input type=\"reset\" value=\"Clear\"></p>"
					" </form>";

		body += links;
	}
	else if( r->path_ == "/nameparams") // input1.php
	{
		title = "result";

		body = "";

		for (map<string, string>::const_iterator i = r->params_.begin();
			i != r->params_.end();i++)
		{
			printf("got param %s = %s", i->first.c_str(), i->second.c_str());

			body += "<br>" + i->first + " = " + i->second;
		}
	}
	else if (r->path_ == "/form")
	{
		title   = "Fill a form";

		body    = "<h1>Fill a form</h1>";
		body   += "<form action='/form'>"
			"<table>"
			"<tr><td>Field 1</td><td><input name=field_1></td></tr>"
			"<tr><td>Field 2</td><td><input name=field_2></td></tr>"
			"<tr><td>Field 3</td><td><input name=field_3></td></tr>"
			"</table>"
			"<input type=submit></form>";

		for (map<string, string>::const_iterator i = r->params_.begin();
			i != r->params_.end();
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
		body    = string ("<table>")                                   +
		"<tr><td>Accept:</td><td>"          + r->accept_          + "</td></tr>" +
		"<tr><td>Accept-Encoding:</td><td>" + r->accept_encoding_ + "</td></tr>" +
		"<tr><td>Accept-Language:</td><td>" + r->accept_language_ + "</td></tr>" +
		"<tr><td>User-Agent:</td><td>"      + r->user_agent_      + "</td></tr>" +
		"</table>"                                                +
		links;
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
