import httplib, urllib

import urllib
import urllib2

def getpage(code):
	url = 'http://nayade.msc.es/Splayas/ciudadano/ciudadanoVerZonaAction.do'
	values = {'codZona' : code}
	
	data = urllib.urlencode(values)
	req = urllib2.Request(url, data)
	response = urllib2.urlopen(req)
	the_page = response.read()
	outfile = open(code + ".html", "w")
	print >>outfile, the_page

num=range(50,2000)
for x in num:
	getpage(str(x))

#
#params = urllib.urlencode({'codZona': 1153})
#headers = {"Content-type": "application/x-www-form-urlencoded",
# "Accept": "text/plain"}
#conn = httplib.HTTPConnection("http://nayade.msc.es:80")
#conn.request("GET", "/home.html")
##conn.request("POST", "/Splayas/ciudadano/ciudadanoVerZonaAction.do", params, headers)
#response = conn.getresponse()
#print response.status, response.reason
#
#data = response.read()
#conn.close()
