#include "webserver.h"
#include "base64.h"
#include <stdio.h>
#include <stdlib.h>

/// TODO: Handle %0A etc.
std::string Htmlize(const std::string _in)
{
	std::string _out = "<pre>";

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

	std::string title;
	std::string body;
	std::string bgcolor="#ffffff";
	std::string links =
      "<br><a href='/form'>form</a> "
      "<br><a href='/header'>show some HTTP header details</a> "
      "<br><a href='/SetValue'>Set some value</a> "
      "<br><a href='/GetValue'>Get some value</a> "
      "<br><a href='/GetProperty'>Get some property</a> "
      "<br><a href='/SetProperty'>Set some property</a> "
      ;

	if(r->path_ == "/") {
		title = "Web Server Example";
		body  = "<h1>Web Server Example</h1>"
		"I wonder what you're going to click"  + links;
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
		body = std::string("<br>Executing:<br><br>") + Htmlize(r->params_["script"]);
	}
	else if(r->path_ == "/form2")
	{
		title   = "Interaction";

		//body = "<form name=\"test\" method=\"post\" action=\"input1.php\">"

		body = "<form name=\"test\" action='/nameparams'>"
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

		for (std::map<std::string, std::string>::const_iterator i = r->params_.begin();
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

		for (std::map<std::string, std::string>::const_iterator i = r->params_.begin();
			i != r->params_.end();
			i++) {
			printf("got param %s = %s", i->first.c_str(), i->second.c_str());
			body += "<br>" + i->first + " = " + i->second;
		}

		body += "<hr>" + links;
	}
	else if (r->path_ == "/header") {
		title   = "some HTTP header details";
		body    = std::string ("<table>")                                   +
		"<tr><td>Accept:</td><td>"          + r->accept_          + "</td></tr>" +
		"<tr><td>Accept-Encoding:</td><td>" + r->accept_encoding_ + "</td></tr>" +
		"<tr><td>Accept-Language:</td><td>" + r->accept_language_ + "</td></tr>" +
		"<tr><td>User-Agent:</td><td>"      + r->user_agent_      + "</td></tr>" +
		"</table>"                                                +
		links;
	}
	else {
		r->status_ = "404 Not Found";
		title      = "Wrong URL";
		body       = "<h1>Wrong URL</h1>";
		body      += "Path is : &gt;" + r->path_ + "&lt;"; 
	}

	r->answer_  = "<html><head><title>";
	r->answer_ += title;
	r->answer_ += "</title></head><body bgcolor='" + bgcolor + "'>";
	r->answer_ += body;
	r->answer_ += "</body></html>";
}
